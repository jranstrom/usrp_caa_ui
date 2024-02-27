#include "maingui.h"
#include "./ui_maingui.h"
#include "custom/RadioSysConfig.h"
#include "custom/matHF.h"
#include <iostream>
#include <QTimer>


mainGUI::mainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainGUI)
{
    ui->setupUi(this);

    connect(uio, &uiobj::transmissionStatusChanged, this, &mainGUI::updateTransmitStatus);
    connect(uio,&uiobj::rxSetupStatusChanged,this,&mainGUI::)

    uio->setTransmissionInProgress(false);

     std::cout << "Is this korv?" << std::endl;
}

void mainGUI::updateTransmitStatus(bool status){
    if(status){
        SetWidgetColor(ui->indicator_tx_in_progress,9433252);
    }else{
        SetWidgetColor(ui->indicator_tx_in_progress,16146769);
    }
}

void mainGUI::updateTxSetupStatus(bool status)
{

}

void mainGUI::updateRxSetupStatus(bool status)
{

}

mainGUI::~mainGUI()
{
    delete ui;
}

void mainGUI::setRadioSysObject(RadioSysObject *RadObj)
{
    radObj = RadObj;
}

void mainGUI::SetWidgetColor(QWidget *widg,int colorc){

    // YELLOW :: 16380011
    // RED :: 16146769
    // GREEN :: 9433252
    // GREY :: 12631996

    widg->setAutoFillBackground(true);
    QPalette palette = widg->palette();
    palette.setColor(QPalette::Button, QColor(colorc));
    widg->setPalette(palette);
    widg->show();
}

void mainGUI::on_button_rx_test_connection_released()
{
    SetWidgetColor(ui->indicator_rx_connection,9433252);
}


void mainGUI::on_button_tx_test_connection_released()
{
    SetWidgetColor(ui->indicator_tx_connection,9433252);
}

void mainGUI::on_buttonGroup_tx_pps_buttonClicked(int val)
{
    // if(val == -2){
    //     radObj->sysConf.tx.PPS_Source = "external";
    // }else{
    //     radObj->sysConf.tx.PPS_Source = "internal";
    // }
}

void mainGUI::on_buttonGroup_tx_ref_buttonClicked(int val)
{
    // if(val == -2){
    //     radObj->sysConf.tx.REF_Source = "external";
    // }else{
    //     radObj->sysConf.tx.REF_Source = "internal";
    // }
}

void mainGUI::on_buttonGroup_rx_pps_buttonClicked(int val)
{
    // if(val == -2){
    //     radObj->sysConf.rx.PPS_Source = "external";
    // }else{
    //     radObj->sysConf.rx.PPS_Source = "internal";
    // }
}

void mainGUI::on_buttonGroup_rx_ref_buttonClicked(int val)
{
    // if(val == -2){
    //     radObj->sysConf.rx.REF_Source = "external";
    // }else{
    //     radObj->sysConf.rx.REF_Source = "internal";
    // }
}



void mainGUI::on_button_load_cfg_released()
{
    if(radObj->readConfigFile()){
        //SetWidgetColor(ui->button_load_cfg,9433252);
        ui->lineEdit_tx_ip->setText(QString::fromStdString(radObj->sysConf.GetCleanTX_IPaddress()));
        ui->lineEdit_rx_ip->setText(QString::fromStdString(radObj->sysConf.GetCleanRX_IPaddress()));

        ui->vslider_tx_gain->setValue(radObj->sysConf.tx.Gain);
        ui->vslider_rx_gain->setValue(radObj->sysConf.rx.Gain);
        double tx_fc = static_cast<double>(std::round((radObj->sysConf.tx.CarrierFrequency)/1e6));
        ui->lineEdit_tx_fc->setText(QString::number(tx_fc/1e3));
        ui->hslider_tx_fc->setValue(tx_fc);
        double rx_fc = static_cast<double>(std::round((radObj->sysConf.rx.CarrierFrequency)/1e6));
        ui->lineEdit_rx_fc->setText(QString::number(rx_fc/1e3));
        ui->hslider_rx_fc->setValue(rx_fc);

        // Set PPS and REF Source
        if(radObj->sysConf.tx.PPS_Source == "external"){
            ui->buttonGroup_tx_pps->button(-2)->setChecked(true);
        }else{
            ui->buttonGroup_tx_pps->button(-3)->setChecked(true);
        }
        if(radObj->sysConf.tx.REF_Source == "external"){
            ui->buttonGroup_tx_ref->button(-2)->setChecked(true);
        }else{
            ui->buttonGroup_tx_ref->button(-3)->setChecked(true);
        }
        if(radObj->sysConf.rx.PPS_Source == "external"){
            ui->buttonGroup_rx_pps->button(-2)->setChecked(true);
        }else{
            ui->buttonGroup_rx_pps->button(-3)->setChecked(true);
        }
        if(radObj->sysConf.rx.REF_Source == "external"){
            ui->buttonGroup_rx_ref->button(-2)->setChecked(true);
        }else{
            ui->buttonGroup_rx_ref->button(-3)->setChecked(true);
        }
    }
}


