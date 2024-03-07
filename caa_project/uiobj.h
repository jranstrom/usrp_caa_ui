#ifndef UIOBJ_H
#define UIOBJ_H

#include <QObject>
#include "custom/radiosysobject.h"
#include "custom/RadioSysConfig.h"
#include "custom/TeensyCOM.h"

class uiobj : public QObject
{
    Q_OBJECT
public:
    explicit uiobj(QObject *parent = nullptr);
    bool transmissionInProgress() const {return transmission_in_progress; }
    bool txSetupStatus() const {return tx_setup_complete;}
    bool rxSetupStatus() const {return rx_setup_complete;}

    // Snippet Generated "uio_property"

    double getTxCarrierFrequency(){return radObj->sysConf.getTxCarrierFrequency();}
    double getRxCarrierFrequency(){return radObj->sysConf.getRxCarrierFrequency();}

    void setTxCarrierFrequency(double value);
    void setRxCarrierFrequency(double value);

    double getTxGain(){return radObj->sysConf.getTxGain();}
    double getRxGain(){return radObj->sysConf.getRxGain();}

    void setTxGain(double value);
    void setRxGain(double value);

    std::string getTxIPAddress(){return radObj->sysConf.getTxIPAddress();}
    std::string getRxIPAddress(){return radObj->sysConf.getRxIPAddress();}

    void setTxIPAddress(std::string value);
    void setRxIPAddress(std::string value);

    std::string getTxREFSource(){return radObj->sysConf.getTxREFSource();}
    std::string getRxREFSource(){return radObj->sysConf.getRxREFSource();}

    double getRxLO_Offset(){return radObj->sysConf.getRxLO_offset();}
    void setRxLO_Offset(double value);

    double getTxSamplingRate(){return radObj->sysConf.getTxSamplingRate();}
    double getRxSamplingRate(){return radObj->sysConf.getRxSamplingRate();}

    void setTxSamplingRate(double value);
    void setRxSamplingRate(double value);

    void setTxREFSource(std::string value);
    void setRxREFSource(std::string value);

    std::string getTxPPSSource(){return radObj->sysConf.getTxPPSSource();}
    std::string getRxPPSSource(){return radObj->sysConf.getRxPPSSource();}

    void setTxPPSSource(std::string value);
    void setRxPPSSource(std::string value);

    //

    void txUSRPConfigurationChanged(bool val);
    void rxUSRPConfigurationChanged(bool val);

    void setTransmissionInProgress(bool value);
    void setReceptionInProgress(bool value);

    void setTxSetupStatus(bool value);
    void setRxSetupStatus(bool value);

    void setRadioSysObject(RadioSysObject *RadObj);

    void ForceUpdateAll();

signals:
    void txCarrierFrequencyChanged(bool status);
    void rxCarrierFrequencyChanged(bool status);
    void txGainChanged(bool status);
    void rxGainChanged(bool status);
    void txIPAddressChanged(bool status);
    void rxIPAddressChanged(bool status);
    void txREFSourceChanged(bool status);
    void rxREFSourceChanged(bool status);
    void txPPSSourceChanged(bool status);
    void rxPPSSourceChanged(bool status);

    void txSamplingRateChanged(bool status);
    void rxSamplingRateChanged(bool status);

    void rxLO_OffsetChanged(bool status);

    void USRPConfigurationChanged(bool status);

    void transmissionStatusChanged(bool in_progress);
    void receptionStatusChanged(bool in_progress);
    void txSetupStatusChanged(bool isComplete);
    void rxSetupStatusChanged(bool isComplete);

private:

    RadioSysObject * radObj;

    TeensyCOM tcom = TeensyCOM();

    double tx_carrier_frequency = -1;
    double rx_carrier_frequency = -1;

    bool transmission_in_progress = false;
    bool reception_in_progress = false;
    bool tx_setup_complete = false;
    bool rx_setup_complete = false;
};

#endif // UIOBJ_H
