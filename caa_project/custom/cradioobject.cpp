#include "cradioobject.h"
#include <filesystem>
#include <fstream>
#include "custom/config_file.h"


cRadioObject::cRadioObject(std::string serial_p, std::string type_p, std::string addr_p)
{
    serial = serial_p;
    type = type_p;
    address = addr_p;

    stagedConfiguration["rx-carrier-frequency"] = std::make_shared<cRadioFrequencyProperty>("Rx Carrier Frequency","GHz",0.0);
    stagedConfiguration["tx-carrier-frequency"] = std::make_shared<cRadioFrequencyProperty>("Tx Carrier Frequency","GHz",0.0);

    stagedConfiguration["rx-sampling-rate"] = std::make_shared<cRadioFrequencyProperty>("Rx Sampling Rate","MHz",0.0);
    stagedConfiguration["tx-sampling-rate"] = std::make_shared<cRadioFrequencyProperty>("Tx Sampling Rate","MHz",0.0);

    stagedConfiguration["pps-source"] = std::make_shared<cRadioPPSSourceProperty>("PPS Source","","external");
    stagedConfiguration["ref-source"] = std::make_shared<cRadioREFSourceProperty>("REF Source","","external");
    stagedConfiguration["rx-antenna"] = std::make_shared<cRadioAntennaProperty>("Rx antenna","","CAL");
    stagedConfiguration["tx-antenna"] = std::make_shared<cRadioAntennaProperty>("Tx antenna","","CAL");

    stagedConfiguration["tx-gain"] = std::make_shared<cRadioGainProperty>("Tx Gain","dB",0.0);
    stagedConfiguration["rx-gain"] = std::make_shared<cRadioGainProperty>("Rx Gain","dB",0.0);

    stagedConfiguration["rx-lo-offset"] = std::make_shared<cRadioFrequencyProperty>("Rx LO-offset","MHz",0.0);
    stagedConfiguration["tx-lo-offset"] = std::make_shared<cRadioFrequencyProperty>("Tx LO-offset","MHz",0.0);

    stagedConfiguration["tx-filter-bandwidth"] = std::make_shared<cRadioFrequencyProperty>("Tx Filter BW","MHz",0.0);
    stagedConfiguration["rx-filter-bandwidth"] = std::make_shared<cRadioFrequencyProperty>("Rx Filter BW","MHz",0.0);

    //

    appliedConfiguration["rx-carrier-frequency"] = std::make_shared<cRadioFrequencyProperty>("Rx Carrier Frequency","GHz",0.0);
    appliedConfiguration["tx-carrier-frequency"] = std::make_shared<cRadioFrequencyProperty>("Tx Carrier Frequency","GHz",0.0);

    appliedConfiguration["rx-sampling-rate"] = std::make_shared<cRadioFrequencyProperty>("Rx Sampling Rate","MHz",0.0);
    appliedConfiguration["tx-sampling-rate"] = std::make_shared<cRadioFrequencyProperty>("Tx Sampling Rate","MHz",0.0);

    appliedConfiguration["pps-source"] = std::make_shared<cRadioPPSSourceProperty>("PPS Source","","external");
    appliedConfiguration["ref-source"] = std::make_shared<cRadioREFSourceProperty>("REF Source","","external");
    appliedConfiguration["rx-antenna"] = std::make_shared<cRadioAntennaProperty>("Rx antenna","","CAL");
    appliedConfiguration["tx-antenna"] = std::make_shared<cRadioAntennaProperty>("Tx antenna","","CAL");

    appliedConfiguration["tx-gain"] = std::make_shared<cRadioGainProperty>("Tx Gain","dB",0.0);
    appliedConfiguration["rx-gain"] = std::make_shared<cRadioGainProperty>("Rx Gain","dB",0.0);

    appliedConfiguration["rx-lo-offset"] = std::make_shared<cRadioFrequencyProperty>("Rx LO-offset","MHz",0.0);
    appliedConfiguration["tx-lo-offset"] = std::make_shared<cRadioFrequencyProperty>("Tx LO-offset","MHz",0.0);

    appliedConfiguration["tx-filter-bandwidth"] = std::make_shared<cRadioFrequencyProperty>("Tx Filter BW","MHz",0.0);
    appliedConfiguration["rx-filter-bandwidth"] = std::make_shared<cRadioFrequencyProperty>("Rx Filter BW","MHz",0.0);

}

