/**
*@file xcptask.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief XCP transport layer driver for XCP over Ethernet UDP/IP. The class is responsible for managing the XCP data exchange with the slave.
*/

#include "xcptask.h"
#include "xcpConfig/ethernetconfig.h"

#include <QBitArray>
#include <iostream>
#include <stdexcept>


XcpTask::XcpTask(QSharedPointer<Model> &model, QObject *parent)
    : model(model),
      state(State::Disconnected),
      xcpClientConfig(model->getXcpClientConfig()),
      QObject(parent),
      timerPollingList(QMap<uint64_t, QSharedPointer<QTimer>>()),
      commandQueue(QQueue<CommandPayload>()),
      pollingList(std::map<uint64_t, CommandPayload>()),
      isChecksumValid(false) {

    createTimerResponseTimeout();
    createTimerTriggerSendNextCommandInQueue();

    //Signal xcpTask to slot udpXcp

    //Signal udpXcp to slot xcpTask
    QObject::connect(&udpXcp, SIGNAL(sigResponseReceived(const QByteArray)), this, SLOT(slotResponseReceived(const QByteArray)));
}

/***************************************************************
*  User actions
****************************************************************/
void XcpTask::slotConnect() {
    if(state == State::Disconnected) {
        bool ret = false;
        switch (model->getTransportProtocolSelected()) {
        case Model::TRANSPORT_PROTOCOL::ETHERNET: {
            switch (model->getEthernetConfig()->getProtocol()) {
            case EthernetConfig::PROTOCOL_UDP: {
                ret = udpXcp.initSocket(*model->getEthernetConfig(), model->getXcpHostConfig()->getEndian());
                clearCommandQueue();
                CommandPayload cmdPayload = payloadConnect();
                addToCommandQueue(cmdPayload);
                isSendingPossible = true;
                sendNextCommandInQueue();
                addRecordElementsToPollingOrEventDaqList();
                break;
            }
            case EthernetConfig::PROTOCOL_TCP:
                break;
            default:
                break;
            }
            break;
        }
        case Model::TRANSPORT_PROTOCOL::USB: {
            break;
        }
        default:
            break;
        }
        if(!ret) {
            setStateAndInformBackend(State::Error);
        }
    }
}

void XcpTask::slotDisconnect() {
    if(state == State::Connected || state == State::Stop) {
        switch (model->getTransportProtocolSelected()) {
        case Model::TRANSPORT_PROTOCOL::ETHERNET: {
            switch (model->getEthernetConfig()->getProtocol()) {
            case EthernetConfig::PROTOCOL_UDP: {
                timerPollingList.clear();
                eventDaqList.clear();
                clearCommandQueue();
                CommandPayload cmdPayload = payloadDisconnect();
                addToCommandQueue(cmdPayload);
                isSendingPossible = true;
                sendNextCommandInQueue();
                resetXcpTask();
                setStateAndInformBackend(State::Disconnected);
                break;
            }
            case EthernetConfig::PROTOCOL_TCP:
                break;
            default:
                break;
            }
            break;
        }
        case Model::TRANSPORT_PROTOCOL::USB: {
            break;
        }
        default:
            break;
        }
    }
}

void XcpTask::slotRecordStart() {
    if(state == State::Connected || state == State::Stop) {
        switch (model->getTransportProtocolSelected()) {
        case Model::TRANSPORT_PROTOCOL::ETHERNET: {
            switch (model->getEthernetConfig()->getProtocol()) {
            case EthernetConfig::PROTOCOL_UDP: {
                for(const auto& cmdPayload: pollingList) {
                    startTimerAddQueuePollingCommand(cmdPayload.second.getId(), cmdPayload.second.getRateInMsec());
                }
                daqDynamicConfig();
                setStateAndInformBackend(State::Run);
                timerTriggerSendNextCommandInQueue->start();
                break;
            }
            case EthernetConfig::PROTOCOL_TCP:
                break;
            default:
                break;
            }
            break;
        }
        case Model::TRANSPORT_PROTOCOL::USB: {
            break;
        }
        default:
            break;
        }
    }
}

void XcpTask::slotRecordStop() {
    timerResponseTimeout->stop();
    timerTriggerSendNextCommandInQueue->stop();
    timerPollingList.clear();
    clearCommandQueue();
    daqStopAll();
    timerResponseTimeout->stop();
    setStateAndInformBackend(State::Stop);

}

/***************************************************************
*  Send to client
****************************************************************/
void XcpTask::sendCommand(const CommandPayload& cmdPayload) {
    udpXcp.writeDatagram(cmdPayload.getPayload());
}

