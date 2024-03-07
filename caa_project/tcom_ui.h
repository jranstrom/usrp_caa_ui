#ifndef TCOM_UI_H
#define TCOM_UI_H

#include <QObject>
#include "custom/TeensyCOM.h"

class Tcom_ui : public QObject
{
    Q_OBJECT
public:
    explicit Tcom_ui(QObject *parent = nullptr);

    void requestAvailableDevices();
    void requestToConnect(std::string port);
    void requestStatus();
    void requestInfo();
    void requestDebugToggle();
    void requestAutoToggle();
    void requestMCId();
    void requestUESelect(int value);

    std::string getMCType() {
        std::string resp = "";
        if(mcCurrentType > 0){
            resp = mcTypes[mcCurrentType];
        }
        return resp;
    }

    void setMCType(int value){
        if(value != mcCurrentType){
            mcCurrentType = value;
            std::string mcType = "unknown";
            if(value > -1){
                mcType = mcTypes[value];
            }
            emit mcTypeChanged(mcType);
        }
    }

    std::vector<std::string> getDeviceList() {return DeviceList;}

signals:
    void availableDevicesChanged(std::vector<std::string> & value);
    void connectionChanged(std::string value);
    void responseChanged(std::string value);
    void mcIdChanged(std::string value);
    void mcTypeChanged(std::string value);

private:

    TeensyCOM tcom = TeensyCOM(115200);

    std::vector<std::string> DeviceList;
    int mcCurrentType = -1; // UE Switch = 0; Element Switch = 1
    std::vector<std::string> mcTypes_code = {"UESWITCH","ELSWITCH"};
    std::vector<std::string> mcTypes = {"UE Switch","Element Switch"};
};

#endif // TCOM_UI_H
