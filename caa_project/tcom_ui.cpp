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
        setMCType(-1);
        emit mcIdChanged("");
        emit connectionChanged("");
        emit responseChanged("Could not connect, device may be busy...");
    }
}

void Tcom_ui::requestStatus()
{
    if(tcom.WriteCommand("s")){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}

void Tcom_ui::requestInfo()
{
    if(tcom.WriteCommand("i")){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}

void Tcom_ui::requestDebugToggle()
{
    if(tcom.WriteCommand("m")){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}

void Tcom_ui::requestAutoToggle()
{
    if(tcom.WriteCommand("a")){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}

void Tcom_ui::requestMCId()
{
    if(tcom.WriteCommand("i")){
        std::string id_string = tcom.ReadLine();

        if(id_string.size() > 12){
            emit mcIdChanged(id_string.substr(0, 12));
        }else{
            emit mcIdChanged(id_string);
        }

        int found = -1;
        for(int t=0;t<mcTypes_code.size();t++){
            if(id_string.find(mcTypes_code[t]) != std::string::npos){
                found = t;
            }
        }
        setMCType(found);
    }else{
        setMCType(-1);
    }
}

void Tcom_ui::requestUESelect(int value)
{
    if(tcom.WriteCommand("c" + std::to_string(value))){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}

void Tcom_ui::requestELSelect(int value)
{
    if(tcom.WriteCommand("p" + std::to_string(value))){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}
