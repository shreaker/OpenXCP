/**
*@file backend.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The Backend class is the core of the software architecture. It controls the program flow. The Backend is the so called business logic in the model-view-controller design pattern.
*/

#include "backend.h"

#include "serialize/serializeJson.h"
#include "serialize/serializeA2l.h"
#include "xcp/xcptask.h"
#include "record/recordElement.h"
#include "record/record.h"

#include <memory>
#include <string>
#include <QString>
#include <QCryptographicHash>

#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QDir>
#include <QDebug>



using std::string;

Backend::Backend(QObject *parent)
    : QObject(parent),
      model(QSharedPointer<Model>::create()),
      xcpTask(model),
      xcpState(XcpTask::State::Disconnected) {

    recordStartTimestamp = 0;

    //Signal backend to slot xcpTask
    QObject::connect(this, SIGNAL(sigConnectToClient()), &xcpTask, SLOT(slotConnect()));
    QObject::connect(this, SIGNAL(sigDisconnectClient()), &xcpTask, SLOT(slotDisconnect()));
    QObject::connect(this, SIGNAL(sigRecordStart()), &xcpTask, SLOT(slotRecordStart()));
    QObject::connect(this, SIGNAL(sigRecordStop()), &xcpTask, SLOT(slotRecordStop()));
    QObject::connect(this, SIGNAL(sigCalibration(int)), &xcpTask, SLOT(slotCalibration(int)));

    //Signal xcpTask to slot backend
    QObject::connect(&xcpTask, SIGNAL(sigVariableValueUpdate(uint64_t, int64_t)), this, SLOT(slotVariableValueUpdate(uint64_t, int64_t)));
    QObject::connect(&xcpTask, SIGNAL(sigStateChange(XcpTask::State)), this, SLOT(slotStateChangeXcpTask(XcpTask::State)));
    QObject::connect(&xcpTask, SIGNAL(sigPrintMessage(QString, bool)), this, SLOT(slotPrintMessage(QString, bool)));
}

QSharedPointer<Model> Backend::getModel() const {
    return model;
}

void Backend::setModel(const QSharedPointer<Model> &value) {
    model = value;
}

void Backend::parse() {
    QString elfFilePath = model->getFilesElfFile();
    QString workspacePath = model->getFilesSourcePath();

    std::unique_ptr<ParserManager> parserManager(new ParserManager(elfFilePath.toStdString(), workspacePath.toStdString()));

    QObject::connect(parserManager.get(), SIGNAL(sigParseProgress(int)), this, SLOT(slotParseProgress(int)));

    parserManager->parse();

    model->setHashElfFile(fileChecksum(elfFilePath));

    updateModelWithParseResults(parserManager);

    emit sigParseProgress(100);
}

void Backend::saveProject() {
    SerializeJson serializeJson(model, model->getFilesProjectFile());
    serializeJson.save();
}

void Backend::loadProject() {
    model->removeAllFromRecordElements();
    model->removeAllFromParseResults();

    SerializeJson serializeJson(model, model->getFilesProjectFile());
    serializeJson.load();
    bool isElfFileModified = hasFileBeenModified(model->getFilesElfFile(), model->getHashElfFile());
    if(isElfFileModified) {
        model->removeAllFromParseResults();
    }
    for(const auto& parseResult : model->getParseResults()) {
        if(parseResult->getIsSelectedByUser()) {
            addToRecordElementsInModel(parseResult);
        }
    }

    emit sigLoadProjectCompleted(isElfFileModified);
}

void Backend::exportToA2l() {
    SerializeA2L serializeA2L(model, model->getFilesA2lFile());
    serializeA2L.save();
}

void Backend::removeNonSourceCommentParseResults() {
    for(auto& parseResult : model->getParseResults()) {
        if(parseResult->getSrcParserObj() == nullptr) {
            model->removeFromParseResults(parseResult);
        }
    }
    emit sigUpdateGuiFromModel();
}

bool Backend::hasFileBeenModified(const QString& fileName, const QByteArray& fileHash) {
    static const int MEMORY_BLOCK_EQUAL = 0;
    bool isModified = true;
    QByteArray newHash = fileChecksum(fileName);
    int newHashSize = newHash.size();

    if(newHashSize == fileHash.size()) {
        int ret = memcmp(newHash, fileHash, newHashSize);
        if(ret == MEMORY_BLOCK_EQUAL) {
            isModified = false;
        }
    }

    return isModified;
}

