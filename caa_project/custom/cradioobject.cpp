#include "cradioobject.h"
#include <fstream>
#include "custom/config_file.h"


cRadioObject::cRadioObject(std::string serial_p, std::string type_p, std::string addr_p)
{
    serial = serial_p;
    type = type_p;
    address = addr_p;
}

cRadioResponse cRadioObject::configureRadio(bool def,std::string filepath)
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    bool useFile = true;

    std::string filepath_p = "";
    if(def == true){
        // get default configuration file
        filepath_p = "usrp_default_configs/" + type + ".cfg";
    }

    if(useFile == true){
        response = readConfigurationFile(filepath_p);
    }

    // Set up USRP part

    usrp = uhd::usrp::multi_usrp ::make("addr=" + address);

    std::string REF_source = rConf.refSource;
    usrp->set_clock_source(REF_source);

    std::string PPS_source = rConf.ppsSource;
    usrp->set_time_source(PPS_source);

    double rxSamplingRate = rConf.rxSamplingRate;
    usrp->set_rx_rate(rxSamplingRate);

    double txSamplingRate = rConf.txSamplingRate;
    usrp->set_tx_rate(txSamplingRate);

    size_t channel = 0;

    double rxCarrierFrequency = rConf.rxCarrierFrequency;
    double rxLO_offset = rConf.rxLOoffset;
    uhd::tune_request_t rxTuneRequest(rxCarrierFrequency,rxLO_offset);
    usrp->set_rx_freq(rxTuneRequest,channel);

    double txCarrierFrequency = rConf.txCarrierFrequency;
    double txLO_offset = rConf.txLOoffset;
    uhd::tune_request_t txTuneRequest(rxCarrierFrequency,rxLO_offset);
    usrp->set_tx_freq(txTuneRequest,channel);

    double rxGain = rConf.rxGain;
    double txGain = rConf.txGain;
    usrp->set_rx_gain(rxGain);
    usrp->set_tx_gain(txGain);

    std::string rxAntenna = rConf.rxAntenna;
    std::string txAntenna = rConf.txAntenna;
    usrp->set_rx_antenna(rxAntenna,channel);
    usrp->set_tx_antenna(txAntenna,channel);

    double rxFilterBandwidth = rConf.rxFilterBandwidth;
    double txFilterBandwidth = rConf.txFilterBandwidth;
    usrp->set_rx_bandwidth(rxFilterBandwidth,channel);
    usrp->set_tx_bandwidth(txFilterBandwidth,channel);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::vector<std::string> txSensorNames = usrp->get_tx_sensor_names(0);
    if(std::find(txSensorNames.begin(), txSensorNames.end(),"lo_locked") != txSensorNames.end()){
        uhd::sensor_value_t lo_locked = usrp->get_tx_sensor("lo_locked",0);
        UHD_ASSERT_THROW(lo_locked.to_bool());
    }

    std::vector<std::string> rxSensorNames = usrp->get_rx_sensor_names(0);
    if(std::find(rxSensorNames.begin(), rxSensorNames.end(),"lo_locked") != rxSensorNames.end()){
        uhd::sensor_value_t lo_locked = usrp->get_rx_sensor("lo_locked",0);
        UHD_ASSERT_THROW(lo_locked.to_bool());
    }

    return response;
}

cRadioResponse cRadioObject::readConfigurationFile(std::string filepath)
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    try{
        std::ifstream iconf_file(filepath);
        CFG::ReadFile(iconf_file,rConf.configurationVectorEntries,
                      rConf.refSource,
                      rConf.ppsSource,
                      rConf.txCarrierFrequency,
                      rConf.txSamplingRate,
                      rConf.txGain,
                      rConf.txAntenna,
                      rConf.txFilterBandwidth,
                      rConf.txLOoffset,
                      rConf.rxCarrierFrequency,
                      rConf.rxSamplingRate,
                      rConf.rxGain,
                      rConf.rxAntenna,
                      rConf.rxFilterBandwidth,
                      rConf.rxLOoffset);

    }catch(...){
        response.code = -1;
        response.message = "error reading configuration";
    }

    return response;
}

cRadioResponse cRadioObject::writeConfiurationFile(std::string filepath)
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    std::ofstream f_out(filepath);
    const std::string str_true = "true";
    const std::string str_false = "false";

    CFG::WriteFile(f_out,rConf.configurationVectorEntries,
                   rConf.refSource,
                   rConf.ppsSource,
                   rConf.txCarrierFrequency,
                   rConf.txSamplingRate,
                   rConf.txGain,
                   rConf.txAntenna,
                   rConf.txFilterBandwidth,
                   rConf.txLOoffset,
                   rConf.rxCarrierFrequency,
                   rConf.rxSamplingRate,
                   rConf.rxGain,
                   rConf.rxAntenna,
                   rConf.rxFilterBandwidth,
                   rConf.rxLOoffset);
    f_out.close();

    return response;
}
