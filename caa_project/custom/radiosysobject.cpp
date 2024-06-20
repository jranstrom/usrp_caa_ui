#include "radiosysobject.h"
#include "RadioSysConfig.h"
#include "config_file.h"
#include "../circbuffer.h"
#include "matHF.h"
#include "uhd_clib.h"

#include <chrono>
#include <complex>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>
#include <cmath>
#include <stdexcept>

#include <uhd/types/tune_request.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/static.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std::chrono_literals;

RadioSysObject::RadioSysObject() : syncPointBuffer(1e3),frequencyOffsetBuffer(1e3) {
    std::cout<<"RadioSysObject created" << std::endl;

    int response = requestFrameCaptureFormat(frameOffset,frameLength,numFrameCaptures); // set default capture format

}

bool RadioSysObject::writeConfigFile(std::string configFilepath){
    bool success = false;
    std::ofstream f_out(configFilepath);

    std::vector<std::string> ln = {"tx-address","rx-address",
        "tx-freq","tx-rate","tx-gain","tx-ant","tx-time-source","tx-clock-source","tx-file","f-tx-bw","tx-buffer-size",
        "rx-freq","rx-rate","rx-gain","rx-ant","rx-time-source","rx-clock-source","rx-lo-offset","rx-file","f-rx-bw","rx-buffer-size",
        "tx-rx-time-offset","default-command-list","teensy-master-port","type"
    };

    if(! f_out)
        std::cout << "Error writing file !" << std::endl;
    else
    {
        std::string tmp_txIP = sysConf.GetCleanTX_IPaddress();
        std::string tmp_rxIP = sysConf.GetCleanRX_IPaddress();
        CFG::WriteFile(f_out, ln,
                       sysConf.tx.IP_Address,
                       sysConf.rx.IP_Address,
                       sysConf.tx.CarrierFrequency,
                       sysConf.tx.SamplingRate,
                       sysConf.tx.Gain,
                       sysConf.tx.ANT,
                       sysConf.tx.PPS_Source,
                       sysConf.tx.REF_Source,
                       sysConf.tx.File,
                       sysConf.tx.FILT_BW,
                       sysConf.tx.BufferSize,
                       sysConf.rx.CarrierFrequency,
                       sysConf.rx.SamplingRate,
                       sysConf.rx.Gain,
                       sysConf.rx.ANT,
                       sysConf.rx.PPS_Source,
                       sysConf.rx.REF_Source,
                       sysConf.rx.RX_LO_Offset,
                       sysConf.rx.File,
                       sysConf.rx.FILT_BW,
                       sysConf.rx.BufferSize,
                       sysConf.txRELrxTimeOffset,
                       sysConf.default_command_list,
                       "",
                       sysConf.confFile.type);
        f_out.close();
        success = true;
    }

    return success;
}

bool RadioSysObject::readConfigFile(std::string configFilepath)
{
    bool success = false;
    std::vector<std::string> ln = {"tx-address","rx-address",
        "tx-freq","tx-rate","tx-gain","tx-ant","tx-time-source","tx-clock-source","tx-file","f-tx-bw","tx-buffer-size",
        "rx-freq","rx-rate","rx-gain","rx-ant","rx-time-source","rx-clock-source","rx-lo-offset","rx-file","f-rx-bw","rx-buffer-size",
        "tx-rx-time-offset","default-command-list","teensy-master-port","type"
    };

    std::string tmp_rx_address;
    std::string tmp_tx_address;
    std::string desired_teensy_master_port;
    try{
        std::ifstream iconf_file(configFilepath);
        CFG::ReadFile(iconf_file,ln,
                      sysConf.tx.IP_Address,
                      sysConf.rx.IP_Address,
                      sysConf.tx.CarrierFrequency,
                      sysConf.tx.SamplingRate,
                      sysConf.tx.Gain,
                      sysConf.tx.ANT,
                      sysConf.tx.PPS_Source,
                      sysConf.tx.REF_Source,
                      sysConf.tx.File,
                      sysConf.tx.FILT_BW,
                      sysConf.tx.BufferSize,
                      sysConf.rx.CarrierFrequency,
                      sysConf.rx.SamplingRate,
                      sysConf.rx.Gain,
                      sysConf.rx.ANT,
                      sysConf.rx.PPS_Source,
                      sysConf.rx.REF_Source,
                      sysConf.rx.RX_LO_Offset,
                      sysConf.rx.File,
                      sysConf.rx.FILT_BW,
                      sysConf.rx.BufferSize,
                      sysConf.txRELrxTimeOffset,
                      sysConf.default_command_list,
                      desired_teensy_master_port,
                      sysConf.confFile.type);
        iconf_file.close();

        sysConf.SetrxUSRP_IPaddress(tmp_rx_address);
        sysConf.SettxUSRP_IPaddress(tmp_tx_address);

        success = true;
    } catch(...){

    }



    return success;

}
bool RadioSysObject::readConfigSignalFile(std::string filepath)
{
    bool success = true;
    try{
        sysConf.SetNfft(matHF::read_double(filepath,"sys.Nfft"));
        sysConf.SetSamplingRate(matHF::read_double(filepath,"sys.fs"));
        sysConf.SetGuardLengths(matHF::read_as_int_vec(filepath,"sys.guardLengths"));
        sysConf.SetSymbolLengths(matHF::read_as_int_vec(filepath,"sys.symbolLengths"));

        sysConf.SetSignalPattern(matHF::read_single_char_cell_array(filepath,"sys.sigPattern"));
        sysConf.SetSynchCorrelationPattern(matHF::read_as_int_vec(filepath,"sys.synchCorrFormat"));

        sysConf.SetIndexGrid(matHF::read_as_int_mat(filepath,"sys.iGrid"));

        sysConf.SetTransmitSignal(matHF::read_complex_double_vec(filepath,"sys.txSignal"));
        sysConf.SetSynchSignal(matHF::read_complex_double_vec(filepath,"sys.tSynch"));

        sysConf.SetSynchOffset(static_cast<size_t>(matHF::read_double(filepath,"sys.synchOffset")));
        sysConf.SetSynchTolerance(matHF::read_as_int(filepath,"sys.synchCorrTolerance"));


        sysConf.csvPattern = matHF::read_as_int_mat("csv_last_params.mat","sys.csvPattern");
        sysConf.csvPattern = uhd_clib::transpose_int_mat(sysConf.csvPattern);

        //CircBuffer<std::complex<short>> tx_signal_buff = uhd_clib::cvec_conv_double2short(sysConf.txSignal);
        txSignalBuffer.from_vector(uhd_clib::cvec_conv_double2short(sysConf.txSignal));
        txSynchSignalBuffer.from_vector(uhd_clib::cvec_conv_double2short(sysConf.synchSignal));
        //txSignalBuffer = &tx_signal_buff;
    }catch(...){
        success = false;
    }

    try{
        sysConf.SetSupportWindowSynch(static_cast<int>(matHF::read_double(filepath,"sys.supportWindowSynch")));
        sysConf.SetWindowSynchronizationSize(matHF::read_as_int(filepath,"sys.windowSynchSize"));
    }catch(...){

    }

    try{
        sysConf.SetDataModulationType(matHF::read_string(filepath,"sys.dataModulation"));
    }catch(...){

    }

    try{
        sysConf.SetDate(matHF::read_string(filepath,"sys.date"));
    }catch(...){

    }

    try{
        sysConf.SetSignalType(matHF::read_string(filepath,"sys.signalType"));
    }catch(...){

    }

    return success;
}