cRadioObject::~cRadioObject()
{
    cRadioResponse contRecepResp = stopContinousReception();
    if(contRecepResp.code != 0){
        std::cout << "Could not stop continous reception." << std::endl;
    }
}

cRadioResponse cRadioObject::configureRadio()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    bool ok;
    appliedConfiguration["pps-source"]->setProperty(stagedConfiguration["pps-source"]->getPropertyValueStr(),ok);
    appliedConfiguration["ref-source"]->setProperty(stagedConfiguration["ref-source"]->getPropertyValueStr(),ok);
    appliedConfiguration["rx-antenna"]->setProperty(stagedConfiguration["rx-antenna"]->getPropertyValueStr(),ok);
    appliedConfiguration["tx-antenna"]->setProperty(stagedConfiguration["tx-antenna"]->getPropertyValueStr(),ok);

    appliedConfiguration["rx-carrier-frequency"]->setProperty(stagedConfiguration["rx-carrier-frequency"]->getPropertyValueDouble(),ok);
    appliedConfiguration["tx-carrier-frequency"]->setProperty(stagedConfiguration["tx-carrier-frequency"]->getPropertyValueDouble(),ok);

    appliedConfiguration["rx-sampling-rate"]->setProperty(stagedConfiguration["rx-sampling-rate"]->getPropertyValueDouble(),ok);
    appliedConfiguration["tx-sampling-rate"]->setProperty(stagedConfiguration["tx-sampling-rate"]->getPropertyValueDouble(),ok);

    appliedConfiguration["tx-gain"]->setProperty(stagedConfiguration["tx-gain"]->getPropertyValueDouble(),ok);
    appliedConfiguration["rx-gain"]->setProperty(stagedConfiguration["rx-gain"]->getPropertyValueDouble(),ok);

    appliedConfiguration["rx-lo-offset"]->setProperty(stagedConfiguration["rx-lo-offset"]->getPropertyValueDouble(),ok);

    appliedConfiguration["tx-lo-offset"]->setProperty(stagedConfiguration["tx-lo-offset"]->getPropertyValueDouble(),ok);
    appliedConfiguration["rx-filter-bandwidth"]->setProperty(stagedConfiguration["rx-filter-bandwidth"]->getPropertyValueDouble(),ok);
    appliedConfiguration["tx-filter-bandwidth"]->setProperty(stagedConfiguration["tx-filter-bandwidth"]->getPropertyValueDouble(),ok);

    configurationApplied = true;

    if(isLoadedConfiguration == false){
        response.code = -1;
        response.message = "No configuration has been loaded";
        return response;
    }else{
        response = runRadioConfigurationProcess();
    }

    if(response.code == 0){

        stagedConfiguration["pps-source"]->setProperty(appliedConfiguration["pps-source"]->getPropertyValueStr(),ok);
        stagedConfiguration["ref-source"]->setProperty(appliedConfiguration["ref-source"]->getPropertyValueStr(),ok);
        stagedConfiguration["rx-antenna"]->setProperty(appliedConfiguration["rx-antenna"]->getPropertyValueStr(),ok);
        stagedConfiguration["tx-antenna"]->setProperty(appliedConfiguration["tx-antenna"]->getPropertyValueStr(),ok);

        stagedConfiguration["rx-carrier-frequency"]->setProperty(appliedConfiguration["rx-carrier-frequency"]->getPropertyValueDouble(),ok);
        stagedConfiguration["tx-carrier-frequency"]->setProperty(appliedConfiguration["tx-carrier-frequency"]->getPropertyValueDouble(),ok);

        stagedConfiguration["rx-sampling-rate"]->setProperty(appliedConfiguration["rx-sampling-rate"]->getPropertyValueDouble(),ok);
        stagedConfiguration["tx-sampling-rate"]->setProperty(appliedConfiguration["tx-sampling-rate"]->getPropertyValueDouble(),ok);

        stagedConfiguration["tx-gain"]->setProperty(appliedConfiguration["tx-gain"]->getPropertyValueDouble(),ok);
        stagedConfiguration["rx-gain"]->setProperty(appliedConfiguration["rx-gain"]->getPropertyValueDouble(),ok);

        stagedConfiguration["rx-lo-offset"]->setProperty(appliedConfiguration["rx-lo-offset"]->getPropertyValueDouble(),ok);

        stagedConfiguration["tx-lo-offset"]->setProperty(appliedConfiguration["tx-lo-offset"]->getPropertyValueDouble(),ok);
        stagedConfiguration["rx-filter-bandwidth"]->setProperty(appliedConfiguration["rx-filter-bandwidth"]->getPropertyValueDouble(),ok);
        stagedConfiguration["tx-filter-bandwidth"]->setProperty(appliedConfiguration["tx-filter-bandwidth"]->getPropertyValueDouble(),ok);

        isRadioConfigured = true;
        internalRxCircBuffer = std::make_shared<CircBuffer<std::complex<short>>>(1e5);
    }
    return response;
}

