#include "cradioproperty.h"
#include <cmath>
#include <iomanip>
#include <sstream>



std::unordered_map<std::string, double> cRadioProperty::unitModifiers = {
    {"Hz",1.0},{"kHz",1e3},{"MHz",1e6},{"GHz",1e9},
    {"dB",1.0},{"dBm",1.0}
};

cRadioProperty::cRadioProperty(std::string name_p, std::string unit_p) {
    propertyName = name_p;
    propertyUnit = unit_p;
}

void cRadioProperty::setPropertyUnit(std::string unit_p, bool &ok)
{
    if(validateUnit(unit_p)){
        ok  = true;
        propertyUnit = unit_p;
    }else{
        ok = false;
    }
}

std::string cRadioFrequencyProperty::calculatePropertyAsString(std::string str_p, std::string unit_p, bool &ok, int modifier)
{
    std::string response = "";
    // validate unit
    if(validateUnit(unit_p)){
        ok = true;
        double value_t;
        try {
            value_t = std::stod(str_p);  // Converts string to double
        } catch (...) {
            ok = false;
            return response; // conversion failed
        }

        // success converting string to double
        if(unit_p == "kHz"){
            value_t = value_t*1e3;
        }else if(unit_p == "MHz"){
            value_t = value_t*1e6;
        }else if(unit_p == "GHz"){
            value_t = value_t*1e9;
        }

        value_t = std::round(value_t);

        ok = true;
        response = convertFrequencyToString(value_t,propertyUnit,modifier);

    }else{
        ok = false;
    }

    return response;
}

bool cRadioFrequencyProperty::isPropertyEqualToString(std::string val_p, std::string unit_p, bool &ok)
{
    ok = true;

    if(validateUnit(unit_p) == false){
        ok = false;
        return false;
    }

    double value_t;
    try {
        value_t = std::stod(val_p);  // Converts string to double
    } catch (...) {
        ok = false;
        return false;
    }

    value_t = std::round(value_t*unitModifiers[unit_p]);

    return (value_t == std::round(value));
}

std::string cRadioFrequencyProperty::convertFrequencyToString(double value, std::string unit_p, int precision)
{
    double m_frequency = 1;
    if(propertyUnit == "kHz"){
        m_frequency = 1e3;
    }else if(propertyUnit == "MHz"){
        m_frequency = 1e6;
    }else if(propertyUnit == "GHz"){
        m_frequency = 1e9;
    }

    double r_frequency = value/m_frequency;

    if(precision > -1){
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << r_frequency;
        return oss.str();
    }

    return std::to_string(r_frequency);
}

cRadioPPSandREFSourceProperty::cRadioPPSandREFSourceProperty(std::string name_p, std::string unit_p, std::string value_p) :
    cRadioProperty(name_p,unit_p)
{
    if(verifyInput(value_p)){
        source = value_p;
    }
}

void cRadioPPSandREFSourceProperty::setProperty(std::string value_p, bool &ok)
{
    ok = false;
    if(verifyInput(value_p)){
        source  = value_p;
        ok = true;
    }
}

bool cRadioPPSandREFSourceProperty::verifyInput(std::string value_p)
{
    return (value_p == "internal" || value_p == "external");
}

std::string cRadioDoubleProperty::getPropertyValueStr(int modifier)
{
    double tmp_value = value / unitModifiers[propertyUnit];
    if(modifier > -1){
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(modifier) << tmp_value;
        return oss.str();
    }

    return std::to_string(tmp_value);
}

void cRadioDoubleProperty::setPropertyByStr(std::string str_p, bool &ok)
{
    ok = true;
    double value_t;
    try {
        value_t = std::stod(str_p);  // Converts string to double
    } catch (...) {
        ok = false;
    }
    if(ok){
        value = value_t*unitModifiers[propertyUnit];
    }
}

bool cRadioDoubleProperty::isPropertyEqualToString(std::string val_p, std::string unit_p, bool &ok)
{
    ok = true;

    if(validateUnit(unit_p) == false){
        ok = false;
        return false;
    }

    double value_t;
    try {
        value_t = std::stod(val_p);  // Converts string to double
    } catch (...) {
        ok = false;
        return false;
    }

    return (value_t*unitModifiers[propertyUnit] == value);

}

cRadioStringProperty::cRadioStringProperty(std::string name_p, std::string unit_p, std::string value_p) : cRadioProperty(name_p,unit_p)
{
    if(validateInputString(value_p)){
        value = value_p;
    }
}

void cRadioStringProperty::setProperty(std::string value_p, bool &ok)
{
    ok = false;
    if(validateInputString(value_p)){
        value = value_p;
        ok = true;
    }
}

cRadioDoubleProperty::cRadioDoubleProperty(std::string name_p, std::string unit_p, double value_p) : cRadioProperty(name_p, unit_p)
{
    value = value_p;
}
