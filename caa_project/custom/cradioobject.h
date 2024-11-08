#ifndef CRADIOOBJECT_H
#define CRADIOOBJECT_H

#include <string>

struct cRadioResponse{
    int code; // 0 - success
    std::string message = "";
};

class cRadioObject
{
public:
    cRadioObject(std::string serial_p,std::string type_p);

    cRadioResponse configureRadio();

    std::string getSerial() {return serial;}
    std::string getType() {return type;}
    std::string getAddress() {return address;}

private:
    std::string uhd_findRadioResponse;
    std::string serial;
    std::string type;
    std::string address;
};

#endif // CRADIOOBJECT_H
