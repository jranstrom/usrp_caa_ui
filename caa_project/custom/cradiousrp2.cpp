#include "cradiousrp2.h"


// cRadioResponse cRadioUSRP2::runRadioConfigurationProcess()
// {
//     cRadioResponse response;
//     response.code = 0;
//     response.message = "Success configuring USRP2";

//     return response;
// }

void cRadioUSRP2::runContinousReceptionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> rxCircBuffer,uhd::usrp::multi_usrp::sptr m_usrp)
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
    if(isAllocated == true){
        delete [] rxBuffer;
    }

    continous_reception_running = false;
}