CommandPayload XcpTask::payloadConnect() {
    static const int modeNormal = 0x00;
    static const uint8_t command = XCP_CMD::CONNECT;
    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_CONNECT::MASTER_MODE, modeNormal);
    printMsgDebug("CMD", "CONNECT", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadDisconnect() {
    static const uint8_t command = XCP_CMD::DISCONNECT;
    QByteArray payload;
    payload.insert(XCP_PID, command);
    printMsgDebug("CMD", "DISCONNECT", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadGetStatus() {
    static const uint8_t command = XCP_CMD::GET_STATUS;
    QByteArray payload;
    payload.insert(XCP_PID, command);
    printMsgDebug("CMD", "GET_STATUS", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadShortUploadRecordElement(const std::shared_ptr<RecordElement> recordElement) {
    uint64_t addr = recordElement->getParseResult()->getAddress();
    uint8_t size = recordElement->getParseResult()->getSize();
    CommandPayload cmdPayload = payloadShortUpload((uint32_t) addr, size);
    cmdPayload.setRateInMsec(recordElement->getPollingRateInMs());
    return cmdPayload;
}

CommandPayload XcpTask::payloadShortUpload(const uint32_t addr,const uint8_t dataElements,const uint8_t addrExtension) {
    static const uint8_t command = XCP_CMD::SHORT_UPLOAD;

    uint8_t byte0 =  addr & 0x000000FF;
    uint8_t byte1 = (addr & 0x0000FF00) >> 8;
    uint8_t byte2 = (addr & 0x00FF0000) >> 16;
    uint8_t byte3 = (addr & 0xFF000000) >> 24;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_ELEMENTS, dataElements);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_RESERVED, XCP_HELPER::RESERVED);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_ADDR_EXTENSION, addrExtension);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_ADDR_0, byte0);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_ADDR_1, byte1);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_ADDR_2, byte2);
    payload.insert(XCP_SHORT_UPLOAD::MASTER_ADDR_3, byte3);
    printMsgDebug("CMD", "SHORT_UPLOAD", payload);

    return CommandPayload(payload, command, addr, dataElements);
}

CommandPayload XcpTask::payloadSetMta(const uint32_t addr, const uint8_t addrExtension) {
    static const uint8_t command = XCP_CMD::SET_MTA;

    uint8_t byte0 =  addr & 0x000000FF;
    uint8_t byte1 = (addr & 0x0000FF00) >> 8;
    uint8_t byte2 = (addr & 0x00FF0000) >> 16;
    uint8_t byte3 = (addr & 0xFF000000) >> 24;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_SET_MTA::MASTER_RESERVED_0,XCP_HELPER::RESERVED);
    payload.insert(XCP_SET_MTA::MASTER_RESERVED_1,XCP_HELPER::RESERVED);
    payload.insert(XCP_SET_MTA::MASTER_ADDR_EXTENSION, addrExtension);
    payload.insert(XCP_SET_MTA::MASTER_ADDR_0, byte0);
    payload.insert(XCP_SET_MTA::MASTER_ADDR_1, byte1);
    payload.insert(XCP_SET_MTA::MASTER_ADDR_2, byte2);
    payload.insert(XCP_SET_MTA::MASTER_ADDR_3, byte3);
    printMsgDebug("CMD", "SET_MTA", payload);

    return CommandPayload(payload, command, addr);
}

CommandPayload XcpTask::payloadDownload(const uint8_t dataElements, const uint32_t data, const bool isByteAligned) {
    if(!isByteAligned) {
        return CommandPayload();  // will be "empty" if not byte aligned.
    }
    static const uint8_t command = XCP_CMD::DOWNLOAD;

    uint8_t byte0 =  data & 0x000000FF;
    uint8_t byte1 = (data & 0x0000FF00) >> 8;
    uint8_t byte2 = (data & 0x00FF0000) >> 16;
    uint8_t byte3 = (data & 0xFF000000) >> 24;

    int offset = XCP_DOWNLOAD::MASTER_BYTE_ALIGNMENT_ELEMENT_1;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_DOWNLOAD::MASTER_ELEMENTS, dataElements);
    payload.insert(offset, byte0);
    payload.insert(++offset, byte1);
    payload.insert(++offset, byte2);
    payload.insert(++offset, byte3);
    printMsgDebug("CMD", "DOWNLOAD", payload);

    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadBuildChecksum(const uint32_t blockSize) {
    static const uint8_t command = XCP_CMD::BUILD_CHECKSUM;

    uint8_t byte0 =  blockSize & 0x000000FF;
    uint8_t byte1 = (blockSize & 0x0000FF00) >> 8;
    uint8_t byte2 = (blockSize & 0x00FF0000) >> 16;
    uint8_t byte3 = (blockSize & 0xFF000000) >> 24;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_RESERVED_0, XCP_HELPER::RESERVED);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_RESERVED_1, XCP_HELPER::RESERVED);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_RESERVED_2, XCP_HELPER::RESERVED);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_BLOCK_SIZE_0, byte0);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_BLOCK_SIZE_1, byte1);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_BLOCK_SIZE_2, byte2);
    payload.insert(XCP_BUILD_CHECKSUM::MASTER_BLOCK_SIZE_3, byte3);

    printMsgDebug("CMD", "BUILD_CHECKSUM", payload);
}

void XcpTask::addRecordElementsToPollingOrEventDaqList() {
    for(const auto& recordElement : model->getRecordElements()) {
        if(!recordElement->getIsSelected()) {
            continue;
        }
        RecordElement::TriggerMode triggerMode = recordElement->getTriggerMode();
        if(triggerMode == RecordElement::TriggerMode::Polling) {
            addRecordElementsToPollingList(recordElement);
        } else if(triggerMode == RecordElement::TriggerMode::Event ) {
            addRecordElementsToEventDaqList(recordElement);
        }
    }
}

void XcpTask::addRecordElementsToPollingList(const std::shared_ptr<RecordElement> recordElement) {
    uint64_t key = recordElement->getParseResult()->getAddress();
    CommandPayload cmdPayload = payloadShortUploadRecordElement(recordElement);
    pollingList.insert(std::make_pair(key, cmdPayload));
}

