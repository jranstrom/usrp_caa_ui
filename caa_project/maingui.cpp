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

    rxIPAddressField = new LabelandFieldWidget(this,"RX USRP IP:","192.168.10.3",true);
    txIPAddressField = new LabelandFieldWidget(this,"TX USRP IP:","192.168.10.4",true);

    ui->verticalLayout_12->addWidget(rxIPAddressField);
    ui->verticalLayout_12->addWidget(txIPAddressField);

    connect(rxIPAddressField,&LabelandFieldWidget::fieldTextEditing,this,&mainGUI::userChangedRxIPAddress);
    connect(txIPAddressField,&LabelandFieldWidget::fieldTextEditing,this,&mainGUI::userChangedTxIPAddress);


    txCarrierSlider = new SliderAndLineEdit("Tx Carrier Frequency",6e9,1.2e9,"GHz",1e9,1e6,true,this);
    ui->verticalLayout_12->addWidget(txCarrierSlider);
    connect(txCarrierSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxCarrierFrequency);

    rxCarrierSlider = new SliderAndLineEdit("Rx Carrier Frequency",6e9,1.2e9,"GHz",1e9,1e6,true,this);
    ui->verticalLayout_10->addWidget(rxCarrierSlider);
    connect(rxCarrierSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedRxCarrierFrequency);

    txSamplingRateSlider = new SliderAndLineEdit("Tx Sampling Rate",20e6,10e3,"MHz",1e6,1e3,true,this);
    ui->verticalLayout_10->addWidget(txSamplingRateSlider);
    connect(txSamplingRateSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxSamplingRate);

    rxSamplingRateSlider = new SliderAndLineEdit("Rx Sampling Rate",20e6,10e3,"MHz",1e6,1e3,true,this);
    ui->verticalLayout_10->addWidget(rxSamplingRateSlider);
    connect(rxSamplingRateSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedRxSamplingRate);

    rxLOOffsetSlider = new SliderAndLineEdit("LO Offset",25e6,-25e6,"MHz",1e6,5e5,true,this);
    ui->verticalLayout_12->addWidget(rxLOOffsetSlider);
    connect(rxLOOffsetSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedLOOffset);

    txGainSlider = new SliderAndLineEdit("Tx Gain",15,-15,"dB",1,1,true,this);
    ui->verticalLayout_12->addWidget(txGainSlider);
    connect(txGainSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxGain);

    rxGainSlider = new SliderAndLineEdit("Rx Gain",15,-15,"dB",1,1,true,this);
    ui->verticalLayout_12->addWidget(rxGainSlider);
    connect(rxGainSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedRxGain);

    connect(&uio,&uiobj::txPPSSourceChanged,this,&mainGUI::updateTxPPSSource);
    connect(&uio,&uiobj::rxPPSSourceChanged,this,&mainGUI::updateRxPPSSource);

    connect(&uio,&uiobj::txREFSourceChanged,this,&mainGUI::updateTxREFSource);
    connect(&uio,&uiobj::rxREFSourceChanged,this,&mainGUI::updateRxREFSource);

    connect(&uio,&uiobj::USRPConfigurationChanged,this,&mainGUI::onUSRPConfigurationChanged);
    //connect(&uio,&uiobj::txSetupStatusChanged,this,&mainGUI::updateUSRPSetupChanged);

    connect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_USRP_setup);

    connect(&tcom,&Tcom_ui::availableDevicesChanged,this,&mainGUI::updateAvailableDevices);
    connect(&tcom,&Tcom_ui::connectionChanged,this,&mainGUI::updateConnection);

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

        rxCarrierSlider->Request2SetComponentValue(radObj->sysConf.getRxCarrierFrequency());
        txCarrierSlider->Request2SetComponentValue(radObj->sysConf.getTxCarrierFrequency());

        rxSamplingRateSlider->Request2SetComponentValue(radObj->sysConf.getRxSamplingRate());
        txSamplingRateSlider->Request2SetComponentValue(radObj->sysConf.getTxSamplingRate());

        rxGainSlider->Request2SetComponentValue(radObj->sysConf.getRxGain());
        txGainSlider->Request2SetComponentValue(radObj->sysConf.getTxGain());

        rxLOOffsetSlider->Request2SetComponentValue(radObj->sysConf.getRxLO_offset());

        rxIPAddressField->setFieldText(radObj->sysConf.getRxIPAddress());
        txIPAddressField->setFieldText(radObj->sysConf.getTxIPAddress());

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

