#include "uiobj.h"

uiobj::uiobj(QObject *parent)
    : QObject{parent}
{}

void uiobj::setTxCarrierFrequency(double value)
{
    bool force_update = true;
    if(value > 6e9){
        value = 6e9;
    }else if(value < 1.2e9){
        value = 1.2e9;
    }else{
        force_update = false;
    }

    if(force_update || value != radObj->sysConf.getTxCarrierFrequency()){
        radObj->sysConf.setTxCarrierFrequency(value);

        this->setTxSetupStatus(false);
        emit txCarrierFrequencyChanged(value);
    }
}

void uiobj::setRxCarrierFrequency(double value)
{
    bool force_update = true;
    if(value > 6e9){
        value = 6e9;
    }else if(value < 1.2e9){
        value = 1.2e9;
    }else{
        force_update = false;
    }

    if(force_update || value != radObj->sysConf.getRxCarrierFrequency()){
        radObj->sysConf.setRxCarrierFrequency(value);

        this->setRxSetupStatus(false);
        emit rxCarrierFrequencyChanged(value);
    }
}

void uiobj::setTxGain(double value)
{
    bool force_update = true;
    if(value > 15){
        value = 15;
    }else if(value < -15){
        value = -15;
    }else{
        force_update = false;
    }

    if(force_update || value != radObj->sysConf.getTxGain()){
        radObj->sysConf.setTxGain(value);
        this->setTxSetupStatus(false);
        emit txGainChanged(true);
    }
}

void uiobj::setRxGain(double value)
{
    bool force_update = true;
    if(value > 20){
        value = 20;
    }else if(value < -20){
        value = -20;
    }else{
        force_update = false;
    }

    if(force_update || value != radObj->sysConf.getRxGain()){
        radObj->sysConf.setRxGain(value);
        this->setRxSetupStatus(false);
        emit rxGainChanged(true);
    }
}

void uiobj::setTxIPAddress(std::string value)
{
    if(radObj->sysConf.getTxIPAddress() != value){
        radObj->sysConf.setTxIPAddress(value);
        this->setTxSetupStatus(false);
        emit txIPAddressChanged(true);
    }
}

void uiobj::setRxIPAddress(std::string value)
{
    if(radObj->sysConf.getRxIPAddress() != value){
        radObj->sysConf.setRxIPAddress(value);
        this->setRxSetupStatus(false);
        emit rxIPAddressChanged(true);
    }
}

void uiobj::setTxREFSource(std::string value)
{
    if(radObj->sysConf.getTxREFSource() != value){
        radObj->sysConf.setTxREFSource(value);
        this->setTxSetupStatus(false);
        emit txREFSourceChanged(true);
    }
}

void uiobj::setRxREFSource(std::string value)
{
    if(radObj->sysConf.getRxREFSource() != value){
        radObj->sysConf.setRxREFSource(value);
        this->setRxSetupStatus(false);
        emit rxREFSourceChanged(true);
    }
}

void uiobj::setTxPPSSource(std::string value)
{
    if(radObj->sysConf.getTxPPSSource() != value){
        radObj->sysConf.setTxPPSSource(value);
        this->setTxSetupStatus(false);
        emit txPPSSourceChanged(true);
    }
}

void uiobj::setRxPPSSource(std::string value)
{
    if(radObj->sysConf.getRxPPSSource() != value){
        radObj->sysConf.setRxPPSSource(value);
        this->setRxSetupStatus(false);
        emit rxPPSSourceChanged(true);
    }
}

void uiobj::setTransmissionInProgress(bool value)
{
    if(value != transmission_in_progress){
        transmission_in_progress = value;
        emit transmissionStatusChanged(value);
    }
}

void uiobj::setTxSetupStatus(bool value)
{
    //if(value != radObj->txUSRPSetup){
        radObj->txUSRPSetup = value;
        emit txSetupStatusChanged(value);
    //}
}

void uiobj::setRxSetupStatus(bool value)
{
    if(value != radObj->rxUSRPSetup){
        radObj->rxUSRPSetup = value;
        emit rxSetupStatusChanged(value);
    }
}

void uiobj::setRadioSysObject(RadioSysObject *RadObj)
{
    radObj = RadObj;
}

void uiobj::ForceUpdateAll()
{
    emit txCarrierFrequencyChanged(true);
    emit txGainChanged(true);
    emit txIPAddressChanged(true);
    emit txREFSourceChanged(true);
    emit txPPSSourceChanged(true);

    emit rxCarrierFrequencyChanged(true);
    emit rxGainChanged(true);
    emit rxIPAddressChanged(true);
    emit rxREFSourceChanged(true);
    emit rxPPSSourceChanged(true);
}