void XcpTask::addRecordElementsToEventDaqList(const std::shared_ptr<RecordElement> recordElement) {
    uint16_t eventChannel = recordElement->getEvent().getChannel();
    QMap<uint16_t, DaqList>::iterator it = eventDaqList.find(eventChannel);
    if(it == eventDaqList.end()) {
        DaqList daq(eventChannel); //new daqList
        daq.addToOdtEntries(recordElement);
        eventDaqList.insert(eventChannel, daq);
    } else {
        it->addToOdtEntries(recordElement); //add to existing daqList
    }
}

void XcpTask::updatePollingListValuesFromSlave() {
    for(const auto& cmdPayload: pollingList) {
        addToCommandQueue(cmdPayload.second);
    }
}

void XcpTask::requestChecksumFromSlave(const uint32_t startAddr, const uint32_t size) {
    CommandPayload setMta = payloadSetMta(startAddr);
    addToCommandQueue(setMta);
    CommandPayload buildChecksum = payloadBuildChecksum(size);
    addToCommandQueue(buildChecksum);

    sendNextCommandInQueue();
}
/***************************************************************
*  Data acquisition
****************************************************************/
void XcpTask::daqDynamicConfig() {
    CommandPayload freeDaq = payloadFreeDaq();
    addToCommandQueue(freeDaq);
    CommandPayload allocDaq = payloadAllocDaq();
    addToCommandQueue(allocDaq);

    uint16_t daqListNr = 0;
    for(auto& daq : eventDaqList) {
        daq.setDaqListNr(daqListNr);
        CommandPayload allocOdt = payloadAllocOdt(daqListNr);
        addToCommandQueue(allocOdt);
        daqListNr++;
    }
    daqListNr = 0;
    for(auto& daq : eventDaqList) {
        daq.setDaqListNr(daqListNr);
        uint8_t odtEntriesCount = daq.getOdtEntries().size();
        CommandPayload allocOdtEntry = payloadAllocOdtEntry(daqListNr, odtEntriesCount);
        addToCommandQueue(allocOdtEntry);
        daqListNr++;
    }
    daqListNr = 0;
    for(auto& daq : eventDaqList) {
        daq.setDaqListNr(daqListNr);
        CommandPayload setDaqPtr = payloadSetDaqPtr(daqListNr);
        addToCommandQueue(setDaqPtr);
        for(const auto& odtEntry : daq.getOdtEntries()) {
            uint32_t addr = odtEntry->getParseResult()->getAddress();
            uint8_t size = odtEntry->getParseResult()->getSize();
            CommandPayload writeDaq = payloadWriteDaq(addr, size);
            addToCommandQueue(writeDaq);
        }
        daqListNr++;
    }
    daqListNr = 0;
    for(auto& daq : eventDaqList) {
        daq.setDaqListNr(daqListNr);
        uint16_t eventChannel = daq.getEventChannel(); //limitaion: one event has one daqList.
        CommandPayload setDaqListMode = payloadSetDaqListMode(daqListNr, eventChannel);
        CommandPayload startStopDaqList = payloadStartStopDaqList(daqListNr);
        addToCommandQueue(setDaqListMode);
        addToCommandQueue(startStopDaqList);
        daqListNr++;
    }

    daqStartSelected();
}

void XcpTask::daqStartSelected() {
    CommandPayload startStopSynch = payloadStartStopSynch(true);
    addToCommandQueue(startStopSynch);
}

void XcpTask::daqStopAll() {
    CommandPayload startStopSynch = payloadStartStopSynch(false);
    addToCommandQueue(startStopSynch);
    sendNextCommandInQueue();
}

