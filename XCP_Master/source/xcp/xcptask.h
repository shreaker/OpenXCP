/**
*@file xcptask.h
*@author Michael Wolf
*@date 27.11.2017
*@brief XCP transport layer driver for XCP over Ethernet UDP/IP. The class is responsible for managing the XCP data exchange with the slave.
*/

#ifndef XCPTASK_H
#define XCPTASK_H

#include "main/model.h"
#include "udpxcp.h"
#include "xcpprotocoldefinitions.h"
#include "commandPayload.h"
#include "daqList.h"

#include <memory>
#include <map>

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QMap>
#include <QList>
#include <QMutex>

class XcpTask : public QObject {

    Q_OBJECT
    //Q_ENUMS(XcpTask::State)

  public:
    XcpTask(QSharedPointer<Model>& model, QObject *parent = Q_NULLPTR);
    enum class State {Connected, Disconnected, Error, Run, Stop};
    Q_ENUM(XcpTask::State)

  public slots:
    void slotConnect();
    void slotDisconnect();
    void slotRecordStart();
    void slotRecordStop();
    void slotResponseReceived(const QByteArray xcpPacket);
    void slotCalibration(int recordElementsIndex);

  signals:
    void sigVariableValueUpdate(uint64_t id, int64_t value);
    void sigStateChange(XcpTask::State newState);
    void sigPrintMessage(QString msg, bool isError);

  private:
    State state;
    QSharedPointer<Model> model;
    std::shared_ptr<XcpClientConfig> xcpClientConfig;
    UdpXcp udpXcp;
    bool isChecksumValid;

    //polling
    std::map<uint64_t, CommandPayload> pollingList;
    QQueue<CommandPayload> commandQueue;
    CommandPayload lastCommandRequest;
    bool isSendingPossible;
    QMutex mutexSend;
    QMutex mutexCommandQueue;
    QSharedPointer<QTimer> timerResponseTimeout;
    QSharedPointer<QTimer> timerTriggerSendNextCommandInQueue;
    QMap<uint64_t, QSharedPointer<QTimer>> timerPollingList;

    //event (DAQ)
    QMap<uint16_t, DaqList> eventDaqList;

  private:
    /***************************************************************
    *  Send to client
    ****************************************************************/
    void sendCommand(const CommandPayload& cmdPayload);
    CommandPayload payloadConnect();
    CommandPayload payloadDisconnect();
    CommandPayload payloadGetStatus();
    CommandPayload payloadShortUploadRecordElement(const std::shared_ptr<RecordElement> recordElement);
    CommandPayload payloadShortUpload(const uint32_t addr,const uint8_t dataElements,const uint8_t addrExtension = 0);
    CommandPayload payloadSetMta(const uint32_t addr,const uint8_t addrExtension = 0);
    CommandPayload payloadDownload(const uint8_t dataElements, const uint32_t data,const bool isByteAligned = true);
    CommandPayload payloadBuildChecksum(const uint32_t blockSize);
    void addRecordElementsToPollingOrEventDaqList();
    void addRecordElementsToPollingList(const std::shared_ptr<RecordElement> recordElement);
    void addRecordElementsToEventDaqList(const std::shared_ptr<RecordElement> recordElement);
    void updatePollingListValuesFromSlave();
    void requestChecksumFromSlave(const uint32_t startAddr, const uint32_t size);

    /***************************************************************
    *  Data acquisition
    ****************************************************************/
    CommandPayload payloadFreeDaq();
    CommandPayload payloadAllocDaq();
    CommandPayload payloadAllocOdt(const uint16_t daqListNr);
    CommandPayload payloadAllocOdtEntry(const uint16_t daqListNr, const uint8_t odtEntriesCount);
    CommandPayload payloadSetDaqPtr(const uint16_t daqListNr);
    CommandPayload payloadWriteDaq(const uint32_t addr,const uint8_t size,const uint8_t addrExtension = 0);
    CommandPayload payloadSetDaqListMode(const uint16_t daqListNr, const uint16_t eventChannel);
    CommandPayload payloadStartStopDaqList(const uint16_t daqListNr);
    CommandPayload payloadStartStopSynch(const bool doStart);
    void daqDynamicConfig();
    void daqStartSelected();
    void daqStopAll();

    /***************************************************************
    *  Response from client
    ****************************************************************/
    void responseProcessPositive(const CommandPayload& lastCmdSend, const QByteArray& responsePayload);
    void responseProcessNegative(const QByteArray& responsePayload);
    bool responseConnect(const QByteArray& payload);
    bool responseGetStatus(const QByteArray& payload);
    bool responseShortUpload(const QByteArray& payload, const uint64_t id, const int sizeDatatype);
    bool responseSetMta(const QByteArray& payload);
    bool responseDownload(const QByteArray& payload);
    bool responseBuildChecksum(const QByteArray& payload);
    void responseProcessDaq(const QByteArray& payload);

    /***************************************************************
    *  send command queue
    ****************************************************************/
    void addToCommandQueue(const CommandPayload& cmdPayload);
    void sendNextCommandInQueue();
    CommandPayload  removeHeadOfCommandQueue();
    void clearCommandQueue();

    /***************************************************************
    *  Timer
    ****************************************************************/
    void startTimerAddQueuePollingCommand(const uint64_t key,const int msec);
    void createTimerResponseTimeout();
    void createTimerTriggerSendNextCommandInQueue();

  private slots:
    void slotPollingForRecordElementTimer(uint64_t key);
    void slotResponseTimeout();
    void slotTriggerSendNextCommandInQueue();

    /***************************************************************
    *  Uitility
    ****************************************************************/
  private:
    void setStateAndInformBackend(const State& newState);
    QBitArray byteToBits(uint8_t byte);
    uint8_t bitsToByte(const QBitArray bits);
    bool isPayloadSizeValid(size_t validSize, size_t payloadSize);
    void printMsgDebug(const QString& msgType, const QString& cmd, const QByteArray& packet);
    void resetXcpTask();
};

#endif // XCPTASK_H