void RadioSysObject::StopAll()
{
    //std::cout << "Transmission in progress: " << TransmissionInProgress << std::endl;
    stop_transmit_signal_called = true;
    stop_reception_signal_called = true;
    stop_synchronization_signal_called = true;
    std::this_thread::sleep_for(1s);
    stop_transmit_signal_called = false;
    stop_reception_signal_called = false;
    stop_synchronization_signal_called = false;
    std::cout << "Terminated All" <<std::endl;
    //std::cout << "Transmission in progress: " << TransmissionInProgress << std::endl;
}

bool RadioSysObject::startTransmission()
{
    if(TransmissionInProgress){
        return false;
    }

    TransmissionInProgress = true;

    txSignalBuffer.reset_buffer();

    std::thread txThread(&RadioSysObject::runTransmissionThread,this);

    // Detach the thread from the main thread
    txThread.detach();

    return true;
}

bool RadioSysObject::startReception()
{
    if(ReceptionInProgress){
        return false;
    }

    ReceptionInProgress = true;

    rxSignalBuffer.reset_buffer();

    std::thread rxThread(&RadioSysObject::runReceptionThread,this);

    rxThread.detach();

    return true;
}

bool RadioSysObject::stopTransmission()
{
    stop_transmit_signal_called = true;
    std::this_thread::sleep_for(1s);
    stop_transmit_signal_called = false;
    std::cout << "Transmission in progress: " << isTransmitting() << std::endl;

    return true;
}

bool RadioSysObject::stopReception()
{
    if(SynchronizationInProgress){
        stopSynchronization();
    }
    stop_reception_signal_called = true;
    std::this_thread::sleep_for(1s);
    stop_reception_signal_called = false;
    std::cout << "Reception in progress: " << isReceiving() << std::endl;

    return true;
}

bool RadioSysObject::startSynchronization()
{
    if(!ReceptionInProgress || SynchronizationInProgress){
        return false;
    }

    std::thread synchronizationThread(&RadioSysObject::runSynchronizationThread,this);

    // Detach the thread from the main thread
    synchronizationThread.detach();

    return true;
}

bool RadioSysObject::stopSynchronization()
{
    stop_synchronization_signal_called = true;
    std::this_thread::sleep_for(1s);
    stop_synchronization_signal_called = false;
    return true;
}

bool RadioSysObject::isTransmitting()
{
    return TransmissionInProgress;
}

bool RadioSysObject::isReceiving()
{
    return ReceptionInProgress;
}

bool RadioSysObject::isSynchronizing(){
    return SynchronizationInProgress;
}

void RadioSysObject::requestTxUSRPSetup()
{
    txSetupInProgress = true;
    rxUSRPConfigured = false;
    if(!isTransmitting()){
        std::thread txSetupThread(&RadioSysObject::setupTxUSRP,this);

        txSetupThread.detach();
    }else{
        txSetupInProgress = false;
    }
}

void RadioSysObject::requestRxUSRPSetup()
{
    rxSetupInProgress = true;
    rxUSRPConfigured = false;
    if(!isReceiving()){
        std::thread rxSetupThread(&RadioSysObject::setupRxUSRP,this);

        rxSetupThread.detach();
    }else{
        rxSetupInProgress = false;
    }
}

void RadioSysObject::requestWriteBufferToFile(std::string completeFilepath,int count)
{
    size_t idx_value;
    if(syncPointBuffer.back(&idx_value) > 0){
        std::cout << "Last synch index: " << idx_value << std::endl;
    }
    if(stopReception() && not writingBufferInProgress){
        writingBufferInProgress = true;
        rxCaptureFilepath = completeFilepath;
        std::thread writeBufferThread(&RadioSysObject::writeBufferToFile,this, count);
        writeBufferThread.detach();
    }else{
        return;
    }
}

