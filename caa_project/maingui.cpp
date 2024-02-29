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

    connect(&uio, &uiobj::transmissionStatusChanged, this, &mainGUI::updateTransmitStatus);
    connect(&uio,&uiobj::rxSetupStatusChanged,this,&mainGUI::updateRxSetupStatus);
    connect(&uio,&uiobj::txSetupStatusChanged,this,&mainGUI::updateTxSetupStatus);

    connect(&uio,&uiobj::txCarrierFrequencyChanged,this,&mainGUI::updateTxCarrierFrequency);
    connect(&uio,&uiobj::rxCarrierFrequencyChanged,this,&mainGUI::updateRxCarrierFrequency);

    connect(&uio,&uiobj::txGainChanged,this,&mainGUI::updateTxGain);
    connect(&uio,&uiobj::rxGainChanged,this,&mainGUI::updateRxGain);

    connect(&uio,&uiobj::txIPAddressChanged,this,&mainGUI::updateTxIPAddress);
    connect(&uio,&uiobj::rxIPAddressChanged,this,&mainGUI::updateRxIPAddress);

    connect(&uio,&uiobj::txPPSSourceChanged,this,&mainGUI::updateTxPPSSource);
    connect(&uio,&uiobj::rxPPSSourceChanged,this,&mainGUI::updateRxPPSSource);

    connect(&uio,&uiobj::txREFSourceChanged,this,&mainGUI::updateTxREFSource);
    connect(&uio,&uiobj::rxREFSourceChanged,this,&mainGUI::updateRxREFSource);

    processingTimer.setInterval(500);
    processingTimer.setSingleShot(false);
    processingTimer.start();

    uio.setTransmissionInProgress(false);

    //uio.ForceUpdateAll();

}

void mainGUI::updateTransmitStatus(bool status){
    if(status){
        SetWidgetColor(ui->indicator_tx_in_progress,9433252);
        transmissionStartTime = QDateTime::currentDateTime();
        connect(&processingTimer, &QTimer::timeout, this, &mainGUI::updateTransmissionDuration);
    }else{
        SetWidgetColor(ui->indicator_tx_in_progress,16146769);
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::updateTransmissionDuration);
    }
}

void mainGUI::updateTxSetupStatus(bool status)
{
    //if(status){
        bool txSetupStatus = radObj->txUSRPSetup;
        ui->button_transmit->setEnabled(txSetupStatus);
    //}
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
    uio.setRadioSysObject(radObj);
}

void mainGUI::updateTxCarrierFrequency(bool status)
{
    if(status){
        double value = radObj->sysConf.getTxCarrierFrequency();
        ui->lineEdit_tx_fc->setText(QString::number(value/1e9));
        ui->hslider_tx_fc->setValue(value/1e6);
    }
}

void mainGUI::updateRxCarrierFrequency(bool status)
{
    if(status){
        double value = radObj->sysConf.getRxCarrierFrequency();
        ui->lineEdit_rx_fc->setText(QString::number(value/1e9));
        ui->hslider_rx_fc->setValue(value/1e6);
    }
}

void mainGUI::updateTxGain(bool status)
{
    if(status){
        double value = radObj->sysConf.getTxGain();
        ui->lineEdit_tx_gain->setText(QString::number(value));
        ui->vslider_tx_gain->setValue(value);
    }
}

void mainGUI::updateRxGain(bool status)
{
    if(status){
        double value = radObj->sysConf.getRxGain();
        ui->lineEdit_rx_gain->setText(QString::number(value));
        ui->vslider_rx_gain->setValue(value);
    }
}

void mainGUI::updateTxIPAddress(bool status)
{
    if(status){
        ui->lineEdit_tx_ip->setText(QString::fromStdString(radObj->sysConf.getTxIPAddress()));
    }
}

void mainGUI::updateRxIPAddress(bool status)
{
    if(status){
        ui->lineEdit_rx_ip->setText(QString::fromStdString(radObj->sysConf.getRxIPAddress()));
    }
}

void mainGUI::updateTxPPSSource(bool status)
{
    if(uio.getTxPPSSource() == "external"){
        ui->buttonGroup_tx_pps->button(-2)->setChecked(true);
    }else{
        ui->buttonGroup_tx_pps->button(-3)->setChecked(true);
    }
}

