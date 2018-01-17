/**
*@file controller.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The GUI is managed by the Controller class. The Controller also has direct access to the data in the Model to represent the data in the view.
*/

#include "controller.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QtWidgets>
#include <QHostAddress>
#include <QNetworkInterface>
#include <map>
#include <QCloseEvent>

#include "ui/dialogDeviceEventsInsert.h"
#include "ui/dialogRecordConfiguration.h"

#include "parser/sourceparsemeasurement.h"
#include "parser/sourceparsecharacteristic.h"

void checkCheckBox(QCheckBox* checkBox, bool isChecked);

Controller::Controller(QSharedPointer<Backend> backend, QWidget *parent) :
    backend(backend),
    model(backend->getModel()),
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setGuiInitalView();

    setComboBoxOptions();

    ui->editorProgressBar->hide();
    ui->editorOnlySourceCommentCheckBox->setDisabled(true);
    ui->editorOnlySourceCommentCheckBox->setChecked(false);
    checkCheckBox(ui->editorPhysicalDiscreteCheckBox, false);
    checkCheckBox(ui->editorGeneralWriteableCheckBox, false);

    updateGuiFromModel();

    recentFilePath = "C:\\";

    isGUISetupFinished = true;
}

Controller::~Controller() {
    delete ui;
}

/**************************************************************
 *  Override
 * ***********************************************************/
void Controller::closeEvent (QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "OpenXcp",
                                         tr("Are you sure?\n"),
                                         QMessageBox::Cancel | QMessageBox::Save | QMessageBox::Yes);
    switch (resBtn) {
    case QMessageBox::Yes:
        event->accept();
        break;
    case QMessageBox::Save:
        event->ignore();
        emit on_actionSave_triggered();
        break;
    case QMessageBox::Cancel:
        event->ignore();
        break;
    default:
        event->ignore();
        break;
    }
}

/**************************************************************
 *  Slots
 * ***********************************************************/
void Controller::on_actionContext_Help_triggered() {
    QFile HelpFile(":/resource/OpenXCP_manual.pdf");
    HelpFile.copy(qApp->applicationDirPath().append("/OpenXCP_manual.pdf"));
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath().append("/OpenXCP_manual.pdf")));
}

void Controller::on_actionAbout_OpenXCP_triggered() {
    static const QString TEXT = "Author: "
                                "\nMichael Wolf, 2018"
                                "\n\nLicense:"
                                "\nGPLv3"
                                "\n\nLibraries:"
                                "\nqtcsv"
                                "\nhttps://github.com/iamantony/qtcsv, MIT "
                                "\nlibelfin"
                                "\nhttps://github.com/aclements/libelfin, MIT "
                                "\n\nIcons:"
                                "\nGnome icon theme 3.12"
                                "\nhttps://www.gnome.org/, GPLv2"
                                "\n\nSee LICENSE file for a copy of used licenses.";
    QMessageBox::about(this, "OpenXCP", TEXT);
}

void Controller::on_actionAbout_Qt_triggered() {
    QMessageBox::aboutQt(this, "OpenXCP");
}

void Controller::on_filesProjectFileButton_clicked() {
    selectProjectFile(false);
}

void Controller::on_actionLoad_project_triggered() {
    selectProjectFile(false);
    loadProject();
}

void Controller::on_actionNew_project_triggered() {
    selectProjectFile(true);
}

void Controller::on_actionSave_triggered() {
    if(model->getFilesProjectFile().size() == 0) {
        selectProjectFile(true);
    }
    saveProject();
}

void Controller::on_filesElfFileButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
                       tr("Open GNU 32 bit ELF file"),
                       recentFilePath,
                       tr("ELF file (*.elf)"));
    if (!fileName.isEmpty()) {
        model->setFilesElfFile(fileName);
        ui->filesElfFileEdit->setText(fileName);
        recentFilePath = fileName;
    }
}

void Controller::on_filesRecordPathButton_clicked() {
    static const QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Choose record directory"),
                       recentFilePath,
                       options);
    if (!fileName.isEmpty()) {
        model->setFilesRecordPath(fileName);
        ui->filesRecordPathEdit->setText(fileName);
        recentFilePath = fileName;
    }
}