void RadioSysObject::setupTxUSRP()
{
    std::cout << sysConf.GettxUSRP_IPaddress() << std::endl;

    tx_usrp = uhd::usrp::multi_usrp::make(sysConf.GettxUSRP_IPaddress());

    tx_usrp->set_clock_source(sysConf.tx.REF_Source); // Clock (REF)
    tx_usrp->set_time_source(sysConf.tx.PPS_Source);  // Timing (PPS)

    std::vector<std::string> channel_strings;
    std::vector<size_t> channel_nums;
    boost::split(channel_strings,sysConf.tx.CH,boost::is_any_of("\"',"));
    for (size_t ch=0; ch<channel_strings.size();ch++) {
        size_t chan = std::stoi(channel_strings[ch]);

        size_t chan_act;

        chan_act = tx_usrp->get_tx_num_channels();

        if(chan >= chan_act){
            throw std::runtime_error("Invalid channel(s) specified.");
        }else{
            channel_nums.push_back(std::stoi(channel_strings[ch]));
        }
    }

    // Set Sampling Rate
    double actual_SamplingRate;

    tx_usrp->set_tx_rate(sysConf.tx.SamplingRate);
    actual_SamplingRate = tx_usrp->get_tx_rate();

    if(sysConf.tx.SamplingRate != actual_SamplingRate)
        std::cout << boost::format("Actual Rate: %f Msps\n") % ((actual_SamplingRate)/1e6);

    for (size_t ch=0; ch<channel_nums.size(); ch++){
        size_t channel = channel_nums[ch];

        if (channel_nums.size() > 1)
            std::cout << "Configuring Channel" << channel << std::endl;

        // Set carrier frequency
        double actual_CarrierFrequency;
        uhd::tune_request_t tx_tune_request(sysConf.tx.CarrierFrequency, sysConf.rx.RX_LO_Offset);
        tx_usrp->set_tx_freq(tx_tune_request, channel);
        actual_CarrierFrequency = tx_usrp->get_tx_freq(channel);

        if(sysConf.tx.CarrierFrequency != actual_CarrierFrequency)
            std::cout << boost::format("Desired Carrier freq: %.9f MHz \n") % (sysConf.tx.CarrierFrequency / 1e6);
            std::cout << boost::format("Actual Carrier freq: %.9f MHz\n") % (actual_CarrierFrequency / 1e6);

        // Set gain
        double actual_gain;
        tx_usrp->set_tx_gain(sysConf.tx.Gain,channel);
        actual_gain = tx_usrp->get_tx_gain(channel);

        if(sysConf.tx.Gain != actual_gain)
            std::cout << boost::format("Actual gain: %.2f dB\n") % actual_gain;

        // Set antenna
        tx_usrp->set_tx_antenna(sysConf.tx.ANT,channel);


        // Set Frontend Filter Bandwidth
        double actual_filt_bw;

        tx_usrp->set_tx_bandwidth(sysConf.tx.FILT_BW, channel);
        actual_filt_bw = tx_usrp->get_tx_bandwidth(channel);


        if(sysConf.tx.FILT_BW != actual_filt_bw)
            std::cout << boost::format("Actual TX analog frontend filter bandwidth %.2f MHz\n")
                             % (actual_filt_bw / 1e6);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::vector<std::string> sensor_names;

        sensor_names = tx_usrp->get_tx_sensor_names(0);

        if(std::find(sensor_names.begin(), sensor_names.end(),"lo_locked") != sensor_names.end()) {
            uhd::sensor_value_t lo_locked = tx_usrp->get_tx_sensor("lo_locked",0);
            std::cout << boost::format("Checking TX: %s ..." ) % lo_locked.to_pp_string() << std::endl;
            UHD_ASSERT_THROW(lo_locked.to_bool());
        }
    }

    txUSRPConfigured = true;
    txSetupInProgress = false;
}

