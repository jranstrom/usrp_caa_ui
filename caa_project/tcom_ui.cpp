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

void Tcom_ui::requestToConnect(std::string port,bool silent)
{
    if(tcom.Connect(port)){
        if(!silent){
            emit connectionChanged(port);
        }
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

std::string Tcom_ui::getInfo(std::string port)
{
    std::string currentPort = tcom.GetCurrentPort();
    tcom.Connect(port);

    std::string info_string = "";

    if(tcom.WriteCommand("i")){
        info_string = tcom.ReadLine();
    }

    if(currentPort != ""){
        tcom.Connect(currentPort);
    }

    return info_string;
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

std::string Tcom_ui::getMCId(std::string port)
{
    std::string currentPort = tcom.GetCurrentPort();
    tcom.Connect(port);

    std::string id_string = "";

    if(tcom.WriteCommand("i")){
        id_string = tcom.ReadLine();

        if(id_string.size() > 12){
            id_string = id_string.substr(0, 12);
        }
    }

    if(currentPort != ""){
        tcom.Connect(currentPort);
    }

    return id_string;

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

void Tcom_ui::requestELToggle(int value)
{
    if(tcom.WriteCommand("t" + std::to_string(value))){
        emit responseChanged(tcom.ReadLine());
    }else{
        emit responseChanged("Failed to request status");
    }
}

void Tcom_ui::requestCommand(std::string value, bool awaitRepsonse)
{
    if(tcom.WriteCommand(value)){
        if(awaitRepsonse){
            emit responseChanged(tcom.ReadLine());
        }else{
            tcom.Flush();
            emit responseChanged("");
        }
    }
}

std::string Tcom_ui::getMCType(std::string port)
{
    std::string resp = "";

    if(port == ""){
        if(mcCurrentType > 0){
            resp = mcTypes[mcCurrentType];
        }
    }else{
        std::string info_string = getInfo(port);

        if(info_string != ""){
            int found = -1;
            for(int t=0;t<mcTypes_code.size();t++){
                if(info_string.find(mcTypes_code[t]) != std::string::npos){
                    found = t;
                }
            }

            if(found != -1){
                resp = mcTypes[found];
            }
        }

    }

    return resp;
}