void mainGUI::on_vslider_tx_gain_valueChanged(int value)
{
    ui->lineEdit_tx_gain->setText(QString::number(value));
}


void mainGUI::on_lineEdit_tx_gain_textEdited(const QString &arg1)
{
    int tx_gain = arg1.toInt();
    bool changed = false;
    if(tx_gain > 15){
        tx_gain = 15;
    }else if(tx_gain < -15){
        tx_gain = -15;
    }

    ui->lineEdit_tx_gain->setText(QString::number(tx_gain));

    ui->vslider_tx_gain->setValue(tx_gain);
}


void mainGUI::on_vslider_rx_gain_valueChanged(int value)
{
     ui->lineEdit_rx_gain->setText(QString::number(value));
}


void mainGUI::on_lineEdit_rx_gain_textEdited(const QString &arg1)
{
    int rx_gain = arg1.toInt();
    bool changed = false;
    if(rx_gain > 20){
        rx_gain = 20;
        changed = true;
    }else if(rx_gain < -20){
        rx_gain = -20;
        changed = true;
    }

    if(changed){
        ui->lineEdit_rx_gain->setText(QString::number(rx_gain));
    }
    ui->vslider_rx_gain->setValue(rx_gain);
}


void mainGUI::on_hslider_tx_fc_valueChanged(int value)
{
    double dvalue = value/1e3;
    ui->lineEdit_tx_fc->setText(QString::number(dvalue));
}

void mainGUI::on_lineEdit_tx_fc_editingFinished()
{
    QString arg1 = ui->lineEdit_tx_fc->text();
    double tx_fc = arg1.toDouble();
    int itx_fc = static_cast<int>(std::round(tx_fc*1e3));
    bool changed = false;
    if(itx_fc > 6000){
        itx_fc = 6000;
        changed = true;
    }else if(itx_fc < 1200){
        itx_fc = 1200;
        changed = true;
    }

    if(changed){
        ui->lineEdit_tx_fc->setText(QString::number(itx_fc));
    }
    ui->hslider_tx_fc->setValue(itx_fc);
}


void mainGUI::on_lineEdit_rx_fc_editingFinished()
{
    QString arg1 = ui->lineEdit_rx_fc->text();
    double rx_fc = arg1.toDouble();
    int irx_fc = static_cast<int>(std::round(rx_fc*1e3));
    bool changed = false;
    if(irx_fc > 6000){
        irx_fc = 6000;
        changed = true;
    }else if(irx_fc < 1200){
        irx_fc = 1200;
        changed = true;
    }

    if(changed){
        ui->lineEdit_rx_fc->setText(QString::number(irx_fc));
    }
    ui->hslider_rx_fc->setValue(irx_fc);
}


void mainGUI::on_hslider_rx_fc_valueChanged(int value)
{
    double dvalue = value/1e3;
    ui->lineEdit_rx_fc->setText(QString::number(dvalue));
}


void mainGUI::applyTxConfig()
{
    radObj->sysConf.tx.CarrierFrequency = (ui->lineEdit_tx_fc->text()).toDouble()*1e9;
    radObj->sysConf.tx.Gain = (ui->lineEdit_tx_gain->text()).toDouble();
}

void mainGUI::applyRxConfig()
{
    radObj->sysConf.rx.CarrierFrequency = (ui->lineEdit_rx_fc->text()).toDouble()*1e9;
    radObj->sysConf.rx.Gain = (ui->lineEdit_rx_gain->text()).toDouble();
}

void mainGUI::on_button_apply_config_released()
{
    applyTxConfig();
    applyRxConfig();

    uio->setTxSetupStatus(false);
    uio->setRxSetupStatus(false);

    radObj->setupTxUSRP();
    radObj->setupRxUSRP();

    uio->setTxSetupStatus(true);
    uio->setRxSetupStatus(true);
}

// CONTROL SECTION
void mainGUI::on_button_transmit_released()
{
    radObj->startTransmission();
    uio->setTransmissionInProgress(true);
}


void mainGUI::on_button_load_data_released()
{
    if(radObj->readConfigSignalFile((ui->lineEdit_sig_config->text()).toStdString())){
        SetWidgetColor(ui->indicator_sig_config,9433252);
    }else{
        SetWidgetColor(ui->indicator_sig_config,16380011);
    }
}


void mainGUI::on_button_tx_stop_released()
{
    radObj->StopAll();
    uio->setTransmissionInProgress(false);
}


void mainGUI::on_button_apply_config_clicked()
{

}


void mainGUI::on_button_save_cf_released()
{
    radObj->writeConfigFile();
}

