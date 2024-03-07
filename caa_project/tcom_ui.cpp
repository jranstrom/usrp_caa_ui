#include "tcom_ui.h"

Tcom_ui::Tcom_ui(QObject *parent)
    : QObject{parent}
{}

void Tcom_ui::requestAvailableDevices()
{
    std::vector<std::string> tmp_device_list = tcom.GetDeviceList();

    bool update = false;

    if(tmp_device_list.size() != DeviceList.size()){
        update = true;
    }else{
        for(int i=0;i<tmp_device_list.size();i++){
            if(DeviceList[i] != tmp_device_list[i]){
                update = true;
            }
        }
    }

    if(update){
        DeviceList = tmp_device_list;
        emit availableDevicesChanged(DeviceList);
    }
}

void Tcom_ui::requestToConnect(std::string port)
{
    if(tcom.Connect(port)){
        emit connectionChanged(port);
    }else{
        emit connectionChanged("");
    }
}