cRadioResponse cRadioObject::runRadioConfigurationProcess()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> appliedConfiguration_t = getAppliedConfiguation();

    bool ok;

    size_t channel = 0;
    usrp = uhd::usrp::multi_usrp ::make("addr=" + address);

    std::string REF_source = appliedConfiguration_t["ref-source"]->getPropertyValueStr();
    usrp->set_clock_source(REF_source);

    std::string PPS_source = appliedConfiguration_t["pps-source"]->getPropertyValueStr();
    usrp->set_time_source(PPS_source);

    double rxSamplingRate = appliedConfiguration_t["rx-sampling-rate"]->getPropertyValueDouble();
    usrp->set_rx_rate(rxSamplingRate);
    appliedConfiguration["rx-sampling-rate"]->setProperty(usrp->get_rx_rate(channel),ok);

    double txSamplingRate = appliedConfiguration_t["tx-sampling-rate"]->getPropertyValueDouble();
    usrp->set_tx_rate(txSamplingRate);
    appliedConfiguration["tx-sampling-rate"]->setProperty(usrp->get_tx_rate(channel),ok);


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

    return response;
}

cRadioResponse cRadioObject::loadRadioConfigurationFile(bool def, std::string filepath)
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    if(def == true){
        // get default configuration file
        filepath = "usrp_default_configs/" + type + ".cfg";
    }

    response = readConfigurationFile(filepath);

    if(response.code == 0){
        isLoadedConfiguration = true;
    }

    return response;
}

void cRadioObject::setConfiguration(cRadioConfiguration rConf_p)
{
    rConf = rConf_p;
    isLoadedConfiguration = true;
}

cRadioResponse cRadioObject::stopContinousReception()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "Success";

    stop_continous_reception = true;

    int m_wait = 100;
    int i = 0;
    do{
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        if(i++ > m_wait){
            response.code = -1;
            response.message = "Error; Reception not terminating...";
            return response;
        }

    }while(continous_reception_running == true);


    return response;
}

cRadioResponse cRadioObject::startContinousReception()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "Success";

    //rxCircBuff->reset_buffer();
    if(continous_reception_running == true){
        response.code = -1;
        response.message = "Error; Continous reception already running";
    }else if(isConfigured()){
        stop_continous_reception = false;
        std::thread continousReceptionThread(&cRadioObject::runContinousReceptionProcess,this,internalRxCircBuffer,usrp);
        continousReceptionThread.detach();
    }else{
        response.code = -2;
        response.message = "Error; Radio not yet configured";
    }

    return response;
}