void RadioSysObject::setupRxUSRP()
{
    std::cout << sysConf.getRxIPAddress() << std::endl;

    rx_usrp = uhd::usrp::multi_usrp::make(sysConf.GetrxUSRP_IPaddress());

    rx_usrp->set_clock_source(sysConf.rx.REF_Source); // Clock (REF)
    rx_usrp->set_time_source(sysConf.rx.PPS_Source);  // Timing (PPS)

    std::vector<std::string> channel_strings;
    std::vector<size_t> channel_nums;
    boost::split(channel_strings,sysConf.rx.CH,boost::is_any_of("\"',"));
    for (size_t ch=0; ch<channel_strings.size();ch++) {
        size_t chan = std::stoi(channel_strings[ch]);

        size_t chan_act;

        chan_act = rx_usrp->get_rx_num_channels();

        if(chan >= chan_act){
            throw std::runtime_error("Invalid channel(s) specified.");
        }else{
            channel_nums.push_back(std::stoi(channel_strings[ch]));
        }
    }

    double actual_SamplingRate;

    rx_usrp->set_rx_rate(sysConf.rx.SamplingRate);
    actual_SamplingRate = rx_usrp->get_rx_rate();

    if(sysConf.tx.SamplingRate != actual_SamplingRate)
        std::cout << boost::format("Actual Rate: %f Msps\n") % ((actual_SamplingRate)/1e6);

    for (size_t ch=0; ch<channel_nums.size(); ch++){
        size_t channel = channel_nums[ch];

        if (channel_nums.size() > 1)
            std::cout << "Configuring Channel" << channel << std::endl;

        // Set carrier frequency
        double actual_CarrierFrequency;
        uhd::tune_request_t rx_tune_request(sysConf.rx.CarrierFrequency, sysConf.rx.RX_LO_Offset);
        rx_usrp->set_rx_freq(rx_tune_request, channel);
        actual_CarrierFrequency = rx_usrp->get_rx_freq(channel);

        if(sysConf.rx.CarrierFrequency != actual_CarrierFrequency)
            std::cout << boost::format("Desired Carrier freq: %.9f MHz \n") % (sysConf.rx.CarrierFrequency / 1e6);
            std::cout << boost::format("Actual Carrier freq: %.2f MHz\n") % (actual_CarrierFrequency / 1e6);

        // Set gain
        double actual_gain;
        rx_usrp->set_rx_gain(sysConf.rx.Gain,channel);
        actual_gain = rx_usrp->get_rx_gain(channel);

        if(sysConf.rx.Gain != actual_gain)
            std::cout << boost::format("Actual gain: %.2f dB\n") % actual_gain;

        // Set antenna
        rx_usrp->set_rx_antenna(sysConf.rx.ANT,channel);


        // Set Frontend Filter Bandwidth
        double actual_filt_bw;

        rx_usrp->set_rx_bandwidth(sysConf.rx.FILT_BW, channel);
        actual_filt_bw = rx_usrp->get_rx_bandwidth(channel);


        if(sysConf.rx.FILT_BW != actual_filt_bw)
            std::cout << boost::format("Actual RX analog frontend filter bandwidth %.2f MHz\n")
                             % (actual_filt_bw / 1e6);

        // Align times in the RX USRP
        if (rx_usrp->get_num_mboards() > 1){
            rx_usrp->set_time_unknown_pps(uhd::time_spec_t(0.0));
            std::cout << ">> RX Time Alignment" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::vector<std::string> sensor_names;

        sensor_names = rx_usrp->get_rx_sensor_names(0);

        if(std::find(sensor_names.begin(), sensor_names.end(),"lo_locked") != sensor_names.end()) {
            uhd::sensor_value_t lo_locked = rx_usrp->get_rx_sensor("lo_locked",0);
            std::cout << boost::format("Checking RX: %s ..." ) % lo_locked.to_pp_string() << std::endl;
            UHD_ASSERT_THROW(lo_locked.to_bool());
        }
    }

    rxUSRPConfigured = true;
    rxSetupInProgress = false;

}

void RadioSysObject::extractFrameSequence(int offset,int length)
{

    // Extract latest synched frame
    // If overwritten, then this function should await next frame synch
    size_t rx_index;
    double foc = 0.0;
    std::chrono::nanoseconds duration(5);

    // Loop while no valid synch point (exist and is not overwritten)
    bool synchPoint_exist = syncPointBuffer.back(&rx_index,1);
    bool isOverwritten = false;
    bool isStale = false;

    size_t signal_length = sysConf.txSignal.size();
    size_t synchTolerance = 50; // measured in frames
    if(synchPoint_exist){
        size_t dist_in_frames = (rxSignalBuffer.get_push_count() - rx_index) /  signal_length;
        // Check if stale
        if(dist_in_frames > synchTolerance){
            isStale = true;
        }else{
            rx_index + dist_in_frames*signal_length;
        }
     isOverwritten = rxSignalBuffer.check_overwritten(rx_index);
    }
    while(!synchPoint_exist || isOverwritten || isStale){
        //frequencyOffsetBuffer.back(&foc,1);
        std::this_thread::sleep_for(duration);
        synchPoint_exist = syncPointBuffer.back(&rx_index);
        isOverwritten = false;
        if(synchPoint_exist){
            size_t dist_in_frames = (rxSignalBuffer.get_push_count() - rx_index) /  signal_length;
            // Check if stale
            if(dist_in_frames > synchTolerance){
                isStale = true;
            }else{
                isStale = false;
                rx_index + dist_in_frames*signal_length;
            }
            isOverwritten = rxSignalBuffer.check_overwritten(rx_index);
        }
    }

    int frame_length = 0;

    if(length > 0){
        frame_length = length;
    }else{
        for(int i=0;i<sysConf.winfo.numSymbols; ++i){
            frame_length += sysConf.winfo.guardLengths[i];
            frame_length += sysConf.winfo.symbolLengths[i];
        }
    }

    // size_t synchOffset = sysConf.synchOffset;

    //rx_index -= synchOffset;        // adjust for synch point not being start of frame
    if(offset > -1){
        rx_index += offset;
    }
    // int fart = rxSignalBuffer.get_internal_buff_index(rx_index);
    // std::cout << " :: " << fart << "(internal)" << std::endl;
    extracted_synch_data =  rxSignalBuffer.extract_range(rx_index,frame_length);
    // double dt = 1/sysConf.rx.SamplingRate;
    // double pi = M_PI;
    // std::complex<double> j(0.0,1.0);

    // std::vector<std::complex<double>> tmp_cvec =  uhd_clib::cvec_conv_short2double(extracted_synch_data);
    // for(int ik=0; ik<extracted_synch_data.size();ik++){
    //     tmp_cvec[ik] = tmp_cvec[ik]*std::exp(2.0*j*pi*foc*dt*(double)ik);
    // }

    //extracted_synch_data = uhd_clib::cvec_conv_double2short(tmp_cvec);
}

void RadioSysObject::runTransmissionThread()
{

    txTimeoutCount = 0;
    TransmissionInProgress = true;

    const std::vector<size_t>channel_nums = {0};
    const size_t total_num_channels = 1;
    size_t samps_per_buff = sysConf.tx.BufferSize;

    uhd::stream_args_t stream_args(sysConf.tx.cpu_format,sysConf.tx.wirefmt);
    stream_args.channels = channel_nums;
    uhd::tx_streamer::sptr tx_stream = tx_usrp->get_tx_stream(stream_args);

    double time_offset = timeOffset;

    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    std::chrono::milliseconds delay(100);
    //std::chrono::system_clock::time_point targetTime = currentTime + delay;

    bool continous_stream   = true;

    uhd::tx_metadata_t md;
    md.start_of_burst = true;
    md.end_of_burst = false;
    md.has_time_spec  = true;
    if (time_offset > 0){
        time_offset = 0.0;
    }else{
        time_offset = -1*time_offset;
    }

    //uhd::time_spec_t c_time = tx_usrp->get_time_now(0);
    md.time_spec      = uhd::time_spec_t(3+time_offset);
    md.has_time_spec  = false;
    using samp_type = std::complex<short>;

    samp_type c_smpl;

    if (channel_nums.size() == 1) {
        std::vector<std::complex<short>> buff(samps_per_buff);
        bool file_read = false;
        while(not stop_transmit_signal_called and (continous_stream or not file_read)){

            for(int ik=0;ik<samps_per_buff;ik++){
                txSignalBuffer.circ_pop(&c_smpl);
                buff[ik] = c_smpl;
            }
            size_t num_tx_samps = samps_per_buff;

            const size_t samples_sent = tx_stream->send(&buff.front(),num_tx_samps,md,5.3);

            if(samples_sent != num_tx_samps && ++txTimeoutCount > txTimeoutMax){
                UHD_LOG_ERROR("TX-STREAM",
                              "The tx_stream timed out sending " << num_tx_samps << " samples ("
                                                                 << samples_sent << " sent)");
                TransmissionInProgress = false;
                std::cout << "Transmission thread terminated..." << std::endl;
                return;
            }

        }
    } else {
        throw std::runtime_error("Multiple TX channels not yet supported");
    }
    std::cout << "Transmission thread terminated..." << std::endl;
    TransmissionInProgress = false;
}

void RadioSysObject::runReceptionThread()
{
    ReceptionInProgress = true;

    int max_num_smpls = 0;
    rxSampleCount = 0;

    double time_offset = timeOffset;

    uhd::stream_args_t stream_args(sysConf.confFile.cpu_format,sysConf.confFile.wirefmt);
    uhd::rx_streamer::sptr rx_stream = rx_usrp->get_rx_stream(stream_args);

    uhd::rx_metadata_t md;
    md.has_time_spec = false;
    //uhd::time_spec_t c_time = rx_usrp->get_time_now(0);
    md.time_spec = uhd::time_spec_t(3+time_offset);

    using samp_type = std::complex<short>;

    size_t samps_per_buff = sysConf.rx.BufferSize;

    samp_type* buff;
    bool buff_Allocated = false;
    try{
        buff = new samp_type[samps_per_buff];
        buff_Allocated = true;
    }catch(std::bad_alloc& exc){
        UHD_LOGGER_ERROR("UHD")
            << "Bad memory allocation. "
               "Try a smaller samples per buffer setting or free up additional memory";
        ReceptionInProgress = false;
        std::exit(EXIT_FAILURE);
    }

    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    stream_cmd.num_samps = size_t(0);
    stream_cmd.stream_now = 1;
    //usrp->set_time_now(uhd::time_spec_t(0.0),0);
    stream_cmd.time_spec = uhd::time_spec_t(3+time_offset);
    rx_stream->issue_stream_cmd(stream_cmd);

    size_t tot_num_smpls = 0;

    std::vector<double> ic_time_stamp;;
    double last_time_stamp = 0.0;

    size_t k = 0;
    while(not stop_reception_signal_called and (tot_num_smpls < max_num_smpls or max_num_smpls == 0)){
        //const auto now = std::chrono::steady_clock::now();
        size_t num_rx_samps =
            rx_stream->recv(buff,samps_per_buff,md,5.1,false);

        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
            std::cout << std::endl
                      << "Timeout while streaming" << std::endl;
            if(buff_Allocated){
                delete[] buff;
                buff_Allocated = false;
                ReceptionInProgress = false;
            }
            break;
        }

        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
            const std::lock_guard<std::mutex> lock(recv_mutex);
                std::cerr
                    << boost::format(
                           "Got an overflow indication. Please consider the following:\n"
                           "  Your write medium must sustain a rate of %0.3fMB/s.\n"
                           "  Dropped samples will not be written to the file.\n"
                           "  Please modify this example for your purposes.\n"
                           "  This message will not appear again.\n")
                           % (sysConf.rx.SamplingRate*sizeof(samp_type) / 1e6);
            continue;
        }

        bool continue_on_bad_packet = true;
        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            const std::lock_guard<std::mutex> lock(recv_mutex);
            std::string error = "Receiver error: " + md.strerror();
            if (continue_on_bad_packet) {
                std::cerr << error << std::endl;
                continue;
            } else
                if(buff_Allocated){
                    delete[] buff;
                    buff_Allocated = false;
                    ReceptionInProgress = false;
                }
                throw std::runtime_error(error);
        }

        // double c_time_stamp = time_spec2double(md.time_spec);
        // if(k > 0){
        //     double time_diff = std::round(sample_rate*(c_time_stamp-last_time_stamp));
        //     if (abs(abs(time_diff)-samps_per_buff) > 0){
        //         ic_time_stamp.push_back(time_diff);
        //     }
        // }

        // last_time_stamp = c_time_stamp;

        ++k;

        for(size_t smpl_i = 0;smpl_i<samps_per_buff;smpl_i++){
            ++rxSampleCount;
            rxSignalBuffer.push(buff[smpl_i]);
        }

        tot_num_smpls += num_rx_samps;
    }

    if(buff_Allocated){
        delete[] buff;
        buff_Allocated = false;
        ReceptionInProgress = false;
    }

}