void mainGUI::applyTxConfig()
{
    //radObj->sysConf.tx.CarrierFrequency = (ui->lineEdit_tx_fc->text()).toDouble()*1e9;
    //radObj->sysConf.tx.Gain = (ui->lineEdit_tx_gain->text()).toDouble();
}

void mainGUI::applyRxConfig()
{
   // radObj->sysConf.rx.CarrierFrequency = (ui->lineEdit_rx_fc->text()).toDouble()*1e9;
    //radObj->sysConf.rx.Gain = (ui->lineEdit_rx_gain->text()).toDouble();
}

void mainGUI::removeAllMCControlWidgets()
{
    for (MCControlWidget* customWidget : mcControlWidgets) {
        disconnect(customWidget,&MCControlWidget::cycleButtonReleased,this,&mainGUI::updateMCSCycle);
        ui->gridLayout_mcs->removeWidget(customWidget);
        delete customWidget;
    }

    mcControlWidgets.clear();
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

void mainGUI::on_listWidget_available_devices_itemSelectionChanged()
{
    QListWidgetItem *selectedItem = ui->listWidget_available_devices->currentItem();

    if(selectedItem != nullptr){
        tcom.requestToConnect((selectedItem->text()).toStdString());
    }
}

void mainGUI::on_button_generate_mc_controls_released()
{
    removeAllMCControlWidgets();

    int  L = ui->listWidget_available_devices->count();
    const int rows  = 2;

    for(int i=0;i<L;i++){
        auto item = ui->listWidget_available_devices->item(i);
        std::string port = (item->text()).toStdString();

        std::string MCType = tcom.getMCType(port);

        if(MCType != ""){

            std::string MCMAC = tcom.getMCId(port);

            MCControlWidget *customWidget = new MCControlWidget(port,&tcom,currentMCControlIdentifier++,MCType,this);

            customWidget->setMAC(MCMAC);

            connect(customWidget,&MCControlWidget::cycleButtonReleased,this,&mainGUI::updateMCSCycle);
            int io = i+1;
            ui->gridLayout_mcs->addWidget(customWidget,io % rows,std::floor(io/rows));

            mcControlWidgets.push_back(customWidget);
            //ui->verticalLayout_30->addWidget(customWidget);
            customWidget->show();
        }
    }


}

void mainGUI::on_button_disconnect_released()
{
    if(mcControlWidgets.size() > 0){
        removeAllMCControlWidgets();
    }
}

void mainGUI::updateMCSCycle(int id)
{
    for(int i=0;i<mcControlWidgets.size();i++){
        if(mcControlWidgets[i]->getIdentifier() != id){
            mcControlWidgets[i]->requestRead();
        }
    }
}

void mainGUI::userChangedTxCarrierFrequency(double value)
{
    //uio.setTxCarrierFrequency(value);
    radObj->sysConf.setTxCarrierFrequency(value);
    uio.txUSRPConfigurationChanged(true);
}

void mainGUI::userChangedRxCarrierFrequency(double value)
{
    //uio.setRxCarrierFrequency(value);
    radObj->sysConf.setRxCarrierFrequency(value);
    uio.rxUSRPConfigurationChanged(true);

    //txUSRPConfigurationChanged(true);
}

void mainGUI::userChangedTxSamplingRate(double value)
{
    radObj->sysConf.setTxSamplingRate(value);
    uio.txUSRPConfigurationChanged(true);
}

void mainGUI::userChangedRxSamplingRate(double value)
{
    radObj->sysConf.setRxSamplingRate(value);
    uio.rxUSRPConfigurationChanged(true);
}

void mainGUI::userChangedTxGain(double value)
{
    radObj->sysConf.setTxGain(value);
    uio.txUSRPConfigurationChanged(true);
}

void mainGUI::userChangedRxGain(double value)
{
    radObj->sysConf.setRxGain(value);
    uio.rxUSRPConfigurationChanged(true);
}

void mainGUI::userChangedLOOffset(double value)
{
    radObj->sysConf.setRxLO_offset(value);
    uio.rxUSRPConfigurationChanged(true);
}

void mainGUI::userChangedRxIPAddress(std::string value)
{
    radObj->sysConf.setRxIPAddress(value);
    ui->verticalLayout_12->addWidget(rxIPAddressField);
}

void mainGUI::userChangedTxIPAddress(std::string value)
{
    radObj->sysConf.setTxIPAddress(value);
    ui->verticalLayout_12->addWidget(txIPAddressField);
}

