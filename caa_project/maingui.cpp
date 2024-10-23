#include "maingui.h"
#include "./ui_maingui.h"
#include "custom/RadioSysConfig.h"
#include "custom/matHF.h"
#include <iostream>
#include <QTimer>

#include "custom/config_file.h"

mainGUI::mainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainGUI)
{
    ui->setupUi(this);

    int gui_conf_response = readGUIConfigFile();
    if(gui_conf_response != 0){
        std::cout << "An error has occured reading the gui.cfg file" << std::endl;
    }

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

    txCarrierSlider = new SliderAndLineEdit("Tx Carrier Frequency",6e9,1.2e9,"GHz",1e9,1e3,true,this);
    ui->verticalLayout_12->addWidget(txCarrierSlider);
    connect(txCarrierSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxCarrierFrequency);

    rxCarrierSlider = new SliderAndLineEdit("Rx Carrier Frequency",6e9,1.2e9,"GHz",1e9,1e3,true,this);
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
    ui->indicator_synchronization->setState(1);

    ui->indicator_synchronization->setState(1);

    ui->lafw_signal_config->setLabelText("");
    ui->lafw_signal_config->setDataSource((&GUIConf.sigConfSignalFilepath));
    ui->lafw_signal_config->setEditable(true);

    ui->lafw_synch_capture->setLabelText("Capture filepath:");   
    ui->lafw_synch_capture->setDataSource(&(GUIConf.sigConfCaptureSignalFilepath));
    ui->lafw_synch_capture->setEditable(true);

    ui->indicator_format->setState(1);

    ui->lasbw_antenna_elements->requestSetValue(1,true);
    ui->lasbw_antenna_elements->setMinimum(1);
    ui->lasbw_antenna_elements->setLabelText("Active Element:");

    ui->lasbw_active_class->requestSetValue(1,true);
    ui->lasbw_active_class->setMinimum(1);
    ui->lasbw_active_class->setLabelText("Active class (UE):");

    ui->lasbw_active_sample->requestSetValue(1,true);
    ui->lasbw_active_sample->setMinimum(1);
    ui->lasbw_active_sample->setMaximum(3000);
    ui->lasbw_active_sample->setLabelText("Current sample:");

    //ui->lasbw_num_elements->requestSetValue(1,true);
    ui->lasbw_num_elements->setDataSource(&(GUIConf.sigConfNumberOfElements),true);
    ui->lasbw_num_elements->setMinimum(1);
    ui->lasbw_num_elements->setLabelText("Number of Elements:");

    //ui->lasbw_num_classes->requestSetValue(1,true);
    //ui->lasbw_num_classes->requestSetValue(4,true);
    ui->lasbw_num_classes->setDataSource(&(GUIConf.sigConfNumberOfClasses),true);
    ui->lasbw_num_classes->setMinimum(1);
    ui->lasbw_num_classes->setLabelText("Number of Classes:");

    ui->lasbw_synchCaptureOffset->setLabelText("Capture Offset:");
    ui->lasbw_synchCaptureOffset->setMaximum(500000);
    ui->lasbw_synchCaptureOffset->setDataSource(&(GUIConf.sigConfCaptureOffset),true);
    //ui->lasbw_synchCaptureOffset->requestSetValue(256);




    ui->lasbw_synchCaptureLength->setLabelText("Capture Length:");
    ui->lasbw_synchCaptureLength->setMaximum(500000);
    ui->lasbw_synchCaptureLength->setDataSource(&(GUIConf.sigConfCaptureLength),true);


    ui->lasbw_num_repetitions->setLabelText("Repetitions:");
    ui->lasbw_num_repetitions->setMaximum(100);
    ui->lasbw_num_repetitions->setMinimum(1);
    ui->lasbw_num_repetitions->setDataSource(&(GUIConf.sigConfNumberOfRepetitions),true);

    ui->lasbw_repetition->setLabelText("Repetition:");
    ui->lasbw_repetition->setMaximum(ui->lasbw_num_repetitions->getValue());
    ui->lasbw_repetition->setMinimum(1);

    ui->lacb_use_wndw_sync->setLabelText("Use window synch.");
    ui->lacb_use_wndw_sync->setDataSource(&(GUIConf.sigConfUseWindowSynchronization),true);

    ui->lacb_single_class->setLabelText("Single class");
    ui->lacb_single_class->setDataSource(&(GUIConf.sysConfSingleClass),true);

    ui->lacb_auto_switch->setLabelText("Auto Switch");
    ui->lacb_auto_switch->setDataSource(&(GUIConf.sysConfAutoSwitch),true);

    ui->lacb_save_capture->setLabelText("Save Capture");
    ui->lacb_save_capture->setDataSource(&(GUIConf.sysConfSaveCapture),true);

    ui->lacb_auto_save->setLabelText("Auto Save");
    ui->lacb_auto_save->setDataSource(&(GUIConf.sysConfAutoSave),true);





    connect(ui->lasbw_antenna_elements,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onActiveElementSpinBoxChanged);

    connect(ui->lasbw_active_class,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onActiveClassSpinBoxChanged);

    connect(ui->lasbw_active_sample,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onActiveSampleSpinBoxChanged);

    connect(ui->lasbw_num_elements,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onActiveSampleSpinBoxChanged);

    connect(ui->lasbw_num_classes,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onNumClassesSpinBoxChanged);

    connect(ui->lasbw_synchCaptureOffset,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onSynchCaptureOffsetSpinBoxChanged);

    connect(ui->lasbw_synchCaptureLength,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onSynchCaptureLengthSpinBoxChanged);


    ui->indicator_auto_capture->setState(1);


    hideLayout(ui->layout_frs);


    processingTimer.setInterval(600);
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

void mainGUI::updateSynchronizationStatus(bool status){
    if(status){
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(true);
        ui->button_capture_synch->setEnabled(true);
        addStatusUpdate("Synchronization Initialized",ui->tableWidget_status);
        synchronizationStartTime = QDateTime::currentDateTime();
        connect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackSynchronizationProcess);
    }else{
        if(radObj->isReceiving()){
            ui->pushButton_2->setEnabled(true);
        }
        ui->pushButton_3->setEnabled(false);
        ui->button_capture_synch->setEnabled(false);
        addStatusUpdate("Synchronization terminated",ui->tableWidget_status);
        disconnect(&processingTimer,&QTimer::timeout, this,&mainGUI::trackSynchronizationProcess);
    }
}

