#ifndef CRADIOUSRP2_H
#define CRADIOUSRP2_H

#include "cradioobject.h"

class cRadioUSRP2 : public cRadioObject
{
public:
    cRadioUSRP2(std::string serial_p,std::string type_p,std::string addr_p) : cRadioObject(serial_p,type_p,addr_p) {}

    //cRadioResponse runRadioConfigurationProcess() override;

    void runContinousReceptionProcess(std::shared_ptr<CircBuffer<std::complex<short>>> rxCircBuffer,uhd::usrp::multi_usrp::sptr m_usrp) override;

};

#endif // CRADIOUSRP2_H
