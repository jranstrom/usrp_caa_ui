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
#include "custom/indicatorbuttonwidget.h"
#include "custom/indicatorgroupwidget.h"

class MCControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MCControlWidget(std::string mcPort="",Tcom_ui * tcom_ptr=nullptr,int id=0,std::string mcType="unknown",QWidget *parent=nullptr);

    void InitializeMCControlWidget(int id,std::string mcType,
                                   std::string mcPort,Tcom_ui * tcom_ptr,QWidget * parent);
    int getIdentifier() {return identifier;}
    std::string getMCType() {return MCType;}
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

            mainGroupBox->setTitle(QString::fromStdString(MCType + " " + value));
            //mctype_label->setText(QString::fromStdString(MCType + " " + value));
        }
    }

    void setTCOM(Tcom_ui * tcom_ptr){
        tcom = tcom_ptr;
    }

    void requestRead();
    void requestSelectElement(int value);
    void requestSelectUE(int value);

signals:
    void cycleButtonReleased(int id);

private slots:
   void onStatusButtonReleased();
   void onInfoButtonReleased();
   void onAutoButtonReleased();
   void onDebugButtonReleased();
   void onCycleButtonReleased();
   void onResetButtonReleased();

   void responseChanged(std::string value);

   void onElementButtonReleased(int id);
   void onUEButtonReleased(int id);

private :
    QGroupBox *mainGroupBox;
    QPushButton *status_button;
    QPushButton *info_button;
    QPushButton *debug_button;
    QPushButton *auto_button;
    QPushButton *cycle_button;
    QPushButton *reset_button;

    QHBoxLayout *outerContainerLayout;
    QVBoxLayout *mainGroupBoxLayout;
    QHBoxLayout *infoLayout;
    QGridLayout  *controlLayout;

    QLabel * mctype_label;

    LabelandFieldWidget * port_w;
    LabelandFieldWidget * mac_w;
    LabelandFieldWidget * response_w;

    IndicatorGroupWidget * element_group_w;
    IndicatorGroupWidget * ue_group_w;

    Tcom_ui * tcom;

    int identifier;
    std::string port;
    std::string MAC;
    std::string MCType;

    void configureMCfromResponse(std::string value);
    int getNumberAfterMatch(std::string originalString,std::string substring);


};

#endif // MCCONTROLWIDGET_H