XcpTask::State Backend::getXcpState() const {
    return xcpState;
}

QByteArray Backend::fileChecksum(const QString& fileName) {
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);
        if (hash.addData(&f)) {
            return hash.result().toHex();
        }
    }
    return QByteArray();
}

void Backend::updateModelWithParseResults(const std::unique_ptr<ParserManager>& parserManager) {
    model->removeAllFromRecordElements();
    model->removeAllFromParseResults();
    for(const auto& var : parserManager->getVariables()) {
        model->addToParseResults(var);
    }
    for(const auto& var : parserManager->getArrays()) {
        model->addToParseResults(var);
    }
    for(const auto& var : parserManager->getStructs()) {
        for(const auto& element : var->getElements()) {
            element->setName(var->getName() + "__" + element->getName());
            model->addToParseResults(element);
        }
    }
}

void Backend::addToRecordElementsInModel(const shared_ptr<ParseResultBase> parseResult) {
    if(parseResult->getAbstractDataType().compare("array") == 0) {
        shared_ptr<ParseResultArray> array = std::dynamic_pointer_cast<ParseResultArray>(parseResult);
        for(int i = 0; i < array->getElements(); i++) {
            shared_ptr<ParseResultVariable> arrayElement = std::make_shared<ParseResultVariable>(array->createParseResultVariable(i));
            model->addToRecordElements(arrayElement);
        }
    } else {
        model->addToRecordElements(parseResult);
    }
}

void Backend::removeFromRecordElementsInModel(const shared_ptr<ParseResultBase> parseResult) {
    if(parseResult->getAbstractDataType().compare("array") == 0) {
        shared_ptr<ParseResultArray> array = std::dynamic_pointer_cast<ParseResultArray>(parseResult);
        int sizePerElement = array->getSizePerElement();
        uint64_t startAddress = array->getAddress();
        for(int i = 0; i < array->getElements(); i++) {
            uint64_t offsetAddress = startAddress + (sizePerElement * i);
            for(auto& recordElement : model->getRecordElements()) {
                uint64_t addr = recordElement->getParseResult()->getAddress();
                if(offsetAddress == addr) {
                    model->removeFromRecordElements(recordElement->getParseResult());
                    break;
                }
            }
        }
    } else {
        model->removeFromRecordElements(parseResult);
    }
}

bool Backend::isDataTypeUnSigned(const string& dataType) {
    return (dataType.find("unsigned") != std::string::npos);
}


/***************************************************************
*  Save Record
****************************************************************/
void Backend::saveRecord(const QString& path) {
    QStringList header;
    QtCSV::StringData data;

    writeRecordHeader(header, data);

    QMap<uint64_t, int> idSignalPositionMap;
    writeRecordSignalNames(idSignalPositionMap, data);

    if(!recordList.isEmpty()) {
        double lastRelativeTime = recordList.at(0).getRelativeTime();
        QStringList row;
        QMap<int, Record> recordsWithEqualTimestamp;
        for(auto& record: recordList) {
            double currentRelativeTime = record.getRelativeTime();

            static const double EPSILON_ONE_MS = 0.001;
            if((currentRelativeTime - lastRelativeTime) <= EPSILON_ONE_MS) {
                addRecordEntryToRecordsWithEqualTimestamp(record, recordsWithEqualTimestamp, idSignalPositionMap);
            } else {
                row << QString::number(lastRelativeTime, 'f'); //time
                int posOfRecord;
                int posInRow = 1;
                for(const auto& record : recordsWithEqualTimestamp.values()) {
                    posOfRecord = record.getPosition();
                    for(posInRow; posInRow < posOfRecord; posInRow++) {
                        row << " "; //leading blanks
                    }
                    row << QString::number(record.getValue()); //value
                    posInRow++; //for next loop
                }
                for(int i = posOfRecord; i <= idSignalPositionMap.last(); i++) {
                    row << " "; //trailing blanks
                }
                data.addRow(row);

                //reset
                row.clear();
                lastRelativeTime = currentRelativeTime;
                recordsWithEqualTimestamp.clear();

                //new time (first entry)
                addRecordEntryToRecordsWithEqualTimestamp(record, recordsWithEqualTimestamp, idSignalPositionMap);
            }

        }
    }

    writeRecordCsvFile(path, data, header);
}