void mainGUI::updateReceiveStatus(bool status)
{
    if(status){
        ui->pushButton_2->setEnabled(true); // enable synch start
        addStatusUpdate("Reception Initialized",ui->tableWidget_status);
        SetWidgetColor(ui->indicator_rx_in_progress,9433252);
        SetWidgetColor(ui->indicator_captured_buffer,16146769); // last capture does not correspond to current
        receptionStartTime = QDateTime::currentDateTime();
        ui->button_write_buffer_to_file->setEnabled(true);
        connect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackReceptionProcess);

    }else{
        if(radObj->isSynchronizing()){
            radObj->stopSynchronization();
            ui->indicator_synchronization->setState(1);
        }
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->button_capture_synch->setEnabled(false);
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

void mainGUI::addStatusUpdate(QString entry, QTableWidget *table,int type)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

    table->insertRow(0);

    QTableWidgetItem *item1 = new QTableWidgetItem(currentDateTimeString);
    QTableWidgetItem *item2 = new QTableWidgetItem(entry);

    if(type == -1)
        item2->setForeground(QBrush(QColor(Qt::red)));
    else if(type == 1){
        item2->setForeground(QBrush(QColor(Qt::green)));
    }

    // Add items to the table widget
    table->setItem(0, 0, item1); // Row 0, Column 0
    table->setItem(0, 1, item2); // Row 0, Column 1

    //table->selectRow(0);

}