void RadioSysObject::runSynchronizationThread()
{
    using samp_type = std::complex<short>;

    using namespace arma;

    std::complex<double> j               = std::complex<double>(0,1);
    double pi                            = datum::pi;

    SynchronizationInProgress = true;

    size_t L_base                   = std::pow(2,std::floor(std::log2(2*sysConf.txSignal.size())));        // Read 2 frames at a time
    size_t L                        = L_base;
    bool usingFullFrame             = true;
    double fs                       = sysConf.rx.SamplingRate;
    std::vector<int> synch_pattern  = sysConf.syncCorrelationPattern;   // Hold synchronization pattern (this pattern repeats)
    int synch_tolerance             = sysConf.synchTolerance;
    bool external_freq_ref          = false;                            // use carrier offset compensation




    // Calculate half of subcarrier spacing
    int Nfft = sysConf.GetNfft();
    double half_scs = fs/Nfft/2;

    int num_shift_candidates = 1;
    if(external_freq_ref){
        num_shift_candidates = 28;
    }

    //vec shift_vec = linspace<vec>(-num_shift_candidates,num_shift_candidates,2*num_shift_candidates+1);
    //num_shift_candidates = shift_vec.n_elem;

    txSynchSignalBuffer.reset_pop();



    std::vector<size_t> c_rx_buffer_load(1e4);
    size_t max_rx_buffer_load = 0;

    //const int osr = 4;

    size_t L_tx = txSynchSignalBuffer.get_push_count(); // read number of samples in synch signal    

    std::vector<std::complex<double>> txSynch_alt;
    std::vector<std::complex<double>> txSynch_base(L_tx);
    std::vector<std::complex<double>> txSynch_base_f(L_tx);

    // Convert syncrhonization signal
    samp_type c_smpl;
    for(int i=0;i<L_tx;i++){
        txSynchSignalBuffer.pop(&c_smpl);
        txSynch_base[i] = std::complex<double>(std::real(c_smpl),std::imag(c_smpl));
    }

    txSynch_base_f = uhd_clib::fft_w_zpadd(txSynch_base,L,true);

    size_t ref_rx_indx = rxSignalBuffer.get_pop_count(); // current sample index being processed
    size_t current_rx_indx;

    size_t mk_c;
    std::vector<size_t> mk_index(L);

    size_t mx_count = c_rx_buffer_load.size();

    //assigned_val = true;

    size_t bi = 0;
    int times_t = 0;
    auto start = std::chrono::high_resolution_clock::now();

    // Continously process RX buffer
    while(not stop_synchronization_signal_called){

        usingFullFrame = true;
        L = L_base;                     // Default option is to use L_base length
        txSynch_alt = txSynch_base_f;   // Default to longer correlation sequence

        if(pendingSynchPointReset){
            syncPointBuffer.reset_buffer();
            frequencyOffsetBuffer.reset_buffer();
            pendingSynchPointReset = false;
        }

        ref_rx_indx = rxSignalBuffer.get_pop_count();
        current_rx_indx = rxSignalBuffer.get_push_count();

        // Check if this new synch-point will be within 100 frames of the old one?
        size_t last_synch_point = 1;
        size_t sig_size = sysConf.txSignal.size();
        if(syncPointBuffer.back(&last_synch_point) && ref_rx_indx > last_synch_point){

            size_t dist_in_frames = (ref_rx_indx-last_synch_point) / sig_size;

            if(dist_in_frames < 100){
                // Use last synch point as starting point for the new
                size_t synch_margin = 256;
                ref_rx_indx = last_synch_point+dist_in_frames * sig_size - synch_margin; // calculated latest synch point - margin

                int first_synch_pattern_point = synch_pattern[0];
                synch_pattern.clear();
                synch_pattern.push_back(first_synch_pattern_point); // Only first point is necessary

                L = std::pow(2,std::ceil(std::log2(first_synch_pattern_point + 2 * synch_margin)));

                txSynch_alt = uhd_clib::fft_w_zpadd(txSynch_base,L,true);

                usingFullFrame = false;
            }
        }

        // Wait until a complete L has been received
        std::chrono::nanoseconds duration(10);

        while((not stop_synchronization_signal_called) and current_rx_indx-ref_rx_indx < L){
            std::this_thread::sleep_for(duration);
            current_rx_indx = rxSignalBuffer.get_push_count();
        }


        if(not stop_synchronization_signal_called && !rxSignalBuffer.check_overwritten(ref_rx_indx)){
            std::vector<std::complex<short>> v_frame = rxSignalBuffer.extract_range(ref_rx_indx,L);
            std::vector<std::complex<double>> vv_frame = uhd_clib::cvec_conv_short2double(v_frame,1);

            // Implement repetition based time synchronization
            // std::vector<double> corr_out_bb(L-2*L_tx,0.0);
            // double max_corr_out_bb = 0.0;
            // cx_vec m1;
            // cx_vec m2;
            // for(int i=0;i<L-2*L_tx;i++){
            //    m1 = rx_sig.subvec(i,i+L_tx-1);
            //    m2 = rx_sig.subvec(i+L_tx,i+2*L_tx-1);
            //    corr_out_bb[i] = std::abs(dot(m1,conj(m2)));

            //    if(corr_out_bb[i] > max_corr_out_bb){
            //        max_corr_out_bb = corr_out_bb[i];
            //    }
            // }


            std::vector<int> peak_locs_bb;
            // std::vector<double> peak_vals_bb = uhd_clib::find_peaks(corr_out_bb,peak_locs_bb,0.9*max_corr_out_bb,sysConf.txSignal.size()-sysConf.synchTolerance,10);

            //rx_buffer.skip(debug_var3*L*osr); // skip 5x2 frames to limit computational load
            rxSignalBuffer.skip(0); // skip rest of buffer

            bool synchPointFound = WindowedSynchronizationEnabled;
            double peak_locs_diff_bb = 0.0;
            if(WindowedSynchronizationEnabled && peak_locs_bb.size() > 1){
                peak_locs_diff_bb = peak_locs_bb[1]-peak_locs_bb[0];
                if(std::abs(peak_locs_diff_bb-sysConf.txSignal.size()) < 5){
                    syncPointBuffer.push(ref_rx_indx + peak_locs_bb[0]);
                    frequencyOffsetBuffer.push(0.0);
                    //syncPointBuffer.push(ref_rx_indx);
                    synchPointFound = true;                    
                }
            }

            if(!synchPointFound){
                //vec t = regspace(0,rx_sig.n_elem-1)/fs; // define time vector

                std::vector<double> corr_max(num_shift_candidates,0.0);
                double curr_corr_max = 0.0;
                int curr_corr_max_ix = 0;
                std::vector<int> synch_point(num_shift_candidates,-1);

                for(int it=0;it<num_shift_candidates;it++){
                    double foc = 0.0;//half_scs*shift_vec(it); // current frequency offset candidate
                    //start = std::chrono::high_resolution_clock::now();
                    //vec corr_out = abs(ifft(tx_synch % fft(rx_sig % exp((double)2*j*pi*foc*t))));
                    //vec corr_out = abs(ifft(tx_synch % fft(rx_sig)));

                    std::vector<double> corr_out_fftw = uhd_clib::fft_correlation_w_ref(txSynch_alt,vv_frame);

                    //auto end = std::chrono::high_resolution_clock::now();
                    //std::chrono::duration<double> duration_t = end - start;
                    //std::cout << "Execution time: " << duration_t.count() << " seconds" << std::endl;

                    if(! usingFullFrame && false){
                        // ----- Save files to debug -----
                        std::string filename = "corr_out.dat";
                        std::ofstream file(filename, std::ios::out | std::ios::binary);

                        // Write the size of the vector first (optional, but useful for reading back)
                        size_t size = corr_out_fftw.size();
                        // Write the data to the file
                        file.write(reinterpret_cast<const char*>(corr_out_fftw.data()), size * sizeof(double));

                        file.close();

                        std::ofstream file2("rx_signal.dat", std::ios::out | std::ios::binary);

                        std::vector<double> tmp_v(2*vv_frame.size());
                        // Write the size of the vector first
                        size_t size2 = tmp_v.size();

                        for(int ik=0;ik<size2/2;ik++){
                            tmp_v[ik] = std::real(vv_frame[ik]);
                            tmp_v[ik+size2/2] = std::imag(vv_frame[ik]);
                        }

                        // Write the data to the file
                        file2.write(reinterpret_cast<const char*>(tmp_v.data()), size2 * sizeof(double));

                        file2.close();
                    }

                    // ------------------------------------------
                    double crr_max = 0.0;
                    for(int ik=0;ik<corr_out_fftw.size();ik++){
                        if(crr_max < corr_out_fftw[ik]){
                            crr_max = corr_out_fftw[ik];
                        }
                    }

                    //double crr_max = sum(corr_out);

                    std::vector<int> peak_locs;
                    //std::vector<double> corr_out_std_vec = conv_to<std::vector<double>>::from(corr_out);
                    std::vector<double> peak_vals = uhd_clib::find_peaks(corr_out_fftw,
                                                                         peak_locs,
                                                                         0.7*crr_max,
                                                                         L_tx-sysConf.synchTolerance,
                                                                         20); // find the peaks

                    // Validate peaks
                    std::vector<int> peak_locs_diff;
                    for(int ii=1;ii<peak_locs.size(); ii++){
                        peak_locs_diff.push_back(peak_locs[ii]-peak_locs[ii-1]);
                    }
                    bool match_synch_pattern = false;
                    int ii = -1;

                    // Loop through to match peaks with synch pattern
                    while(not match_synch_pattern and (ii < (int)(peak_locs_diff.size()-synch_pattern.size()))){
                        ++ii;
                        int ik = 0;
                        bool match = true;
                        while(ik < synch_pattern.size()){
                            if (abs(synch_pattern[ik]-peak_locs_diff[ik+ii]) > synch_tolerance){
                                match = false;
                            }
                            ++ik;
                        }
                        match_synch_pattern = match;
                    }

                    // If synch-pattern is found
                    if(match_synch_pattern){
                        synch_point[it] = peak_locs[ii]; // save starting point
                        if(crr_max > curr_corr_max){
                            curr_corr_max = crr_max;
                            curr_corr_max_ix = it;
                        }
                    }
                }

                if(synch_point[curr_corr_max_ix] != -1){
                   syncPointBuffer.push(ref_rx_indx + synch_point[curr_corr_max_ix]); // put synch point in buffer;
                   //frequencyOffsetBuffer.push(half_scs*shift_vec(curr_corr_max_ix));
                   frequencyOffsetBuffer.push(0.0);
                }

            }


        }

        // if (++times_t > 10){
        //     auto end = std::chrono::high_resolution_clock::now();
        //     std::chrono::duration<double> duration_t = end - start;
        //     std::cout << "Execution time: " << duration_t.count() << " seconds" << std::endl;

        //     times_t = 0;
        //     start = std::chrono::high_resolution_clock::now();
        // }

    }

    // Read block from rx signal buffer

    //syncPointBuffer.

    // Run synchronization on it!

    // Put synchronization point in buffer

    SynchronizationInProgress = false;

}