void Controller::on_filesSourcePathButton_clicked() {
    static const QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                        tr("Choose ECU source directory"),
                        recentFilePath,
                        options);
    if (!directory.isEmpty()) {
        model->setFilesSourcePath(directory);
        ui->filesSourcePathEdit->setText(directory);
        recentFilePath = directory;
    }
}

void Controller::on_filesA2LFileButton_clicked() {
    selectA2lFile();
}

void Controller::on_actionExport_to_A2L_triggered() {
    if(model->getFilesA2lFile().size() == 0) {
        selectA2lFile();
    }
    exportToA2lFile();
}

void Controller::on_editorParseElfAndSourceBtn_clicked() {
    if((model->getFilesElfFile().size() > 0) && (model->getFilesSourcePath().size() > 0)) {
        ui->editorProgressBar->show();
        ui->editorOnlySourceCommentCheckBox->setChecked(false);
        ui->editorOnlySourceCommentCheckBox->setDisabled(false);
        emit sigParseElfAndSource();
    } else {
        ui->editorProgressBar->hide();
        writeToLogWindow("Set ELF file and source path before.", true);
    }
}

void Controller::on_editorParseElfBtn_clicked() {
    if(model->getFilesElfFile().size() > 0) {
        ui->editorProgressBar->show();
        emit sigParseElf();
    } else {
        ui->editorProgressBar->hide();
        writeToLogWindow("Set ELF file before.", true);
    }
}

void Controller::on_editorParseSourceBtn_clicked() {
    if(model->getFilesSourcePath().size() > 0) {
        ui->editorProgressBar->show();
        emit sigParseSource();
    } else {
        ui->editorProgressBar->hide();
        writeToLogWindow("Set source path before.", true);
    }
}


void Controller::on_editorTable_itemDoubleClicked(QTableWidgetItem *item) {
    int rowIndex = item->row();
    shared_ptr<ParseResultBase> var = model->getParseResults().at(rowIndex);

    bool isSelected = var->getIsSelectedByUser();
    if(isSelected) { //toggle view
        ui->editorTable->setItem(rowIndex, 0, new QTableWidgetItem(""));
    } else {
        setEditorTableItemSelected(rowIndex);
    }

    emit sigParseResultSelectedByUser(rowIndex, isSelected); //toggle model

    //update view
    updateRecordCalibrationTableView();
    updateRecordMeasurementTableView();
}

void Controller::on_editorTable_itemPressed(QTableWidgetItem *item) {
    shared_ptr<ParseResultBase> var = model->getParseResults().at(item->row());
    clearEditorVariableSettingsView();
    updateEditorVariableSettingsView(var);
}

void Controller::on_recordConnectBtn_clicked() {
    emit sigConnectToClient();
    ui->recordStartBtn->setEnabled(true);
    ui->recordDisconnectBtn->setEnabled(true);
    ui->recordConnectBtn->setEnabled(false);
    ui->recordConfigBtn->setEnabled(false);
    ui->mainDeviceTab->setEnabled(false);
    ui->mainEditorTab->setEnabled(false);
}

void Controller::on_recordDisconnectBtn_clicked() {
    emit sigDisconnectClient();
    ui->recordConnectBtn->setEnabled(true);
    ui->recordConfigBtn->setEnabled(true);
    ui->mainDeviceTab->setEnabled(true);
    ui->mainEditorTab->setEnabled(true);
    ui->recordDisconnectBtn->setEnabled(false);
    ui->recordStartBtn->setEnabled(false);
    ui->recordStopBtn->setEnabled(false);
}

void Controller::on_recordConfigBtn_clicked() {
    DialogRecordConfiguration dialog(model->getRecordElements(), model->getXcpHostConfig()->getEvents());
    int dialogCode = dialog.exec();
    if(dialogCode == QDialog::Accepted) {
        updateRecordMeasurementTableView();
        updateRecordCalibrationTableView();
        ui->recordConnectBtn->setEnabled(true);
    }
}

