#ifndef MCCONTROLWIDGET_H
#define MCCONTROLWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>
#include <iostream>
#include <QLabel>
#include "tcom_ui.h"
#include "custom/labelandfieldwidget.h"

class MCControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MCControlWidget(QWidget *parent = nullptr,int id=0,std::string mcType="unknown");

    int getIdentifier() {return identifier;}
    std::string getPort() {return port;}
    void setPort(std::string value){
        if(value != port){
            port = value;
            port_w->setFieldText(value);
        }
    }

    void setMAC(std::string value){
        if(value != MAC){
            MAC = value;
            mac_w->setFieldText(value);

            mctype_label->setText(QString::fromStdString(MCType + " " + value));
        }
    }

    void setTCOM(Tcom_ui * tcom_ptr){
        tcom = tcom_ptr;
    }

signals:

private slots:
   void onStatusButtonReleased();
   void onInfoButtonReleased();
   void onAutoButtonReleased();

   void responseChanged(std::string value);

private :
    QGroupBox *main_group;
    QPushButton *status_button;
    QPushButton *info_button;
    QPushButton *auto_button;

    QVBoxLayout *main_group_layout;
    QHBoxLayout *info_layout;
    QGridLayout  *control_layout;

    QLabel * mctype_label;

    LabelandFieldWidget * port_w;
    LabelandFieldWidget * mac_w;
    LabelandFieldWidget * response_w;

    Tcom_ui * tcom;

    int identifier;
    std::string port;
    std::string MAC;
    std::string MCType;
};

#endif // MCCONTROLWIDGET_H