CommandPayload XcpTask::payloadFreeDaq() {
    static const uint8_t command = XCP_CMD::FREE_DAQ;
    QByteArray payload;
    payload.insert(XCP_PID, command);
    printMsgDebug("CMD", "FREE_DAQ", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadAllocDaq() {
    static const uint8_t command = XCP_CMD::ALLOC_DAQ;

    uint16_t daqCount = eventDaqList.size();
    uint8_t byte0 =  daqCount & 0x000000FF;
    uint8_t byte1 = (daqCount & 0x0000FF00) >> 8;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_ALLOC_DAQ::MASTER_RESERVED, XCP_HELPER::RESERVED);
    payload.insert(XCP_ALLOC_DAQ::MASTER_DAQ_COUNT_0, byte0);
    payload.insert(XCP_ALLOC_DAQ::MASTER_DAQ_COUNT_1, byte1);
    printMsgDebug("CMD", "ALLOC_DAQ", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadAllocOdt(const uint16_t daqListNr) {
    static const uint8_t command = XCP_CMD::ALLOC_ODT;

    uint8_t byte0 =  daqListNr & 0x000000FF;
    uint8_t byte1 = (daqListNr & 0x0000FF00) >> 8;

    static const uint8_t odtCount = 1;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_ALLOC_ODT::MASTER_RESERVED, XCP_HELPER::RESERVED);
    payload.insert(XCP_ALLOC_ODT::MASTER_DAQ_LIST_NR_0, byte0);
    payload.insert(XCP_ALLOC_ODT::MASTER_DAQ_LIST_NR_1, byte1);
    payload.insert(XCP_ALLOC_ODT::MASTER_ODT_COUNT, odtCount);
    printMsgDebug("CMD", "ALLOC_ODT", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadAllocOdtEntry(const uint16_t daqListNr, const uint8_t odtEntriesCount) {
    static const uint8_t command = XCP_CMD::ALLOC_ODT_ENTRY;

    uint8_t byte0 =  daqListNr & 0x000000FF;
    uint8_t byte1 = (daqListNr & 0x0000FF00) >> 8;

    static const uint8_t odtNr = 0;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_ALLOC_ODT_ENTRY::MASTER_RESERVED, XCP_HELPER::RESERVED);
    payload.insert(XCP_ALLOC_ODT_ENTRY::MASTER_DAQ_LIST_NR_0, byte0);
    payload.insert(XCP_ALLOC_ODT_ENTRY::MASTER_DAQ_LIST_NR_1, byte1);
    payload.insert(XCP_ALLOC_ODT_ENTRY::MASTER_ODT_NR, odtNr);
    payload.insert(XCP_ALLOC_ODT_ENTRY::MASTER_ODT_ENTRIES_COUNT, odtEntriesCount);
    printMsgDebug("CMD", "ALLOC_ODT_ENTRY", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadSetDaqPtr(const uint16_t daqListNr) {
    static const uint8_t command = XCP_CMD::SET_DAQ_PTR;

    uint8_t byte0 =  daqListNr & 0x000000FF;
    uint8_t byte1 = (daqListNr & 0x0000FF00) >> 8;

    static const uint8_t odtNr = 0;
    static const uint8_t odtEntryNr = 0;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_SET_DAQ_PTR::MASTER_RESERVED, XCP_HELPER::RESERVED);
    payload.insert(XCP_SET_DAQ_PTR::MASTER_DAQ_LIST_NR_0, byte0);
    payload.insert(XCP_SET_DAQ_PTR::MASTER_DAQ_LIST_NR_1, byte1);
    payload.insert(XCP_SET_DAQ_PTR::MASTER_ODT_NR, odtNr);
    payload.insert(XCP_SET_DAQ_PTR::MASTER_ODT_ENTRY_NR, odtEntryNr);
    printMsgDebug("CMD", "SET_DAQ_PTR", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadWriteDaq(const uint32_t addr, const uint8_t size, const uint8_t addrExtension) {
    static const uint8_t command = XCP_CMD::WRITE_DAQ;

    uint8_t byte0 =  addr & 0x000000FF;
    uint8_t byte1 = (addr & 0x0000FF00) >> 8;
    uint8_t byte2 = (addr & 0x00FF0000) >> 16;
    uint8_t byte3 = (addr & 0xFF000000) >> 24;

    static const uint8_t IGNORE = 0xFF;
    static const uint8_t bitOffset = IGNORE;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_WRITE_DAQ::MASTER_BIT_OFFSET, bitOffset);
    payload.insert(XCP_WRITE_DAQ::MASTER_SIZE_DAQ_ELEMENT, size);
    payload.insert(XCP_WRITE_DAQ::MASTER_ADDR_EXTENSION, addrExtension);
    payload.insert(XCP_WRITE_DAQ::MASTER_ADDR_0, byte0);
    payload.insert(XCP_WRITE_DAQ::MASTER_ADDR_1, byte1);
    payload.insert(XCP_WRITE_DAQ::MASTER_ADDR_2, byte2);
    payload.insert(XCP_WRITE_DAQ::MASTER_ADDR_3, byte3);

    printMsgDebug("CMD", "WRITE_DAQ", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadSetDaqListMode(const uint16_t daqListNr, const uint16_t eventChannel) {
    static const uint8_t command = XCP_CMD::SET_DAQ_LIST_MODE;

    QBitArray modeBits(8);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_ALTERNATE, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_DIRECTION, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_RESERVED_0, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_DTO_CTR, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_TIMESTAMP, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_PID_OFF, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_RESERVED_1, false);
    modeBits.setBit(XCP_SET_DAQ_LIST_MODE::MASTER_MODE_RESERVED_2, false);
    uint8_t mode = bitsToByte(modeBits);

    uint8_t daqListNrByte0 =  daqListNr & 0x000000FF;
    uint8_t daqListNrByte1 = (daqListNr & 0x0000FF00) >> 8;

    uint8_t eventChannelByte0 =  eventChannel & 0x000000FF;
    uint8_t eventChannelByte1 = (eventChannel & 0x0000FF00) >> 8;

    static const uint8_t transmissionRatePrescaler = 1;
    static const uint8_t daqListPriority = 0;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_MODE, mode);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_DAQ_LIST_NR_0, daqListNrByte0);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_DAQ_LIST_NR_1, daqListNrByte1);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_EVENT_CHANNEL_NR_0, eventChannelByte0);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_EVENT_CHANNEL_NR_1, eventChannelByte1);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_TRANS_RATE_PRESC, transmissionRatePrescaler);
    payload.insert(XCP_SET_DAQ_LIST_MODE::MASTER_DAQ_LIST_PRIO, daqListPriority);

    printMsgDebug("CMD", "SET_DAQ_LIST_MODE", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadStartStopDaqList(const uint16_t daqListNr) {
    static const uint8_t command = XCP_CMD::START_STOP_DAQ_LIST;

    static const uint8_t SELECT = 0x02;
    static const uint8_t mode = SELECT;

    uint8_t byte0 =  daqListNr & 0x000000FF;
    uint8_t byte1 = (daqListNr & 0x0000FF00) >> 8;

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_START_STOP_DAQ_LIST::MASTER_MODE, mode);
    payload.insert(XCP_START_STOP_DAQ_LIST::MASTER_DAQ_LIST_NR_0, byte0);
    payload.insert(XCP_START_STOP_DAQ_LIST::MASTER_DAQ_LIST_NR_1, byte1);

    printMsgDebug("CMD", "START_STOP_DAQ_LIST", payload);
    return CommandPayload(payload, command);
}

CommandPayload XcpTask::payloadStartStopSynch(const bool doStart) {
    static const uint8_t command = XCP_CMD::START_STOP_SYNCH;

    static const uint8_t STOP_ALL  = 0x00;
    static const uint8_t START_SELECTED = 0x01;
    static const uint8_t STOP_SELECTED  = 0x02;

    uint8_t mode;
    if(doStart) {
        mode = START_SELECTED;
    } else {
        mode = STOP_ALL;
    }

    QByteArray payload;
    payload.insert(XCP_PID, command);
    payload.insert(XCP_START_STOP_SYNCH::MASTER_MODE, mode);

    printMsgDebug("CMD", "XCP_START_STOP_SYNCH", payload);
    return CommandPayload(payload, command);
}

/***************************************************************
*  Response from client
****************************************************************/
void XcpTask::slotResponseReceived(const QByteArray payload) {
    timerResponseTimeout->stop();

    const uint8_t pid = payload.at(XCP_PID);

    switch (pid) {
    case XCP_CMD_RESPONSE_PACKET::RES_POSITIVE: {
        responseProcessPositive(lastCommandRequest, payload);
        isSendingPossible = true;
        sendNextCommandInQueue();
        break;
    }
    case XCP_CMD_RESPONSE_PACKET::ERROR: {
        responseProcessNegative(payload);
        isSendingPossible = true;
        sendNextCommandInQueue();
        break;
    }
    case XCP_CMD_RESPONSE_PACKET::SERVICE_REQUEST: {
        break;
    }
    case XCP_CMD_RESPONSE_PACKET::EVENT: {
        break;
    }
    default:
        responseProcessDaq(payload);
        break;
    }
}

void XcpTask::responseProcessPositive(const CommandPayload &lastCmdSend,  const QByteArray& responsePayload) {
    if(lastCmdSend.getCmd() == CommandPayload::UNINITIALIZED) {
        return;
    }

    bool ret;
    switch (lastCmdSend.getCmd()) {
    case XCP_CMD::CONNECT: {
        ret = responseConnect(responsePayload);
        if(ret) {
            setStateAndInformBackend(State::Connected);
            CommandPayload cmdPayload = payloadGetStatus();
            addToCommandQueue(cmdPayload);
        }
        break;
    }
    case XCP_CMD::DISCONNECT: {
        setStateAndInformBackend(State::Disconnected);
        break;
    }
    case XCP_CMD::GET_STATUS: {
        ret = responseGetStatus(responsePayload);
        updatePollingListValuesFromSlave();
        break;
    }
    case XCP_CMD::SHORT_UPLOAD: {
        ret = responseShortUpload(responsePayload, lastCmdSend.getId(), lastCmdSend.getSizeDataType());
        break;
    }
    case XCP_CMD::SET_MTA: {
        ret = responseSetMta(responsePayload);
        break;
    }
    case XCP_CMD::DOWNLOAD: {
        ret = responseDownload(responsePayload);
        break;
    }
    case XCP_CMD::BUILD_CHECKSUM: {
        ret = responseBuildChecksum(responsePayload);
        break;
    }
    default:
        printMsgDebug("RESP. CMD: " + QString::number(lastCmdSend.getCmd(), 16).toUpper(), "Payload: ", responsePayload);
        break;
    }
}

void XcpTask::responseProcessNegative(const QByteArray &responsePayload) {

    uint8_t errorCode;
    try {
        errorCode = responsePayload.at(1);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in responseProcessNegative(): " << oor.what() << std::endl;
    }

    switch (errorCode) {
    case XCP_ERROR::ERR_CMD_SYNCH: {
        emit sigPrintMessage("XCP Error: ERR_CMD_SYNCH", true);
        break;
    }
    case XCP_ERROR::ERR_CMD_BUSY: {
        emit sigPrintMessage("XCP Error: ERR_CMD_BUSY", true);
        break;
    }
    case XCP_ERROR::ERR_DAQ_ACTIVE: {
        emit sigPrintMessage("XCP Error: ERR_DAQ_ACTIVE", true);
        break;
    }
    case XCP_ERROR::ERR_PGM_ACTIVE: {
        emit sigPrintMessage("XCP Error: ERR_PGM_ACTIVE", true);
        break;
    }
    case XCP_ERROR::ERR_CMD_UNKNOWN: {
        emit sigPrintMessage("XCP Error: ERR_CMD_UNKNOWN", true);
        break;
    }
    case XCP_ERROR::ERR_CMD_SYNTAX: {
        emit sigPrintMessage("XCP Error: ERR_CMD_SYNTAX", true);
        break;
    }
    case XCP_ERROR::ERR_OUT_OF_RANGE: {
        emit sigPrintMessage("XCP Error: ERR_OUT_OF_RANGE", true);
        break;
    }
    case XCP_ERROR::ERR_WRITE_PROTECTED: {
        emit sigPrintMessage("XCP Error: ERR_WRITE_PROTECTED", true);
        break;
    }
    case XCP_ERROR::ERR_ACCESS_DENIED: {
        emit sigPrintMessage("XCP Error: ERR_ACCESS_DENIED", true);
        break;
    }
    case XCP_ERROR::ERR_ACCESS_LOCKED: {
        emit sigPrintMessage("XCP Error: ERR_ACCESS_LOCKED", true);
        break;
    }
    case XCP_ERROR::ERR_PAGE_NOT_VALID: {
        emit sigPrintMessage("XCP Error: ERR_PAGE_NOT_VALID", true);
        break;
    }
    case XCP_ERROR::ERR_MODE_NOT_VALID: {
        emit sigPrintMessage("XCP Error: ERR_MODE_NOT_VALID", true);
        break;
    }
    case XCP_ERROR::ERR_SEGMENT_NOT_VALID: {
        emit sigPrintMessage("XCP Error: ERR_SEGMENT_NOT_VALID", true);
        break;
    }
    case XCP_ERROR::ERR_SEQUENCE: {
        emit sigPrintMessage("XCP Error: ERR_SEQUENCE", true);
        break;
    }
    case XCP_ERROR::ERR_DAQ_CONFIG: {
        emit sigPrintMessage("XCP Error: ERR_DAQ_CONFIG", true);
        break;
    }
    case XCP_ERROR::ERR_MEMORY_OVERFLOW: {
        emit sigPrintMessage("XCP Error: ERR_MEMORY_OVERFLOW", true);
        break;
    }
    case XCP_ERROR::ERR_GENERIC: {
        emit sigPrintMessage("XCP Error: ERR_GENERIC", true);
        break;
    }
    case XCP_ERROR::ERR_VERIFY: {
        emit sigPrintMessage("XCP Error: ERR_VERIFY", true);
        break;
    }
    case XCP_ERROR::ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE: {
        emit sigPrintMessage("XCP Error: ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE", true);
        break;
    }
    case XCP_ERROR::ERR_SUBCMD_UNKNOWN: {
        emit sigPrintMessage("XCP Error: ERR_SUBCMD_UNKNOWN", true);
        break;
    }
    default: {
        emit sigPrintMessage("XCP Error: unknown error", true);
        break;
    }
    }
}

bool XcpTask::responseConnect(const QByteArray &payload) {
    printMsgDebug("RESP.", "CONNECT", payload);
    bool isSizeValid = isPayloadSizeValid(XCP_CONNECT::SLAVE_LENGTH, payload.size());
    if(!isSizeValid) {
        return false;
    }
    QBitArray ressourceBits = byteToBits(payload.at(XCP_CONNECT::SLAVE_RESOURCE));
    QBitArray commBasicBits = byteToBits(payload.at(XCP_CONNECT::SLAVE_COMM_BASIC));

    XcpClientConfig::IsSupported isSupported = {};
    isSupported.calibrationPaging = ressourceBits.at(XCP_CONNECT::SLAVE_RESOURCE_CAL_PAG);
    isSupported.daqList = ressourceBits.at(XCP_CONNECT::SLAVE_RESOURCE_DAQ);
    isSupported.stimulation = ressourceBits.at(XCP_CONNECT::SLAVE_RESOURCE_STIM);
    isSupported.programming = ressourceBits.at(XCP_CONNECT::SLAVE_RESOURCE_PGM);
    isSupported.slaveBlockModeAvailable = commBasicBits.at(XCP_CONNECT::SLAVE_COMM_BASIC_SLAVE_BLOCK_MODE);
    isSupported.optionalAvailable = commBasicBits.at(XCP_CONNECT::SLAVE_COMM_BASIC_OPTIONAL);
    xcpClientConfig->setIsSupported(isSupported);

    if(commBasicBits.at(XCP_CONNECT::SLAVE_COMM_BASIC_BYTE_ORDER)) {
        xcpClientConfig->setEndian(XcpOptions::ENDIAN_BIG);
    } else {
        xcpClientConfig->setEndian(XcpOptions::ENDIAN_LITTLE);
    }
    QBitArray addrGranularityBits(2);
    addrGranularityBits.setBit(0, commBasicBits.at(XCP_CONNECT::SLAVE_COMM_BASIC_ADDR_GRANULARITY_0));
    addrGranularityBits.setBit(1, commBasicBits.at(XCP_CONNECT::SLAVE_COMM_BASIC_ADDR_GRANULARITY_1));
    uint8_t addrGranularity = bitsToByte(addrGranularityBits);
    if(addrGranularity == 0) {
        xcpClientConfig->setAddrGranularity(XcpOptions::ADDRESS_GRANULARITY_BYTE);
    } else if(addrGranularity == 1) {
        xcpClientConfig->setAddrGranularity(XcpOptions::ADDRESS_GRANULARITY_WORD);
    } else if(addrGranularity == 2) {
        xcpClientConfig->setAddrGranularity(XcpOptions::ADDRESS_GRANULARITY_DWORD);
    }

    return true;
}

bool XcpTask::responseGetStatus(const QByteArray &payload) {
    printMsgDebug("RESP.", "GET_STATUS", payload);
    bool isSizeValid = isPayloadSizeValid(XCP_GET_STATUS::SLAVE_LENGTH, payload.size());
    if(!isSizeValid) {
        return false;
    }
    QBitArray sessionStatusBits = byteToBits(payload.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS));
    QBitArray resourceProtectStatusBits = byteToBits(payload.at(XCP_GET_STATUS::SLAVE_RESOURCE_PROTECT_STATUS));

    XcpClientConfig::SessionStatus sessionStatus = {};
    sessionStatus.storeCalReq = sessionStatusBits.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS_STORE_CAL_REQ);
    sessionStatus.storeDaqReq = sessionStatusBits.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS_STORE_DAQ_REQ);
    sessionStatus.clearDaqReq = sessionStatusBits.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS_CLEAR_DAQ_REQ);
    sessionStatus.daqRunning = sessionStatusBits.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS_DAQ_RUNNING);
    sessionStatus.resume = sessionStatusBits.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS_RESUME);
    xcpClientConfig->setSessionStatus(sessionStatus);

    XcpClientConfig::ResourceProtectStatus resourceProtectStatus = {};
    resourceProtectStatus.calPag = resourceProtectStatusBits.at(XCP_GET_STATUS::SLAVE_RESOURCE_PROTECT_STATUS_CAL_PAG);
    resourceProtectStatus.daq = resourceProtectStatusBits.at(XCP_GET_STATUS::SLAVE_RESOURCE_PROTECT_STATUS_DAQ);
    resourceProtectStatus.stim = resourceProtectStatusBits.at(XCP_GET_STATUS::SLAVE_RESOURCE_PROTECT_STATUS_STIM);
    resourceProtectStatus.pgm = resourceProtectStatusBits.at(XCP_GET_STATUS::SLAVE_RESOURCE_PROTECT_STATUS_PGM);
    xcpClientConfig->setResourceProtectStatus(resourceProtectStatus);

    xcpClientConfig->setStateNum(payload.at(XCP_GET_STATUS::SLAVE_SESSION_STATUS));
    xcpClientConfig->setSessionConfigId(payload.at(XCP_GET_STATUS::SLAVE_SESSION_CONFIG_ID));

    return true;
}

