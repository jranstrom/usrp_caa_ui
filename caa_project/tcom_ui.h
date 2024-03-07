#ifndef TCOM_UI_H
#define TCOM_UI_H

#include <QObject>
#include "custom/TeensyCOM.h"

class Tcom_ui : public QObject
{
    Q_OBJECT
public:
    explicit Tcom_ui(QObject *parent = nullptr);

    void requestAvailableDevices();
    void requestToConnect(std::string port);
    std::vector<std::string> getDeviceList() {return DeviceList;}

signals:
    void availableDevicesChanged(std::vector<std::string> & value);
    void connectionChanged(std::string value);

private:

    TeensyCOM tcom = TeensyCOM(115200);

    std::vector<std::string> DeviceList;
};

#endif // TCOM_UI_H