void Controller::on_recordStartBtn_clicked() {
    emit sigRecordStart();
    ui->recordStopBtn->setEnabled(true);
    ui->recordStartBtn->setEnabled(false);
    ui->recordDisconnectBtn->setEnabled(false);
}

void Controller::on_recordStopBtn_clicked() {
    emit sigRecordStop();
    ui->recordStartBtn->setEnabled(true);
    ui->recordDisconnectBtn->setEnabled(true);
    ui->recordStopBtn->setEnabled(false);
}

void Controller::on_recordCalibrationTable_itemDoubleClicked(QTableWidgetItem *item) {
    if((backend->getXcpState() == XcpTask::State::Disconnected) || (backend->getXcpState() == XcpTask::State::Error)) {
        writeToLogWindow("Connect to slave before calibrating.", false);
        return;
    }

    int row = item->row();
    int recordElementsIndex = ui->recordCalibrationTable->item(row, 0)->text().toInt();
    try {
        shared_ptr<RecordElement> recordElement = model->getRecordElements().at(recordElementsIndex);
        shared_ptr<ParseResultBase> parseResult = recordElement->getParseResult();
        if(recordElement->getIsFloat()) {
            bool ok;
            float valueNew = (float) QInputDialog::getDouble(this, "Calibration float" + QString::fromStdString(parseResult->getName()),
                             "Set Value for " + QString::fromStdString(parseResult->getName()), recordElement->getLatestValueFloat(),
                             parseResult->getSrcParserObj()->getLowerLimit(), parseResult->getSrcParserObj()->getUpperLimit(), 6, &ok);
            if (ok) {
                recordElement->setValueToSetFloat(valueNew);
                emit sigCalibration(recordElementsIndex);
            }

        } else {
            bool ok;
            int valueNew = QInputDialog::getInt(this, "Calibration integer",
                                                "Set Value for " + QString::fromStdString(parseResult->getName()), recordElement->getLatestValueInt(),
                                                parseResult->getSrcParserObj()->getLowerLimit(), parseResult->getSrcParserObj()->getUpperLimit(),
                                                (int) parseResult->getSrcCharateristic()->getStepSize(), &ok);
            if (ok) {
                recordElement->setValueToSetInt(valueNew);
                emit sigCalibration(recordElementsIndex);
            }
        }
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in on_recordCalibrationTable_itemDoubleClicked(): " << oor.what() << std::endl;
    }
}

void Controller::on_deviceTransportEthernetIpHostComboBox_currentIndexChanged(const QString &ipHost) {
    bool isValid = model->getEthernetConfig()->setIpHost(ipHost);
    if(!isValid) {
        writeToLogWindow("Ip not valid.", true);
    }
}

void Controller::on_deviceTransportEthernetIpClientEdit_textChanged() {
    bool isValid = model->getEthernetConfig()->setIpClient(ui->deviceTransportEthernetIpClientEdit->text());
    if(!isValid) {
        writeToLogWindow("Ip not valid.", true);
    }
}

void Controller::on_deviceTransportEthernetPortClientEdit_textChanged() {
    int port = ui->deviceTransportEthernetPortClientEdit->text().toInt();
    model->getEthernetConfig()->setPortClient(port);
}

void Controller::on_deviceTransportEthernetProtocolComboBox_currentIndexChanged(int index) {
    if(isGUISetupFinished) {
        EthernetConfig::PROTOCOL selection = static_cast<EthernetConfig::PROTOCOL>(index);
        model->getEthernetConfig()->setProtocol(selection);
    }
}
void Controller::on_deviceXcpVersionComboBox_currentIndexChanged(int index) {
    if(isGUISetupFinished) {
        XcpOptions::VERSION selection = static_cast<XcpOptions::VERSION>(index);
        model->getXcpHostConfig()->setVersion(selection);
    }
}

void Controller::on_deviceXcpEndiannessComboBox_currentIndexChanged(int index) {
    if(isGUISetupFinished) {
        XcpOptions::ENDIAN selection = static_cast<XcpOptions::ENDIAN>(index);
        model->getXcpHostConfig()->setEndian(selection);
    }
}