void RadioSysObject::writeBufferToFile(int count)
{

    size_t L;

    if(count < 0){
        if(rxSignalBuffer.check_filled()){
            L = rxSignalBuffer.get_capacity();
        }
    }else{
        if(count > rxSignalBuffer.get_push_count()){
            L = rxSignalBuffer.get_push_count();
        }else{
            L = static_cast<size_t>(count);
        }
    }


    if (count <= static_cast<size_t>(std::numeric_limits<int>::max())) {
        count = static_cast<int>(L);
    } else {
        std::cerr << "Cannot convert size_t value to int due to potential overflow." << std::endl;
    }


    captured_data.clear();
    std::ofstream outfile;
    outfile.open(rxCaptureFilepath.c_str(),std::ofstream::binary);

    using samp_type = std::complex<short>;

    samp_type* buff;
    try{
        buff = new samp_type[L];
    }catch(std::bad_alloc& exc){

    }

    samp_type c_smpl;


    while(rxSignalBuffer.back(&c_smpl,count) && count > -1){
        buff[count--] = c_smpl;
        captured_data.push_back(c_smpl);
    }

    if (outfile.is_open()){
        outfile.write((const char *) buff,L*sizeof(samp_type));
    }
    delete[] buff;

    writingBufferInProgress = false;
}