void mainGUI::plotPhaseComparison(bool timePlot,bool relative)
{

    QVector<QPen> line_colors;
    line_colors.push_back(QPen(Qt::blue));
    line_colors.push_back(QPen(Qt::red));
    line_colors.push_back(QPen(Qt::green));
    line_colors.push_back(QPen(Qt::magenta));

    // get the captures
    using cmtrx = std::vector<std::vector<std::complex<double>>>;
    using mtrx = std::vector<std::vector<double>>;

    cmtrx capturedFrames;
    if(radObj->getComplexCaptureFrames(capturedFrames)){
        size_t cols = capturedFrames[0].size();
        size_t rows = capturedFrames.size();

        mtrx phase_angle(rows,std::vector<double>(cols));

        //std::cout << radObj->sysConf.

        std::vector<std::complex<double>> ref_s = capturedFrames[0];
        std::vector<double> sample_ix(cols);
        int Lc = ref_s.size();

        if(!timePlot){
            ref_s = uhd_clib::fft_w_zpadd(ref_s,cols);
            Lc = radObj->sysConf.numActiveSubcarriers[2];
        }

        std::vector<int> index_vec(Lc);

        for(int ic=0;ic<Lc;ic++){
            index_vec[ic] = ic;
            if(!timePlot){
                index_vec[ic] = radObj->sysConf.iGrid[2][ic];
            }
        }

        for(size_t r=0;r<rows;r++){

            std::vector<std::complex<double>>t_cvec = capturedFrames[r];
            if(!timePlot){
                t_cvec = uhd_clib::fft_w_zpadd(t_cvec,cols);
            }
            QVector<double> sample_ix;
            QVector<double> phase_sample;

            for(size_t c=0;c<Lc;c++){
                if(relative){
                    phase_angle[r][c] = uhd_clib::angle(ref_s[index_vec[c]] / t_cvec[index_vec[c]]);
                }else{
                    phase_angle[r][c] = uhd_clib::angle(t_cvec[index_vec[c]]);
                }
                sample_ix.push_back(c);
                phase_sample.push_back(phase_angle[r][c]);
            }

            ui->phase_plot->addGraph();
            ui->phase_plot->graph(r)->setData(sample_ix, phase_sample);
            ui->phase_plot->graph(r)->setPen(line_colors[r]);
            ui->phase_plot->xAxis->setLabel("Sample");
            ui->phase_plot->yAxis->setLabel("Phase");
            ui->phase_plot->xAxis->setRange(0, sample_ix.size());
            ui->phase_plot->yAxis->setRange(-3.14, 3.14);
            ui->phase_plot->replot();


        }
    }
}

void mainGUI::hideLayout(QLayout *layout)
{
    for(int i=0;i<layout->count(); i++){
        QLayoutItem* item = layout->itemAt(i);
        if(item->layout()){
            hideLayout(item->layout());
        }else if(item->widget()){
            item->widget()->hide();
        }
    }
}

void mainGUI::unhideLayout(QLayout *layout)
{
    for(int i=0;i<layout->count(); i++){
        QLayoutItem* item = layout->itemAt(i);
        if(item->layout()){
            unhideLayout(item->layout());
        }else if(item->widget()){
            item->widget()->show();
        }
    }
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

void mainGUI::trackSynchronizationProcess()
{
    if(radObj->isSynchronizing()){
        ui->pushButton_3->setEnabled(true);
        if(radObj->getSynchPointCount() > 0){
            ui->indicator_synchronization->setState(2);
            ui->button_capture_synch->setEnabled(true);
        }else{
           ui->indicator_synchronization->setState(3);
           ui->button_capture_synch->setEnabled(false);
        }

    }else{
        ui->indicator_synchronization->setState(1);
    }
}

void mainGUI::trackCaptureBufferProcess()
{
    if(not radObj->isWritingBufferToFile()){
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackCaptureBufferProcess);
        addStatusUpdate("Success writing buffer to file",ui->tableWidget_status,1);
        ui->button_receive->setEnabled(true);
        SetWidgetColor(ui->indicator_captured_buffer,9433252);

        std::vector<std::complex<short>> vc_data = radObj->getCapturedData();

        plot_time_and_freq(vc_data);

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

        addStatusUpdate("Success loading configuration",ui->tableWidget_status,1);

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
        addStatusUpdate("Error loading configuration",ui->tableWidget_status,-1);
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
                addStatusUpdate("Success configurating USRPs",ui->tableWidget_status,1);
                pendingConfigurationRequest = false;
            }
        }else{
            SetWidgetColor(ui->indicator_configuration_set,16146769); // Red indicator
            ui->button_receive->setEnabled(false);
            ui->button_transmit->setEnabled(false);
            if(pendingConfigurationRequest){
                addStatusUpdate("Failed configurating USRPs",ui->tableWidget_status,-1);
                pendingConfigurationRequest = false;
            }
        }

        ui->button_apply_config->setEnabled(true);
    }else{
       SetWidgetColor(ui->indicator_configuration_set,16380011); // Yellow indicator
        ui->button_apply_config->setEnabled(false);
    }
}