void Controller::on_deviceXcpAdressGranularityComboBox_currentIndexChanged(int index) {
    if(isGUISetupFinished) {
        XcpOptions::ADDRESS_GRANULARITY selection = static_cast<XcpOptions::ADDRESS_GRANULARITY>(index);
        model->getXcpHostConfig()->setAddrGranularity(selection);
    }
}

void Controller::on_deviceDaqComboBox_currentIndexChanged(int index) {
    if(isGUISetupFinished) {
        XcpOptions::DAQ_MODE selection = static_cast<XcpOptions::DAQ_MODE>(index);
        model->getXcpHostConfig()->setDaqMode(selection);
    }
}

void Controller::on_deviceXcpTimeoutEdit_textChanged() {
    int timeout = ui->deviceXcpTimeoutEdit->text().toInt();
    model->getXcpHostConfig()->setTimeout(timeout);
}

void Controller::on_deviceXcpMaxCtoEdit_textChanged() {
    int maxCto = ui->deviceXcpMaxCtoEdit->text().toInt();
    model->getXcpHostConfig()->setMaxCto(maxCto);
}

void Controller::on_deviceXcpMaxDtoEdit_textChanged() {
    int maxDto = ui->deviceXcpMaxDtoEdit->text().toInt();
    model->getXcpHostConfig()->setMaxDto(maxDto);
}

void Controller::on_deviceEventsInsertBtn_clicked() {
    DialogDeviceEventsInsert dialog;
    int dialogCode = dialog.exec();
    if(dialogCode == QDialog::Accepted) {
        XcpEvent event = XcpEvent(dialog.getName(), dialog.getChannel(), dialog.getRate());
        model->getXcpHostConfig()->addEvent(event);
        updateEventTableView();
    }
}

void Controller::on_deviceEventsDeleteBtn_clicked() {
    QStringList items;
    for(const auto& event : model->getXcpHostConfig()->getEvents()) {
        items.append(event.getName());
    }
    bool ok;
    QString name = QInputDialog::getItem(this, "Delete event",
                                         "Event:", items, 0, false, &ok);
    if (ok && !name.isEmpty()) {
        model->getXcpHostConfig()->removeEvent(name);
        updateEventTableView();
    }
}

void Controller::on_editorOnlySourceCommentCheckBox_stateChanged(int checkState) {
    if(checkState == Qt::Checked) {
        emit sigRemoveNonSourceCommentParseResults();
        ui->editorOnlySourceCommentCheckBox->setDisabled(true);
    }
}

void Controller::slotParseProgress(int percent) {
    ui->editorProgressBar->setValue(percent);
    if(percent >= 100) {
        updateEditorTableView();
    }
}

void Controller::slotLoadProjectCompleted(bool isElfFileModified) {
    updateGuiFromModel();
    if(isElfFileModified) {
        writeToLogWindow("ELF file has been modified. New parsing required.", true);
    }
    updateEditorTableView();
    setRecentFilePath(model->getFilesProjectFile());
    writeToLogWindow("OpenXCP project loaded from: " + model->getFilesProjectFile().toStdString(), false);
}

void Controller::slotUpdateGuiFromModel() {
    updateGuiFromModel();
    updateEditorTableView();
    updateRecordMeasurementTableView();
    updateRecordCalibrationTableView();
}

void Controller::slotUpdateRecordMeasurement() {
    updateRecordMeasurementTableView();
}

void Controller::slotUpdateRecordCalibration() {
    updateRecordCalibrationTableView();
}

void Controller::slotPrintMessage(QString msg, bool isError) {
    writeToLogWindow(msg.toStdString(), isError);
}
/**************************************************************
*  Utility
* ***********************************************************/
void Controller::writeToLogWindow(std::string message, bool isError) {
    ui->infoLogEdit->append("");
    if(isError) {
        ui->infoLogEdit->insertHtml(R"(<font color="red">)" + QString::fromStdString(message) + R"(</font>)");
    } else {
        ui->infoLogEdit->insertHtml(R"(<font color="black">)" + QString::fromStdString(message) + R"(</font>)");
    }
}

