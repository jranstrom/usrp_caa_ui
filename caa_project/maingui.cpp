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
    connect(&uio, &uiobj::receptionStatusChanged,this,&mainGUI::updateReceiveStatus);

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

    connect(&uio,&uiobj::rxLO_OffsetChanged,this,&mainGUI::updateRxLO_Offset);

    connect(&uio,&uiobj::txSamplingRateChanged,this,&mainGUI::updateTxSamplingRate);
    connect(&uio,&uiobj::rxSamplingRateChanged,this,&mainGUI::updateRxSamplingRate);


    connect(&uio,&uiobj::USRPConfigurationChanged,this,&mainGUI::onUSRPConfigurationChanged);
    //connect(&uio,&uiobj::txSetupStatusChanged,this,&mainGUI::updateUSRPSetupChanged);

    connect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_USRP_setup);

    connect(&tcom,&Tcom_ui::availableDevicesChanged,this,&mainGUI::updateAvailableDevices);
    connect(&tcom,&Tcom_ui::connectionChanged,this,&mainGUI::updateConnection);
    connect(&tcom,&Tcom_ui::responseChanged,this,&mainGUI::updateResponse);
    connect(&tcom,&Tcom_ui::mcIdChanged,this,&mainGUI::updateMCId);
    connect(&tcom,&Tcom_ui::mcTypeChanged,this,&mainGUI::updateMCType);

    SetWidgetColor(ui->indicator_captured_buffer,16146769);

    processingTimer.setInterval(500);
    processingTimer.setSingleShot(false);
    processingTimer.start();

    //uio.ForceUpdateAll();

}

void mainGUI::updateTransmitStatus(bool status){
    if(status){
        addStatusUpdate("Transmission Initialized",ui->tableWidget_status);
        SetWidgetColor(ui->indicator_tx_in_progress,9433252);
        transmissionStartTime = QDateTime::currentDateTime();
        connect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackTransmissionProcess);
    }else{
        addStatusUpdate("Transmission terminated",ui->tableWidget_status);
        SetWidgetColor(ui->indicator_tx_in_progress,16146769);
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackTransmissionProcess);
    }
}

void mainGUI::updateReceiveStatus(bool status)
{
    if(status){
        addStatusUpdate("Reception Initialized",ui->tableWidget_status);
        SetWidgetColor(ui->indicator_rx_in_progress,9433252);
        SetWidgetColor(ui->indicator_captured_buffer,16146769); // last capture does not correspond to current
        receptionStartTime = QDateTime::currentDateTime();
        ui->button_write_buffer_to_file->setEnabled(true);
        connect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackReceptionProcess);

    }else{
        addStatusUpdate("Reception terminated",ui->tableWidget_status);
        SetWidgetColor(ui->indicator_rx_in_progress,16146769);
        ui->button_write_buffer_to_file->setEnabled(false);
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackReceptionProcess);
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

void mainGUI::updateRxLO_Offset(bool status)
{
    if(status){
        double value = radObj->sysConf.getRxLO_offset();
        ui->lineEdit_lo_offset->setText(QString::number(value / 1e6));
        ui->vslider_lo_offset->setValue(value / 1e5);
    }
}

mainGUI::~mainGUI()
{
    delete ui;
}

