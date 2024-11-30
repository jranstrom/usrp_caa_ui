#ifndef CRADIOPROPERTY_H
#define CRADIOPROPERTY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

class cRadioProperty
{
public:

    cRadioProperty(){};
    cRadioProperty(std::string name_p,std::string unit_p);

    virtual std::string getPropertyValueStr(int modifier=-1) {return "";}
    virtual double getPropertyValueDouble(){throw std::runtime_error("getPropertyValueDouble not implemented");}
    virtual double getPropertyValueInt(){throw std::runtime_error("getPropertyValueInt not implemented");}
    virtual double getPropertyValueBool(){throw std::runtime_error("getPropertyValueBool not implemented");}

    virtual std::string getPropertyName() { return propertyName;}

    virtual void setProperty(double value_p,bool &ok){ throw std::runtime_error("setProperty for input double not implemented");}
    virtual void setProperty(std::string value_p,bool &ok) { throw std::runtime_error("setProperty for input std::string not implemented");}
    virtual void setProperty(int value_p,bool &ok){ throw std::runtime_error("setProperty for input int not implemented");}
    virtual void setProperty(bool value_p, bool &ok){ throw std::runtime_error("setProperty for input boolean not implemented");}

    virtual void setPropertyUnit(std::string unit_p,bool &ok);

    virtual bool validateUnit(std::string unit_p) {return true;}

    virtual void setPropertyByStr(std::string str_p,bool &ok){ throw std::runtime_error("setPropertyByStr not implemented");}

    virtual bool isPropertyEqual(std::string str_p) {throw std::runtime_error("isPropertyEqual not implemented");}
    virtual bool isPropertyEqualToString(std::string val_p,std::string unit_p,bool &ok){throw std::runtime_error("isPropertyEqualToString not implemented");}

    std::string getPropertyUnit(){return propertyUnit;}


    virtual std::string calculatePropertyAsString(std::string str_p,std::string unit_p, bool &ok, int modifier=-1) {throw std::runtime_error("calculatePropertyAsString not implemented");}


    std::string propertyName = "";
    std::string propertyUnit = "";

    static std::unordered_map<std::string, double> unitModifiers;
};

class cRadioStringProperty : public cRadioProperty {
public:
    cRadioStringProperty(){};
    cRadioStringProperty(std::string name_p,std::string unit_p, std::string value_p);

    void setProperty(std::string value_p,bool &ok) override;
    void setPropertyByStr(std::string value_p,bool &ok) override {setProperty(value_p,ok);}
    std::string getPropertyValueStr(int modifier=-1) override {return value;}
    bool isPropertyEqualToString(std::string str_p,std::string unit,bool &ok) override {ok = true;return str_p == value;}

    virtual bool validateInputString(std::string input) {return true;}

    std::string value;

private:

};

class cRadioDoubleProperty : public cRadioProperty
{
public:
    cRadioDoubleProperty() {};
    cRadioDoubleProperty(std::string name_p,std::string unit_p, double value_p);

    double getPropertyValueDouble() override {return value;}
    std::string getPropertyValueStr(int modifier=-1) override;

    virtual void setProperty(double value_p,bool &ok) override {value = value_p;ok=true;};
    void setPropertyByStr(std::string str_p,bool &ok) override;

    virtual bool validateInputDouble(double input) {return true;}

    bool isPropertyEqualToString(std::string val_p,std::string unit_p,bool &ok) override;


    double value;

};

class cRadioGainProperty : public cRadioDoubleProperty
{
public:
    cRadioGainProperty(std::string name_p,std::string unit_p, double value_p) : cRadioDoubleProperty(name_p,unit_p,value_p) {}
    bool validateUnit(std::string unit_p) override {return (unit_p == "dB" || unit_p == "dBm");}
};

class cRadioFrequencyProperty : public cRadioDoubleProperty
{
public:
    cRadioFrequencyProperty(){}
    cRadioFrequencyProperty(std::string name_p,std::string unit_p,double frequency_p) : cRadioDoubleProperty(name_p,unit_p,frequency_p) {}

    std::string calculatePropertyAsString(std::string str_p,std::string unit_p, bool &ok, int modifier=-1) override;
    bool isPropertyEqualToString(std::string val_p,std::string unit_p,bool &ok) override;


    bool validateUnit(std::string unit_p) override {return (unit_p == "Hz" || unit_p == "kHz" || unit_p == "MHz" || unit_p == "GHz");}

private :
    std::string convertFrequencyToString(double value,std::string unit_p,int precision);
};

class cRadioREFSourceProperty : public cRadioStringProperty {
public:
    cRadioREFSourceProperty(std::string name_p,std::string unit_p, std::string value_p) : cRadioStringProperty(name_p,unit_p,value_p) {}

    bool validateInputString(std::string input) {return(input == "external" || input == "none");}
};

class cRadioPPSSourceProperty : public cRadioStringProperty {
public:
    cRadioPPSSourceProperty(std::string name_p,std::string unit_p, std::string value_p) : cRadioStringProperty(name_p,unit_p,value_p) {}

    bool validateInputString(std::string input) {return(input == "external" || input == "none");}
};

class cRadioPPSandREFSourceProperty : public cRadioProperty
{
public :
    cRadioPPSandREFSourceProperty(){}
    cRadioPPSandREFSourceProperty(std::string name_p,std::string unit_p, std::string value_p);
    std::string getPropertyValueStr(int modifier) override {return source;}
    void setProperty(std::string value_p,bool &ok) override;

    std::string source;
private :
    bool verifyInput(std::string value_p);
};

class cRadioAntennaProperty : public cRadioStringProperty
{
public:
    cRadioAntennaProperty(std::string name_p,std::string unit_p, std::string value_p) : cRadioStringProperty(name_p,unit_p,value_p) {}

    bool validateInputString(std::string input) {return(input == "TX/RX" || input == "RX2" || input == "CAL");}
};

#endif // CRADIOPROPERTY_H
