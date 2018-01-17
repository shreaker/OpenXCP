/**
*@file backend.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The Backend class is the core of the software architecture. It controls the program flow. The Backend is the so called business logic in the model-view-controller design pattern.
*/

#ifndef BACKEND_H
#define BACKEND_H

#include "model.h"
#include "parser/parserManager.h"
#include "xcp/xcptask.h"
#include <record/recordElement.h>
#include <record/record.h>

#include <memory>
#include <QObject>
#include <QSharedPointer>

#include "libs/qtcsv/include/qtcsv/stringdata.h"
#include "libs/qtcsv/include/qtcsv/writer.h"

class Backend : public QObject {

    Q_OBJECT

  public:
    Backend(QObject *parent = Q_NULLPTR);

    QSharedPointer<Model> getModel() const;
    void setModel(const QSharedPointer<Model> &value);

    XcpTask::State getXcpState() const;

  public slots:
    void parse();
    void saveProject();
    void loadProject();
    void exportToA2l();
    void removeNonSourceCommentParseResults();
    void slotConnectToClient();
    void slotDisconnectClient();
    void slotRecordStart();
    void slotRecordStop();
    void slotParseResultSelectedByUser(int parseResultIndex, bool isSelected);
    void slotVariableValueUpdate(uint64_t id, int64_t value);
    void slotStateChangeXcpTask(XcpTask::State newState);
    void slotCalibration(int recordElementsIndex);
    void slotPrintMessage(QString msg, bool isError);

  signals:
    void sigParseProgress(int percent);
    void sigLoadProjectCompleted(bool isElfFileModified);
    void sigUpdateGuiFromModel();
    void sigConnectToClient();
    void sigDisconnectClient();
    void sigRecordStart();
    void sigRecordStop();
    void sigCalibration(int recordElementsIndex);
    void sigUpdateRecordMeasurement();
    void sigUpdateRecordCalibration();
    void sigPrintMessage(QString msg, bool isError);


  private slots:
    void slotParseProgress(int percent);

  private:
    void updateModelWithParseResults(const std::unique_ptr<ParserManager>& parserManager);
    void addToRecordElementsInModel(const shared_ptr<ParseResultBase> parseResult);
    void removeFromRecordElementsInModel(const shared_ptr<ParseResultBase> parseResult);
    bool isDataTypeUnSigned(const string& dataType);
    /**
     * @brief fileChecksum
     * @param fileName
     * @return hash on success or empty QByteArray() on failure.
     */
    QByteArray fileChecksum(const QString &fileName);
    bool hasFileBeenModified(const QString& fileName,const QByteArray& fileHash);

    void saveRecord(const QString& filename);
    void writeRecordHeader(QStringList& header, QtCSV::StringData& data);
    void writeRecordSignalNames(QMap<uint64_t, int>& idSignalPositionMap, QtCSV::StringData& data);
    void addRecordEntryToRecordsWithEqualTimestamp(Record& record, QMap<int, Record>& recordsWithEqualTimestamp, const QMap<uint64_t, int>& idSignalPositionMap);
    void writeRecordCsvFile(const QString& path, const QtCSV::StringData& data, const QStringList& header);




    QSharedPointer<Model> model;
    XcpTask xcpTask;
    XcpTask::State xcpState;
    QList<Record> recordList;
    qint64 recordStartTimestamp;
};

#endif // BACKEND_H