void Controller::setGuiInitalView() {
    ui->mainTab->setCurrentIndex(1);
    ui->deviceTab->setCurrentIndex(0);
}

void Controller::selectProjectFile(bool isNewProject) {
    QString fileName;
    if(isNewProject) {
        fileName = QFileDialog::getSaveFileName(this, tr("New OpenXCP project file"), recentFilePath, tr("OpenXCP project file (*.json)"));
    } else {
        fileName = QFileDialog::getOpenFileName(this, tr("Load OpenXCP project file"), recentFilePath, tr("OpenXCP project file (*.json)"));
    }
    if (!fileName.isEmpty()) {
        model->setFilesProjectFile(fileName);
        ui->filesProjectFileEdit->setText(fileName);
        recentFilePath = fileName;
    }
}

void Controller::selectA2lFile() {
    QString fileName = QFileDialog::getSaveFileName(this,
                       tr("Export A2l file"),
                       recentFilePath,
                       tr("A2l file (*.a2l)"));
    if (!fileName.isEmpty()) {
        model->setFilesA2lFile(fileName);
        ui->filesA2LFileEdit->setText(fileName);
        recentFilePath = fileName;
    }
}

void Controller::loadProject() {
    if(model->getFilesProjectFile().size() != 0) {
        emit sigLoadProject();
    } else {
        writeToLogWindow("No Project file location selected.", true);
    }
}

void Controller::saveProject() {
    if(model->getFilesProjectFile().size() != 0) {
        emit sigSaveProject();
        writeToLogWindow("OpenXCP project exported to: " + model->getFilesProjectFile().toStdString(), false);
    } else {
        writeToLogWindow("No Project file location selected.", true);
    }
}

void Controller::exportToA2lFile() {
    if(model->getFilesA2lFile().size() != 0) {
        emit sigExportToA2l();
        writeToLogWindow("A2l exported to: " + model->getFilesA2lFile().toStdString(), false);
    } else {
        writeToLogWindow("No A2l file location selected.", true);
    }
}

void Controller::updateRecordMeasurementTableView() {
    QList<shared_ptr<RecordElement>> recordElements = model->getRecordElements();

    ui->recordMeasurementTable->clearContents();

    int rows = recordElements.size();
    ui->recordMeasurementTable->setRowCount(rows);
    int columns = ui->recordMeasurementTable->columnCount();

    int row = 0;
    for(int i = 0; i < recordElements.size(); i++) {
        shared_ptr<ParseResultBase> parseResult = recordElements[i]->getParseResult();
        if(recordElements[i]->getIsSelected() && (parseResult->getSrcMeasurement() != nullptr)) {
            for (int column=0; column<columns; column++) {
                switch (column) {
                case 0:
                    ui->recordMeasurementTable->setItem( row, column, new QTableWidgetItem(QString::number(i)));
                    break;
                case 1:
                    ui->recordMeasurementTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getName())));
                    break;
                case 2:
                    ui->recordMeasurementTable->setItem( row, column, new QTableWidgetItem(QString::number(recordElements[i]->getLatestValueInt())));
                    break;
                case 3:
                    ui->recordMeasurementTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getSrcParserObj()->getPhyUnit())));
                    break;
                case 4:
                    ui->recordMeasurementTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getSrcParserObj()->getComment())));
                    break;
                default:
                    break;
                }
            }
            row++;
        }
    }
}

void Controller::updateRecordCalibrationTableView() {
    QList<shared_ptr<RecordElement>> recordElements = model->getRecordElements();

    ui->recordCalibrationTable->clearContents();

    int rows = recordElements.size();
    ui->recordCalibrationTable->setRowCount(rows);
    int columns = ui->recordCalibrationTable->columnCount();

    int row = 0;
    for(int i = 0; i < recordElements.size(); i++) {
        shared_ptr<ParseResultBase> parseResult = recordElements[i]->getParseResult();
        if(recordElements[i]->getIsSelected() && (parseResult->getSrcCharateristic() != nullptr)) {
            for (int column=0; column<columns; column++) {
                switch (column) {
                case 0:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::number(i)));
                    break;
                case 1:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getName())));
                    break;
                case 2:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::number(recordElements[i]->getLatestValueInt())));
                    break;
                case 3:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getSrcParserObj()->getPhyUnit())));
                    break;
                case 4:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(parseResult->getSrcParserObj()->getComment())));
                    break;
                case 5:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::number(parseResult->getSrcParserObj()->getLowerLimit())));
                    break;
                case 6:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::number(parseResult->getSrcParserObj()->getUpperLimit())));
                    break;
                case 7:
                    ui->recordCalibrationTable->setItem( row, column, new QTableWidgetItem(QString::number(parseResult->getSrcCharateristic()->getStepSize())));
                    break;
                default:
                    break;
                }
            }
            row++;
        }
    }
}

