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

        txUSRPConfigurationChanged(true);
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

        rxUSRPConfigurationChanged(true);
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
        txUSRPConfigurationChanged(true);
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
        rxUSRPConfigurationChanged(true);
        emit rxGainChanged(true);
    }
}

void uiobj::setTxIPAddress(std::string value)
{
    if(radObj->sysConf.getTxIPAddress() != value){
        radObj->sysConf.setTxIPAddress(value);
        txUSRPConfigurationChanged(true);
        emit txIPAddressChanged(true);
    }
}

void uiobj::setRxIPAddress(std::string value)
{
    if(radObj->sysConf.getRxIPAddress() != value){
        radObj->sysConf.setRxIPAddress(value);
        rxUSRPConfigurationChanged(true);
        emit rxIPAddressChanged(true);
    }
}

void uiobj::setRxLO_Offset(double value)
{
    bool force_update = true;
    if(value > 20e6){
        value = 20e6;
    }else if(value <-20e6){
        value = 20e6;
    }else{
        force_update = false;
    }
    if(force_update || value != radObj->sysConf.getRxLO_offset()){
            radObj->sysConf.setRxLO_offset(value);
            rxUSRPConfigurationChanged(true);
            emit rxLO_OffsetChanged(value);
    }
}

void uiobj::setTxSamplingRate(double value)
{
    bool force_update = true;
    if(value > 20e6){
        value = 20e6;
    }else if(value < 10e3){
        value = 10e3;
    }else{
        force_update = false;
    }

    if(force_update || value != radObj->sysConf.getTxSamplingRate()){
        radObj->sysConf.setTxSamplingRate(value);

        txUSRPConfigurationChanged(true);
        emit txSamplingRateChanged(value);
    }
}

void uiobj::setRxSamplingRate(double value)
{
    bool force_update = true;
    if(value > 20e6){
        value = 20e6;
    }else if(value < 10e3){
        value = 10e3;
    }else{
        force_update = false;
    }

    if(force_update || value != radObj->sysConf.getRxSamplingRate()){
        radObj->sysConf.setRxSamplingRate(value);

        rxUSRPConfigurationChanged(true);
        emit rxSamplingRateChanged(value);
    }
}

void uiobj::setTxREFSource(std::string value)
{
    if(radObj->sysConf.getTxREFSource() != value){
        radObj->sysConf.setTxREFSource(value);
        txUSRPConfigurationChanged(true);
        emit txREFSourceChanged(true);
    }
}

void uiobj::setRxREFSource(std::string value)
{
    if(radObj->sysConf.getRxREFSource() != value){
        radObj->sysConf.setRxREFSource(value);
        rxUSRPConfigurationChanged(true);
        emit rxREFSourceChanged(true);
    }
}

void uiobj::setTxPPSSource(std::string value)
{
    if(radObj->sysConf.getTxPPSSource() != value){
        radObj->sysConf.setTxPPSSource(value);
        txUSRPConfigurationChanged(true);
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

void uiobj::txUSRPConfigurationChanged(bool val)
{
    radObj->setTxUSRPConfigured(false);
    emit USRPConfigurationChanged(true);
}

void uiobj::rxUSRPConfigurationChanged(bool val)
{
    radObj->setRxUSRPConfigured(false);
    emit USRPConfigurationChanged(true);
}

void uiobj::setTransmissionInProgress(bool value)
{
    bool radObjTxStatus = radObj->isTransmitting();
    if(radObjTxStatus == value){
        // Set to same as USRP status
        if(value != transmission_in_progress){
            transmission_in_progress = value;
            emit transmissionStatusChanged(value);
        }
    }else if(radObjTxStatus != value){
        // This may be seen as a request to change USRP status
        radObj->stopTransmission();
        transmission_in_progress = value;
        emit transmissionStatusChanged(value);
    }
}

void uiobj::setReceptionInProgress(bool value)
{
    bool radObjRxStatus = radObj->isReceiving();
    if(radObjRxStatus == value){
        // Set to same as USRP status
        if(value != reception_in_progress){
            reception_in_progress = value;
            emit receptionStatusChanged(value);
        }
    }else if(radObjRxStatus != value){
        // This may be seen as a request to change USRP status
        radObj->stopReception();
        reception_in_progress = value;
        emit receptionStatusChanged(value);
    }
}

void uiobj::setTxSetupStatus(bool value)
{
    radObj->txUSRPSetup = value;
    emit txSetupStatusChanged(value);
    radObj->setTxUSRPConfigured(false);
    txUSRPConfigurationChanged(true);
}

void uiobj::setRxSetupStatus(bool value)
{
    radObj->rxUSRPSetup = value;
    emit rxSetupStatusChanged(value);
    radObj->setRxUSRPConfigured(false);
    rxUSRPConfigurationChanged(true);
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
    emit txSamplingRateChanged(true);

    emit rxCarrierFrequencyChanged(true);
    emit rxGainChanged(true);
    emit rxIPAddressChanged(true);
    emit rxREFSourceChanged(true);
    emit rxPPSSourceChanged(true);
    emit rxSamplingRateChanged(true);

    emit rxLO_OffsetChanged(true);

    rxUSRPConfigurationChanged(true);
}
