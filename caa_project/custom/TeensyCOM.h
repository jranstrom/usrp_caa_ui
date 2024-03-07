#ifndef TEENSYCOM_HEADER_H
#define TEENSYCOM_HEADER_H

#include <vector>
#include <string>
#include "third-party/serial/serial.h"


class TeensyCOM {
	public :
		TeensyCOM(int baudrate=9600); // constructor
		~TeensyCOM(); 	// Destructor
        void ListAvailablePorts();
		bool CheckConnection(std::string device_port="",int timeout_duration=3000);
		bool Connect(std::string device_port="",int timeout_duration=3000);
		bool WriteCommand(std::string command);
		void FlushInput();
		void FlushOutput();
		void Flush();
		std::string ReadLine();
        std::vector<std::string> GetDeviceList();

	private:
		std::vector<std::string> DeviceList;
		int BaudRate;
		bool Connected = false;
		serial::Serial CurrentCOM = serial::Serial("");		
};

#endif