bool XcpTask::responseShortUpload(const QByteArray& payload, const uint64_t id, const int sizeDatatype) {
    printMsgDebug("RESP.", "SHORT_UPLOAD", payload);
    bool isSizeValid = isPayloadSizeValid(sizeDatatype + 1, payload.size());
    if(!isSizeValid) {
        return false;
    }

    uint32_t value = 0;
    int readStart = sizeDatatype;
    int readEnd = XCP_PID+1;
    for(int i = readStart; i >= readEnd; i--) { //little endian
        value |= (unsigned char) payload.at(i);
        if(i > readEnd) {
            value = value << 8;
        }
    }
    emit sigVariableValueUpdate(id, (int64_t) value);
    return true;
}

bool XcpTask::responseSetMta(const QByteArray &payload) {
    printMsgDebug("RESP.", "SET_MTA", payload);
    bool isSizeValid = isPayloadSizeValid(1, payload.size());
    if(!isSizeValid) {
        return false;
    }
    return true;
}

bool XcpTask::responseDownload(const QByteArray &payload) {
    printMsgDebug("RESP.", "DOWNLOAD", payload);
    bool isSizeValid = isPayloadSizeValid(1, payload.size());
    if(!isSizeValid) {
        return false;
    }
    return true;
}

bool XcpTask::responseBuildChecksum(const QByteArray &payload) {
    printMsgDebug("RESP.", "BUILD_CHECKSUM", payload);
    bool isSizeValid = isPayloadSizeValid(XCP_BUILD_CHECKSUM::SLAVE_LENGTH, payload.size());
    if(!isSizeValid) {
        return false;
    }

    uint32_t value = 0;
    int readStart = XCP_BUILD_CHECKSUM::SLAVE_CHECKSUM_3;
    int readEnd = XCP_BUILD_CHECKSUM::SLAVE_CHECKSUM_0;
    for(int i = readStart; i >= readEnd; i--) { //little endian
        value |= (unsigned char) payload.at(i);
        if(i > readEnd) {
            value = value << 8;
        }
    }

    //TODO: sum up value

    return true;
}