void mainGUI::setRadioSysObject(RadioSysObject *RadObj)
{
    radObj = RadObj;
    uio.setRadioSysObject(radObj);

    uio.ForceUpdateAll();
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

void mainGUI::updateTxSamplingRate(bool status)
{
    if(status){
        double value = radObj->sysConf.getTxSamplingRate() / 1e6 * 1e3;
        ui->lineEdit_tx_rs->setText(QString::number(value));
        ui->hslider_tx_rs->setValue(value);
    }
}

void mainGUI::updateRxSamplingRate(bool status)
{
    if(status){
        double value = radObj->sysConf.getRxSamplingRate() / 1e6 * 1e3;
        ui->lineEdit_rx_rs->setText(QString::number(value));
        ui->hslider_rx_rs->setValue(value);
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

void mainGUI::SetWidgetColorState(QWidget *widg, bool state)
{
    if(state){
        SetWidgetColor(widg,9433252);
    }else{
       SetWidgetColor(widg,16146769);
    }
}

void mainGUI::SetWidgetColorState(QWidget *widg, int state)
{
    // YELLOW :: 3
    // RED :: 0
    // GREEN :: 1
    // GREY :: -1

    if(state == 1){
        SetWidgetColor(widg,9433252);
    }else if(state == 0){
        SetWidgetColor(widg,16146769);
    }else if(state == 3){
        SetWidgetColor(widg,16380011);
    }else if(state == -1){
        SetWidgetColor(widg,12631996);
    }
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

    table->selectRow(0);

}

void mainGUI::on_button_rx_test_connection_released()
{
    SetWidgetColor(ui->indicator_rx_connection,9433252);
}


void mainGUI::on_button_tx_test_connection_released()
{
    SetWidgetColor(ui->indicator_tx_connection,9433252);
}

void mainGUI::trackTransmissionProcess()
{
    if(radObj->isTransmitting()){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int diff_seconds = transmissionStartTime.secsTo(currentDateTime);

    int hours = diff_seconds / 3600;
    int minutes = (diff_seconds % 3600) / 60;
    int seconds = diff_seconds % 60;

    QString hours_ = QString("%1").arg(hours, 2, 10, QChar('0'));
    QString minutes_ = QString("%1").arg(minutes, 2, 10, QChar('0'));
    QString seconds_ = QString("%1").arg(seconds, 2, 10, QChar('0'));

    ui->label_tx_duration->setText(hours_ +":"+minutes_+":"+seconds_);
    }else{
        uio.setTransmissionInProgress(false);
    }
}

void mainGUI::trackReceptionProcess()
{
    if(radObj->isReceiving()){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int diff_seconds = receptionStartTime.secsTo(currentDateTime);

    int hours = diff_seconds / 3600;
    int minutes = (diff_seconds % 3600) / 60;
    int seconds = diff_seconds % 60;

    QString hours_ = QString("%1").arg(hours, 2, 10, QChar('0'));
    QString minutes_ = QString("%1").arg(minutes, 2, 10, QChar('0'));
    QString seconds_ = QString("%1").arg(seconds, 2, 10, QChar('0'));

    ui->label_rx_duration->setText(hours_ +":"+minutes_+":"+seconds_);

    ui->lineEdit_rx_sample_count->setText(QString("%L1").arg(radObj->getRxSampleCount()));
    }else{
        uio.setReceptionInProgress(false);
    }
}

void mainGUI::trackCaptureBufferProcess()
{
    if(not radObj->isWritingBufferToFile()){
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackCaptureBufferProcess);
        addStatusUpdate("Success writing buffer to file",ui->tableWidget_status);
        ui->button_receive->setEnabled(true);
        SetWidgetColor(ui->indicator_captured_buffer,9433252);

        std::vector<std::complex<short>> vc_data = radObj->getCapturedData();

        QVector<double> x(vc_data.size()), y(vc_data.size()); // initialize with entries 0..100
        double max_element = 0;

        for (int i=0; i<vc_data.size(); ++i)
        {
            x[i] = i;
            y[i] = 10 * std::log10(std::norm(vc_data[i]));

            if(y[i] > max_element){
                max_element = y[i];
            }
        }

        ui->captured_sig_plot->addGraph();
        ui->captured_sig_plot->graph(0)->setData(x, y);
        ui->captured_sig_plot->xAxis->setLabel("Sample");
        ui->captured_sig_plot->yAxis->setLabel("Power (dB)");
        ui->captured_sig_plot->xAxis->setRange(0, vc_data.size());
        ui->captured_sig_plot->yAxis->setRange(0, max_element);
        ui->captured_sig_plot->replot();

    }
}

void mainGUI::updateUSRPSetupChanged(bool val)
{
    // Configuration is complete
    if(val && radObj->isRxUSRPConfigured() && radObj->isTxUSRPConfigured()){
        std::cout << "configuation complete" << std::endl;
    }else{
        std::cout << "configuation incomplete" << std::endl;
    }
}

void mainGUI::onUSRPConfigurationChanged(bool val)
{
    if(radObj->isProcessingTxSetup() || radObj->isProcessingRxSetup()){
        usrpSetupInterrupted = true;
    }
    // if(val){
    //     radObj->setRxUSRPConfigured(false);
    //     radObj->setTxUSRPConfigured(false);
    //     SetWidgetColor(ui->indicator_configuration_set,16146769);
    // }
}

void mainGUI::updateTxUSRPSetupStatus(bool val)
{

}

void mainGUI::updateRxUSRPSetupStatus(bool val)
{

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
    if(radObj->readConfigFile((ui->lineEdit_cfg_file->text()).toStdString())){
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
    usrpSetupInterrupted = false;
    pendingConfigurationRequest = true;

    radObj->requestTxUSRPSetup();
    radObj->requestRxUSRPSetup();
}

void mainGUI::processing_USRP_setup()
{
    bool allSetup = true;
    if(not radObj->isProcessingTxSetup() && not radObj->isProcessingRxSetup()){
        if(radObj->isTxUSRPConfigured() && not usrpSetupInterrupted){

        }else{
            allSetup = false;
        }

        if(radObj->isRxUSRPConfigured() && not usrpSetupInterrupted){

        }else{
            allSetup = false;
        }

        if(allSetup){
            SetWidgetColor(ui->indicator_configuration_set,9433252); // Green indicator
            ui->button_receive->setEnabled(true);
            ui->button_transmit->setEnabled(true);
            if(pendingConfigurationRequest){
                addStatusUpdate("Success configurating USRPs",ui->tableWidget_status);
                pendingConfigurationRequest = false;
            }
        }else{
            SetWidgetColor(ui->indicator_configuration_set,16146769); // Red indicator
            ui->button_receive->setEnabled(false);
            ui->button_transmit->setEnabled(false);
            if(pendingConfigurationRequest){
                addStatusUpdate("Failed configurating USRPs",ui->tableWidget_status);
                pendingConfigurationRequest = false;
            }
        }

        ui->button_apply_config->setEnabled(true);
    }else{
       SetWidgetColor(ui->indicator_configuration_set,16380011); // Yellow indicator
        ui->button_apply_config->setEnabled(false);
    }
}

// CONTROL SECTION
void mainGUI::on_button_transmit_released()
{
    radObj->startTransmission();
    uio.setTransmissionInProgress(true);
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
    radObj->stopTransmission();
    uio.setTransmissionInProgress(false);
}


void mainGUI::on_button_apply_config_clicked()
{

}


void mainGUI::on_button_save_cf_released()
{
    if(radObj->writeConfigFile((ui->lineEdit_cfg_file->text()).toStdString())){
        addStatusUpdate("Success saving configuration",ui->tableWidget_status);
    }else{
        addStatusUpdate("Error saving configuration",ui->tableWidget_status);
    }


}


void mainGUI::on_lineEdit_tx_ip_editingFinished()
{
    uio.setTxIPAddress((ui->lineEdit_tx_ip->text()).toStdString());
}


void mainGUI::on_lineEdit_rx_ip_editingFinished()
{
    uio.setRxIPAddress((ui->lineEdit_rx_ip->text()).toStdString());
}


void mainGUI::on_button_receive_released()
{
    radObj->startReception();
    uio.setReceptionInProgress(true);

}


void mainGUI::on_button_rx_stop_released()
{
    radObj->stopReception();
    uio.setReceptionInProgress(false);

}


void mainGUI::on_button_write_buffer_to_file_released()
{
    ui->button_write_buffer_to_file->setEnabled(false);
    ui->button_receive->setEnabled(false);
    SetWidgetColor(ui->indicator_captured_buffer,16380011);
    connect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackCaptureBufferProcess);
    radObj->stopReception();
    radObj->requestWriteBufferToFile((ui->lineEdit_buffer_file_capture_path->text()).toStdString(),
                                     ui->spinBox_capture_samples->value()*1e3);
}


void mainGUI::on_lineEdit_lo_offset_textEdited(const QString &arg1)
{
    //QString arg1 = ui->lineEdit_lo_offset->text();

    double offset = arg1.toDouble();
    uio.setRxLO_Offset(offset*1e6);
}


void mainGUI::on_vslider_lo_offset_valueChanged(int value)
{
    uio.setRxLO_Offset(value*1e5);
}


void mainGUI::on_lineEdit_rx_rs_textEdited(const QString &arg1)
{
    double rx_rs = arg1.toDouble() * 1e6 / 1e3;
    uio.setRxSamplingRate(rx_rs);
}


void mainGUI::on_lineEdit_tx_rs_textEdited(const QString &arg1)
{
    double tx_rs = arg1.toDouble() * 1e6 / 1e3;
    uio.setTxSamplingRate(tx_rs);
}


void mainGUI::on_hslider_rx_rs_valueChanged(int value)
{
    uio.setRxSamplingRate(value * 1e6 / 1e3);
}


void mainGUI::on_hslider_tx_rs_valueChanged(int value)
{
    uio.setTxSamplingRate(value * 1e6 / 1e3);
}


void mainGUI::on_pushButton_released()
{
    tcom.requestAvailableDevices();
}

void mainGUI::updateAvailableDevices(std::vector<std::string> & value)
{
    ui->listWidget_available_devices->clear();
    std::cout << tcom.getDeviceList().size() << std::endl;
    for(int i=0;i<value.size();i++){
        ui->listWidget_available_devices->addItem(QString::fromStdString(value[i]));
    }
}

void mainGUI::updateConnection(std::string value)
{
    if(value != ""){
        tcom.requestMCId();
        tcom.requestStatus();
    }
}

void mainGUI::updateResponse(std::string value)
{
    ui->lineEdit_tcom_response->setText(QString::fromStdString(value));
    size_t pos = value.find("caae");
    int caae_config = 0;
    bool elementStates[] = {false,false,false,false};
    if (pos != std::string::npos) {
        pos += 4;
        std::string number = "";

        while(isdigit(value[pos])){
            number += value[pos++];
        }

        caae_config = std::stoi(number);
        elementStates[0] = static_cast<bool>((caae_config >> 0) & 1);
        elementStates[1] = static_cast<bool>((caae_config >> 1) & 1);
        elementStates[2] = static_cast<bool>((caae_config >> 2) & 1);
        elementStates[3] = static_cast<bool>((caae_config >> 3) & 1);

        SetWidgetColorState(ui->button_cmc_element_1,elementStates[0]);
        SetWidgetColorState(ui->button_cmc_element_2,elementStates[1]);
        SetWidgetColorState(ui->button_cmc_element_3,elementStates[2]);
        SetWidgetColorState(ui->button_cmc_element_4,elementStates[3]);
    }
}

void mainGUI::updateMCId(std::string value)
{
    ui->lineEdit_mc_id->setText(QString::fromStdString(value));
}

void mainGUI::updateMCType(std::string value)
{
    ui->lineEdit_mc_type->setText(QString::fromStdString(value));

    ui->button_mc_status->setEnabled(false);
    ui->button_mc_info->setEnabled(false);
    ui->button_mc_debug_mode->setEnabled(false);
    ui->button_mc_auto_mode->setEnabled(false);
    ui->button_mc_select_ue->setEnabled(false);
    ui->groupBox_cmc_elements->setEnabled(false);
    // ui->button_cmc_element_1->setEnabled(false);
    // ui->button_cmc_element_2->setEnabled(false);
    // ui->button_cmc_element_3->setEnabled(false);
    // ui->button_cmc_element_4->setEnabled(false);


    if(value == "UE Switch"){
        ui->button_mc_status->setEnabled(true);
        ui->button_mc_info->setEnabled(true);
        ui->button_mc_debug_mode->setEnabled(true);
        ui->button_mc_select_ue->setEnabled(true);
    }else if(value == "Element Switch"){
        ui->button_mc_status->setEnabled(true);
        ui->button_mc_info->setEnabled(true);
        ui->button_mc_debug_mode->setEnabled(true);
        ui->button_mc_auto_mode->setEnabled(true);
        ui->groupBox_cmc_elements->setEnabled(true);
        // ui->button_cmc_element_1->setEnabled(true);
        // ui->button_cmc_element_2->setEnabled(true);
        // ui->button_cmc_element_3->setEnabled(true);
        // ui->button_cmc_element_4->setEnabled(true);
    }
}


void mainGUI::on_listWidget_available_devices_itemSelectionChanged()
{
    QListWidgetItem *selectedItem = ui->listWidget_available_devices->currentItem();

    if(selectedItem != nullptr){
        tcom.requestToConnect((selectedItem->text()).toStdString());
    }
}


void mainGUI::on_button_mc_status_released()
{
    tcom.requestStatus();
}


void mainGUI::on_button_mc_info_released()
{
    tcom.requestInfo();
}


void mainGUI::on_button_mc_debug_mode_released()
{
    tcom.requestDebugToggle();
}


void mainGUI::on_button_mc_auto_mode_released()
{
    tcom.requestAutoToggle();
}


void mainGUI::on_button_mc_select_ue_released()
{
    tcom.requestUESelect(ui->spinBox_mc_ue_select->value());
}

