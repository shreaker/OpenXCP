/**
*@file dialogDeviceEventsInsert.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief GUI dialog class to manage DAQ events.
*/

#include "dialogDeviceEventsInsert.h"
#include "ui_dialogDeviceEventsInsert.h"

DialogDeviceEventsInsert::DialogDeviceEventsInsert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDeviceEventsInsert) {
    ui->setupUi(this);
    ui->rateComboBox->setCurrentIndex(0);
}

DialogDeviceEventsInsert::~DialogDeviceEventsInsert() {
    delete ui;
}

void DialogDeviceEventsInsert::on_buttonBox_accepted() {
    name = ui->name->text();
    channel = ui->channel->text().toInt();
    QString rateString = ui->rateComboBox->currentText();
    if(rateString.compare("non cyclic") == 0) {
        rate = 0;
    } else {
        rate = rateString.toInt();
    }

}

void DialogDeviceEventsInsert::on_buttonBox_rejected() {

}

int DialogDeviceEventsInsert::getRate() const {
    return rate;
}

int DialogDeviceEventsInsert::getChannel() const {
    return channel;
}

QString DialogDeviceEventsInsert::getName() const {
    return name;
}