void XcpTask::responseProcessDaq(const QByteArray &payload) {
    printMsgDebug("RESP.", "DAQ_DATA", payload);

    const int DAQ_LIST_NR_BYTE = 0;
    uint8_t daqListNrResponse = payload.at(DAQ_LIST_NR_BYTE);

    for(const auto& daq : eventDaqList) {
        uint16_t daqListNr = daq.getDaqListNr();

        if(daqListNrResponse == daqListNr) { //find matching daq
            QList<std::shared_ptr<RecordElement>> odtEntries = daq.getOdtEntries();
            int payloadOffset = DAQ_LIST_NR_BYTE;

            for(const auto& odtEntry : odtEntries) {
                uint32_t sizeDatatype = odtEntry->getParseResult()->getSize();
                uint32_t value = 0;
                int readStart = payloadOffset + sizeDatatype;
                int readEnd = payloadOffset +1;
                for(int i = readStart; i >= readEnd; i--) { //little endian
                    try {
                        value |= (unsigned char) payload.at(i);
                        if(i > readEnd) {
                            value = value << 8;
                        }
                    } catch (const std::out_of_range& oor) {
                        std::cerr << "Out of Range error in responseProcessDaq(): " << oor.what() << std::endl;
                    }
                }
                emit sigVariableValueUpdate(odtEntry->getParseResult()->getAddress(), (int64_t) value);

                payloadOffset += sizeDatatype;
            }
            break;
        }
    }
}


