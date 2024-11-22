#ifndef CRADIOOBJECT_H
#define CRADIOOBJECT_H

#include "circbuffer.h"
#include "cradioproperty.h"
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
    virtual cRadioResponse runRadioConfigurationProcess();

    cRadioResponse loadRadioConfigurationFile(bool def=true,std::string filepath="");

    bool isConfigured() {return isRadioConfigured;}

    cRadioConfiguration getConfiguration() {return rConf;}
    void setConfiguration(cRadioConfiguration rConf_p);

    std::string getSerial() {return serial;}
    std::string getType() {return type;}
    std::string getAddress() {return address;}

    cRadioResponse startContinousReception();
    cRadioResponse stopContinousReception();
    std::atomic<bool> stop_continous_reception = false;
    std::atomic<bool> continous_reception_running = false;
    virtual void runContinousReceptionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> rxCircBuffer,uhd::usrp::multi_usrp::sptr &m_usrp);

    cRadioResponse startContinousTransmission();
    cRadioResponse stopContinousTransmission();
    std::atomic<bool> stop_continous_transmission = false;
    std::atomic<bool> continous_transmission_running = false;
    virtual void runContinousTransmissionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> txCircBuffer,uhd::usrp::multi_usrp::sptr &m_usrp);

    std::vector<std::complex<short>> getLastReceivedSamples(size_t N);

    uhd::usrp::multi_usrp::sptr usrp;

    static bool isConfigurationsEqual(cRadioConfiguration radr,cRadioConfiguration radl);

    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> getStagedConfiguation() {return stagedConfiguration;}
    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> getAppliedConfiguation() {return appliedConfiguration;}
    void setAppliedConfiguration(std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> configuration_p){appliedConfiguration = configuration_p;}


private:

    std::string uhd_findRadioResponse;
    std::string serial;
    std::string type;
    std::string address;

    cRadioConfiguration rConf;

    bool isLoadedConfiguration= false;
    bool isRadioConfigured = false;

    bool configurationApplied = false;

    std::shared_ptr<CircBuffer<std::complex<short>>> internalRxCircBuffer;
    std::shared_ptr<CircBuffer<std::complex<short>>> internalTxCircBuffer;

    cRadioResponse readConfigurationFile(std::string filepath);
    cRadioResponse writeConfiurationFile(std::string filepath);

    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> stagedConfiguration;
    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> appliedConfiguration;
};

#endif // CRADIOOBJECT_H
