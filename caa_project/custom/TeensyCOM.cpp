#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <cstdlib>
#include "TeensyCOM.h"
#include "third-party/serial/serial.h"

TeensyCOM::TeensyCOM(int baudrate){
	BaudRate = baudrate;
}

TeensyCOM::~TeensyCOM(){
	if(Connected){
		//~CurrentCOM();
		//delete CurrentConnection; // not needed
	}
}

void TeensyCOM::ListAvailablePorts(){
	GetDeviceList();

	if(DeviceList.size() > 0){
		for(int ii=0; ii < DeviceList.size(); ii++){
			std::cout << "Port" << ii << ": " << DeviceList[ii] << std::endl;
		}
	}else{
		std::cout << "No devices found..." << std::endl;
	}
}

bool TeensyCOM::CheckConnection(std::string device_port,int timeout_duration){

	TeensyCOM::GetDeviceList();
	bool connection_success = false;

	if(DeviceList.size() > 0){
		if(device_port.empty()){
			device_port = DeviceList[0];
		}

		try{
			serial::Serial teensy_serial(device_port,BaudRate,serial::Timeout::simpleTimeout(timeout_duration));		
			connection_success = teensy_serial.isOpen();
			std::cout << "no error here" << std::endl;
		}catch(...){
			std::cerr << "Error not able to connect, check if device is busy..." << std::endl;
		}

	}else{
		std::cerr << "No devices found..." << std::endl;
	}

	return connection_success;
}

std::vector<std::string> TeensyCOM::GetDeviceList(){
	DeviceList.clear();

	std::vector<serial::PortInfo> devices_found = serial::list_ports();
    std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

    while( iter != devices_found.end() )
    {
        serial::PortInfo device = *iter++;
        if(std::strcmp(device.hardware_id.c_str(),"n/a") != 0){
        	DeviceList.push_back(device.port.c_str());
        }
    }

    return DeviceList;
}

bool TeensyCOM::Connect(std::string device_port,int timeout_duration){

	bool connection_success = false;
	serial::Timeout tmout = serial::Timeout::simpleTimeout(timeout_duration); 

	GetDeviceList();

	if(DeviceList.size() < 1){
		throw std::runtime_error("No devices connected...");
	}

	if(device_port.empty()){
		device_port = DeviceList[0];
	}

	bool valid_device = false;
	for(int i = 0;i < DeviceList.size();i++){
		if(device_port.compare(DeviceList[i])){
			valid_device = true;
		}
	}

	if(valid_device){
		
		if(CurrentCOM.isOpen()){
			CurrentCOM.close();
		}

		CurrentCOM.setPort(device_port);
		CurrentCOM.setBaudrate(BaudRate);
		CurrentCOM.setTimeout(tmout);

		try{
			CurrentCOM.open();
			if(CurrentCOM.isOpen()){			
				connection_success = true;
			}
		}catch(...){
			std::cerr << "Error not able to connect, check if device " << device_port << " is busy..." << std::endl;
		}
	}

    Connected = connection_success;

	return connection_success;
}

bool TeensyCOM::WriteCommand(std::string command){
	bool success = false;
	size_t bytes_w = 0;
	if(Connected && not command.empty()){
		bytes_w = CurrentCOM.write(command);
		success = true;
	}
	return success;
}

std::string TeensyCOM::ReadLine(){
	std::string result_str = "Timeout: No reponse...";
	if(Connected){
		result_str = CurrentCOM.readline();
	}

	return result_str;
}

void TeensyCOM::FlushInput(){
	if(Connected){
		CurrentCOM.flushInput();
	}
}

void TeensyCOM::FlushOutput(){
	if(Connected){
		CurrentCOM.flushOutput();
	}
}

void TeensyCOM::Flush(){
	if(Connected){
		CurrentCOM.flush();
	}
}