void Controller::updateGuiFromModel() {
    updateFilesFromModel();

    shared_ptr<XcpHostConfig> xcpConfig = model->getXcpHostConfig();
    try {
        QString version = XcpOptions::getVersionsKeyValue().at(xcpConfig->getVersion());
        ui->deviceXcpVersionComboBox->setCurrentText(version);
        QString endian = XcpOptions::getEndianTypesKeyValue().at(xcpConfig->getEndian());
        ui->deviceXcpEndiannessComboBox->setCurrentText(endian);
        QString addrGranularity = XcpOptions::getAddrGranularitiesKeyValue().at(xcpConfig->getAddrGranularity());
        ui->deviceXcpAdressGranularityComboBox->setCurrentText(addrGranularity);
        QString daqMode = XcpOptions::getDaqModesKeyValue().at(xcpConfig->getDaqMode());
        ui->deviceDaqComboBox->setCurrentText(daqMode);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in updateGuiFromModel(): " << oor.what() << std::endl;
    }
    ui->deviceXcpTimeoutEdit->setText(QString::number(xcpConfig->getTimeout()));
    ui->deviceXcpMaxCtoEdit->setText(QString::number(xcpConfig->getMaxCto()));
    ui->deviceXcpMaxDtoEdit->setText(QString::number(xcpConfig->getMaxDto()));
    updateEventTableView();

    switch (model->getTransportProtocolSelected()) {
    case Model::TRANSPORT_PROTOCOL::ETHERNET: {
        ui->deviceTransportEhernerRadioBtn->setChecked(true);
        shared_ptr<EthernetConfig> ethernetConfig = model->getEthernetConfig();
        ui->deviceTransportEthernetIpHostComboBox->setCurrentText(ethernetConfig->getIpHost().toString());
        ui->deviceTransportEthernetIpClientEdit->setText(ethernetConfig->getIpClient().toString());
        ui->deviceTransportEthernetPortClientEdit->setText(QString::number(ethernetConfig->getPortClient()));
        try {
            QString protocol = EthernetConfig::getProtocolTypes().at(ethernetConfig->getProtocol());
            ui->deviceTransportEthernetProtocolComboBox->setCurrentText(protocol);
        } catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error in getTransportProtocolSelected(): " << oor.what() << std::endl;
        }
        break;
    }
    case Model::TRANSPORT_PROTOCOL::USB: {
        ui->deviceTransportUsbRadioBtn->setChecked(true);
        break;
    }
    default:
        break;
    }
}

void Controller::updateFilesFromModel() {
    QString path = model->getFilesProjectFile();
    ui->filesProjectFileEdit->setText(path);
    path = model->getFilesElfFile();
    ui->filesElfFileEdit->setText(path);
    path = model->getFilesSourcePath();
    ui->filesSourcePathEdit->setText(path);
    path = model->getFilesA2lFile();
    ui->filesA2LFileEdit->setText(path);
    path = model->getFilesRecordPath();
    ui->filesRecordPathEdit->setText(path);
}

bool Controller::setRecentFilePath(const QString &newFilePath) {
    if(newFilePath.size() != 0) {
        QFileInfo projectFileInfo = QFileInfo(model->getFilesProjectFile());
        recentFilePath = projectFileInfo.path();
        return true;
    } else {
        return false;
    }
}