/***************************************************************
*  send command queue
****************************************************************/
inline void XcpTask::sendNextCommandInQueue() {
    QMutexLocker locker(&mutexSend);
    lastCommandRequest = removeHeadOfCommandQueue();
    if(isSendingPossible && lastCommandRequest.getCmd() != CommandPayload::UNINITIALIZED) {
        isSendingPossible = false;
        sendCommand(lastCommandRequest);
        timerResponseTimeout->start();
    }
}

inline void XcpTask::addToCommandQueue(const CommandPayload& cmdPayload) {
    static const int MAX_QUEUE_SIZE = 100;
    QMutexLocker locker(&mutexCommandQueue);
    if(commandQueue.size() < MAX_QUEUE_SIZE) {
        commandQueue.enqueue(cmdPayload);
    } else {
        qWarning() << "XCP command queue full.";
    }
}

inline CommandPayload XcpTask::removeHeadOfCommandQueue() {
    CommandPayload head;
    QMutexLocker locker(&mutexCommandQueue);
    if(!commandQueue.isEmpty()) {
        head = commandQueue.head();
        commandQueue.dequeue();
    }
    return head; // will be "empty" if not found
}

inline void XcpTask::clearCommandQueue() {
    QMutexLocker locker(&mutexCommandQueue);
    commandQueue.clear();
}


