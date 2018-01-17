/**
*@file dialogRecordConfiguration.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief GUI dialog class to set polling intervals or events to a signal/parameter.
*/

#include "dialogRecordConfiguration.h"
#include "ui_dialogRecordConfiguration.h"
#include "dialogrecordconfigurationhelper.h"

DialogRecordConfiguration::DialogRecordConfiguration(QList<std::shared_ptr<RecordElement>> recordElements,const QVector<XcpEvent>& events, QWidget *parent) :
    recordElements(recordElements),
    events(events),
    QDialog(parent),
    ui(new Ui::DialogRecordConfiguration) {
    ui->setupUi(this);

    updateTableView();
}

DialogRecordConfiguration::~DialogRecordConfiguration() {
    delete ui;
}

void DialogRecordConfiguration::updateTableView() {
    ui->table->setSortingEnabled(false);

    int rows = recordElements.size();
    ui->table->setRowCount(rows);
    int columns = ui->table->columnCount();

    int row = 0;
    for(const std::shared_ptr<RecordElement>& recordElement : recordElements) {
        std::shared_ptr<ParseResultBase> parseResult = recordElement->getParseResult();
        for (int column=0; column<columns; column++) {
            switch (column) {
            case 0:
                ui->table->setItem( row, column, new QTableWidgetItem(recordElement->getIsSelected()));
                break;
            case 1: {
                ui->table->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getName())));
                break;
            }
            case 2: {
                SourceParserObj* srcParserObj = parseResult->getSrcParserObj();
                if(srcParserObj != nullptr) {
                    QString objectType = QString::fromStdString(srcParserObj->getTypeString());
                    ui->table->setItem( row, column, new QTableWidgetItem(objectType));
                }
                break;
            }
            case 3: {
                QString triggerName;
                switch (recordElement->getTriggerMode()) {
                case RecordElement::TriggerMode::Polling:
                    triggerName = "polling";
                    break;
                case RecordElement::TriggerMode::Event:
                    triggerName = recordElement->getEvent().getName();
                    break;
                default:
                    break;
                }
                ui->table->setItem(row, column, new QTableWidgetItem(triggerName));
                break;
            }
            case 4:
                ui->table->setItem( row, column, new QTableWidgetItem(QString::number(recordElement->getPollingRateInMs())));
                break;
            default:
                break;
            }
        }
        if(recordElement->getIsSelected()) {
            setEditorTableItemSelected(row);
        }
        row++;
    }
}

void DialogRecordConfiguration::on_buttonBox_accepted() {

}

void DialogRecordConfiguration::on_buttonBox_rejected() {

}

void DialogRecordConfiguration::on_table_itemDoubleClicked(QTableWidgetItem *item) {
    int rowIndex = item->row();
    std::shared_ptr<RecordElement> recordElement = recordElements.at(rowIndex);

    bool isSelected = recordElement->getIsSelected();
    if(isSelected) { //toggle view
        ui->table->setItem(rowIndex, 0, new QTableWidgetItem(""));
    } else {
        setEditorTableItemSelected(rowIndex);
        openInputDialogTrigger(recordElement);
    }
    recordElement->setIsSelected(!isSelected); //toggle model
    updateTableView();
}



void DialogRecordConfiguration::setEditorTableItemSelected(int rowIndex) {
    const static QIcon SELECT_ICON = QIcon(":/resource/icons_gnome/24x24/status/starred.png");
    ui->table->setItem(rowIndex, 0, new QTableWidgetItem(SELECT_ICON, "select", 0));
}

void DialogRecordConfiguration::openInputDialogTrigger(std::shared_ptr<RecordElement> recordElement) {
    DialogRecordConfigurationHelper dialog(events, recordElement->getPollingRateInMs());
    int dialogCode = dialog.exec();
    if(dialogCode == QDialog::Accepted) {
        QString triggerName = dialog.getTrigger();
        if(triggerName.compare("polling") == 0) {
            recordElement->setTriggerMode(RecordElement::TriggerMode::Polling);
            recordElement->setPollingRateInMs(dialog.getRate());
        } else {
            recordElement->setTriggerMode(RecordElement::TriggerMode::Event);
            for(const auto& event : events) {
                if(triggerName.compare(event.getName()) == 0) {
                    recordElement->setEvent(event);
                    break;
                }
            }
        }
    }
}