cRadioResponse cRadioObject::readConfigurationFile(std::string filepath)
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    if(std::filesystem::exists(filepath) == false)
    {
        response.code = -2;
        response.message = "Error; file does not exist";
        return response;
    }

    double rxCarrierFrequency;
    double rxSamplingRate;
    double txCarrierFrequency;
    double txSamplingRate;

    double txFilterBandwidth;
    double rxFilterBandwidth;

    double txLOoffset;
    double rxLOoffset;

    std::string ppsSource;
    std::string refSource;
    std::string txAntenna;
    std::string rxAntenna;
    double txGain;
    double rxGain;

    try{
        std::ifstream iconf_file(filepath);
        CFG::ReadFile(iconf_file,rConf.configurationVectorEntries,
                      refSource,
                      ppsSource,
                      txCarrierFrequency,
                      txSamplingRate,
                      txGain,
                      txAntenna,
                      txFilterBandwidth,
                      txLOoffset,
                      rxCarrierFrequency,
                      rxSamplingRate,
                      rxGain,
                      rxAntenna,
                      rxFilterBandwidth,
                      rxLOoffset,
                      rConf.internalRxBufferSize);

    }catch(...){
        response.code = -1;
        response.message = "error reading configuration";
    }

    rConf.ppsSource = ppsSource;
    rConf.refSource = refSource;

    rConf.txAntenna = txAntenna;
    rConf.rxAntenna = rxAntenna;

    rConf.txGain = txGain;
    rConf.rxGain = rxGain;

    rConf.txFilterBandwidth = txFilterBandwidth;
    rConf.txLOoffset = txLOoffset;
    rConf.rxFilterBandwidth = rxFilterBandwidth;
    rConf.rxLOoffset = rxLOoffset;

    rConf.txCarrierFrequency = txCarrierFrequency;
    rConf.txSamplingRate = txSamplingRate;

    rConf.rxCarrierFrequency = rxCarrierFrequency;
    rConf.rxSamplingRate = rxSamplingRate;


    bool ok;
    stagedConfiguration["rx-carrier-frequency"]->setProperty(rxCarrierFrequency,ok);
    stagedConfiguration["tx-carrier-frequency"]->setProperty(txCarrierFrequency,ok);
    stagedConfiguration["rx-sampling-rate"]->setProperty(rxSamplingRate,ok);
    stagedConfiguration["tx-sampling-rate"]->setProperty(txSamplingRate,ok);
    stagedConfiguration["pps-source"]->setProperty(ppsSource,ok);
    stagedConfiguration["ref-source"]->setProperty(refSource,ok);
    stagedConfiguration["rx-antenna"]->setProperty(rxAntenna,ok);
    stagedConfiguration["tx-antenna"]->setProperty(txAntenna,ok);
    stagedConfiguration["tx-gain"]->setProperty(txGain,ok);
    stagedConfiguration["rx-gain"]->setProperty(rxGain,ok);
    stagedConfiguration["tx-filter-bandwidth"]->setProperty(txFilterBandwidth,ok);
    stagedConfiguration["rx-filter-bandwidth"]->setProperty(rxFilterBandwidth,ok);
    stagedConfiguration["rx-lo-offset"]->setProperty(rxLOoffset,ok);
    stagedConfiguration["tx-lo-offset"]->setProperty(txLOoffset,ok);

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
                   rConf.rxLOoffset,
                   rConf.internalRxBufferSize);
    f_out.close();

    return response;
}

void cRadioObject::runContinousReceptionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> rxCircBuffer, uhd::usrp::multi_usrp::sptr m_usrp)
{
    uhd::stream_args_t stream_args("sc16","sc16");
    stream_args.channels = {0};
    uhd::rx_streamer::sptr rx_stream = m_usrp->get_rx_stream(stream_args);



    using smplType = std::complex<short>;
    size_t smplsPerBuffer = 1472;

    smplType * rxBuffer;
    bool isAllocated = false;
    try{
        rxBuffer = new smplType[smplsPerBuffer];
        isAllocated = true;
    }catch(std::bad_alloc& exc){
        UHD_LOGGER_ERROR("UHD") << "Bad memory allocation";
        std::exit(EXIT_FAILURE);
    }

    uhd::rx_metadata_t md;
    md.has_time_spec = false;

    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    stream_cmd.num_samps = size_t(0);
    stream_cmd.stream_now = 1;
    //stream_cmd.time_spec = uhd::time_spec_t(1);
    rx_stream->issue_stream_cmd(stream_cmd);

    continous_reception_running = true;
    while(stop_continous_reception == false){

        size_t numRxSmpls = rx_stream->recv(rxBuffer,smplsPerBuffer,md,5.1,false);

        if(md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT){
            std::cout << "Timeout while streaming" << std::endl;
            if(isAllocated == true){
                delete [] rxBuffer;
                isAllocated = false;
            }
            break;
        }

        for(size_t smpl_i =0;smpl_i<smplsPerBuffer;smpl_i++){
            rxCircBuffer->push(rxBuffer[smpl_i]);
        }
    }

    uhd::stream_cmd_t stop_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);

    usrp->issue_stream_cmd(stop_cmd);

    continous_reception_running = false;
}