void mainGUI::updateRxPPSSource(bool status)
{
    if(uio.getRxPPSSource() == "external"){
        ui->buttonGroup_rx_pps->button(-2)->setChecked(true);
    }else{
        ui->buttonGroup_rx_pps->button(-3)->setChecked(true);
    }
}

void mainGUI::updateTxREFSource(bool status)
{
    if(uio.getTxREFSource() == "external"){
        ui->buttonGroup_tx_ref->button(-2)->setChecked(true);
    }else{
        ui->buttonGroup_tx_ref->button(-3)->setChecked(true);
    }
}

void mainGUI::updateRxREFSource(bool status)
{
    if(uio.getRxREFSource() == "external"){
        ui->buttonGroup_rx_ref->button(-2)->setChecked(true);
    }else{
        ui->buttonGroup_rx_ref->button(-3)->setChecked(true);
    }
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

void mainGUI::addStatusUpdate(QString entry, QTableWidget *table)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

    table->insertRow(0);

    QTableWidgetItem *item1 = new QTableWidgetItem(currentDateTimeString);
    QTableWidgetItem *item2 = new QTableWidgetItem(entry);

    // Add items to the table widget
    table->setItem(0, 0, item1); // Row 0, Column 0
    table->setItem(0, 1, item2); // Row 0, Column 1

}

void mainGUI::on_button_rx_test_connection_released()
{
    SetWidgetColor(ui->indicator_rx_connection,9433252);
}


void mainGUI::on_button_tx_test_connection_released()
{
    SetWidgetColor(ui->indicator_tx_connection,9433252);
}

void mainGUI::updateTransmissionDuration()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int diff_seconds = transmissionStartTime.secsTo(currentDateTime);

    int hours = diff_seconds / 3600;
    int minutes = (diff_seconds % 3600) / 60;
    int seconds = diff_seconds % 60;

    QString hours_ = QString("%1").arg(hours, 2, 10, QChar('0'));
    QString minutes_ = QString("%1").arg(minutes, 2, 10, QChar('0'));
    QString seconds_ = QString("%1").arg(seconds, 2, 10, QChar('0'));

    ui->label_tx_duration->setText(hours_ +":"+minutes_+":"+seconds_);
}

void mainGUI::on_buttonGroup_tx_pps_buttonClicked(int val)
{    
    if(val == -2){
        uio.setTxPPSSource("external");
    }else{
        uio.setTxPPSSource("none");
    }
}

void mainGUI::on_buttonGroup_tx_ref_buttonClicked(int val)
{
    if(val == -2){
        uio.setTxREFSource("external");
    }else{
        uio.setTxREFSource("internal");
    }
}

void mainGUI::on_buttonGroup_rx_pps_buttonClicked(int val)
{
    if(val == -2){
        uio.setRxPPSSource("external");
    }else{
        uio.setRxPPSSource("none");
    }
}

void mainGUI::on_buttonGroup_rx_ref_buttonClicked(int val)
{
    if(val == -2){
        uio.setRxREFSource("external");
    }else{
        uio.setRxREFSource("internal");
    }
}



void mainGUI::on_button_load_cfg_released()
{
    if(radObj->readConfigFile()){
        //SetWidgetColor(ui->button_load_cfg,9433252);


        uio.ForceUpdateAll();

        addStatusUpdate("Success loading configuration",ui->tableWidget_status);

        // Set PPS and REF Source
        // if(radObj->sysConf.tx.PPS_Source == "external"){
        //     ui->buttonGroup_tx_pps->button(-2)->setChecked(true);
        // }else{
        //     ui->buttonGroup_tx_pps->button(-3)->setChecked(true);
        // }
        // if(radObj->sysConf.tx.REF_Source == "external"){
        //     ui->buttonGroup_tx_ref->button(-2)->setChecked(true);
        // }else{
        //     ui->buttonGroup_tx_ref->button(-3)->setChecked(true);
        // }
        // if(radObj->sysConf.rx.PPS_Source == "external"){
        //     ui->buttonGroup_rx_pps->button(-2)->setChecked(true);
        // }else{
        //     ui->buttonGroup_rx_pps->button(-3)->setChecked(true);
        // }
        // if(radObj->sysConf.rx.REF_Source == "external"){
        //     ui->buttonGroup_rx_ref->button(-2)->setChecked(true);
        // }else{
        //     ui->buttonGroup_rx_ref->button(-3)->setChecked(true);
        // }
    }else{
        addStatusUpdate("Error loading configuration",ui->tableWidget_status);
    }
}


