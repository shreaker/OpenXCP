/**
*@file controller.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The GUI is managed by the Controller class. The Controller also has direct access to the data in the Model to represent the data in the view.
*/


#ifndef MAINWINDOW_H
#define CONTROLLER_H

#include <QMainWindow>

#include "backend.h"

#include <QString>
#include <QStringList>
#include <string>

#include <QTableWidgetItem>


namespace Ui {
class MainWindow;
}

class Controller : public QMainWindow {
    Q_OBJECT

  public:
    explicit Controller(QSharedPointer<Backend> backend, QWidget *parent = Q_NULLPTR);
    ~Controller();

  public slots:
    void slotParseProgress(int percent);
    void slotLoadProjectCompleted(bool isElfFileModified);
    void slotUpdateGuiFromModel();
    void slotUpdateRecordMeasurement();
    void slotUpdateRecordCalibration();
    void slotPrintMessage(QString msg, bool isError);

  private slots:
    void on_filesProjectFileButton_clicked();
    void on_filesElfFileButton_clicked();
    void on_filesSourcePathButton_clicked();

    void on_editorParseElfAndSourceBtn_clicked();
    void on_editorParseElfBtn_clicked();
    void on_editorParseSourceBtn_clicked();

    void on_editorTable_itemDoubleClicked(QTableWidgetItem *item);

    void on_editorTable_itemPressed(QTableWidgetItem *item);

    void on_recordConnectBtn_clicked();

    void on_recordDisconnectBtn_clicked();

    void on_recordConfigBtn_clicked();

    void on_recordStartBtn_clicked();

    void on_recordStopBtn_clicked();

    void on_actionExport_to_A2L_triggered();

    void on_deviceTransportEthernetIpClientEdit_textChanged();

    void on_deviceTransportEthernetPortClientEdit_textChanged();

    void on_deviceTransportEthernetProtocolComboBox_currentIndexChanged(int index);

    void on_deviceXcpVersionComboBox_currentIndexChanged(int index);

    void on_deviceXcpEndiannessComboBox_currentIndexChanged(int index);

    void on_deviceXcpAdressGranularityComboBox_currentIndexChanged(int index);

    void on_actionNew_project_triggered();

    void on_actionLoad_project_triggered();

    void on_filesA2LFileButton_clicked();

    void on_deviceEventsInsertBtn_clicked();

    void on_deviceEventsDeleteBtn_clicked();

    void on_deviceXcpTimeoutEdit_textChanged();

    void on_deviceXcpMaxCtoEdit_textChanged();

    void on_deviceXcpMaxDtoEdit_textChanged();

    void on_deviceDaqComboBox_currentIndexChanged(int index);

    void on_actionSave_triggered();

    void on_editorOnlySourceCommentCheckBox_stateChanged(int checkState);

    void on_deviceTransportEthernetIpHostComboBox_currentIndexChanged(const QString &arg1);

    void on_recordCalibrationTable_itemDoubleClicked(QTableWidgetItem *item);

    void on_filesRecordPathButton_clicked();

    void on_actionAbout_OpenXCP_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionContext_Help_triggered();

  signals:
    void sigParseElfAndSource();
    void sigParseElf();
    void sigParseSource();
    void sigExportToA2l();
    void sigSaveProject();
    void sigLoadProject();
    void sigRemoveNonSourceCommentParseResults();
    void sigConnectToClient();
    void sigDisconnectClient();
    void sigRecordStart();
    void sigRecordStop();
    void sigParseResultSelectedByUser(int parseResultIndex, bool isSelected);
    void sigCalibration(int recordElementsIndex);

  private:
    void writeToLogWindow(std::string message, bool isError);
    void closeEvent (QCloseEvent *event) override;

    void updateEditorTableView();
    void setEditorTableItemSelected(int rowIndex);
    bool updateEditorVariableSettingsView(const shared_ptr<ParseResultBase>& var);
    void clearEditorVariableSettingsView();
    bool hasParseResultSourceComment(const shared_ptr<ParseResultBase>& var);

    void updateEventTableView();
    void updateGuiFromModel();
    void updateFilesFromModel();
    bool setRecentFilePath(const QString& newFilePath);

    void setComboBoxOptions();
    void setGuiInitalView();

    void selectProjectFile(bool isNewProject);
    void selectA2lFile();
    void saveProject();
    void loadProject();
    void exportToA2lFile();

    void updateRecordMeasurementTableView();
    void updateRecordCalibrationTableView();

  private:
    Ui::MainWindow *ui;
    QSharedPointer<Backend> backend;
    QSharedPointer<Model> model;

    QString recentFilePath;

    bool isGUISetupFinished = false;
};

#endif // CONTROLLER_H
