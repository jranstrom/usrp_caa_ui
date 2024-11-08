#include "cradioobject.h"


cRadioObject::cRadioObject(std::string serial_p, std::string type_p)
{
    serial = serial_p;
    type = type_p;
}

cRadioResponse cRadioObject::configureRadio()
{
    cRadioResponse response;
    response.code = 0;
    response.message = "success";



    return response;
}
