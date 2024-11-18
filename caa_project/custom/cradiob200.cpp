#include "cradiob200.h"


cRadioResponse cRadioB200::runRadioConfigurationProcess()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "Success configuring B200";

    std::cout << "Running B200 configuration" << std::endl;

    const std::string addr = "type=b200,serial=" + getSerial();
    usrp = uhd::usrp::multi_usrp::make(addr);


    response.code = -1;
    response.message = "B200 - Configuration not yet implemented";

    return response;
}