void mainGUI::on_vslider_tx_gain_valueChanged(int value)
{
    uio.setTxGain(value);
}


void mainGUI::on_lineEdit_tx_gain_textEdited(const QString &arg1)
{
    double tx_gain = arg1.toDouble();
    uio.setTxGain(tx_gain);
}


void mainGUI::on_vslider_rx_gain_valueChanged(int value)
{
    uio.setRxGain(value);
}


void mainGUI::on_lineEdit_rx_gain_textEdited(const QString &arg1)
{
    double rx_gain = arg1.toDouble();
    uio.setRxGain(rx_gain);
}


void mainGUI::on_hslider_tx_fc_valueChanged(int value)
{
    uio.setTxCarrierFrequency(value*1e6);
}

void mainGUI::on_lineEdit_tx_fc_editingFinished()
{
    QString arg1 = ui->lineEdit_tx_fc->text();
    double tx_fc = arg1.toDouble();
    tx_fc = std::round(tx_fc*100)/100;
    uio.setTxCarrierFrequency(tx_fc*1e9);
}


void mainGUI::on_lineEdit_rx_fc_editingFinished()
{
    QString arg1 = ui->lineEdit_rx_fc->text();
    double rx_fc = arg1.toDouble();
    rx_fc = std::round(rx_fc*100)/100;
    uio.setRxCarrierFrequency(rx_fc*1e9);
}


void mainGUI::on_hslider_rx_fc_valueChanged(int value)
{
    uio.setRxCarrierFrequency(value*1e6);
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
    addStatusUpdate("Initalized USRP Configuration",ui->tableWidget_status);
    ui->button_apply_config->setEnabled(false);

    connect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_USRP_setup);

    radObj->setupTxUSRP();
    radObj->setupRxUSRP();

    uio.setTxSetupStatus(true);
    uio.setRxSetupStatus(true);

}

void mainGUI::processing_USRP_setup()
{
    std::cout << "Triggered!" << std::endl;
    if(radObj->rxUSRPSetup && radObj->txUSRPSetup){
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_USRP_setup);
        addStatusUpdate("Finished USRP Configuration",ui->tableWidget_status);
        ui->button_apply_config->setEnabled(true);
    }
}

// CONTROL SECTION
void mainGUI::on_button_transmit_released()
{
    radObj->startTransmission();
    uio.setTransmissionInProgress(true);
    addStatusUpdate("Transmission Initialized",ui->tableWidget_status);

}


void mainGUI::on_button_load_data_released()
{
    if(radObj->readConfigSignalFile((ui->lineEdit_sig_config->text()).toStdString())){
        SetWidgetColor(ui->indicator_sig_config,9433252);
        addStatusUpdate("Signal file loaded",ui->tableWidget_status);
    }else{
        SetWidgetColor(ui->indicator_sig_config,16380011);
    }
}


void mainGUI::on_button_tx_stop_released()
{
    radObj->StopAll();
    uio.setTransmissionInProgress(false);
    addStatusUpdate("Transmission terminated",ui->tableWidget_status);

}


void mainGUI::on_button_apply_config_clicked()
{

}


void mainGUI::on_button_save_cf_released()
{
    radObj->writeConfigFile();
    addStatusUpdate("Configuration saved",ui->tableWidget_status);

}


void mainGUI::on_lineEdit_tx_ip_editingFinished()
{
    uio.setTxIPAddress((ui->lineEdit_tx_ip->text()).toStdString());
}


void mainGUI::on_lineEdit_rx_ip_editingFinished()
{
    uio.setRxIPAddress((ui->lineEdit_rx_ip->text()).toStdString());
}