/***************************************************************
*  Timer
****************************************************************/
void XcpTask::createTimerResponseTimeout() {
    timerResponseTimeout = QSharedPointer<QTimer>::create(this);
    connect(timerResponseTimeout.data(), SIGNAL(timeout()), this, SLOT(slotResponseTimeout()));
    timerResponseTimeout->setSingleShot(true);
    timerResponseTimeout->setInterval(model->getXcpHostConfig()->getTimeout());
}

void XcpTask::createTimerTriggerSendNextCommandInQueue() {
    timerTriggerSendNextCommandInQueue = QSharedPointer<QTimer>::create(this);
    connect(timerTriggerSendNextCommandInQueue.data(), SIGNAL(timeout()), this, SLOT(slotTriggerSendNextCommandInQueue()));
    timerTriggerSendNextCommandInQueue->setInterval(10);
}

void XcpTask::slotTriggerSendNextCommandInQueue() {
    if(isSendingPossible) {
        sendNextCommandInQueue();
    }
}

void XcpTask::slotResponseTimeout() {
    qDebug() << "Response timeout. Add last command to end of queue.";
    addToCommandQueue(lastCommandRequest);
    isSendingPossible = true;
    sendNextCommandInQueue();
}

void XcpTask::startTimerAddQueuePollingCommand(const uint64_t key, const int msec) {
    QSharedPointer<QTimer> timer = QSharedPointer<QTimer>::create(this);
    connect(timer.data(), &QTimer::timeout, [=]() {
        slotPollingForRecordElementTimer(key);
    });
    timerPollingList.insert(key, timer);
    timer->start(msec);
}

void XcpTask::slotPollingForRecordElementTimer(const uint64_t key) {
    try {
        addToCommandQueue(pollingList.at(key));
    } catch (const std::out_of_range& oor) {
        std::cout << "Out of Range error in slotPollingForRecordElementTimer(): " << oor.what() << std::endl;
    }
}

/***************************************************************
*  Uitility
****************************************************************/
QBitArray XcpTask::byteToBits(uint8_t byte) {
    QBitArray bits(8);
    for(int i=0; i<8; i++) {
        bool value = (byte & (1<<i)) != 0;
        bits.setBit(i, value);
    }
    return bits;
}

uint8_t XcpTask::bitsToByte(const QBitArray bits) {
    uint8_t sum = 0;
    for(int i = 0; i < bits.size(); i++) {
        if(bits.at(i)) {
            sum += std::pow(i,2);
        }
    }
    return sum;
}

bool XcpTask::isPayloadSizeValid(size_t validMinimumSize, size_t payloadSize) {
    bool isSizeValid = true;
    if(payloadSize < validMinimumSize) {
        isSizeValid = false;
#ifdef DEBUG
        qDebug() << "Payload size is not valid. Payload size: " << payloadSize << "should be: " << validMinimumSize;
#endif
    }
    return isSizeValid;
}

void XcpTask::printMsgDebug(const QString& msgType, const QString& cmd, const QByteArray& packet) {
#ifdef DEBUG
    qDebug() << msgType << cmd << packet.toHex(' ').toUpper();
#endif
}

void XcpTask::resetXcpTask() {
    timerResponseTimeout->stop();
    timerTriggerSendNextCommandInQueue->stop();
    timerPollingList.clear();
    clearCommandQueue();
    pollingList.clear();
    eventDaqList.clear();
}

void XcpTask::setStateAndInformBackend(const State& newState) {
    state = newState;
    emit sigStateChange(newState);
}

void XcpTask::slotCalibration(int recordElementsIndex) {
    try {
        shared_ptr<RecordElement> recordElement = model->getRecordElements().at(recordElementsIndex);
        shared_ptr<ParseResultBase> parseResult = recordElement->getParseResult();

        CommandPayload setMta = payloadSetMta((uint32_t) parseResult->getAddress());
        CommandPayload download;
        if(recordElement->getIsFloat()) {
            download = payloadDownload(parseResult->getSize(), (uint32_t) recordElement->getValueToSetFloat()); //@TODO hanlde floats
        } else {
            download = payloadDownload(parseResult->getSize(), (uint32_t) recordElement->getValueToSetInt());
        }
        CommandPayload shortUpload = payloadShortUploadRecordElement(recordElement);

        addToCommandQueue(setMta);
        addToCommandQueue(download);
        addToCommandQueue(shortUpload);
        sendNextCommandInQueue();
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in slotCalibration(): " << oor.what() << std::endl;
    }
}




