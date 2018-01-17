/**
*@file main.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Main C++ file, main method.
*/

#include "controller.h"
#include <QApplication>

#include "backend.h"

#include <QSharedPointer>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QSharedPointer<Backend> backend = QSharedPointer<Backend>::create();

    Controller controller(backend);
    controller.show();

    /**************************************************************
     * Signal controller to slot backend
     * ***********************************************************/
    QObject::connect(&controller, SIGNAL(sigParseElfAndSource()), backend.data(), SLOT(parse()));
    QObject::connect(&controller, SIGNAL(sigExportToA2l()), backend.data(), SLOT(exportToA2l()));
    QObject::connect(&controller, SIGNAL(sigSaveProject()), backend.data(), SLOT(saveProject()));
    QObject::connect(&controller, SIGNAL(sigLoadProject()), backend.data(), SLOT(loadProject()));
    QObject::connect(&controller, SIGNAL(sigRemoveNonSourceCommentParseResults()), backend.data(), SLOT(removeNonSourceCommentParseResults()));
    QObject::connect(&controller, SIGNAL(sigConnectToClient()), backend.data(), SLOT(slotConnectToClient()));
    QObject::connect(&controller, SIGNAL(sigDisconnectClient()), backend.data(), SLOT(slotDisconnectClient()));
    QObject::connect(&controller, SIGNAL(sigRecordStart()), backend.data(), SLOT(slotRecordStart()));
    QObject::connect(&controller, SIGNAL(sigRecordStop()), backend.data(), SLOT(slotRecordStop()));
    QObject::connect(&controller, SIGNAL(sigParseResultSelectedByUser(int, bool)), backend.data(), SLOT(slotParseResultSelectedByUser(int, bool)));
    QObject::connect(&controller, SIGNAL(sigCalibration(int)), backend.data(), SLOT(slotCalibration(int)));

    /**************************************************************
     * Signal backend to slot controller
     * ***********************************************************/
    QObject::connect(backend.data(), SIGNAL(sigParseProgress(int)), &controller, SLOT(slotParseProgress(int)));
    QObject::connect(backend.data(), SIGNAL(sigLoadProjectCompleted(bool)), &controller, SLOT(slotLoadProjectCompleted(bool)));
    QObject::connect(backend.data(), SIGNAL(sigUpdateGuiFromModel()), &controller, SLOT(slotUpdateGuiFromModel()));
    QObject::connect(backend.data(), SIGNAL(sigUpdateRecordMeasurement()), &controller, SLOT(slotUpdateRecordMeasurement()));
    QObject::connect(backend.data(), SIGNAL(sigUpdateRecordCalibration()), &controller, SLOT(slotUpdateRecordCalibration()));
    QObject::connect(backend.data(), SIGNAL(sigPrintMessage(QString, bool)), &controller, SLOT(slotPrintMessage(QString, bool)));


    return a.exec();
}