void Backend::writeRecordHeader(QStringList& header, QtCSV::StringData& data) {
    header << "date" << "time";
    QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    QStringList dateTime;
    dateTime << date << time;
    data.addRow(dateTime);
    data.addEmptyRow();
}

void Backend::writeRecordSignalNames(QMap<uint64_t, int>& idSignalPositionMap, QtCSV::StringData& data) {
    QMap<uint64_t, shared_ptr<RecordElement>> recordElementSorted;
    for(const auto& recordElement: model->getRecordElements()) {
        if(recordElement->getIsSelected()) {
            recordElementSorted.insert(recordElement->getParseResult()->getAddress(), recordElement);
        }
    }

    int position = 1;
    QStringList signalNames;
    signalNames << "t[s]";
    for(const auto& recordElement : recordElementSorted.values()) {
        idSignalPositionMap.insert(recordElement->getParseResult()->getAddress(), position++);
        signalNames << QString::fromStdString(recordElement->getParseResult()->getName()) + "[" +QString::fromStdString(recordElement->getParseResult()->getSrcParserObj()->getPhyUnit()) + "]";
    }
    data.addRow(signalNames);
}

void Backend::addRecordEntryToRecordsWithEqualTimestamp(Record &record, QMap<int, Record> &recordsWithEqualTimestamp, const QMap<uint64_t, int> &idSignalPositionMap) {
    int position = idSignalPositionMap.find(record.getId()).value();
    record.setPosition(position);
    recordsWithEqualTimestamp.insert(position, record);
}

void Backend::writeRecordCsvFile(const QString& path, const QtCSV::StringData& data, const QStringList& header) {
    static const QString seperator = ";", textDelimeter = "";
    QString filename = QDateTime::currentDateTime().toString("'Record_'yyyy-MM-dd_hh-mm-ss'.csv'");
    QString filePath = path + "/" + filename;
    QtCSV::Writer::write(filePath, data, seperator, textDelimeter, QtCSV::Writer::REWRITE, header);
}

/***************************************************************
*  Slots
****************************************************************/
void Backend::slotParseProgress(int percent) {
    emit sigParseProgress(percent);
}

void Backend::slotConnectToClient() {
    emit sigConnectToClient();
}

void Backend::slotDisconnectClient() {
    emit sigDisconnectClient();
}

void Backend::slotRecordStart() {
    recordList.clear();
    recordStartTimestamp = QDateTime::currentMSecsSinceEpoch();
    emit sigRecordStart();
}

void Backend::slotRecordStop() {
    emit sigRecordStop();
    saveRecord(model->getFilesRecordPath());
}

void Backend::slotParseResultSelectedByUser(int parseResultIndex, bool isSelected) {
    shared_ptr<ParseResultBase> parseResult = model->getParseResults().at(parseResultIndex);
    parseResult->setIsSelectedByUser(!isSelected); //toggle model
    if(isSelected) {
        removeFromRecordElementsInModel(parseResult);
    } else {
        addToRecordElementsInModel(parseResult);
    }
}

void Backend::slotCalibration(int recordElementsIndex) {
    emit sigCalibration(recordElementsIndex);
}

void Backend::slotPrintMessage(QString msg, bool isError) {
    emit sigPrintMessage(msg, isError);
}

void Backend::slotVariableValueUpdate(uint64_t id, int64_t value) {

    for(auto& recordElement : model->getRecordElements()) {
        uint64_t addr = recordElement->getParseResult()->getAddress();
        if(id == addr) {
            if(isDataTypeUnSigned(recordElement->getParseResult()->getDataType())) { //convert to signed/unisgned
                value = (uint32_t) value; //unsigned
            } else {
                value = (int32_t) value; //signed
            }
            recordElement->setLatestValueIntAddHistory(value);
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            qint64 timeDiff = (currentTime - recordStartTimestamp);
            static const double SECOND = 1000.0;
            double secondsSinceRecordStart =  (double) timeDiff / SECOND;
            Record record(secondsSinceRecordStart, addr, value);
            recordList.append(record);

            if(recordElement->getParseResult()->getSrcMeasurement() != nullptr) {
                emit sigUpdateRecordMeasurement();
            } else {
                emit sigUpdateRecordCalibration();
            }
            break;
        }
    }
}

void Backend::slotStateChangeXcpTask(XcpTask::State newState) {
    xcpState = newState;
}