void mainGUI::processing_automatic_capture()
{
    // > Validate conditions
    // Frame structure is set up
    // Transmission is running
    // Reception is running
    // Synchronization is running


    int validation_response = validateAutomaticCapture();
    if(validation_response == 1){
        if(!radObj->hasPendingSynchPointReset()){

            int currentElement = ui->lasbw_antenna_elements->getValue();
            int currentClass = ui->lasbw_active_class->getValue();
            int currentSample = ui->lasbw_active_sample->getValue();

            int currentRepetition = ui->lasbw_repetition->getValue();
            bool useRepetition = (ui->lasbw_num_repetitions->getValue() > 1);

            bool increment_class = false;
            bool increment_repetition = false;
            bool increment_sample = false; // Equivalently true if auto capture is finsihed
            bool increment_element = false;
            bool plot_capture = false;

            int increment = 1;

            if(radObj->requestCaptureSynchFrame(currentElement-1)){

                if(radObj->isCapturedFramesReadyToSave()){
                    // Capture has been made for all elements
                    SaveSynchCaptures();
                    plotPhaseComparison(ui->cb_time_plot->isChecked(),ui->cb_relative_plot->isChecked());
                    radObj->resetCurrentFramesCaptured();

                    // Indicates if all (maybe single) class is complete
                    bool finished_classes = false;

                    // Change premise
                    increment_sample = true;
                    increment_class = true;
                    increment_repetition = true;

                    // Check if single class
                    if(ui->lacb_single_class->getValue()){
                        //finished = true;
                        finished_classes = true;
                        increment_class = false;
                    }else if(currentClass == ui->lasbw_active_class->getMaximum()){
                        // is last class
                        finished_classes = true;
                    }else{
                        // not last class
                        increment_sample = false; // not finished
                    }

                    if(useRepetition && finished_classes){
                        if(currentRepetition != ui->lasbw_repetition->getMaximum()){
                            // not last repetition
                            increment_sample = false; // not finished
                        }
                    }else{
                        increment_repetition = false; // don't increment repetition
                    }

                    plot_capture = true;
                }

                increment_element = true;

            }

            if(increment_repetition){
                currentRepetition = ui->lasbw_repetition->requestSetValue(currentRepetition+increment,true); //always increment repetition
            }

            if(increment_sample){
                currentSample = ui->lasbw_active_sample->requestSetValue(currentSample+increment,true); // increment sample
                automaticCaptureRunning = false;
                ui->indicator_auto_capture->setState(2);
                disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_automatic_capture);
                addStatusUpdate("Automatic Capture finished...",ui->tableWidget_status,1);
            }

            if(increment_class){
                currentClass = ui->lasbw_active_class->requestSetValue(currentClass+increment,true); //increment class
            }

            if(increment_element){
                currentElement = ui->lasbw_antenna_elements->requestSetValue(currentElement+increment,true); //increment element
                radObj->requestResetSynchPoints();
            }

            if(plot_capture){
                std::vector<std::complex<short>> vc_data = radObj->getExtractedSynchData();
                plot_time_and_freq(vc_data);
            }
        }

       // std::this_thread::sleep_for(std::chrono::seconds(3));

    }else{
        ui->indicator_auto_capture->setState(1);
        addStatusUpdate("Error running automatic capture",ui->tableWidget_status,-1);
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_automatic_capture);
        automaticCaptureRunning = false;
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
    std::string filepath = ui->lafw_signal_config->getFieldText();
    int response = radObj->readConfigSignalFile(filepath);
    if(response == 0){
        SetWidgetColor(ui->indicator_sig_config,9433252);
        addStatusUpdate("Signal file succesfully loaded",ui->tableWidget_status,1);
        unhideLayout(ui->layout_frs);

        ui->lafw_frs_num_symbols->setLabelText("Number of symbols: ");
        ui->lafw_frs_num_symbols->setFieldText(radObj->sysConf.GetNumberOfSymbols());

        ui->lafw_frs_symbol_id->setLabelText("Frame symbols:");
        ui->lafw_frs_symbol_id->setFieldText(radObj->sysConf.GetSignalPattern());

        ui->lafw_frs_synch_length->setLabelText("Synch. sequence length: ");
        ui->lafw_frs_synch_length->setFieldText(static_cast<int>(radObj->sysConf.synchSignal.size()));

        ui->lafw_frs_fs->setLabelText("Spec. sampling rate (MHz):");
        ui->lafw_frs_fs->setFieldText(radObj->sysConf.GetSamplingRate()/1e6,4);

        ui->lafw_frs_data_modulation->setLabelText("Data Modulation:");
        ui->lafw_frs_data_modulation->setFieldText(radObj->sysConf.GetDataModulationType());

        ui->lafw_frs_date->setLabelText("File Modified Date:");
        ui->lafw_frs_date->setFieldText(radObj->sysConf.GetDate());

        ui->lafw_frs_signal_type->setLabelText("Signal type:");
        ui->lafw_frs_signal_type->setFieldText(radObj->sysConf.GetSignalType());

        ui->lafw_frs_signal_length->setLabelText("Signal length:");
        ui->lafw_frs_signal_length->setFieldText(radObj->sysConf.txSignal.size());

    }else if(response == -2){
        QString error_message = QString::fromStdString("Error, file " + filepath + " does not exist");
        addStatusUpdate(error_message,ui->tableWidget_status,-1);
    }else{
        SetWidgetColor(ui->indicator_sig_config,16380011);
        hideLayout(ui->layout_frs);
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
        addStatusUpdate("Success saving configuration",ui->tableWidget_status,1);
    }else{
        addStatusUpdate("Error saving configuration",ui->tableWidget_status,-1);
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

    //addStatusUpdate("Synchronization started",ui->tableWidget_status);

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


void mainGUI::on_pushButton_2_released()
{
    radObj->requestResetSynchPoints();
    if(radObj->startSynchronization()){
        updateSynchronizationStatus(true);
        //addStatusUpdate("Synchronization started",ui->tableWidget_status);
    }
}


void mainGUI::on_pushButton_3_released()
{
    if(radObj->isSynchronizing()){
        if(radObj->stopSynchronization()){
            updateSynchronizationStatus(false);
            ui->indicator_synchronization->setState(1);
            //addStatusUpdate("Synchronization stopped",ui->tableWidget_status);
            //addStatusUpdate("Synchronization Points:" + QString::number(radObj->getSynchPointCount()),ui->tableWidget_status);

        }
        else{
            addStatusUpdate("Synchronization could not be stopped",ui->tableWidget_status);
        }
    }else{
        addStatusUpdate("Synchronization was not running...",ui->tableWidget_status);
    }
}


void mainGUI::on_button_capture_synch_released()
{
    int currentElement = ui->lasbw_antenna_elements->getValue();
    int currentClass = ui->lasbw_active_class->getValue();
    int currentSample = ui->lasbw_active_sample->getValue();

    if(radObj->requestCaptureSynchFrame(currentElement-1)){

        ui->button_save_synch_capture->setEnabled(radObj->isCapturedFramesReadyToSave());

        int increment = 0;
        if(ui->lacb_auto_switch->getValue()){
            increment = 1;
        }

        if(radObj->isCapturedFramesReadyToSave()){

            // Since it can be saved, it must be the last antenna element
            if(ui->lacb_auto_save->getValue()){
                SaveSynchCaptures();
            }

            if(currentClass == ui->lasbw_active_class->getMaximum()){
                // It is the last class
                // -> Reset the class and element
                // -> Increment Sample

                currentClass = ui->lasbw_active_class->getMaximum();
                currentElement = ui->lasbw_antenna_elements->getMaximum();

                currentSample = ui->lasbw_active_sample->requestSetValue(currentSample+increment,true); // increment sample
            }

            currentClass = ui->lasbw_active_class->requestSetValue(currentClass+increment,true); //increment class
        }

        currentElement = ui->lasbw_antenna_elements->requestSetValue(currentElement+increment,true); //increment element

        radObj->requestResetSynchPoints();

        std::vector<std::complex<short>> vc_data = radObj->getExtractedSynchData();

        plot_time_and_freq(vc_data);

        if(ui->lacb_save_capture->getValue()){
            if(radObj->requestWriteLastCapturedFrame()){
               addStatusUpdate("Successfully saved capture...",ui->tableWidget_status,1);

            }else{
               addStatusUpdate("Error, could not save capture...",ui->tableWidget_status,-1);
            }
        }

        radObj->requestResetSynchPoints();

    }else{
        addStatusUpdate("Synchronization must be running...",ui->tableWidget_status);
    }
}


void mainGUI::on_button_set_synch_format_released()
{
    int response = radObj->requestFrameCaptureFormat(ui->lasbw_synchCaptureOffset->getValue(),
                                                     ui->lasbw_synchCaptureLength->getValue(),
                                                     ui->lasbw_num_elements->getValue(),
                                                     ui->lafw_synch_capture->getFieldText(),
                                                     ui->lacb_use_wndw_sync->getValue());
    if(response == 1){
        addStatusUpdate("Frame format updated...",ui->tableWidget_status);

        ui->lasbw_antenna_elements->setMaximum(ui->lasbw_num_elements->getValue());
        ui->lasbw_active_class->setMaximum(ui->lasbw_num_classes->getValue());

        ui->lasbw_repetition->setMaximum(ui->lasbw_num_repetitions->getValue());
        ui->lasbw_repetition->requestSetValue(1);

        frameFormatStatus = 1;
        ui->indicator_format->setState(2);
    }else if(response == -1){
        addStatusUpdate("Coult not update frame format: synchronization running...",ui->tableWidget_status);
        frameFormatStatus = 0;
        ui->indicator_format->setState(1);
    }else{
        addStatusUpdate("Unknown error updating frame format...",ui->tableWidget_status);
        frameFormatStatus = 0;
        ui->indicator_format->setState(1);
    }
}


void mainGUI::on_button_save_synch_capture_released()
{
    SaveSynchCaptures();
}


void mainGUI::plot_time_and_freq(std::vector<std::complex<short>> vc_data)
{
    QVector<double> x(vc_data.size()), y(vc_data.size()); // initialize with entries 0..100
    double max_element = 0;

    QVector<double> freq(vc_data.size()),spectrum(vc_data.size());
    std::vector<std::complex<double>> vc_data_dbl = uhd_clib::cvec_conv_short2double(vc_data);
    arma::cx_vec arma_sig = arma::conv_to<arma::cx_vec>::from(vc_data_dbl);
    arma::vec spectrum_c  = arma::pow(arma::abs(arma::fft(arma_sig)),2);

    int N = spectrum_c.n_elem;
    int halfN = (N + 1) / 2;
    arma::vec shifted_spectrum(N);

    // Split and swap the vector
    shifted_spectrum.head(N - halfN) = spectrum_c.tail(N - halfN);
    shifted_spectrum.tail(halfN) = spectrum_c.head(halfN);

    std::vector<double> freq_spectrum  = arma::conv_to<std::vector<double>>::from(shifted_spectrum);

    for (int i=0; i<vc_data.size(); ++i)
    {
        x[i] = i;
        y[i] = 10 * std::log10(std::norm(vc_data[i]));

        freq[i] = ((double)i/(double)N -0.5)*(radObj->sysConf.getRxSamplingRate()) / 1e3;
        spectrum[i] = 10 * std::log10(shifted_spectrum(i));

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

    auto max_spec = std::max_element(spectrum.begin(), spectrum.end());
    int max_spec_value = *max_spec;

    auto min_spec = std::min_element(spectrum.begin(), spectrum.end());
    int min_spec_value = *min_spec;

    ui->capture_spectrum_plot->addGraph();
    ui->capture_spectrum_plot->graph(0)->setData(freq,spectrum);
    ui->capture_spectrum_plot->xAxis->setLabel("Frequency (kHz)");
    ui->capture_spectrum_plot->yAxis->setLabel("Power (dB)");
    ui->capture_spectrum_plot->xAxis->setRange(freq[0],freq[N-1]);
    ui->capture_spectrum_plot->yAxis->setRange(min_spec_value,max_spec_value);

    ui->capture_spectrum_plot->replot();
}

void mainGUI::SaveSynchCaptures()
{
    int c_sample = ui->lasbw_active_sample->getValue();
    int c_class = ui->lasbw_active_class->getValue();


    //"data";//captureFileFormatField->getFieldText();
    std::string fileDirectory = ui->lafw_synch_capture->getFieldText();
    if(fileDirectory != ""){
        fileDirectory = fileDirectory + "/";
    }

    if(ui->lasbw_num_repetitions->getValue()>1){
        fileDirectory = fileDirectory + "/rep" + std::to_string(ui->lasbw_repetition->getValue()) + "/";

        // check if directory exist, otherwise create it
        if(!std::filesystem::exists(fileDirectory)){
            std::filesystem::create_directories(fileDirectory);
        }
    }

    std::string filepath = fileDirectory + "class"  + std::to_string(c_class) +
                           "_sample" + std::to_string(c_sample) + ".csv";

    SaveSyncCaptures(filepath,"Saved " + filepath);
}

void mainGUI::SaveSyncCaptures(std::string filepath, std::string message)
{
    int response = radObj->requestWriteFramesToFile(filepath,"csv");
    if(response == 1){
        addStatusUpdate(QString::fromStdString(message),ui->tableWidget_status,0);
    }else{
        addStatusUpdate("Errors saving " + QString::fromStdString(filepath),ui->tableWidget_status,-1);
    }
}

void mainGUI::SetActiveElementForAllUEs(int value)
{
    std::string currentType;
    for(int i=0;i<mcControlWidgets.size();i++){
        currentType = mcControlWidgets[i]->getMCType();
        if(currentType == "Element Switch"){
            mcControlWidgets[i]->requestSelectElement(value);
        }
    }
}

void mainGUI::SetActiveUEForAll(int value)
{
    std::string currentType;
    for(int i=0;i<mcControlWidgets.size();i++){
        currentType = mcControlWidgets[i]->getMCType();
        if(currentType == "UE Switch"){
            mcControlWidgets[i]->requestSelectUE(value);
        }
    }
}


void mainGUI::on_button_reset_tx_released()
{
    addStatusUpdate("Transmitter reset",ui->tableWidget_status,0);
    radObj->requestResetTransmitter();
}


void mainGUI::on_button_auto_capture_released()
{
    // run auto capture
    if(!automaticCaptureRunning){
        // > Validate that process should run
        // Format is set up
        // Auto switch is on
        automaticCaptureRunning = true;
        ui->indicator_auto_capture->setState(3);
        connect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_automatic_capture);
    }else{
        automaticCaptureRunning = false;
        addStatusUpdate("Automatic Capture terminated...",ui->tableWidget_status,-1);
        ui->indicator_auto_capture->setState(1);
        disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_automatic_capture);
    }
}

