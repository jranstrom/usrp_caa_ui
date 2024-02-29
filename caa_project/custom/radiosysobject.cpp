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

RadioSysObject::RadioSysObject() {
    std::cout<<"RadioSysObject created" << std::endl;
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

        //CircBuffer<std::complex<short>> tx_signal_buff = uhd_clib::cvec_conv_double2short(sysConf.txSignal);
        txSignalBuffer.from_vector(uhd_clib::cvec_conv_double2short(sysConf.txSignal));
        //txSignalBuffer = &tx_signal_buff;
    }catch(...){
        success = false;
    }

    return success;
}

void RadioSysObject::StopAll()
{
    std::cout << "Transmission in progress: " << TransmissionInProgress << std::endl;
    stop_signal_called = true;
    std::this_thread::sleep_for(1s);
    stop_signal_called = false;
    std::cout << "Terminated All" <<std::endl;
    std::cout << "Transmission in progress: " << TransmissionInProgress << std::endl;
}

bool RadioSysObject::startTransmission()
{
    if(TransmissionInProgress || !txUSRPSetup){
        return false;
    }

    std::cout << "Starting transmission..." << std::endl;

    txSignalBuffer.reset_buffer();

    std::thread txThread(&RadioSysObject::runTransmissionThread,this);

    // Detach the thread from the main thread
    txThread.detach();

    return true;
}

bool RadioSysObject::startReception()
{
    return true;
}

bool RadioSysObject::isTransmitting()
{
    return TransmissionInProgress;
}

void RadioSysObject::setupTxUSRP()
{
    txUSRPSetup = false;

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
            std::cout << boost::format("Actual Carrier freq: %.2f MHz\n") % (actual_CarrierFrequency / 1e6);

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

    txUSRPSetup = true;
}

void RadioSysObject::setupRxUSRP()
{
    rxUSRPSetup = false;

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

    rxUSRPSetup = true;

}

void RadioSysObject::runTransmissionThread()
{
    TransmissionInProgress = true;

    const std::vector<size_t>channel_nums = {0};
    const size_t total_num_channels = 1;
    size_t samps_per_buff = sysConf.tx.BufferSize;

    uhd::stream_args_t stream_args(sysConf.confFile.cpu_format,sysConf.confFile.wirefmt);
    stream_args.channels = channel_nums;
    uhd::tx_streamer::sptr tx_stream = tx_usrp->get_tx_stream(stream_args);

    double time_offset = timeOffset;

    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    std::chrono::milliseconds delay(100);
    std::chrono::system_clock::time_point targetTime = currentTime + delay;

    bool continous_stream   = true;

    uhd::tx_metadata_t md;
    md.start_of_burst = false;
    md.end_of_burst = false;
    md.has_time_spec  = true;
    if (time_offset > 0){
        time_offset = 0.0;
    }else{
        time_offset = -1*time_offset;
    }

    uhd::time_spec_t c_time = tx_usrp->get_time_now(0);
    md.time_spec      = uhd::time_spec_t(3+time_offset);
    md.has_time_spec  = false;
    using samp_type = std::complex<short>;

    samp_type c_smpl;

    if (channel_nums.size() == 1) {
        std::vector<std::complex<short>> buff(samps_per_buff);
        bool file_read = false;
        while(not stop_signal_called and (continous_stream or not file_read)){

            for(int ik=0;ik<samps_per_buff;ik++){
                txSignalBuffer.circ_pop(&c_smpl);
                buff[ik] = c_smpl;
            }
            size_t num_tx_samps = samps_per_buff;

            const size_t samples_sent = tx_stream->send(&buff.front(),num_tx_samps,md,5.3);

            if(samples_sent != num_tx_samps){
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