//save_buffer_to_csv<samp_type>("class_sample.csv",Nfft,auth_sig,csv_pattern,smpl_i);
// std::string bformat = "data/class%d_sample%d.csv";


bool RadioSysObject::requestCaptureSynchFrame(int captureIndex){

    if(SynchronizationInProgress){
        extractFrameSequence(frameOffset,frameLength);

        std::vector<std::complex<double>> frame_temp = uhd_clib::cvec_conv_short2double(extracted_synch_data);
        // double xi;
        // double yi;
        // for(int i=0;i<frameLength;i++){
        //     //xi = 10 * std::log10(std::norm(vc_data[i]));
        //     xi = std::real(extracted_synch_data[i]);
        //     yi = std::imag(extracted_synch_data[i]);

        //     frame_temp[i] = std::complex<double>(xi,yi);
        // }

        // put in the vector
        for(int i=0; i<frameLength;i++){

            capturedFrames[i][2*captureIndex] = std::real(frame_temp[i]);
            capturedFrames[i][2*captureIndex+1] = std::imag(frame_temp[i]);
        }

        currentFramesCaptured[captureIndex] = true;

        return true;
    }else{
        return false;
    }
}

bool RadioSysObject::requestWriteLastCapturedFrame(std::string filepath)
{
    if(filepath == ""){
        filepath = captureSynchFilepath;
    }

    std::ofstream outfile;
    outfile.open(filepath,std::ofstream::binary);

    std::complex<short> * buff;
    buff = new std::complex<short>[frameLength];

    for(int i = 0;i<frameLength;i++){
        buff[i] = extracted_synch_data[i];
    }

    if(outfile.is_open()){
        outfile.write((const char *)buff,frameLength*sizeof(std::complex<short>));
    }
    delete[] buff;

    return true;
}