void mainGUI::onActiveElementSpinBoxChanged(int value,bool noSynchReset)
{
    // Update antenna elements
    SetActiveElementForAllUEs(value);
    if(~noSynchReset){
        radObj->requestResetSynchPoints();
    }
}

void mainGUI::onActiveClassSpinBoxChanged(int value,bool noSynchReset)
{
    // Update UE
    SetActiveUEForAll(value);
    radObj->resetCurrentFramesCaptured();
    if(~noSynchReset){
        radObj->requestResetSynchPoints();
    }
}

void mainGUI::onActiveSampleSpinBoxChanged(int value,bool noSynchReset)
{
    std::cout << "Active Sample is now: " << value << std::endl;
    radObj->resetCurrentFramesCaptured();
    if(~noSynchReset){
        radObj->requestResetSynchPoints();
    }
}

void mainGUI::onNumElementSpinBoxChanged(int value)
{
    triggerFormatChanged(true);
}

void mainGUI::onNumClassesSpinBoxChanged(int value)
{
    triggerFormatChanged(true);
}

void mainGUI::onSynchCaptureOffsetSpinBoxChanged(int value)
{
    triggerFormatChanged(true);
}

void mainGUI::onSynchCaptureLengthSpinBoxChanged(int value)
{
    triggerFormatChanged(true);
}

