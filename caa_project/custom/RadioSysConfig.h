#ifndef RADIOSYSCONFIG_HEADER_H
#define RADIOSYSCONFIG_HEADER_H

#include <vector>
#include <string>
#include <complex>

namespace RSC{

	struct PHYconfig{
		double Nfft;
		double samplingRate;
		int numSymbols;
		std::vector<int> guardLengths;
		std::vector<int> symbolLengths;
	};

	struct FileConfig{		
		
		std::string subdev;
		std::string type 				= "short";
		std::string wirefmt 			= "sc16";
        std::string cpu_format          = "sc16";

		// RX object
		//std::string rxUSRP_IPaddress;
    	std::string ANT;
    	std::string CH 					= "0";
    	std::string File;
    	std::string REF_Source;    	
    	std::string PPS_Source;
    	double SamplingRate;
    	double CarrierFrequency;
    	double Gain;
    	double FILT_BW;
    	double RX_LO_Offset;

    	size_t BufferSize;

		// RX object
		//std::string rxUSRP_IPaddress;
    	std::string rxUSRP_ANT;
    	std::string rxUSRP_CH 			= "0";
    	std::string rxFile;
    	std::string rxUSRP_REF_Source;    	
    	std::string rxUSRP_PPS_Source;
    	double rxUSRP_SamplingRate;
    	double rxUSRP_CarrierFrequency;
    	double rxUSRP_Gain;
    	double rxUSRP_FILT_BW;
    	double rxUSRP_LO_Offset;

    	// TX object
    	//std::string txUSRP_IPaddress;
    	std::string txUSRP_ANT;
    	std::string txUSRP_CH 			= "0";
    	std::string txFile;
    	std::string txUSRP_REF_Source;
    	std::string txUSRP_PPS_Source;
    	double txUSRP_SamplingRate;
    	double txUSRP_CarrierFrequency;
    	double txUSRP_Gain;
    	double txUSRP_FILT_BW;

    	size_t samplesPerBuffer = 1151;
    	size_t txBufferSize;
    	size_t rxBufferSize;   	
    	
    	double txRELrxTimeOffset;
	};

} // namespace radio system configuration (rsc)

class RadioSysConfig {
	public :
		RadioSysConfig(); 		// Constructor
		//~RadioSysConfig(); 		// Destructor

		double rxSamplingRate;
		double txSamplingRate;

		//void SetMATConfigFileName(std::string file_name);

		//void ReadPropertyFromMat(std::string property_name,std::string property_path,std::string file_name="");

		void SetNfft(double value);
		void SetSamplingRate(double value);
		void SetGuardLengths(std::vector<int> value);
		void SetSymbolLengths(std::vector<int> value);

		void SetSignalPattern(std::vector<char> value);
		void SetSynchCorrelationPattern(std::vector<int> value);

		void SetIndexGrid(std::vector<std::vector<int>> value);

		void SetTransmitSignal(std::vector<std::complex<double>> value);
		void SetSynchSignal(std::vector<std::complex<double>>  value);

		void SetSynchOffset(size_t value);
		void SetSynchTolerance(int value);

		void SetrxUSRP_IPaddress(std::string ip_address);
		void SettxUSRP_IPaddress(std::string ip_address);

        std::string GetrxUSRP_IPaddress() { return rxUSRP_IPaddress;}
        std::string GettxUSRP_IPaddress() { return txUSRP_IPaddress;}

        std::string GetCleanRX_IPaddress() {return rxUSRP_IPaddress.substr(5);}
        std::string GetCleanTX_IPaddress() {return txUSRP_IPaddress.substr(5);}

		void Print();

		RSC::PHYconfig winfo;
		RSC::FileConfig confFile;

		RSC::FileConfig tx;
		RSC::FileConfig rx;

		std::vector<std::vector<int>> iGrid;

		std::vector<char> signalPattern;
		std::vector<std::complex<double>> txSignal;
		
		std::vector<int> syncCorrelationPattern;
		std::vector<std::complex<double>> synchSignal;
		size_t synchOffset;
		int synchTolerance = 1;

		double txRELrxTimeOffset;
		std::string default_command_list;

		


	private:

	std::string rxUSRP_IPaddress;
	std::string txUSRP_IPaddress;

	//std::string MATConfigFileName;
	enum string_code {Nfft, SampleRate, GuardLengths};
	string_code hashit (std::string const& inString) {
    	if (inString == "Nfft") return Nfft;
    	if (inString == "SampleRate") return SampleRate;
    	if (inString == "GuardLengths") return GuardLengths;
	}

		
};



#endif
