#include "cradioobject.h"
#include <filesystem>
#include <fstream>
#include "custom/config_file.h"


cRadioObject::cRadioObject(std::string serial_p, std::string type_p, std::string addr_p)
{
    serial = serial_p;
    type = type_p;
    address = addr_p;
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

    if(isLoadedConfiguration == false){
        response.code = -1;
        response.message = "No configuration has been loaded";
        return response;
    }else{
        response = runRadioConfigurationProcess();
    }

    if(response.code == 0){
        isRadioConfigured = true;
        internalRxCircBuffer = std::make_shared<CircBuffer<std::complex<short>>>(rConf.internalRxBufferSize);
    }
    return response;
}

cRadioResponse cRadioObject::runRadioConfigurationProcess()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

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
    uhd::tune_request_t txTuneRequest(txCarrierFrequency,rxLO_offset);
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

cRadioResponse cRadioObject::loadRadioConfigurationFile(bool def, std::string filepath)
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";

    std::string filepath_p = "";
    if(def == true){
        // get default configuration file
        filepath_p = "usrp_default_configs/" + type + ".cfg";
    }

    response = readConfigurationFile(filepath_p);

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

    int m_wait = 10;
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
        response.message = "Error; Cannot start, continous reception already running";
    }else if(isConfigured()){
        stop_continous_reception = false;
        std::thread continousReceptionThread(&cRadioObject::runContinousReceptionProcess,this,internalRxCircBuffer,usrp);
        continousReceptionThread.detach();
    }else{
        response.code = -2;
        response.message = "Error; Cannot start reception as radio is not yet configured";
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
                      rConf.rxLOoffset,
                      rConf.internalRxBufferSize);

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
                   rConf.rxLOoffset,
                   rConf.internalRxBufferSize);
    f_out.close();

    return response;
}

void cRadioObject::runContinousReceptionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> rxCircBuffer,uhd::usrp::multi_usrp::sptr m_usrp)
{
    uhd::stream_args_t stream_args("sc16","sc16");
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
    continous_reception_running = false;
}