cRadioResponse cRadioObject::startContinousTransmission()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "Success";

    if(continous_transmission_running == true){
        response.code = -1;
        response.message = "Error; Continous Transmission already running";
    }else if(txSignalLoaded == false){
        response.code = -3;
        response.message = "Error; No Tx signal specified";
    }else if(isConfigured()){
        stop_continous_transmission = false;
        std::thread continousTransmissionThread(&cRadioObject::runContinousTransmissionProcess,this,internalTxCircBuffer,usrp);
        continousTransmissionThread.detach();
    }else{
        response.code = -2;
        response.message = "Error; Radio not yet configured";
    }

    return response;
}

cRadioResponse cRadioObject::stopContinousTransmission()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "Success";

    stop_continous_transmission = true;

    int m_wait = 10;
    int i =0;
    do{
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        if(i++ > m_wait){
            response.code = -1;
            response.message = "Error, transmission could not be terminated...";
            return response;
        }
    }while(continous_transmission_running == true);

    return response;
}

void cRadioObject::runContinousTransmissionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> txCircBuffer,uhd::usrp::multi_usrp::sptr m_usrp)
{

    if(timed_transmission_running == true){
        return; // cannot run if timed transmission is ongoing
    }

    int txTimeoutCount = 0;
    const int txTimeoutMax = 1e3;
    continous_transmission_running = true;

    const std::vector<size_t> channel_nums = {0};
    const size_t total_num_channels = 1;
    size_t smpl_per_buffer = 1472;

    uhd::stream_args_t stream_args("sc16","sc16");
    stream_args.channels = channel_nums;
    uhd::tx_streamer::sptr tx_stream = m_usrp->get_tx_stream(stream_args);

    uhd::tx_metadata_t md;
    md.start_of_burst = true;
    md.end_of_burst = false;
    md.has_time_spec = false;

    using smpl_type = std::complex<short>;

    smpl_type c_smpl;

    if(channel_nums.size() == 1){
        std::vector<std::complex<short>> buffer(smpl_per_buffer);
        while(stop_continous_transmission == false){
            for(int ik=0;ik<smpl_per_buffer;ik++){
                txCircBuffer->circ_pop(&c_smpl);
                buffer[ik] = c_smpl;
            }
            size_t num_tx_smpls = smpl_per_buffer;
            const size_t smpls_sent = tx_stream->send(&buffer.front(),num_tx_smpls,md,5.3);

            if(smpls_sent != num_tx_smpls && ++txTimeoutCount > txTimeoutMax){
                UHD_LOG_ERROR("TX-STREAM",
                              "The tx_stream timed out sending " << num_tx_smpls << " samples ("
                                                                 << smpls_sent << " sent)");
                continous_transmission_running = false;
                std::cout << "Transmission thread terminated..." << std::endl;
                return;
            }
        }
    }else{
        throw std::runtime_error("Multiple Tx channels not yet supported");
    }
    continous_transmission_running = false;
}

cRadioResponse cRadioObject::startTimedTransmission()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    std::thread timedTransmissionThread(&cRadioObject::runTimedTransmissionProcess,this,internalTxCircBuffer,usrp);
    timedTransmissionThread.detach();

    return response;
}