void mainGUI::triggerFormatChanged(bool value)
{
    if(value){
        if(frameFormatStatus == 1){
            frameFormatStatus = 2;
            ui->indicator_format->setState(3);
        }
    }
}

int mainGUI::validateAutomaticCapture()
{
    // 1    = success
    // 0    = multiple errors
    // -1   = transmission not running
    // -2   = reception not running
    // -3   = synchronization not running
    // -4   = synchronization timeout (?) (not implemented)
    // -5   = frame structure not loaded (not implemented)

    int response = 1;
    int error_count = 0;


    if(!radObj->isTransmitting()){
        ++error_count;
        response = -1;
    }

    if(!radObj->isReceiving()){
        ++error_count;
        response = -2;
    }

    if(!radObj->isSynchronizing()){
        ++error_count;
        response = -3;
    }

    if(error_count > 1){
        response = 0;
    }

    return response;
}

int mainGUI::readGUIConfigFile()
{
    int response = 0; // success;

    try{
        std::ifstream iconf_file("gui.cfg");
        CFG::ReadFile(iconf_file,GUIConf.confEntriesVector,
                        GUIConf.sdrConfConfigFilepath,
                        GUIConf.sigConfSignalFilepath,
                        GUIConf.sigConfNumberOfClasses,
                        GUIConf.sigConfNumberOfElements,
                        GUIConf.sigConfNumberOfRepetitions,
                        GUIConf.sigConfCaptureOffset,
                        GUIConf.sigConfCaptureLength,
                        GUIConf.sigConfUseWindowSynchronization,
                        GUIConf.sigConfCaptureSignalFilepath,
                        GUIConf.sysConfAutoSwitch,
                        GUIConf.sysConfAutoSave,
                        GUIConf.sysConfSaveCapture,
                        GUIConf.sysConfSingleClass);
    }catch(...){
        response = -1;
    }


    return response; //
}

