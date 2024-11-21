#include "cradiob200.h"


cRadioResponse cRadioB200::runRadioConfigurationProcess()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "Success configuring B200";

    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> appliedConfiguration_t = getAppliedConfiguation();

    bool ok;

    std::cout << "Running B200 configuration" << std::endl;

    const std::string addr = "type=b200,serial=" + getSerial();
    usrp = uhd::usrp::multi_usrp::make(addr);

    std::string REF_source = appliedConfiguration_t["ref-source"]->getPropertyValueStr();
    usrp->set_clock_source(REF_source);

    std::string PPS_source = appliedConfiguration_t["pps-source"]->getPropertyValueStr();
    usrp->set_time_source(PPS_source);

    size_t channel = 0;
    double rxSamplingRate = appliedConfiguration_t["rx-sampling-rate"]->getPropertyValueDouble();
    usrp->set_rx_rate(rxSamplingRate);
    appliedConfiguration_t["rx-sampling-rate"]->setProperty(usrp->get_rx_rate(channel),ok);

    double txSamplingRate = appliedConfiguration_t["tx-sampling-rate"]->getPropertyValueDouble();
    usrp->set_tx_rate(txSamplingRate);
    appliedConfiguration_t["tx-sampling-rate"]->setProperty(usrp->get_tx_rate(channel),ok);

    double rxCarrierFrequency = appliedConfiguration_t["rx-carrier-frequency"]->getPropertyValueDouble();
    double rxLO_offset = appliedConfiguration_t["rx-lo-offset"]->getPropertyValueDouble();
    uhd::tune_request_t rxTuneRequest(rxCarrierFrequency,rxLO_offset);
    usrp->set_rx_freq(rxTuneRequest,channel);
    appliedConfiguration_t["rx-carrier-frequency"]->setProperty(usrp->get_rx_freq(channel),ok);

    double txCarrierFrequency = appliedConfiguration_t["tx-carrier-frequency"]->getPropertyValueDouble();
    double txLO_offset = appliedConfiguration_t["tx-lo-offset"]->getPropertyValueDouble();
    uhd::tune_request_t txTuneRequest(txCarrierFrequency,rxLO_offset);
    usrp->set_tx_freq(txTuneRequest,channel);

    appliedConfiguration_t["tx-carrier-frequency"]->setProperty(usrp->get_tx_freq(channel),ok);

    double rxGain = appliedConfiguration_t["rx-gain"]->getPropertyValueDouble();
    double txGain = appliedConfiguration_t["tx-gain"]->getPropertyValueDouble();
    usrp->set_rx_gain(rxGain);
    usrp->set_tx_gain(txGain);

    appliedConfiguration_t["rx-gain"]->setProperty(usrp->get_rx_gain(channel),ok);
    appliedConfiguration_t["tx-gain"]->setProperty(usrp->get_tx_gain(channel),ok);

    std::string rxAntenna = appliedConfiguration_t["rx-antenna"]->getPropertyValueStr();
    std::string txAntenna = appliedConfiguration_t["tx-antenna"]->getPropertyValueStr();
    usrp->set_rx_antenna(rxAntenna,channel);
    usrp->set_tx_antenna(txAntenna,channel);

    double rxFilterBandwidth = appliedConfiguration_t["rx-filter-bandwidth"]->getPropertyValueDouble();
    double txFilterBandwidth = appliedConfiguration_t["tx-filter-bandwidth"]->getPropertyValueDouble();
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


    setAppliedConfiguration(appliedConfiguration_t);
    //response.code = -1;
    //response.message = "B200 - Configuration not yet implemented";

    return response;
}
