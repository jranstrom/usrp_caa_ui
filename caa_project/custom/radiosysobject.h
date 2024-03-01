#ifndef RADIOSYSOBJECT_H
#define RADIOSYSOBJECT_H

#include "RadioSysConfig.h"
#include "../circbuffer.h"
#include <uhd/usrp/multi_usrp.hpp>

class RadioSysObject
{
public:
    RadioSysObject();
    ~RadioSysObject(){
        StopAll();
    }
    bool readConfigFile(std::string configFilepath ="usrp.cfg");
    bool readConfigSignalFile(std::string filepath ="sys_last_params.mat");

    bool writeConfigFile(std::string configFilepath ="usrp.cfg");

    void StopAll();

    bool startTransmission();
    bool startReception();

    bool stopTransmission();
    bool stopReception();

    bool isTransmitting();
    bool isReceiving();

    bool isProcessingTxSetup() {return txSetupInProgress;}
    bool isProcessingRxSetup() {return rxSetupInProgress;}

    bool isTxUSRPConfigured() {return txUSRPConfigured;}
    void setTxUSRPConfigured(bool value){
        if(value != txUSRPConfigured){
            txUSRPConfigured = value;
        }
    }

    bool isRxUSRPConfigured() {return rxUSRPConfigured;}
    void setRxUSRPConfigured(bool value){
        if(value != rxUSRPConfigured){
            rxUSRPConfigured = value;
        }
    }

    void requestTxUSRPSetup();
    void requestRxUSRPSetup();

    void requestWriteBufferToFile(std::string completeFilepath,int count=-1);
    bool isWritingBufferToFile() {return writingBufferInProgress;}

    size_t getRxSampleCount() { return rxSampleCount; }

    RadioSysConfig sysConf = RadioSysConfig();

    bool txUSRPSetup = false;
    bool rxUSRPSetup = false;

private:
    std::mutex recv_mutex;

    void runTransmissionThread();
    void runReceptionThread();

    void writeBufferToFile(int count);

    void setupTxUSRP();
    void setupRxUSRP();

    bool txSetupInProgress = false;
    bool rxSetupInProgress = false;

    std::string rxCaptureFilepath = "rx_capture_buffer.dat";

    bool writingBufferInProgress = false;

    size_t rxSampleCount = 0;

    bool txUSRPConfigured = false;
    bool rxUSRPConfigured = false;

    bool TransmissionInProgress = false;
    bool ReceptionInProgress = false;
    bool SynchronizationInProgress = false;



    uhd::usrp::multi_usrp::sptr tx_usrp;
    uhd::usrp::multi_usrp::sptr rx_usrp;

    //CircBuffer<std::complex<short>> * txSignalBuffer;
    CircBuffer<std::complex<short>> txSignalBuffer;
    CircBuffer<std::complex<short>> rxSignalBuffer;

    double timeOffset = 0.0;

    bool stop_signal_called = false;
    bool stop_transmit_signal_called = false;
    bool stop_reception_signal_called = false;

};

#endif // RADIOSYSOBJECT_H
