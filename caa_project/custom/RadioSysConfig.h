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
        std::string dataModulationType = "none";
        std::string date = "";
	};

	struct FileConfig{		
		
		std::string subdev;
		std::string type 				= "short";
		std::string wirefmt 			= "sc16";
        std::string cpu_format          = "sc16";

		// RX object
		//std::string rxUSRP_IPaddress;
        std::string ANT                 = "TX/RX";
    	std::string CH 					= "0";
        std::string File                = "usrp_rx_samples.dat";
        std::string REF_Source          = "external";
        std::string PPS_Source          = "external";
        std::string IP_Address          = "192.168.0.1";
        double SamplingRate             = 10e6;
        double CarrierFrequency         = 2.5e9;
        double Gain                     = 0;
        double FILT_BW                  = 10e6;
        double RX_LO_Offset             = 12e6;

        size_t BufferSize               = 1472;
    	
        double txRELrxTimeOffset        =0;
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
        int GetNfft() {return static_cast<int>(winfo.Nfft);}
		void SetSamplingRate(double value);
        double GetSamplingRate() { return winfo.samplingRate;}
		void SetGuardLengths(std::vector<int> value);
		void SetSymbolLengths(std::vector<int> value);

        std::string GetDataModulationType(){ return winfo.dataModulationType;}
        void SetDataModulationType(std::string value){
            if(winfo.dataModulationType != value){
                winfo.dataModulationType = value;
            }
        }

        std::string GetDate(){return winfo.date;}
        void SetDate(std::string value){
            if(winfo.date != value){
                winfo.date = value;
            }
        }

		void SetSignalPattern(std::vector<char> value);
        std::string GetSignalPattern();
		void SetSynchCorrelationPattern(std::vector<int> value);

		void SetIndexGrid(std::vector<std::vector<int>> value);

		void SetTransmitSignal(std::vector<std::complex<double>> value);
		void SetSynchSignal(std::vector<std::complex<double>>  value);

		void SetSynchOffset(size_t value);
		void SetSynchTolerance(int value);

		void SetrxUSRP_IPaddress(std::string ip_address);
		void SettxUSRP_IPaddress(std::string ip_address);

        double getTxCarrierFrequency() {return tx.CarrierFrequency;}
        double getRxCarrierFrequency() {return rx.CarrierFrequency;}
        void setTxCarrierFrequency(double value){
            if(tx.CarrierFrequency != value){
                tx.CarrierFrequency = value;
            }
        }
        void setRxCarrierFrequency(double value){
            if(rx.CarrierFrequency != value){
                rx.CarrierFrequency = value;
            }
        }

        double getRxLO_offset() {return rx.RX_LO_Offset;}
        void setRxLO_offset(double value){
            if(rx.RX_LO_Offset != value){
                rx.RX_LO_Offset = value;
            }
        }

        double getTxGain() {return tx.Gain;}
        double getRxGain() {return rx.Gain;}
        void setTxGain(double value){
            if(tx.Gain != value){
                tx.Gain = value;
            }
        }
        void setRxGain(double value){
            if(rx.Gain != value){
                rx.Gain = value;
            }
        }

        double getTxSamplingRate() {return tx.SamplingRate;}
        double getRxSamplingRate() {return rx.SamplingRate;}
        void setTxSamplingRate(double value){
            if(tx.SamplingRate != value){
                tx.SamplingRate = value;
            }
        }
        void setRxSamplingRate(double value){
            if(rx.SamplingRate != value){
                rx.SamplingRate = value;
            }
        }

        std::string getTxIPAddress() {return tx.IP_Address;}
        std::string getRxIPAddress() {return rx.IP_Address;}
        void setTxIPAddress(std::string value){
            if(tx.IP_Address != value){
                tx.IP_Address = value;
            }
        }
        void setRxIPAddress(std::string value){
            if(rx.IP_Address != value){
                rx.IP_Address = value;
            }
        }

        std::string getTxREFSource() {return tx.REF_Source;}
        std::string getRxREFSource() {return rx.REF_Source;}
        void setTxREFSource(std::string value){
            if(tx.REF_Source != value){
                tx.REF_Source = value;
            }
        }
        void setRxREFSource(std::string value){
            if(rx.REF_Source != value){
                rx.REF_Source = value;
            }
        }

        std::string getTxPPSSource() {return tx.PPS_Source;}
        std::string getRxPPSSource() {return rx.PPS_Source;}
        void setTxPPSSource(std::string value){
            if(tx.PPS_Source != value){
                tx.PPS_Source = value;
            }
        }
        void setRxPPSSource(std::string value){
            if(rx.PPS_Source != value){
                rx.PPS_Source = value;
            }
        }

        bool GetSupportWindowSync(){return supportWindowSynchronization;}
        void SetSupportWindowSynch(int value){
            if(value == 1){
                supportWindowSynchronization = true;
            }else{
                supportWindowSynchronization = false;
            }
        }
        int GetWindowSynchronizationSize(){return windowSynchronizationSize;}
        void SetWindowSynchronizationSize(int value){
            windowSynchronizationSize = value;
        }

        int GetNumberOfSymbols(){ return winfo.numSymbols;}

        std::string GetrxUSRP_IPaddress() { return "addr=" + getRxIPAddress();}
        std::string GettxUSRP_IPaddress() { return "addr=" + getTxIPAddress();}

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

        bool supportWindowSynchronization = false;
        int windowSynchronizationSize = 256;

		double txRELrxTimeOffset;
		std::string default_command_list;

        std::vector<std::vector<int>> csvPattern;

		


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