int mainGUI::writeGUIConfigFile()
{
    std::ofstream f_out("gui.cfg");
    bool save_capture = false;
    const std::string str_true = "true";
    const std::string str_false = "false";
    CFG::WriteFile(f_out,GUIConf.confEntriesVector,
                  GUIConf.sdrConfConfigFilepath,
                  GUIConf.sigConfSignalFilepath,
                  GUIConf.sigConfNumberOfClasses,
                  GUIConf.sigConfNumberOfElements,
                  GUIConf.sigConfNumberOfRepetitions,
                  GUIConf.sigConfCaptureOffset,
                  GUIConf.sigConfCaptureLength,
                  GUIConf.sigConfUseWindowSynchronization ? str_true : str_false,
                  GUIConf.sigConfCaptureSignalFilepath,
                  GUIConf.sysConfAutoSwitch ? str_true : str_false,
                  GUIConf.sysConfAutoSave ? str_true : str_false,
                  GUIConf.sysConfSaveCapture ? str_true : str_false,
                  GUIConf.sysConfSingleClass ? str_true : str_false);
    f_out.close();

    return 0;
}


void mainGUI::on_button_save_default_format_released()
{
    int response = writeGUIConfigFile();
    if(response == 0){
       // std::cout << "Success writing file!" << std::endl;
        addStatusUpdate("Saved default settings",ui->tableWidget_status,1);
    }
}

