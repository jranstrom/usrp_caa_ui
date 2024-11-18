#ifndef CRADIOB200_H
#define CRADIOB200_H

#include "cradioobject.h"


class cRadioB200 : public cRadioObject
{
public:
    cRadioB200(std::string serial_p,std::string type_p,std::string addr_p) : cRadioObject(serial_p,type_p,addr_p) {}

    cRadioResponse runRadioConfigurationProcess() override;
};

#endif // CRADIOB200_H
