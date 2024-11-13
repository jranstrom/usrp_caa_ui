#ifndef CRADIOOBJECT_H
#define CRADIOOBJECT_H

#include "circbuffer.h"
#include <string>
#include <uhd/usrp/multi_usrp.hpp>

struct cRadioResponse{
    int code; // 0 - success
    std::string message = "";
};

struct cRadioConfiguration{

    std::string refSource;
    std::string ppsSource;

    double rxSamplingRate;
    double rxCarrierFrequency;
    double rxGain;
    double rxLOoffset;
    std::string rxAntenna;
    double rxFilterBandwidth;

    double txSamplingRate;
    double txCarrierFrequency;
    double txGain;
    double txLOoffset = 0.0;
    std::string txAntenna;
    double txFilterBandwidth;

    size_t internalRxBufferSize = 1e5;

    std::vector<std::string> configurationVectorEntries = {"ref-source",
                                                           "pps-source",
                                                           "tx-carrier-frequency",
                                                           "tx-sampling-rate",
                                                           "tx-gain",
                                                           "tx-ant",
                                                           "tx-filter-bandwidth",
                                                           "tx-lo-offset",
                                                           "rx-carrier-frequency",
                                                           "rx-sampling-rate",
                                                           "rx-gain",
                                                           "rx-ant",
                                                           "rx-filter-bandwidth",
                                                           "rx-lo-offset",
                                                           "internal-rx-buffer-size"};
};

class cRadioObject
{
public:
    cRadioObject(std::string serial_p,std::string type_p,std::string addr_p);
    ~cRadioObject();

    cRadioResponse configureRadio();
    cRadioResponse loadRadioConfigurationFile(bool def=true,std::string filepath="");
    bool isConfigured();
    cRadioConfiguration getConfiguration() {return rConf;}

    std::string getSerial() {return serial;}
    std::string getType() {return type;}
    std::string getAddress() {return address;}

    cRadioResponse startConinousReceptionProcess();

private:

    std::string uhd_findRadioResponse;
    std::string serial;
    std::string type;
    std::string address;

    cRadioConfiguration rConf;
    bool isLoadedConfiguration= false;

    uhd::usrp::multi_usrp::sptr usrp;

    std::shared_ptr<CircBuffer<std::complex<short>>> rxCircBuff;

    cRadioResponse readConfigurationFile(std::string filepath);
    cRadioResponse writeConfiurationFile(std::string filepath);

    void continousReceptionProcess();
};

#endif // CRADIOOBJECT_H