int RadioSysObject::requestFrameCaptureFormat(int rframeOffset,
                                               int rframeLength,
                                               int rnumFrameCaptures,
                                               std::string filepath,
                                               bool enableWindowSynch)
{
    if(SynchronizationInProgress){
        return -1;
    }

    frameOffset = rframeOffset;
    frameLength = rframeLength;
    numFrameCaptures = rnumFrameCaptures;

    captureSynchFilepath = filepath;
    WindowedSynchronizationEnabled = false;
    if(enableWindowSynch){
        if(sysConf.GetSupportWindowSync()){
            WindowedSynchronizationEnabled = true;
        }else{
            return -2;
        }
    }


    std::vector<std::vector<double>> captureFrame_tmp(frameLength, std::vector<double>(2*numFrameCaptures));
    capturedFrames = captureFrame_tmp;

    std::vector<bool> currentFramesCaptured_tmp(numFrameCaptures);
    for(int i=0; i<numFrameCaptures;i++){
        currentFramesCaptured_tmp[i] = false;
    }

    currentFramesCaptured = currentFramesCaptured_tmp;

    return 1;
}

int RadioSysObject::requestWriteFramesToFile(std::string filepath, std::string fileType)
{
    if(fileType != "csv"){
        return -2; // Error unsupported file format
    }

    if(!isCapturedFramesReadyToSave()){
        return -3; // Error missing frames
    }

    std::ofstream outfile;
    outfile.open(filepath);
    if(outfile.is_open()){ // make sure file is open
        for(int i=0;i<frameLength;i++){
            for(int k=0;k<2*numFrameCaptures-1;k++){
                outfile << capturedFrames[i][k] << ",";
            }
            outfile << capturedFrames[i][2*numFrameCaptures-1] << "\n";
        }
        outfile.close();
    }

    //currentFramesCaptured();

    return 1; // success
}

bool RadioSysObject::isCapturedFramesReadyToSave()
{
    bool all_finished = true;
    for(int i=1;i<numFrameCaptures;i++){
        if(!currentFramesCaptured[i]){
            all_finished = false;
        }
    }

    return all_finished;
}

bool RadioSysObject::resetCurrentFramesCaptured()
{
    for(int i=0;i<numFrameCaptures;i++){
        currentFramesCaptured[i] = false;
    }

    return true;
}

bool RadioSysObject::requestResetSynchPoints()
{
    if(isSynchronizing()){
       // pendingSynchPointReset = true;
    }else{
       // syncPointBuffer.reset_buffer();
      //  frequencyOffsetBuffer.reset_buffer();
    }

    return true;
}

int RadioSysObject::requestResetTransmitter()
{
    if(tx_usrp != nullptr){
        StopAll();
        uhd::stream_cmd_t stop_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        stop_cmd.stream_now = true;

        tx_usrp->issue_stream_cmd(stop_cmd);
        tx_usrp.reset();

        setTxUSRPConfigured(false);
    }

}