void cRadioObject::runTimedTransmissionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> txCircBuffer, uhd::usrp::multi_usrp::sptr m_usrp)
{
    if(continous_transmission_running == true){
        return; // cannot run timed transmission if contrinous transmission is already ongoing
    }

    const std::vector<size_t> channel_nums = {0};

    timed_transmission_running = true;

    uhd::stream_args_t stream_args("sc16","sc16");

    stream_args.channels = channel_nums;

    uhd::time_spec_t zero_time_spec(0.0);
    //m_usrp->set_time_next_pps(zero_time_spec);
    m_usrp->set_time_now(zero_time_spec);

    uhd::tx_metadata_t tx_meta;
    tx_meta.has_time_spec = true;

    uhd::time_spec_t tx_time_spec(0,0.2);

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    tx_meta.time_spec = tx_time_spec;

    uhd::tx_streamer::sptr tx_stream = m_usrp->get_tx_stream(stream_args);

    size_t N = 131072;//4096;

    std::complex<short> c_smpl;

    std::vector<std::complex<short>> buffer(N);
    for(int ik=0;ik<N;ik++){
        txCircBuffer->circ_pop(&c_smpl);
        buffer[ik] = c_smpl;
    }

    const size_t smpls_sent = tx_stream->send(&buffer.front(),N,tx_meta,0.3);

    std::cout << m_usrp->get_time_now().get_full_secs() << std::endl;

    if(smpls_sent == N){
        std::cout << "All samples sent" << std::endl;
    }else{
        std::cout << "Not all samples sent: " << N-smpls_sent << std::endl;
    }

    timed_transmission_running = false;

}

std::vector<std::complex<short>> cRadioObject::getLastReceivedSamples(size_t N, cRadioResponse &response)
{
    std::vector<std::complex<short>> result_v;

    size_t pushCount = internalRxCircBuffer->get_push_count();

    if(internalRxCircBuffer->get_capacity() <= N){
        response.message = "Internal Rx buffer capacity must exceed " + std::to_string(N);
        response.code = -2;
        return result_v;
    }

    if(pushCount < N){
        response.message = "Not enough samples in buffer";
        response.code = -1;
        return result_v;
    }

    if(internalRxCircBuffer->check_overwritten(pushCount-N)){
        response.message = "Desired sample overwritten";
        response.code = -3;
    }

     result_v = internalRxCircBuffer->extract_range(pushCount-N,N);

    return result_v;
}

bool cRadioObject::isConfigurationsEqual(cRadioConfiguration radr, cRadioConfiguration radl)
{
    bool isEqual = true;

    isEqual = isEqual && (radr.ppsSource == radl.ppsSource);
    isEqual = isEqual && (radr.refSource == radl.refSource);
    isEqual = isEqual && (radr.rxSamplingRate == radl.rxSamplingRate);
    isEqual = isEqual && (radr.rxCarrierFrequency == radl.rxCarrierFrequency);
    isEqual = isEqual && (radr.rxGain == radl.rxGain);
    isEqual = isEqual && (radr.rxLOoffset == radl.rxLOoffset);
    isEqual = isEqual && (radr.rxAntenna == radl.rxAntenna);
    isEqual = isEqual && (radr.rxFilterBandwidth == radl.rxFilterBandwidth);
    isEqual = isEqual && (radr.txSamplingRate == radl.txSamplingRate);
    isEqual = isEqual && (radr.txCarrierFrequency == radl.txCarrierFrequency);
    isEqual = isEqual && (radr.txGain == radl.txGain);
    isEqual = isEqual && (radr.txLOoffset == radl.txLOoffset);
    isEqual = isEqual && (radr.txAntenna == radl.txAntenna);
    isEqual = isEqual && (radr.txFilterBandwidth == radl.txFilterBandwidth);

    return isEqual;
}

cRadioResponse cRadioObject::setTransmitSignal(std::vector<std::complex<short> > &txSignal)
{
    cRadioResponse response;
    response.message = "Success";
    response.code = 0;

    if(txSignalLoaded == false){
        internalTxCircBuffer = std::make_shared<CircBuffer<std::complex<short>>>(1e4);
    }

    try{
    internalTxCircBuffer->from_vector(txSignal);
    }catch(...){
        response.message = "Could not set transmit signal";
        response.code = -1;
    }

    if(response.code == 0){
        txSignalLoaded = true;
    }

    return response;
}
