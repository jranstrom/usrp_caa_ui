#include "uiobj.h"

uiobj::uiobj(QObject *parent)
    : QObject{parent}
{}

void uiobj::setTransmissionInProgress(bool value)
{
    if(value != transmission_in_progress){
        transmission_in_progress = value;
        emit transmissionStatusChanged(value);
    }
}

void uiobj::setTxSetupStatus(bool value)
{
    if(value != tx_setup_complete){
        tx_setup_complete = value;
        emit txSetupStatusChanged(value);
    }
}

void uiobj::setRxSetupStatus(bool value)
{
    if(value != rx_setup_complete){
        rx_setup_complete = value;
        emit rxSetupStatusChanged(value);
    }
}