void Controller::updateEventTableView() {
    QVector<XcpEvent> xcpEvents = model->getXcpHostConfig()->getEvents();

    ui->deviceEventsTable->setSortingEnabled(false);
    int columns = ui->deviceEventsTable->columnCount();
    int rows = xcpEvents.size();
    ui->deviceEventsTable->setRowCount(rows);

    int row = 0;
    for(const auto& event : xcpEvents) {
        for(int column = 0; column < columns; column++) {
            switch (column) {
            case 0:
                ui->deviceEventsTable->setItem(row, column, new QTableWidgetItem(event.getName()));
                break;
            case 1:
                ui->deviceEventsTable->setItem(row, column, new QTableWidgetItem(QString::number(event.getChannel())));
                break;
            case 2:
                if(event.getRateInMs() == 0) {
                    ui->deviceEventsTable->setItem(row, column, new QTableWidgetItem("non cyclic"));
                } else {
                    ui->deviceEventsTable->setItem(row, column, new QTableWidgetItem(QString::number(event.getRateInMs())));
                }
                break;
            default:
                break;
            }
        }
        row++;
    }
}

void Controller::updateEditorTableView() {
    QVector<shared_ptr<ParseResultBase>> parseResults = model->getParseResults();

    int rows = parseResults.size();
    ui->editorTable->setRowCount(rows);
    int columns = ui->editorTable->columnCount();

    int row = 0;
    for(const shared_ptr<ParseResultBase>& var : parseResults) {
        for (int column=0; column<columns; column++) {
            switch (column) {
            case 0:
                ui->editorTable->setItem( row, column, new QTableWidgetItem(var->getIsSelectedByUser()));
                break;
            case 1:
                ui->editorTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(var->getDataType())));
                break;
            case 2:
                ui->editorTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(var->getName())));
                break;
            case 3:
                ui->editorTable->setItem( row, column, new QTableWidgetItem(QString::number(var->getAddress(), 16).toUpper()));
                break;
            case 4: {
                QString comment = "";
                if(hasParseResultSourceComment(var)) {
                    comment = QString::fromStdString(var->getSrcParserObj()->getComment());

                }
                ui->editorTable->setItem( row, column, new QTableWidgetItem(comment));
                break;
            }
            case 5:
                ui->editorTable->setItem( row, column, new QTableWidgetItem(QString::fromStdString(var->getAbstractDataType())));
                break;
            default:
                break;
            }
        }
        if(var->getIsSelectedByUser()) {
            setEditorTableItemSelected(row);
        }
        row++;
    }
}

void Controller::setEditorTableItemSelected(int rowIndex) {
    const static QIcon SELECT_ICON = QIcon(":/resource/icons_gnome/24x24/status/starred.png");
    ui->editorTable->setItem(rowIndex, 0, new QTableWidgetItem(SELECT_ICON, "select", 0));
}

