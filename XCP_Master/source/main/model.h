/**
*@file model.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The main data-storage class.
*/

#ifndef UIMODEL_H
#define UIMODEL_H

#include <QString>
#include <QVector>
#include <QList>
#include <QByteArray>
#include <memory>

#include "parser/parseResultVariable.h"
#include "parser/parseResultStruct.h"
#include "parser/parseResultArray.h"

#include "xcpConfig/ethernetconfig.h"
#include "xcpConfig/xcpHostConfig.h"
#include "xcpConfig/xcpClientConfig.h"

#include "record/recordElement.h"

using std::shared_ptr;

class Model {
  public:
    Model();

    enum class TRANSPORT_PROTOCOL {ETHERNET, USB};

    static const std::map<const TRANSPORT_PROTOCOL, const QString> getTransportProtocolsKeyValue();
    static const std::map<const QString, const TRANSPORT_PROTOCOL> getTransportProtocolsValueKey();

    QString getFilesProjectFile() const;
    void setFilesProjectFile(const QString& value);

    QString getFilesA2lFile() const;
    void setFilesA2lFile(const QString& value);

    QString getFilesSourcePath() const;
    void setFilesSourcePath(const QString& value);

    QString getFilesElfFile() const;
    void setFilesElfFile(const QString& value);

    QVector<shared_ptr<ParseResultBase> > getParseResults() const;
    void setParseResults(const QVector<shared_ptr<ParseResultBase>>& value);
    void addToParseResults(const shared_ptr<ParseResultBase>& value);
    bool removeFromParseResults(const shared_ptr<ParseResultBase>& value);
    void removeAllFromParseResults();

    QList<shared_ptr<RecordElement> > getRecordElements() const;
    void setRecordElements(const QList<shared_ptr<RecordElement>>& value);
    void addToRecordElements(const shared_ptr<ParseResultBase>& parseResult);
    bool removeFromRecordElements(const shared_ptr<ParseResultBase>& parseResult);
    void removeAllFromRecordElements();

    TRANSPORT_PROTOCOL getTransportProtocolSelected() const;
    void setTransportProtocolSelected(const TRANSPORT_PROTOCOL& value);


    shared_ptr<EthernetConfig> getEthernetConfig() const;
    void setEthernetConfig(const shared_ptr<EthernetConfig>& value);

    shared_ptr<XcpHostConfig> getXcpHostConfig() const;
    void setXcpHostConfig(const shared_ptr<XcpHostConfig>& value);

    shared_ptr<XcpClientConfig> getXcpClientConfig() const;
    void setXcpClientConfig(const shared_ptr<XcpClientConfig>& value);

    QByteArray getHashElfFile() const;
    void setHashElfFile(const QByteArray& value);

    QString getFilesRecordPath() const;
    void setFilesRecordPath(const QString &value);

  private:
    QString filesProjectFile;
    QString filesA2lFile;
    QString filesSourcePath;
    QString filesElfFile;
    QString filesRecordPath;

    QByteArray hashElfFile;

    TRANSPORT_PROTOCOL transportProtocolSelected;

    shared_ptr<EthernetConfig> ethernetConfig;
    shared_ptr<XcpHostConfig> xcpHostConfig;
    shared_ptr<XcpClientConfig> xcpClientConfig;

  private:
    QVector<shared_ptr<ParseResultBase>> parseResults;
    QList<shared_ptr<RecordElement>> recordElements;
};

#endif // UIMODEL_H
