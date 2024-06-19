#ifndef RADIOSYSOBJECT_H
#define RADIOSYSOBJECT_H

#include "RadioSysConfig.h"
#include "../circbuffer.h"
#include <uhd/usrp/multi_usrp.hpp>
#include <vector>
#include <cmath>
#include <armadillo>
#include <fftw3.h>

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

    bool startSynchronization();
    bool stopSynchronization();

    bool isTransmitting();
    bool isReceiving();
    bool isSynchronizing();

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
    bool requestCaptureSynchFrame(int captureIndex=0);
    bool isWritingBufferToFile() {return writingBufferInProgress;}


    bool requestWriteLastCapturedFrame(std::string filepath="");
    int requestFrameCaptureFormat(int rframeOffset=0,
                                   int rframeLength=1024,
                                   int rnumFrameCaptures=4,
                                   std::string filepath="capture1.dat",
                                   bool enableWindowSynch=false);
    int requestWriteFramesToFile(std::string filepath,std::string fileType="csv");

    bool isCapturedFramesReadyToSave();
    bool resetCurrentFramesCaptured();

    size_t getRxSampleCount() { return rxSampleCount; }
    bool requestResetSynchPoints();
    size_t getSynchPointCount() {return syncPointBuffer.get_push_count();}

    int getNumFrameCaptures() {return numFrameCaptures;}

    RadioSysConfig sysConf = RadioSysConfig();

    bool txUSRPSetup = false;
    bool rxUSRPSetup = false;

    std::vector<std::complex<short>> getCapturedData(){ return captured_data;}
    std::vector<std::complex<short>> getExtractedSynchData(){return extracted_synch_data;}

    bool getAllCSVDataCaptured() {return allCSVDataCaptured;}

    int requestResetTransmitter();

    bool hasPendingSynchPointReset() {return pendingSynchPointReset; }

private:
    std::mutex recv_mutex;

    void runTransmissionThread();
    void runReceptionThread();
    void runSynchronizationThread();

    void writeBufferToFile(int count);

    void setupTxUSRP();
    void setupRxUSRP();

    void extractFrameSequence(int offset=-1,int length=0);

    bool txSetupInProgress = false;
    bool rxSetupInProgress = false;

    std::string rxCaptureFilepath = "rx_capture_buffer.dat";

    bool writingBufferInProgress = false;

    size_t rxSampleCount = 0;

    size_t txTimeoutCount = 0;
    size_t txTimeoutMax = 1;

    bool txUSRPConfigured = false;
    bool rxUSRPConfigured = false;

    bool TransmissionInProgress = false;
    bool ReceptionInProgress = false;
    bool SynchronizationInProgress = false;

    bool WindowedSynchronizationEnabled = false;


    std::vector<std::complex<short>> captured_data;
    std::string captureSynchFilepath = "";

    std::vector<std::complex<short>> extracted_synch_data;

    bool allCSVDataCaptured = false;
    int frameLength = 1024;
    int frameOffset = 0;
    int numFrameCaptures = 4;

    std::vector<std::vector<double>> capturedFrames;

    std::vector<bool> currentFramesCaptured = {false,false,false,false};

    uhd::usrp::multi_usrp::sptr tx_usrp;
    uhd::usrp::multi_usrp::sptr rx_usrp;

    //CircBuffer<std::complex<short>> * txSignalBuffer;
    CircBuffer<std::complex<short>> txSignalBuffer;
    CircBuffer<std::complex<short>> rxSignalBuffer;
    CircBuffer<std::complex<short>> txSynchSignalBuffer;


    CircBuffer<size_t> syncPointBuffer;
    CircBuffer<double> frequencyOffsetBuffer;

    double timeOffset = 0.0;

    bool stop_signal_called = false;
    bool stop_transmit_signal_called = false;
    bool stop_reception_signal_called = false;
    bool stop_synchronization_signal_called = false;

    bool pendingSynchPointReset = false;

};

#endif // RADIOSYSOBJECT_H
