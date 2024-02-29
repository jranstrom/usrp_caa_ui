#include "RadioSysConfig.h"
#include <complex>
#include <iostream>
#include <cstdlib>

RadioSysConfig::RadioSysConfig(){}

void RadioSysConfig::SetNfft(double value){ this->winfo.Nfft = value; }

void RadioSysConfig::SetSamplingRate(double value){ this->winfo.samplingRate = value; }
void RadioSysConfig::SetGuardLengths(std::vector<int> value){ this->winfo.guardLengths = value; this->winfo.numSymbols = value.size(); }
void RadioSysConfig::SetSymbolLengths(std::vector<int> value){ this->winfo.symbolLengths = value; }

void RadioSysConfig::SetSignalPattern(std::vector<char> value){ this->signalPattern = value; }
void RadioSysConfig::SetSynchCorrelationPattern(std::vector<int> value){ this->syncCorrelationPattern = value; }

void RadioSysConfig::SetIndexGrid(std::vector<std::vector<int>> value){ this->iGrid = value; }

void RadioSysConfig::SetTransmitSignal(std::vector<std::complex<double>> value){ this->txSignal = value; }
void RadioSysConfig::SetSynchSignal(std::vector<std::complex<double>>  value){ this->synchSignal = value; }

void RadioSysConfig::SetSynchOffset(size_t value){ this->synchOffset = value; }
void RadioSysConfig::SetSynchTolerance(int value){ this->synchTolerance = value; }

void RadioSysConfig::SetrxUSRP_IPaddress(std::string ip_address){
	this->rxUSRP_IPaddress = "addr=" + ip_address;
}
void RadioSysConfig::SettxUSRP_IPaddress(std::string ip_address){
	this->txUSRP_IPaddress = "addr=" + ip_address;
}



void RadioSysConfig::Print(){
    std::cout<< "-------------------------------------------" << std::endl;
    std::cout<< "------- Radio System Configuration --------" << std::endl;
    std::cout<< "-------------------------------------------" << std::endl;

    std::cout << "| Frame structure: {";
    for(int i=0;i<this->signalPattern.size();i++){
    	std::cout << this->signalPattern[i] << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "| winfo" << std::endl;
    std::cout << "   -| Sampling Rate: " << this->winfo.samplingRate / 1e6 << " MHz"  << std::endl;
    std::cout << "   -| FFT Size: " << this->winfo.Nfft << std::endl;
    std::cout << "   -| Frame duration (seconds): " << this->txSignal.size() / this->winfo.samplingRate << std::endl;
    std::cout << "   -| Frame duration (samples): " << this->txSignal.size() << std::endl;
    std::cout << "   -| Number of symbols: " << this->winfo.numSymbols <<std::endl;
    std::cout << "   -| Guard Lenghts: {";
    for(int i=0;i<this->winfo.guardLengths.size();i++){
    	std::cout << this->winfo.guardLengths[i] << " ";
    }
    std::cout << "}" << std::endl;
    std::cout << "   -| Symbol Lenghts: {";
    for(int i=0;i<this->winfo.symbolLengths.size();i++){
    	std::cout << this->winfo.symbolLengths[i] << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "| Synchronisation Tolerance: " << this->synchTolerance << " samples" << std::endl;
    std::cout << "| Synchronisation Offset: " << this->synchOffset << " samples" << std::endl;
    

    std::cout << "| confFile" << std::endl;
    std::cout << "   -| Wire FMT: " << confFile.wirefmt << std::endl;
    std::cout << "   -| Type: " << confFile.type << std::endl;
    std::cout << "   -| Subdev: " << confFile.subdev << std::endl;
    std::cout << "   -| Default Command List: " << default_command_list << std::endl;

    std::cout << "   -| Transmitter: " << std::endl;
    std::cout << "         --| IP Address: " << txUSRP_IPaddress << std::endl;
	std::cout << "         --| ANT selection: " << tx.ANT << std::endl;
	std::cout << "         --| File: " << tx.File << std::endl;
	std::cout << "         --| SamplingRate: " << tx.SamplingRate / 1e6 << " MHz" << std::endl;
	std::cout << "         --| Carrier Frequency: " << tx.CarrierFrequency / 1e9 << "GHz" << std::endl;
	std::cout << "         --| Gain: " << tx.Gain << " dB" << std::endl;
	std::cout << "         --| REF Source: " << tx.REF_Source << std::endl;
	std::cout << "         --| PPS Source: " << tx.PPS_Source << std::endl;
	std::cout << "         --| Filter BW: " << tx.FILT_BW / 1e6 << " MHz" << std::endl;

	std::cout << "   -| Receiver: " << std::endl;
    std::cout << "         --| IP Address: " << rxUSRP_IPaddress << std::endl;
	std::cout << "         --| ANT selection: " << rx.ANT << std::endl;
	std::cout << "         --| File: " << rx.File << std::endl;
	std::cout << "         --| SamplingRate: " << rx.SamplingRate / 1e6 << " MHz" << std::endl;
	std::cout << "         --| Carrier Frequency: " << rx.CarrierFrequency / 1e9 << "GHz" << std::endl;
	std::cout << "         --| Gain: " << rx.Gain << " dB" << std::endl;
	std::cout << "         --| REF Source: " << rx.REF_Source << std::endl;
	std::cout << "         --| PPS Source: " << rx.PPS_Source << std::endl;
	std::cout << "         --| Filter BW: " << rx.FILT_BW / 1e6 << " MHz" << std::endl;
	std::cout << "         --| LO Offset: " << rx.RX_LO_Offset / 1e6 << " MHz" << std::endl;



    std::cout<< "-------------------------------------------" << std::endl;
    

}
// void RadioSysConfig::SetMATConfigFile(std::string file_name){
// 	// could check if file exist?
// 	MATConfigFileName = file_name;
// }

// void RadioSysConfig::ReadPropertyFromMat(std::string property_name, std::string property_path, std::string file_name){
// 	if(file_name.empty()){
// 		file_name = MATConfigFileName;
// 	}
// }

