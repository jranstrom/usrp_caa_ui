#ifndef UIOBJ_H
#define UIOBJ_H

#include <QObject>

class uiobj : public QObject
{
    Q_OBJECT
public:
    explicit uiobj(QObject *parent = nullptr);
    bool transmissionInProgress() const {return transmission_in_progress; }
    bool txSetupStatus() const {return tx_setup_complete;}
    bool rxSetupStatus() const {return rx_setup_complete;}

    void setTransmissionInProgress(bool value);
    void setTxSetupStatus(bool value);
    void setRxSetupStatus(bool value);

signals:
    void transmissionStatusChanged(bool in_progress);
    void txSetupStatusChanged(bool isComplete);
    void rxSetupStatusChanged(bool isComplete);

private:
    bool transmission_in_progress = false;
    bool tx_setup_complete = false;
    bool rx_setup_complete = false;
};

#endif // UIOBJ_H
