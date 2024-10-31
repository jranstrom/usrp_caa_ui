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
    void requestToConnect(std::string port,bool silent=false,int timeout=1000);
    void requestStatus();
    void requestInfo();
    std::string getInfo(std::string port);
    void requestDebugToggle();
    void requestAutoToggle();
    void requestMCId();
    std::string getMCId(std::string port);

    void requestUESelect(int value);
    void requestAllUEsOFF();
    void requestELSelect(int value);
    void requestELToggle(int value);
    void requestCommand(std::string value,bool awaitRepsonse=true);
    void requestCycle();
    bool requestRead();
    void requestReset();

    void flush();

    std::string requestConfiguration(bool silent=false);
    void requestConfiguation();

    std::string getMCType(std::string port="");

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

    TeensyCOM tcom = TeensyCOM();

    std::vector<std::string> DeviceList;
    int mcCurrentType = -1; // UE Switch = 0; Element Switch = 1
    std::vector<std::string> mcTypes_code = {"UESWITCH","ELSWITCH"};
    std::vector<std::string> mcTypes = {"UE Switch","Element Switch"};
};

#endif // TCOM_UI_H
