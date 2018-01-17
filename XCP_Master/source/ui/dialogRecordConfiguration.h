/**
*@file dialogRecordConfiguration.h
*@author Michael Wolf
*@date 27.11.2017
*@brief GUI dialog class to set polling intervals or events to a signal/parameter.
*/

#ifndef DIALOGSTARTCONFIGURATION_H
#define DIALOGSTARTCONFIGURATION_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QList>
#include <QVector>

#include <memory>

#include "xcpConfig/xcpEvent.h"
#include "record/recordElement.h"

namespace Ui {
class DialogRecordConfiguration;
}

class DialogRecordConfiguration : public QDialog {
    Q_OBJECT

  public:
    DialogRecordConfiguration(QList<std::shared_ptr<RecordElement>> recordElements,const QVector<XcpEvent>& events, QWidget *parent = 0);
    ~DialogRecordConfiguration();

  private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_table_itemDoubleClicked(QTableWidgetItem *item);

  private:
    Ui::DialogRecordConfiguration *ui;
    QList<std::shared_ptr<RecordElement>> recordElements;
    const QVector<XcpEvent> events;
    QComboBox comboBox;

    void updateTableView();
    void chooseTrigger();
    void setEditorTableItemSelected(int rowIndex);
    void openInputDialogTrigger(std::shared_ptr<RecordElement> recordElement);

};

#endif // DIALOGSTARTCONFIGURATION_H
