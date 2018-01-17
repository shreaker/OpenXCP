/**
*@file dialogDeviceEventsInsert.h
*@author Michael Wolf
*@date 27.11.2017
*@brief GUI dialog class to manage DAQ events.
*/

#ifndef DIALOGDEVICEEVENTSINSERT_H
#define DIALOGDEVICEEVENTSINSERT_H

#include <QDialog>

#include <QString>

namespace Ui {
class DialogDeviceEventsInsert;
}

class DialogDeviceEventsInsert : public QDialog {
    Q_OBJECT

  public:
    explicit DialogDeviceEventsInsert(QWidget *parent = 0);
    ~DialogDeviceEventsInsert();

    QString getName() const;
    int getChannel() const;
    int getRate() const;

  private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

  private:
    Ui::DialogDeviceEventsInsert *ui;
    QString name;
    int channel;
    int rate;
};

#endif // DIALOGDEVICEEVENTSINSERT_H
