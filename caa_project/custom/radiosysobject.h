#ifndef RADIOSYSOBJECT_H
#define RADIOSYSOBJECT_H

#include "RadioSysConfig.h"
#include "../circbuffer.h"
#include <uhd/usrp/multi_usrp.hpp>

class RadioSysObject
{
public:
    RadioSysObject();
    bool readConfigFile(std::string configFilepath ="usrp.cfg");
    bool readConfigSignalFile(std::string filepath ="sys_last_params.mat");

    bool writeConfigFile(std::string configFilepath ="usrp.cfg");

    void StopAll();

    bool startTransmission();
    bool startReception();

    bool isTransmitting();

    void setupTxUSRP();
    void setupRxUSRP();

    RadioSysConfig sysConf = RadioSysConfig();

private:
    void runTransmissionThread();
    void runReceptionThread();

    bool TransmissionInProgress = false;
    bool ReceptionInProgress = false;
    bool SynchronizationInProgress = false;

    bool txUSRPSetup = false;
    bool rxUSRPSetup = false;

    uhd::usrp::multi_usrp::sptr tx_usrp;
    uhd::usrp::multi_usrp::sptr rx_usrp;

    //CircBuffer<std::complex<short>> * txSignalBuffer;
    CircBuffer<std::complex<short>> txSignalBuffer;

    double timeOffset = 0.0;

    bool stop_signal_called = false;

};

#endif // RADIOSYSOBJECT_H
