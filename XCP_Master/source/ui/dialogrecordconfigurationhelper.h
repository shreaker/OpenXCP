/**
*@file dialogrecordconfigurationhelper.h
*@author Michael Wolf
*@date 27.11.2017
*@brief GUI dialog helper class to set polling intervals or events to a signal/parameter.
*/

#ifndef DIALOGRECORDCONFIGURATIONHELPER_H
#define DIALOGRECORDCONFIGURATIONHELPER_H

#include <QDialog>
#include <QString>

#include "xcpConfig/xcpEvent.h"

namespace Ui {
class dialogRecordConfigurationHelper;
}

class DialogRecordConfigurationHelper : public QDialog {
    Q_OBJECT

  public:
    explicit DialogRecordConfigurationHelper(const QVector<XcpEvent>& events, int ratePreset, QWidget *parent = 0);
    ~DialogRecordConfigurationHelper();

    QString getTrigger() const;

    int getRate() const;

  private slots:
    void on_buttonBox_accepted();

    void on_triggerComboBox_currentTextChanged(const QString& comboText);

  private:
    Ui::dialogRecordConfigurationHelper *ui;
    QString trigger;
    int ratePreset;
    int rateUser;

    static const int POLLING_RATE_MIN = 10;

    void addEventsToComboBox(const QVector<XcpEvent>& events);
    bool isPollingMode(const QString &comboText);
};

#endif // DIALOGRECORDCONFIGURATIONHELPER_H