bool Controller::updateEditorVariableSettingsView(const shared_ptr<ParseResultBase>& var) {

    QString name = QString::fromStdString(var->getName());
    QString address = QString::number(var->getAddress(), 16).toUpper();
    QString dataType = QString::fromStdString(var->getDataType());
    QString abstractDataType = QString::fromStdString(var->getAbstractDataType());
    QString size = QString::number(var->getSize());
    ui->editorGeneralNameEdit->setText(name);
    ui->editorAddressAddressEdit->setText(address);
    ui->editorTypeDatatypeComboBox->addItem(dataType);

    SourceParserObj* srcParserObj = var->getSrcParserObj();
    if(srcParserObj == nullptr) {
        return false;
    } else {
        QString comment = QString::fromStdString(srcParserObj->getComment());
        QString type = QString::fromStdString(srcParserObj->getTypeString());
        QString lowerLimit = QString::number(srcParserObj->getLowerLimit());
        QString upperLimit = QString::number(srcParserObj->getUpperLimit());
        QString phyUnit = QString::fromStdString(srcParserObj->getPhyUnit());
        ui->editorGeneralCommentEdit->setText(comment);
        ui->editorPhysicalMinRangeEdit->setText(lowerLimit);
        ui->editorPhysicalMaxRangeEdit->setText(upperLimit);
        ui->editorPhysicalUnitEdit->setText(phyUnit);
        ui->editorGeneralObjecttypeComboBox->addItem(type);

        shared_ptr<SourceParseMeasurement> srcMeasurement = var->getSrcMeasurement();
        shared_ptr<SourceParseCharacteristic> srcCharacteristic = var->getSrcCharateristic();
        if(srcMeasurement != nullptr) {
            QString conversion = QString::fromStdString(srcMeasurement->getConversionFunction());
            bool isDiscrete =  srcMeasurement->getIsDiscrete();
            bool isWriteable = srcMeasurement->getIsWriteable();
            QString maxRefreshRate = QString::number(srcMeasurement->getMaxRefreshRate());
            ui->editorPhysicalConversionComboBox->addItem(conversion);
            checkCheckBox(ui->editorPhysicalDiscreteCheckBox,isDiscrete);
            checkCheckBox(ui->editorGeneralWriteableCheckBox,isWriteable);
            ui->editorPhysicalDiscreteCheckBox->setEnabled(true);
            ui->editorGeneralWriteableCheckBox->setEnabled(true);
            ui->editorPhysicalConversionComboBox->setEnabled(true);
        } else if(srcCharacteristic != nullptr) {
            QString stepSize = QString::number(srcCharacteristic->getStepSize());
            ui->editorPhysicalDiscreteCheckBox->setEnabled(false);
            ui->editorGeneralWriteableCheckBox->setEnabled(false);
            ui->editorPhysicalConversionComboBox->setEnabled(false);
        }
    }
    return true;
}

void Controller::clearEditorVariableSettingsView() {
    ui->editorGeneralNameEdit->setText("");
    checkCheckBox(ui->editorGeneralWriteableCheckBox, false);
    ui->editorGeneralObjecttypeComboBox->removeItem(0);
    ui->editorGeneralCommentEdit->setText("");

    ui->editorAddressAddressEdit->setText("");
    ui->editorTypeDatatypeComboBox->removeItem(0);

    ui->editorPhysicalMinRangeEdit->setText("");
    ui->editorPhysicalMaxRangeEdit->setText("");
    ui->editorPhysicalUnitEdit->setText("");
    ui->editorPhysicalConversionComboBox->removeItem(0);
    checkCheckBox(ui->editorPhysicalDiscreteCheckBox, false);

}

bool Controller::hasParseResultSourceComment(const shared_ptr<ParseResultBase> &var) {
    return (var->getSrcParserObj() != nullptr);
}

void checkCheckBox(QCheckBox* checkBox, bool isChecked) {
    checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    checkBox->setFocusPolicy(Qt::StrongFocus);
    checkBox->setChecked(isChecked);
    checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    checkBox->setFocusPolicy(Qt::NoFocus);
}

void Controller::setComboBoxOptions() {
    QList<QHostAddress> allIps = QNetworkInterface::allAddresses();
    for(const auto& ip : allIps) {
        ui->deviceTransportEthernetIpHostComboBox->addItem(ip.toString());
        ui->deviceTransportEthernetIpHostComboBox->setCurrentText("127.0.0.1");
    }

    for(const auto& protocol : EthernetConfig::getProtocolTypes()) {
        ui->deviceTransportEthernetProtocolComboBox->addItem(protocol.second, protocol.first);
    }
    for(const auto& version : XcpOptions::getVersionsKeyValue()) {
        ui->deviceXcpVersionComboBox->addItem(version.second, version.first);
    }
    for(const auto& endian : XcpOptions::getEndianTypesKeyValue()) {
        ui->deviceXcpEndiannessComboBox->addItem(endian.second, endian.first);
    }
    for(const auto& addrGranularity : XcpOptions::getAddrGranularitiesKeyValue()) {
        ui->deviceXcpAdressGranularityComboBox->addItem(addrGranularity.second, addrGranularity.first);
    }
    for(const auto& daqMode : XcpOptions::getDaqModesKeyValue()) {
        ui->deviceDaqComboBox->addItem(daqMode.second, daqMode.first);
    }
}



/**************************************************************
 *  Getter / Setter
 * ***********************************************************/






















































