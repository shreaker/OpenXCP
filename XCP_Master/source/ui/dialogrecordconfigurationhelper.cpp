/**
*@file dialogrecordconfigurationhelper.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief GUI dialog helper class to set polling intervals or events to a signal/parameter.
*/

#include "dialogrecordconfigurationhelper.h"
#include "ui_dialogrecordconfigurationhelper.h"

DialogRecordConfigurationHelper::DialogRecordConfigurationHelper(const QVector<XcpEvent>& events, int ratePreset, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogRecordConfigurationHelper),
    ratePreset(ratePreset),
    rateUser(0) {
    ui->setupUi(this);
    addEventsToComboBox(events);
    ui->rateTextEdit->setText(QString::number(ratePreset));
}

DialogRecordConfigurationHelper::~DialogRecordConfigurationHelper() {
    delete ui;
}


void DialogRecordConfigurationHelper::on_buttonBox_accepted() {
    trigger = ui->triggerComboBox->currentText();
    if(isPollingMode(trigger)) {
        int rateTmp = ui->rateTextEdit->text().toInt();
        if(rateTmp < POLLING_RATE_MIN) {
            rateUser = POLLING_RATE_MIN;
        } else {
            rateUser = rateTmp;
        }
    }
}

int DialogRecordConfigurationHelper::getRate() const {
    return rateUser;
}

QString DialogRecordConfigurationHelper::getTrigger() const {
    return trigger;
}

void DialogRecordConfigurationHelper::on_triggerComboBox_currentTextChanged(const QString &comboText) {
    ui->rateTextEdit->setEnabled(isPollingMode(comboText));
}

void DialogRecordConfigurationHelper::addEventsToComboBox(const QVector<XcpEvent>& events) {
    ui->triggerComboBox->addItem("polling");
    for(const auto& event: events) {
        ui->triggerComboBox->addItem(event.getName());
    }
}

bool DialogRecordConfigurationHelper::isPollingMode(const QString &comboText) {
    return  (comboText.compare("polling") == 0);
}
