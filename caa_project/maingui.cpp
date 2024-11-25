#include "maingui.h"
#include "./ui_maingui.h"
#include "custom/RadioSysConfig.h"
#include "custom/matHF.h"
#include <iostream>
#include <QTimer>

#include "custom/config_file.h"
#include "radiocontrolwidget.h"
#include "custom/continousreceptionradiocontrol.h"
#include "custom/continoustransmissionradiocontrol.h"

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

    int rows = 4;

    ui->sdr_config_grid_layout->setContentsMargins(0,0,0,0);

    rxIPAddressField->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);

    addToGridLayout(rxIPAddressField,ui->sdr_config_grid_layout,rows);
    addToGridLayout(txIPAddressField,ui->sdr_config_grid_layout,rows);
    //ui->sdr_config_grid_layout->addWidget(rxIPAddressField,io % rows,std::floor(io/rows));
    //ui->sdr_config_grid_layout->addWidget(txIPAddressField,io % rows,std::floor(io/rows));

    connect(rxIPAddressField,&LabelandFieldWidget::fieldTextEditing,this,&mainGUI::userChangedRxIPAddress);
    connect(txIPAddressField,&LabelandFieldWidget::fieldTextEditing,this,&mainGUI::userChangedTxIPAddress);

    txCarrierSlider = new SliderAndLineEdit("Tx Carrier Frequency",6e9,1.2e9,"GHz",1e9,1e3,true,this);
    //ui->verticalLayout_12->addWidget(txCarrierSlider);
    addToGridLayout(txCarrierSlider,ui->sdr_config_grid_layout,rows);
    connect(txCarrierSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxCarrierFrequency);

    rxCarrierSlider = new SliderAndLineEdit("Rx Carrier Frequency",6e9,1.2e9,"GHz",1e9,1e3,true,this);
    //ui->verticalLayout_10->addWidget(rxCarrierSlider);
    addToGridLayout(rxCarrierSlider,ui->sdr_config_grid_layout,rows);
    connect(rxCarrierSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedRxCarrierFrequency);

    txSamplingRateSlider = new SliderAndLineEdit("Tx Sampling Rate",20e6,10e3,"MHz",1e6,1e3,true,this);
    //ui->verticalLayout_10->addWidget(txSamplingRateSlider);
    addToGridLayout(txSamplingRateSlider,ui->sdr_config_grid_layout,rows);
    connect(txSamplingRateSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxSamplingRate);

    rxSamplingRateSlider = new SliderAndLineEdit("Rx Sampling Rate",20e6,10e3,"MHz",1e6,1e3,true,this);
    //ui->verticalLayout_10->addWidget(rxSamplingRateSlider);
     addToGridLayout(rxSamplingRateSlider,ui->sdr_config_grid_layout,rows);
    connect(rxSamplingRateSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedRxSamplingRate);

    rxLOOffsetSlider = new SliderAndLineEdit("LO Offset",25e6,-25e6,"MHz",1e6,5e5,true,this);
    //ui->verticalLayout_12->addWidget(rxLOOffsetSlider);
    addToGridLayout(rxLOOffsetSlider,ui->sdr_config_grid_layout,rows);
    connect(rxLOOffsetSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedLOOffset);

    txGainSlider = new SliderAndLineEdit("Tx Gain",15,-15,"dB",1,1,true,this);
    addToGridLayout(txGainSlider,ui->sdr_config_grid_layout,rows);
    connect(txGainSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedTxGain);

    rxGainSlider = new SliderAndLineEdit("Rx Gain",15,-15,"dB",1,1,true,this);
    addToGridLayout(rxGainSlider,ui->sdr_config_grid_layout,rows);
    connect(rxGainSlider,&SliderAndLineEdit::componentValueChanged,this,&mainGUI::userChangedRxGain);

    ui->verticalLayout_10->removeWidget(ui->sdr_tx_source_container);
    addToGridLayout(ui->sdr_tx_source_container,ui->sdr_config_grid_layout,rows);

    ui->verticalLayout_10->removeWidget(ui->sdr_rx_source_container);
    addToGridLayout(ui->sdr_rx_source_container,ui->sdr_config_grid_layout,rows);

    ui->verticalLayout_10->removeWidget(ui->group_reset_ursp);
    addToGridLayout(ui->group_reset_ursp,ui->sdr_config_grid_layout,rows);

    ui->verticalLayout_10->removeWidget(ui->groupBox_sdr_config_button);
    addToGridLayout(ui->groupBox_sdr_config_button,ui->sdr_config_grid_layout,rows);

    addVerticalSpacersToGridLayout(ui->sdr_config_grid_layout);

    connect(&uio,&uiobj::txPPSSourceChanged,this,&mainGUI::updateTxPPSSource);
    connect(&uio,&uiobj::rxPPSSourceChanged,this,&mainGUI::updateRxPPSSource);

    connect(&uio,&uiobj::txREFSourceChanged,this,&mainGUI::updateTxREFSource);
    connect(&uio,&uiobj::rxREFSourceChanged,this,&mainGUI::updateRxREFSource);

    connect(&uio,&uiobj::USRPConfigurationChanged,this,&mainGUI::onUSRPConfigurationChanged);
    //connect(&uio,&uiobj::txSetupStatusChanged,this,&mainGUI::updateUSRPSetupChanged);

    connect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_USRP_setup);   

    connect(&tcom,&Tcom_ui::availableDevicesChanged,this,&mainGUI::updateAvailableDevices);
    connect(&tcom,&Tcom_ui::connectionChanged,this,&mainGUI::updateConnection);

    //SetWidgetColor(ui->indicator_captured_buffer,16146769);
    ui->indicator_synchronization->setState(1);

    ui->indicator_synchronization->setState(1);

    ui->lafw_signal_config->setLabelText("");
    ui->lafw_signal_config->setDataSource((&GUIConf.sigConfSignalFilepath));
    ui->lafw_signal_config->setEditable(true);

    ui->lafw_synch_capture->setLabelText("Capture filepath:");   
    ui->lafw_synch_capture->setDataSource(&(GUIConf.sigConfCaptureSignalFilepath));
    ui->lafw_synch_capture->setEditable(true);
    connect(ui->lafw_synch_capture,
            &LabelandFieldWidget::fieldTextEditing,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->indicator_format->setState(1);

    ui->lasbw_antenna_elements->requestSetValue(1,true);
    ui->lasbw_antenna_elements->setMinimum(1);
    ui->lasbw_antenna_elements->setLabelText("Current CAA Element:");

    ui->lasbw_active_class->requestSetValue(1,true);
    ui->lasbw_active_class->setMinimum(1);
    ui->lasbw_active_class->setLabelText("Class (UE):");

    ui->lasbw_active_sample->requestSetValue(1,true);
    ui->lasbw_active_sample->setMinimum(1);
    ui->lasbw_active_sample->setMaximum(3000);
    ui->lasbw_active_sample->setLabelText("Sample:");

    ui->lacb_use_sync->setLabelText("Use synchronization");
    connect(ui->lacb_use_sync,&LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onUseSynchronizationChanged);
    ui->lacb_use_sync->setDataSource(&(GUIConf.sigConfUseSynchronization),false);
    connect(ui->lacb_use_sync,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lacb_use_caa->setLabelText("Use CAA");
    connect(ui->lacb_use_caa,&LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onUseCAAChanged);   
    connect(ui->lacb_use_caa,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);
     ui->lacb_use_caa->requestSetValue(true);
     ui->lacb_use_caa->requestSetValue(false); // toggle
     ui->lacb_use_caa->setDataSource(&(GUIConf.autoCaptureUseCAA));

    // --------------------------- MATLAB Script Section ---------------------------
    ui->lafw_matlab_script_name->setLabelText("MATLAB script:");
    ui->lafw_matlab_script_name->setEditable(true);
    ui->lafw_matlab_script_name->setDataSource(&(GUIConf.sigConfMatlabScript));
    connect(ui->lafw_matlab_script_name,
            &LabelandFieldWidget::fieldTextEditing,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lafw_matlab_engine_name->setLabelText("Engine Name:");
    ui->lafw_matlab_engine_name->setEditable(true);
    ui->lafw_matlab_engine_name->setDataSource(&(GUIConf.sigConfMatlabEngineName));
    connect(ui->lafw_matlab_engine_name,
            &LabelandFieldWidget::fieldTextEditing,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lacbw_use_matlab_script->setLabelText("Use MATLAB script");
    ui->lacbw_use_matlab_script->requestSetValue(false);
    ui->lacbw_use_matlab_script->setEnabled(false);
    ui->lacbw_use_matlab_script->setToolTip("MATLAB engine must be connected");
    ui->script_container_auto_capture->hide();
    connect(ui->lacbw_use_matlab_script,&LabelandCheckboxWidget::componentValueChanged,this,
                    &mainGUI::onUseMatlabScript);
    connect(ui->lacbw_use_matlab_script,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lasbw_auto_capture_max_samples->setLabelText("Max Samples:");
    ui->lasbw_auto_capture_max_samples->setMinimum(1);
    ui->lasbw_auto_capture_max_samples->setMaximum(50000);
    ui->lasbw_auto_capture_max_samples->setDataSource(&(GUIConf.autoCaptureScriptMaxSamples));
    connect(ui->lasbw_auto_capture_max_samples,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);


    ui->ibtn_connect_matlab_engine->setState(1);

    ui->lacbw_use_class->setLabelText("Pass 'class' to script");
    ui->lacbw_use_class->requestSetValue(false);
    ui->lacbw_use_class->setDataSource(&(GUIConf.autoCaptureScriptPassClass));
    connect(ui->lacbw_use_class,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lasbw_script_delay->setLabelText("Capture Delay (ms)");
    ui->lasbw_script_delay->setMinimum(0);
    ui->lasbw_script_delay->setMaximum(10000);
    ui->lasbw_script_delay->setDataSource(&(GUIConf.autoCaptureScriptDelay));
    connect(ui->lasbw_script_delay,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    //ui->lacbw_use_matlab_script->setDataSource(&(GUIConf.sigConfUseMatlabScript),false);

    // ---------------------------

    //ui->lasbw_num_elements->requestSetValue(1,true);
    ui->lasbw_num_elements->setDataSource(&(GUIConf.sigConfNumberOfElements),true);
    ui->lasbw_num_elements->setMinimum(1);
    ui->lasbw_num_elements->setLabelText("Number of Elements:");
    connect(ui->lasbw_num_elements,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    //ui->lasbw_num_classes->requestSetValue(1,true);
    //ui->lasbw_num_classes->requestSetValue(4,true);
    ui->lasbw_num_classes->setDataSource(&(GUIConf.sigConfNumberOfClasses),true);
    ui->lasbw_num_classes->setMinimum(1);
    ui->lasbw_num_classes->setLabelText("Number of Classes:");
    connect(ui->lasbw_num_classes,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lasbw_synchCaptureOffset->setLabelText("Capture Offset:");
    ui->lasbw_synchCaptureOffset->setMaximum(500000);
    ui->lasbw_synchCaptureOffset->setDataSource(&(GUIConf.sigConfCaptureOffset),true);
    connect(ui->lasbw_synchCaptureOffset,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    //ui->lasbw_synchCaptureOffset->requestSetValue(256);


    // ------------------- Independent Capture Controls -------------------
    ui->lasbw_class_indep_capture->setLabelText("Current Class");
    ui->lasbw_class_indep_capture->setMinimum(1);
    ui->lasbw_class_indep_capture->requestSetValue(1);

    ui->lasbw_indep_capture_sample->setLabelText("Current Sample");
    ui->lasbw_indep_capture_sample->setMinimum(1);
    ui->lasbw_indep_capture_sample->requestSetValue(1);

    ui->lasbw_class_indep_max->setLabelText("Max classes");
    ui->lasbw_class_indep_max->setMinimum(1);
    ui->lasbw_class_indep_max->setMaximum(50000);
    connect(ui->lasbw_class_indep_max,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onIndependentCaptureMaxClassChanged);
    ui->lasbw_class_indep_max->setDataSource(&(GUIConf.sigConfIndependentCaptureMaxClasses),true);

    ui->lasbw_indep_cap_smpl_max->setLabelText("Max sample");
    ui->lasbw_indep_cap_smpl_max->setMinimum(1);
    ui->lasbw_indep_cap_smpl_max->setMaximum(50000);
    ui->lasbw_indep_cap_smpl_max->setDataSource(&(GUIConf.independentCaptureMaxSamples));
    connect(ui->lasbw_indep_cap_smpl_max,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onIndependentCaptureMaxSampleChanged);

    ui->lacb_indep_capture_auto->setLabelText("Auto Increment");
    ui->lacb_indep_capture_auto->setDataSource(&(GUIConf.sigConfIndependentCaptureAutoIncrement));

    ui->lafw_indep_cap_filepath->setLabelText("Directory path:");
    ui->lafw_indep_cap_filepath->setEditable(true);
    ui->lafw_indep_cap_filepath->setDataSource(&(GUIConf.sigConfIndependentCaptureFilepath));

    ui->lasbw_indep_capture_length->setLabelText("Capture length");
    ui->lasbw_indep_capture_length->setMinimum(1);
    ui->lasbw_indep_capture_length->setMaximum(1e6);
    ui->lasbw_indep_capture_length->setDataSource(&(GUIConf.sigConfIndependentCaptureLength));

// ---------------------------------------------------------

    ui->lasbw_synchCaptureLength->setLabelText("Capture Length:");
    ui->lasbw_synchCaptureLength->setMaximum(500000);
    ui->lasbw_synchCaptureLength->setDataSource(&(GUIConf.sigConfCaptureLength),true);
    connect(ui->lasbw_synchCaptureLength,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);


    ui->lasbw_num_repetitions->setLabelText("Repetitions:");
    ui->lasbw_num_repetitions->setMaximum(100);
    ui->lasbw_num_repetitions->setMinimum(1);
    ui->lasbw_num_repetitions->setDataSource(&(GUIConf.sigConfNumberOfRepetitions),true);
    connect(ui->lasbw_num_repetitions,
            &LabelandSpinBoxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lasbw_repetition->setLabelText("Repetition:");
    ui->lasbw_repetition->setMaximum(ui->lasbw_num_repetitions->getValue());
    ui->lasbw_repetition->setMinimum(1);

    ui->lacb_use_wndw_sync->setLabelText("Use window synch.");
    ui->lacb_use_wndw_sync->setDataSource(&(GUIConf.sigConfUseWindowSynchronization),true);
    connect(ui->lacb_use_wndw_sync,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lacb_single_class->setLabelText("Single class");
    ui->lacb_single_class->setDataSource(&(GUIConf.sysConfSingleClass),true);
    connect(ui->lacb_single_class,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lacb_auto_switch->setLabelText("Auto Switch");
    ui->lacb_auto_switch->setDataSource(&(GUIConf.sysConfAutoSwitch),true);
    connect(ui->lacb_auto_switch,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lacb_save_capture->setLabelText("Save Capture");
    ui->lacb_save_capture->setDataSource(&(GUIConf.sysConfSaveCapture),true);
    connect(ui->lacb_save_capture,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);

    ui->lacb_auto_save->setLabelText("Auto Save");
    ui->lacb_auto_save->setDataSource(&(GUIConf.sysConfAutoSave),true);
    connect(ui->lacb_auto_save,
            &LabelandCheckboxWidget::componentValueChanged,this,
            &mainGUI::onAutoCaptureSettingsChanged);


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

    autoCaptureTimer.setInterval(600);
    autoCaptureTimer.setSingleShot(false);
    autoCaptureTimer.start();

    ui->groupBox_7->adjustSize();
    //uio.ForceUpdateAll();

    for(int i =0; i<ui->tabWidget->count();i++){
        GUITabStruct tab_s;
        tabSizes.push_back(tab_s);
    }

    connect(&tabUpdateTimer, &QTimer::timeout, this, &mainGUI::onTabContentChanged);
    tabUpdateTimer.setSingleShot(true);  // Only run once
    connect(&winResizeTimer, &QTimer::timeout, this, &mainGUI::onWinResizeTimerEnd);
    winResizeTimer.setSingleShot(true);  // Only run once

    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());

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
        //SetWidgetColor(ui->indicator_captured_buffer,16146769); // last capture does not correspond to current
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

void mainGUI::onRadioControlWidgetLoadDefaultConfiguration(std::string serial_p, bool silent)
{
    int loadResponseCode = radObj->loadRadioConfigurationFile(serial_p,true);

    if(loadResponseCode == 0){
        RadioControlWidget * cRCW= qobject_cast<RadioControlWidget *>(sender());
        if(cRCW){
        }else{
            for(int i=0;radioControls.size();i++){
                if(radioControls[i]->getSerial() == serial_p){
                    cRCW = radioControls[i];
                    break;
                }
            }
        }

        cRCW->pushRadioConfiguration(radObj->getRadio(serial_p));
        //cRCW->pushRadioConfiguration(radObj->getRadioConfiguration(serial_p),0);
    }else if(loadResponseCode == -2){
        addStatusUpdate("Error; configuration file does not exist",ui->tableWidget_status,-1);
    }else{
        addStatusUpdate("Error; could not load configuration file",ui->tableWidget_status,-1);
    }
}

void mainGUI::onRadioControlWidgetLoadFileConfiguration(std::string serial_p, std::string filepath, bool silent)
{
    // Check if file exist
    if(std::filesystem::exists(filepath)){
        int response = radObj->loadRadioConfigurationFile(serial_p,false,filepath);
        if(response == 0){
            RadioControlWidget *cRCW = qobject_cast<RadioControlWidget *>(sender());
            cRCW->pushRadioConfiguration(radObj->getRadioConfiguration(serial_p),1);
        }
    }else{
        addStatusUpdate(QString::fromStdString("Error; File: " + filepath + " does not exist"),ui->tableWidget_status,-1);
    }
}

void mainGUI::onRadioControlWidgetTestBtnRelease(std::string serial_p, bool silent)
{
    std::cout << "Test button " << serial_p << " was released!" << std::endl;
    std::shared_ptr<cRadioObject> cRad = radObj->getRadio(serial_p);

    if(cRad->isConfigured() == false){
        cRad->configureRadio();
    }

    if(cRad->continous_reception_running == true){
        cRadioResponse cr_resp = cRad->stopContinousReception();
        std::cout << "Stop Reception Response: " << cr_resp.message << std::endl;
    }else{
        cRadioResponse cr_resp = cRad->startContinousReception();
        std::cout << "Start Reception Response: " << cr_resp.message << std::endl;
    }

}

void mainGUI::onRadioControlWidgetApplyConfigurationBtnReleased(std::string serial_p, cRadioConfiguration radConf_p)
{
    std::shared_ptr<cRadioObject> cRad = radObj->getRadio(serial_p);

    //cRad->setConfiguration(radConf_p);

    cRadioResponse response = cRad->configureRadio();

    RadioControlWidget * cRCW= qobject_cast<RadioControlWidget *>(sender());
    if(cRCW){
        cRCW->pushRadioConfigurationApplyStatus(response.code,cRad->getConfiguration());
    }
    if(response.code != 0){
        addStatusUpdate(QString::fromStdString(response.message),ui->tableWidget_status,-1);
    }
}

void mainGUI::onRadioControlWidgetContinousReceptionBtnReleased(std::string serial_p, bool silent)
{
    std::shared_ptr<cRadioObject> cRad = radObj->getRadio(serial_p);
    continousReceptionRadioControl * crRc = new continousReceptionRadioControl(nullptr,cRad);
    connect(crRc,&RadioControlBaseWidget::controlClosed,this,&mainGUI::onRadioControlWidgetClosed);
    connect(crRc,&RadioControlBaseWidget::statusUpdateRequest,this,&mainGUI::onRadioControlWidgetStatusUpdate);
    crRc->show();
    crRc->resize(420, 340);
}

void mainGUI::onRadioControlWidgetContinousTransmissionBtnReleased(std::string serial_p, bool silent)
{
    std::shared_ptr<cRadioObject> cRad = radObj->getRadio(serial_p);
    continousTransmissionRadioControl * crRc = new continousTransmissionRadioControl(nullptr,cRad);
    connect(crRc,&RadioControlBaseWidget::controlClosed,this,&mainGUI::onRadioControlWidgetClosed);
    connect(crRc,&RadioControlBaseWidget::statusUpdateRequest,this,&mainGUI::onRadioControlWidgetStatusUpdate);
    crRc->show();
    crRc->resize(420, 340);
}

void mainGUI::onRadioControlWidgetClosed()
{
    RadioControlBaseWidget * crRc = qobject_cast<RadioControlBaseWidget *>(sender());
    disconnect(crRc,&RadioControlBaseWidget::controlClosed,this,&mainGUI::onRadioControlWidgetClosed);
    disconnect(crRc,&RadioControlBaseWidget::statusUpdateRequest,this,&mainGUI::onRadioControlWidgetStatusUpdate);
    delete crRc;
}

void mainGUI::onRadioControlWidgetStatusUpdate(std::string message, int code)
{
    RadioControlBaseWidget * crRc = qobject_cast<RadioControlBaseWidget *>(sender());
    std::string serial = crRc->getSerial();
    std::string concat_m = serial + ": " + message;
    addStatusUpdate(QString::fromStdString(concat_m),ui->tableWidget_status,code);
}

mainGUI::~mainGUI()
{
    if(matlabEngineStatus == 1){
       // int response = engClose(matEng);
    }
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

    int maximumHeight = table->rowCount()*table->rowHeight(0)+table->horizontalHeader()->height();

    if(table->rowCount() < 4){
        int minimumHeight = table->rowCount()*table->rowHeight(0)+table->horizontalHeader()->height();
        table->setMinimumHeight(minimumHeight);
    }

    table->setMaximumHeight(maximumHeight);

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

void mainGUI::setEnabledAll(QObject* parent, bool enabled) {
    // If the parent is a QWidget
    if (QWidget* widget = qobject_cast<QWidget*>(parent)) {

        if (qobject_cast<LabelandCheckboxWidget*>(widget)) {
            LabelandCheckboxWidget * t_object = qobject_cast<LabelandCheckboxWidget*>(widget);
            t_object->setEnabled(enabled);
            return;
        }else if(qobject_cast<LabelandSpinBoxWidget*>(widget)){
            LabelandSpinBoxWidget * t_object = qobject_cast<LabelandSpinBoxWidget*>(widget);
            t_object->setEnabled(enabled);
            return;
        }else if(qobject_cast<LabelandFieldWidget*>(widget)){
            LabelandFieldWidget * t_object = qobject_cast<LabelandFieldWidget*>(widget);
            t_object->setEnabled(enabled);
            return;
        }

        // Set the widget's enabled state (unless it's a button)
        widget->setEnabled(enabled);

        // Recursively disable/enable all children
        for (QObject* child : widget->children()) {
            if (QWidget* childWidget = qobject_cast<QWidget*>(child)) {
                setEnabledAll(childWidget, enabled);  // Recursively handle child widget
            }
        }
    }
    // If the parent is a QLayout
    else if (QLayout* layout = qobject_cast<QLayout*>(parent)) {
        // Iterate through the layout items and set the enabled state for any widgets
        for (int i = 0; i < layout->count(); ++i) {
            QLayoutItem* item = layout->itemAt(i);
            if (QWidget* childWidget = item->widget()) {
                // Special check for QPushButton (or subclass of QAbstractButton)
                if (qobject_cast<LabelandCheckboxWidget*>(widget)) {
                    LabelandCheckboxWidget * t_object = qobject_cast<LabelandCheckboxWidget*>(widget);
                    t_object->setEnabled(enabled);
                    continue;
                }else if(qobject_cast<LabelandSpinBoxWidget*>(widget)){
                    LabelandSpinBoxWidget * t_object = qobject_cast<LabelandSpinBoxWidget*>(widget);
                    t_object->setEnabled(enabled);
                    continue;
                }else if(qobject_cast<LabelandFieldWidget*>(widget)){
                    LabelandFieldWidget * t_object = qobject_cast<LabelandFieldWidget*>(widget);
                    t_object->setEnabled(enabled);
                    continue;
                }

                // Set the enabled state for the widget
                childWidget->setEnabled(enabled);

                // Recursively check for nested layouts in child widgets
                setEnabledAll(childWidget, enabled);  // Recursively handle nested layouts/widgets
            } else if (QLayout* nestedLayout = item->layout()) {
                // Recursively handle nested layouts
                setEnabledAll(nestedLayout, enabled);
            }
        }
    }
    else {
        qWarning() << "The provided object is neither a QWidget nor a QLayout.";
    }
}

int mainGUI::runMATLABScript()
{
    if(matlabEngineStatus == 1){
        bool setClassParam = ui->lacbw_use_class->getValue();

        std::string class_command = "";
        if(setClassParam){
            class_command = "param_class = " + std::to_string(ui->lasbw_active_class->getValue()) + ";";
            matlabPtr->eval(stringToU16String(class_command));
        }

        std::string script_name = ui->lafw_matlab_script_name->getFieldText();

        matlabPtr->eval(stringToU16String(script_name));

        QString statusStr = QString::fromStdString("Success, script: " + script_name);
        if(setClassParam){
            statusStr = QString::fromStdString("Success, script: " + script_name + "; " + class_command);
        }

        addStatusUpdate(statusStr,ui->tableWidget_status,1);

        return 0;
    }else{
        addStatusUpdate("Error running MATLAB script...",ui->tableWidget_status,-1);
        return -1;
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
        //SetWidgetColor(ui->indicator_captured_buffer,9433252);

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

void mainGUI::runMATLABEngine()
{    

    // matlabEngineStatus = 2; // starting up

    // matEng = engOpen("Engine_1");
    // if(matEng == nullptr){
    //     std::cout<< "Error starting/connection to Matlab engine..." << std::endl;
    //     addStatusUpdate("Error starting/connection to Matlab engine...",ui->tableWidget_status,-1);
    // }else{
    //     matlabEngineStatus = 1; // running
    //     addStatusUpdate("MATLAB engine running...",ui->tableWidget_status,1);
    // }
}

std::u16string mainGUI::stringToU16String(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> convert32;

    auto u32string = convert32.from_bytes(str);
    std::u16string u16str(u32string.begin(),u32string.end());
    return u16str;
}

void mainGUI::resizeEvent(QResizeEvent *event)
{
    QSize newSize = event->size();
    QSize oldSize = event->oldSize();
    if(tabResize == false){

        int currentTabIndex = ui->tabWidget->currentIndex();
        tabSizes[currentTabIndex].set = true;
        tabSizes[currentTabIndex].width = newSize.width();
        tabSizes[currentTabIndex].height = newSize.height();

       // std::cout << "old = [" << oldSize.width() << ", " << oldSize.height() << "] :: ";
       // std::cout << "new = [" << newSize.width() << ", " << newSize.height() << "]" << std::endl;
    }
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

    bool useCAA = ui->lacb_use_caa->getValue();
    bool useSynchronization = ui->lacb_use_sync->getValue();
    bool useScript = ui->lacbw_use_matlab_script->getValue();

    int validation_response = validateAutomaticCapture();

    if(validation_response == 1 && useScript == true && useCAA == false && useSynchronization == false){
        processAutomaticCaptureScriptWOSynch();
    }

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

void mainGUI::processAutomaticCaptureCaaWSynch()
{
    int currentElement = ui->lasbw_antenna_elements->getValue();
    int currentClass = ui->lasbw_active_class->getValue();
    int currentSample = ui->lasbw_active_sample->getValue();
    int currentRepetition = ui->lasbw_repetition->getValue();

    bool useRepetition = (ui->lasbw_num_repetitions->getValue() > 1);

    bool finished = false;
    bool plotCapture = true;

    if(radObj->requestCaptureSynchFrame(currentElement-1)){
        if(radObj->isCapturedFramesReadyToSave()){
            // Capture is ready (all elements ahve been looped through)
            SaveSynchCaptures();
            plotPhaseComparison(ui->cb_time_plot->isChecked(),ui->cb_relative_plot->isChecked());
            radObj->resetCurrentFramesCaptured();
        }

        // Increment Element (should always be performed)
        ui->lasbw_antenna_elements->requestSetValue(currentElement+1);
        currentElement = ui->lasbw_antenna_elements->getValue();

        // Was the prev. the last element?
        if(currentElement == 1){

            ui->lasbw_repetition->requestSetValue(currentRepetition+1);
            currentRepetition = ui->lasbw_repetition->getValue();

            // Was the prev. the last repetition?
            if(currentRepetition == 1){

                if(ui->lacb_single_class->getValue() == true){
                    finished = true;
                }else{
                    ui->lasbw_active_class->requestSetValue(currentClass+1);
                    currentClass = ui->lasbw_active_class->getValue();

                    // Was the prev. the last class?
                    if(currentClass == 1){
                        ui->lasbw_active_sample->requestSetValue(currentSample+1);
                        currentSample = ui->lasbw_active_sample->getValue();

                        finished = true;
                    }
                }
            }

        }

        if(plotCapture){
            std::vector<std::complex<short>> vc_data = radObj->getExtractedSynchData();
            plot_time_and_freq(vc_data);
        }

    }

    if(finished == true){
        disconnect(&autoCaptureTimer,&QTimer::timeout,this,&mainGUI::processAutomaticCaptureCaaWSynch);
        automaticCaptureStatus = 0;
        ui->indicator_auto_capture->setState(1);
    }
}

void mainGUI::processAutomaticCaptureScriptWOSynch()
{
    int currentSample = ui->lasbw_active_sample->getValue();
    int currentClass = ui->lasbw_active_class->getValue();
    int maxSample = ui->lasbw_active_sample->getMaximum();

    bool finished = false;

    bool plot_capture = true;

    bool isSingleClass = ui->lacb_single_class->getValue();

    if(radObj->requestCaptureFrame()){
        if(radObj->isCapturedFramesReadyToSave()){
            // Capture has been made for current settings
            SaveSynchCaptures();
            radObj->resetCurrentFramesCaptured();

            ui->lasbw_active_sample->requestSetValue(currentSample+1);

            currentSample = ui->lasbw_active_sample->getValue();

            if(currentSample == 1){
                // Processed all samples
                if(isSingleClass == true){
                    finished = true;
                }else{
                    ui->lasbw_active_class->requestSetValue(currentClass+1);
                    currentClass = ui->lasbw_active_class->getValue();
                    if(currentClass == 1){
                        finished = true;
                    }
                }
            }

            if(finished == true){
                disconnect(&autoCaptureTimer,&QTimer::timeout,this,&mainGUI::processAutomaticCaptureScriptWOSynch);
                automaticCaptureStatus = 0;
                ui->indicator_auto_capture->setState(1);
            }else{
                runMATLABScript();
            }

            if(plot_capture){
                std::vector<std::complex<short>> vc_data = radObj->getExtractedSynchData();
                plot_time_and_freq(vc_data);
            }

        }

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
    int currentSample = ui->lasbw_indep_capture_sample->getValue();
    int currentClass = ui->lasbw_class_indep_capture->getValue();
    bool auto_increment = ui->lacb_indep_capture_auto->getValue();
    int captureLength = ui->lasbw_indep_capture_length->getValue();

    int maxSample = ui->lasbw_indep_cap_smpl_max->getValue();
    int maxClass = ui->lasbw_class_indep_max->getValue();

    // capture and save code goes here
    std::string fileDirectory = ui->lafw_indep_cap_filepath->getFieldText();

    // Check if directory exist first
    if(!std::filesystem::exists(fileDirectory)){
        std::filesystem::create_directories(fileDirectory);
    }

    std::string filename = "class" + std::to_string(currentClass) + "_sample" + std::to_string(currentSample) + ".csv";

    std::cout << filename << std::endl;

    std::vector<std::complex<short>> capturedData = radObj->writeBufferEndToFile(fileDirectory + "/" + filename,captureLength);

    plot_time_and_freq(capturedData);

    std::string status = "saved file " + fileDirectory + "/" + filename;
    addStatusUpdate(QString::fromStdString(status),ui->tableWidget_status,0);

    // Increment?
    if(auto_increment){
        // It is the last sample -> increment sample and increment class
        if(currentSample == maxSample){
            ui->lasbw_class_indep_capture->requestSetValue(currentClass+1,true);
        }

        ui->lasbw_indep_capture_sample->requestSetValue(currentSample+1,true);
    }

    //addStatusUpdate("Synchronization started",ui->tableWidget_status);

    // if(false){
    //     ui->button_write_buffer_to_file->setEnabled(false);
    //     ui->button_receive->setEnabled(false);
    //     SetWidgetColor(ui->indicator_captured_buffer,16380011);
    //     connect(&processingTimer, &QTimer::timeout, this, &mainGUI::trackCaptureBufferProcess);
    //     radObj->stopReception();
    //     radObj->requestWriteBufferToFile((ui->lineEdit_buffer_file_capture_path->text()).toStdString(),
    //                                      ui->spinBox_capture_samples->value()*1e3);
    // }
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
    int ik = 0;

    for(int i=0;i<L;i++){
        auto item = ui->listWidget_available_devices->item(i);
        std::string port = (item->text()).toStdString();

        std::string MCType = tcom.getMCType(port);

        if(MCType != ""){

            std::string MCMAC = tcom.getMCId(port);

            MCControlWidget *customWidget = new MCControlWidget(port,&tcom,currentMCControlIdentifier++,MCType,this);

            customWidget->setMAC(MCMAC);

            connect(customWidget,&MCControlWidget::cycleButtonReleased,this,&mainGUI::updateMCSCycle);
            int io = ik+1;
            ui->gridLayout_mcs->addWidget(customWidget,io % rows,std::floor(io/rows));

            mcControlWidgets.push_back(customWidget);
            //ui->verticalLayout_30->addWidget(customWidget);
            customWidget->show();

            ++ik;
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
    //ui->verticalLayout_12->addWidget(rxIPAddressField);
}

void mainGUI::userChangedTxIPAddress(std::string value)
{
    radObj->sysConf.setTxIPAddress(value);
    //ui->verticalLayout_12->addWidget(txIPAddressField);
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

        //ui->button_save_synch_capture->setEnabled(radObj->isCapturedFramesReadyToSave());

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
    bool useCAA = ui->lacb_use_caa->getValue();
    int numElements = 1;
    if(useCAA){
        numElements = ui->lasbw_num_elements->getValue();
        ui->lasbw_active_sample->setMaximum(5000);
    }else{
        ui->lasbw_active_sample->setMaximum(ui->lasbw_auto_capture_max_samples->getValue());
    }

    ui->lasbw_active_class->setMaximum(ui->lasbw_num_classes->getValue());
    int response = radObj->requestFrameCaptureFormat(ui->lasbw_synchCaptureOffset->getValue(),
                                                     ui->lasbw_synchCaptureLength->getValue(),
                                                     numElements,
                                                     ui->lafw_synch_capture->getFieldText(),
                                                     ui->lacb_use_wndw_sync->getValue());

    //ui->group_auto_capture->setEnabled(true);
    setEnabledAll(ui->group_auto_capture,true);
    ui->indicator_format->setState(2);

    // Save all
    ui->lafw_signal_config->saveCurrentValue();
    ui->lafw_synch_capture->saveCurrentValue();
    ui->lacb_use_sync->saveCurrentValue();
    ui->lacb_use_caa->saveCurrentValue();
    ui->lafw_matlab_script_name->saveCurrentValue();
    ui->lafw_matlab_engine_name->saveCurrentValue();
    ui->lacbw_use_matlab_script->saveCurrentValue();
    ui->lasbw_auto_capture_max_samples->saveCurrentValue();
    ui->lacbw_use_class->saveCurrentValue();
    ui->lasbw_script_delay->saveCurrentValue();
    ui->lasbw_num_elements->saveCurrentValue();
    ui->lasbw_num_classes->saveCurrentValue();
    ui->lasbw_synchCaptureOffset->saveCurrentValue();
    ui->lasbw_synchCaptureLength->saveCurrentValue();
    ui->lasbw_num_repetitions->saveCurrentValue();
    ui->lacb_use_wndw_sync->saveCurrentValue();
    ui->lacb_single_class->saveCurrentValue();
    ui->lacb_auto_switch->saveCurrentValue();
    ui->lacb_save_capture->saveCurrentValue();
    ui->lacb_auto_save->saveCurrentValue();

    if(response == 1){
        addStatusUpdate("Frame format updated...",ui->tableWidget_status);

        if(useCAA){
            ui->lasbw_antenna_elements->setMaximum(ui->lasbw_num_elements->getValue());
            ui->lasbw_active_class->setMaximum(ui->lasbw_num_classes->getValue());
        }

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

    // Check if directory exist first
    if(!std::filesystem::exists(fileDirectory)){
        std::filesystem::create_directories(fileDirectory);
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
    // Check if directory exist first

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
    if(automaticCaptureStatus == 0){

        bool useCAA = ui->lacb_use_caa->getValue();
        bool useSynchronization = ui->lacb_use_sync->getValue();
        bool useScript = ui->lacbw_use_matlab_script->getValue();

        int validation_response = validateAutomaticCapture();

        if(validation_response == 1 && useScript == true && useCAA == false && useSynchronization == false){
            int delay = ui->lasbw_script_delay->getValue();
            autoCaptureTimer.setInterval(delay);
            automaticCaptureStatus = 1;
            runMATLABScript();
            connect(&autoCaptureTimer,&QTimer::timeout,this,&mainGUI::processAutomaticCaptureScriptWOSynch);
        }else if(validation_response == 1 && useCAA == true && useSynchronization == true){
            autoCaptureTimer.setInterval(800);
            automaticCaptureStatus = 2;
            connect(&autoCaptureTimer,&QTimer::timeout,this,&mainGUI::processAutomaticCaptureCaaWSynch);
        }else{
            std::string error_response = "";
            switch(validation_response){
            case -1:
                error_response = "Transmission not running";
                break;
            case -2:
                error_response = "Reception not running";
                break;
            case -3:
                error_response = "Synchroniztaion not running";
                break;
            case -4:
                error_response = "Use script not enabled";
                break;

            }

            addStatusUpdate(QString::fromStdString("Automatic capture not started: '" + error_response + "'"),ui->tableWidget_status,-1);
        }

        if(automaticCaptureStatus != 0){
            ui->indicator_auto_capture->setState(2);
        }

        // > Validate that process should run
        // Format is set up
        // Auto switch is on
        // automaticCaptureRunning = true;
        // ui->indicator_auto_capture->setState(3);
        // connect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_automatic_capture);
    }else{
        automaticCaptureRunning = false;
        addStatusUpdate("Automatic Capture terminated...",ui->tableWidget_status,-1);


        switch (automaticCaptureStatus) {
        case 1:
            disconnect(&autoCaptureTimer,&QTimer::timeout,this,&mainGUI::processAutomaticCaptureScriptWOSynch);
            break;
        case 2:
            disconnect(&autoCaptureTimer,&QTimer::timeout,this,&mainGUI::processAutomaticCaptureCaaWSynch);
        default:
            break;
        }

        automaticCaptureStatus = 0;
        ui->indicator_auto_capture->setState(1);

        //disconnect(&processingTimer, &QTimer::timeout, this, &mainGUI::processing_automatic_capture);
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

void mainGUI::onUseSynchronizationChanged(bool value)
{
    // ui->lasbw_num_elements->setLabelText(value ? "Number of Elements:" : "Number of Samples:");
    if(value){
        //hideLayout(ui->vl_matlab_script);
        ui->synch_container_auto_capture->show();
        ui->synch_container_auto_capture_settings->show();
    }else{
       //unhideLayout(ui->vl_matlab_script);
        ui->synch_container_auto_capture->hide();
       ui->synch_container_auto_capture_settings->hide();
    }
}

void mainGUI::onUseCAAChanged(bool value)
{
    if(value){
        //hideLayout(ui->vl_matlab_script);
        ui->matlab_script_container->hide();
        ui->caa_auto_capture_settings_container->show();
        ui->lasbw_auto_capture_max_samples->hide();
        ui->lasbw_num_elements->show();
        ui->caa_container_auto_capture->show();       
    }else{
        //unhideLayout(ui->vl_matlab_script);
        ui->matlab_script_container->show();
        ui->caa_auto_capture_settings_container->hide();
        ui->lasbw_auto_capture_max_samples->show();
        ui->lasbw_num_elements->hide();
        ui->caa_container_auto_capture->hide();
    }
}

void mainGUI::onIndependentCaptureMaxClassChanged(int value)
{
    ui->lasbw_class_indep_capture->setMaximum(value);
}

void mainGUI::onIndependentCaptureMaxSampleChanged(int value)
{
    ui->lasbw_indep_capture_sample->setMaximum(value);
}

void mainGUI::onAutoCaptureSettingsChanged()
{
    QObject *senderObj = sender();
    //std::cout << (senderObj->objectName()).toStdString() << std::endl;
    std::string senderClass = (senderObj->metaObject()->className());

    bool newConfig = false;
    if(senderClass == "LabelandCheckboxWidget"){
        LabelandCheckboxWidget * cObject = qobject_cast<LabelandCheckboxWidget *>(senderObj);
        newConfig = !(cObject->isSaved());
    }else if(senderClass == "LabelandSpinBoxWidget"){
        LabelandSpinBoxWidget * cObject = qobject_cast<LabelandSpinBoxWidget *>(senderObj);
        newConfig = !(cObject->isSaved());
    }else if(senderClass == "LabelandFieldWidget"){
        LabelandFieldWidget * cObject = qobject_cast<LabelandFieldWidget *>(senderObj);
        newConfig = !(cObject->isSaved());
    }

    if(newConfig == true){
        setEnabledAll(ui->group_auto_capture,false);
        //ui->group_auto_capture->setEnabled(false);
        ui->indicator_format->setState(1);
    }else{
        setEnabledAll(ui->group_auto_capture,true);
        //ui->group_auto_capture->setEnabled(true);
        ui->indicator_format->setState(2);
    }
}

void mainGUI::onUseMatlabScript(bool value)
{
    if(value){
        if(matlabEngineStatus == 1){
             ui->script_container_auto_capture->show();
        }else{
            ui->lacbw_use_matlab_script->requestSetValue(false,true);
            addStatusUpdate("Must first connect to MATLAB engine",ui->tableWidget_status,-1);
        }
    }else{
        ui->script_container_auto_capture->hide();
    }
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
    // -4   = Use script not enabled
    // -5   = frame structure not loaded (not implemented)

    int response = 1;
    int error_count = 0;

    bool useCAA = ui->lacb_use_caa->getValue();
    if(radObj->isTransmitting() == false && useCAA == true){
        ++error_count;
        response = -1;
    }

    bool useScript = ui->lacbw_use_matlab_script->getValue();
    if(useCAA == false && useScript == false){
        ++error_count;
        response = -4;
    }

    if(!radObj->isReceiving()){
        ++error_count;
        response = -2;
    }

    bool useSynchronization = ui->lacb_use_sync->getValue();
    if(radObj->isSynchronizing() == false && useSynchronization == true){
        ++error_count;
        response = -3;
    }    


    // if(error_count > 1){
    //     response = 0;
    // }

    return response;
}

void mainGUI::addToGridLayout(QWidget *widget, QGridLayout *gridLayout, int rows)
{
    int io = gridLayout->count();

    bool isLastInRow = ((io % rows) == rows-1);
    gridLayout->addWidget(widget,io % rows,std::floor(io/rows));

}

void mainGUI::addVerticalSpacersToGridLayout(QGridLayout *gridLayout)
{
    int rows = gridLayout->rowCount();
    int cols = gridLayout->columnCount();

    int io = gridLayout->count() % rows;

    for(int i = 0;i<cols;i++){
        QSpacerItem * vspacer = new QSpacerItem(5,0,QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
        gridLayout->addItem(vspacer,rows,i);
    }

    //QSpacerItem * vspacer = new QSpacerItem(5,0,QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
    //gridLayout->addItem(vspacer,io,cols-1,rows,1);


}

int mainGUI::readGUIConfigFile()
{
    int response = 0; // success;    //     std::string script_name = ui->lafw_matlab_script_name->getFieldText();
    //     matlabPtr->eval(stringToU16String(script_name));

    try{
        std::ifstream iconf_file("gui.cfg");
        CFG::ReadFile(iconf_file,GUIConf.confEntriesVector,
                      GUIConf.sdrConfConfigFilepath,
                      GUIConf.sigConfSignalFilepath,
GUIConf.sigConfUseSynchronization,
                      GUIConf.sigConfNumberOfClasses,
                      GUIConf.sigConfNumberOfElements,
                      GUIConf.sigConfNumberOfRepetitions,
                      GUIConf.sigConfCaptureOffset,
                      GUIConf.sigConfCaptureLength,
                      GUIConf.sigConfUseWindowSynchronization,
GUIConf.sigConfUseMatlabScript,
GUIConf.sigConfMatlabScript,
GUIConf.sigConfMatlabEngineName,
                      GUIConf.sigConfIndependentCaptureMaxClasses,
                      GUIConf.sigConfIndependentCaptureFilepath,
                      GUIConf.sigConfIndependentCaptureLength,
GUIConf.sigConfIndependentCaptureAutoIncrement,
                      GUIConf.sigConfCaptureSignalFilepath,
                      GUIConf.sysConfAutoSwitch,
                      GUIConf.sysConfAutoSave,
                      GUIConf.sysConfSaveCapture,
                      GUIConf.sysConfSingleClass,
GUIConf.autoCaptureUseCAA,
                      GUIConf.autoCaptureScriptMaxSamples,
                      GUIConf.autoCaptureScriptPassClass,
                      GUIConf.autoCaptureScriptDelay,
GUIConf.independentCaptureMaxSamples);
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
GUIConf.sigConfUseSynchronization ? str_true : str_false,
                   GUIConf.sigConfNumberOfClasses,    //     std::string script_name = ui->lafw_matlab_script_name->getFieldText();
                   //     matlabPtr->eval(stringToU16String(script_name));
                   GUIConf.sigConfNumberOfElements,
                   GUIConf.sigConfNumberOfRepetitions,
                   GUIConf.sigConfCaptureOffset,
                   GUIConf.sigConfCaptureLength,
                   GUIConf.sigConfUseWindowSynchronization ? str_true : str_false,
GUIConf.sigConfUseMatlabScript ? str_true : str_false,
GUIConf.sigConfMatlabScript,
GUIConf.sigConfMatlabEngineName,
                   GUIConf.sigConfIndependentCaptureMaxClasses,
                   GUIConf.sigConfIndependentCaptureFilepath,
                   GUIConf.sigConfIndependentCaptureLength,
                   GUIConf.sigConfIndependentCaptureAutoIncrement ? str_true : str_false,
                   GUIConf.sigConfCaptureSignalFilepath,
                   GUIConf.sysConfAutoSwitch ? str_true : str_false,
                   GUIConf.sysConfAutoSave ? str_true : str_false,
                   GUIConf.sysConfSaveCapture ? str_true : str_false,
                   GUIConf.sysConfSingleClass ? str_true : str_false,
GUIConf.autoCaptureUseCAA ? str_true : str_false,
                   GUIConf.autoCaptureScriptMaxSamples,
                   GUIConf.autoCaptureScriptPassClass ? str_true : str_false,
                   GUIConf.autoCaptureScriptDelay,
GUIConf.independentCaptureMaxSamples);
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


void mainGUI::on_btn_run_script_released()
{
    int response = runMATLABScript();
    // if(matlabEngineStatus == 1){
    //     std::string script_name = ui->lafw_matlab_script_name->getFieldText();
    //     matlabPtr->eval(stringToU16String(script_name));
    // }
}


void mainGUI::on_btn_connect_matlab_engine_released()
{
    if(matlabEngineStatus == 0){
        std::vector<std::u16string> names = matlab::engine::findMATLAB();
        std::vector<std::u16string>::iterator it;
        std::string engineName_t = ui->lafw_matlab_engine_name->getFieldText();

        it = std::find(names.begin(),names.end(),stringToU16String(engineName_t));
        if(it != names.end()){
            matlabPtr = matlab::engine::connectMATLAB(*it);
        }

        std::string response_c = "";

        if(matlabPtr){
            matlab::data::ArrayFactory factory;
            matlab::data::CharArray arg = factory.createCharArray("-release");
            matlab::data::CharArray version = matlabPtr->feval(u"version",arg);
            std::cout << "Connected to: " << version.toAscii() << std::endl;

            //matlabPtr->eval(u"adalm_pluto_tx");
            response_c = "Connected to MATLAB engine '" + engineName_t + "'";
            addStatusUpdate(QString::fromStdString(response_c),ui->tableWidget_status,1);
            matlabEngineStatus = 1;
            ui->btn_connect_matlab_engine->setText("Disconnect");
            ui->ibtn_connect_matlab_engine->setState(2);
            ui->lacbw_use_matlab_script->setEnabled(true);
            ui->btn_run_script_indep->setEnabled(true);

        }else{
            std::cout << engineName_t << " not found" << std::endl;
            response_c = "MATLAB engine '" + engineName_t + "' not found";
            matlabEngineStatus = 0;
            ui->ibtn_connect_matlab_engine->setState(3);
            addStatusUpdate(QString::fromStdString(response_c),ui->tableWidget_status,-1);
            ui->lacbw_use_matlab_script->setEnabled(false);
            ui->btn_run_script_indep->setEnabled(false);
        }

    }else{
        // stop matlab engine
        //int response = engClose(matEng);
        matlabEngineStatus = 0;
        matlabPtr.reset(nullptr);
        ui->btn_connect_matlab_engine->setText("Connect");
        addStatusUpdate("Stopped MATLAB engine",ui->tableWidget_status,-1);
        ui->ibtn_connect_matlab_engine->setState(1);
        ui->lacbw_use_matlab_script->requestSetValue(false);
        ui->lacbw_use_matlab_script->setEnabled(false);
        ui->btn_run_script_indep->setEnabled(false);
    }
}


void mainGUI::on_btn_test_released()
{

    for(auto rCW : radioControls ){
        std::string serial = rCW->getSerial();
        int count = radObj->getRadio(serial).use_count();
        std::cout << serial << ": " << count << std::endl;
    }
    // int response_find_radios = radObj->findRadios();
    // int response_connect_radio = radObj->connectRadio("F270FD");
    // switch(response_connect_radio){
    //     case -2:
    //         std::cout << "Radio already connected" << std::endl;
    //     break;
    //     case -1:
    //         std::cout << "Radio not found" << std::endl;
    //         break;
    // }

}


void mainGUI::on_btn_run_script_indep_released()
{
    if(matlabEngineStatus == 1){
        bool setClassParam = ui->lacbw_use_class->getValue();

        if(setClassParam){
            std::string class_command = "param_class = " + std::to_string(ui->lasbw_class_indep_capture->getValue()) + ";";
            matlabPtr->eval(stringToU16String(class_command));
        }

        std::string script_name = ui->lafw_matlab_script_name->getFieldText();

        matlabPtr->eval(stringToU16String(script_name));

        addStatusUpdate("Succes running MATLAB script",ui->tableWidget_status,1);

    }else{
        addStatusUpdate("Error running MATLAB script...",ui->tableWidget_status,-1);

    }
}


void mainGUI::on_tabWidget_3_currentChanged(int index)
{
    if(index == 1){
        ui->group_auto_capture->hide();
        ui->groupBox_7->show();
    }else{
        ui->groupBox_7->hide();
        ui->group_auto_capture->show();
    }
}


void mainGUI::on_btn_find_radios_released()
{
    int response = radObj->findRadios();
    if(response ==0 ){
        ui->listWidget_available_radios->clear();

        std::vector<std::string> addrs_v = radObj->getAvailableRadiosProperty("addr");
        std::vector<std::string> serials_v = radObj->getAvailableRadiosProperty("serial");
        std::vector<std::string> types_v = radObj->getAvailableRadiosProperty("type");


        addStatusUpdate("Success " + QString::number(addrs_v.size()) + ", radios found",ui->tableWidget_status,1);

        for(int i=0; i<addrs_v.size();i++){
            std::string type_t = types_v[i];
            transform(type_t.begin(), type_t.end(), type_t.begin(), ::toupper);
            std::string list_entry = type_t + " :: " + serials_v[i] + " :: " + addrs_v[i];
            ui->listWidget_available_radios->addItem(QString::fromStdString(list_entry));
        }
    }else{
       addStatusUpdate("No radios found; Check connections",ui->tableWidget_status,-1);
    }
}


void mainGUI::on_btn_connect_radio_released()
{
    std::vector<std::string> serials_v = radObj->getAvailableRadiosProperty("serial");
    QListWidgetItem *currentItem = ui->listWidget_available_radios->currentItem();
    int index = ui->listWidget_available_radios->row(currentItem);

    if(index == -1){
        return; // no index selected
    }

    bool isConnectMode = (ui->btn_connect_radio->text() == "Connect");

    if(isConnectMode){
        int response = radObj->connectRadio(serials_v[index],false);
        if(response == 0){
            addStatusUpdate("Success connecting to " + QString::fromStdString(serials_v[index]) + "",ui->tableWidget_status,1);
            RadioControlWidget * rcWidget = new RadioControlWidget(this,radObj->getRadio(serials_v[index]));            
            connect(rcWidget,&RadioControlWidget::loadDefaultConfigurationRequest,this,&mainGUI::onRadioControlWidgetLoadDefaultConfiguration);
            connect(rcWidget,&RadioControlWidget::loadFileConfigurationRequest,this,&mainGUI::onRadioControlWidgetLoadFileConfiguration);
            connect(rcWidget,&RadioControlWidget::testRequest,this,&mainGUI::onRadioControlWidgetTestBtnRelease);
            connect(rcWidget,&RadioControlWidget::applyConfigurationRequest,this,&mainGUI::onRadioControlWidgetApplyConfigurationBtnReleased);
            connect(rcWidget,&RadioControlWidget::continousReceptionControlRequest,this,&mainGUI::onRadioControlWidgetContinousReceptionBtnReleased);
            connect(rcWidget,&RadioControlWidget::continousTransmissionControlRequest,this,&mainGUI::onRadioControlWidgetContinousTransmissionBtnReleased);

            radioControls.push_back(rcWidget);

            ui->vl_radio_controls->addWidget(rcWidget);

            onRadioControlWidgetLoadDefaultConfiguration(serials_v[index],false); // automatically load default

            //ui->vl_radio_controls->insertWidget(ui->vl_radio_controls->indexOf(ui->vspacer_vl_radio_controls),rcWidget);
            ui->btn_connect_radio->setText("Disconnect");
        }else{
            addStatusUpdate("Error; Radio " + QString::fromStdString(serials_v[index]) + " could not connect",ui->tableWidget_status,-1);
        }
    }else{

        int foundIndex = -1;
        for(int i=0;i<radioControls.size();i++){
            if(radioControls[i]->getSerial() == serials_v[index]){
                foundIndex = i;
            }
        }
        int response = 0;
        if(foundIndex != -1){
            ui->vl_radio_controls->removeWidget(radioControls[foundIndex]);
            disconnect(radioControls[foundIndex],&RadioControlWidget::loadDefaultConfigurationRequest,this,&mainGUI::onRadioControlWidgetLoadDefaultConfiguration);
            disconnect(radioControls[foundIndex],&RadioControlWidget::loadFileConfigurationRequest,this,&mainGUI::onRadioControlWidgetLoadFileConfiguration);
            disconnect(radioControls[foundIndex],&RadioControlWidget::testRequest,this,&mainGUI::onRadioControlWidgetTestBtnRelease);
            disconnect(radioControls[foundIndex],&RadioControlWidget::applyConfigurationRequest,this,&mainGUI::onRadioControlWidgetApplyConfigurationBtnReleased);
            disconnect(radioControls[foundIndex],&RadioControlWidget::continousReceptionControlRequest,this,&mainGUI::onRadioControlWidgetContinousReceptionBtnReleased);
            disconnect(radioControls[foundIndex],&RadioControlWidget::continousTransmissionControlRequest,this,&mainGUI::onRadioControlWidgetContinousTransmissionBtnReleased);

            delete radioControls[foundIndex];
            radioControls.erase(radioControls.begin()+foundIndex);
            response = radObj->disconnectRadio(serials_v[index],false);
        }

        if(response == 0){
            addStatusUpdate("Success disconnecting " + QString::fromStdString(serials_v[index]) + "",ui->tableWidget_status,1);
            ui->btn_connect_radio->setText("Connect");
        }else{
            addStatusUpdate("Error; Radio " + QString::fromStdString(serials_v[index]) + " could not disconnect",ui->tableWidget_status,-1);
        }
    }

}


void mainGUI::on_btn_configure_radio_released()
{

}


void mainGUI::on_tabWidget_currentChanged(int index)
{
    ui->sdr_config_container->hide();
    ui->signal_conf_container->hide();
    ui->system_control_container->hide();
    ui->mc_container->hide();
    switch(index){
    case 1:
        ui->sdr_config_container->show();
        break;
    case 2:
        ui->signal_conf_container->show();
        break;
    case 3:
        ui->system_control_container->show();
        break;
    case 4:
        ui->mc_container->show();
        break;
    }

    tabResize = true;
    tabUpdateTimer.start(100);
    winResizeTimer.start(200);

}

void mainGUI::onTabContentChanged()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    if((tabSizes[currentTabIndex]).set == true){
        QSize size_p(tabSizes[currentTabIndex].width,tabSizes[currentTabIndex].height);
        this->resize(size_p);
    }else{
        this->resize(this->minimumSizeHint());
    }
}

void mainGUI::onWinResizeTimerEnd()
{
    tabResize = false;
}



void mainGUI::on_listWidget_available_radios_itemSelectionChanged()
{
    std::vector<std::string> serials_v = radObj->getAvailableRadiosProperty("serial");
    QListWidgetItem *currentItem = ui->listWidget_available_radios->currentItem();
    int index = ui->listWidget_available_radios->row(currentItem);

    bool isConnected = false;
    for(int i=0;i<radioControls.size();i++){
        std::string rc_serial = radioControls[i]->getSerial();
        if(serials_v[index] == rc_serial){
            radioControls[i]->show();
            isConnected = true;
        }else{
            radioControls[i]->hide();
        }
    }

    if(isConnected == true){
        ui->btn_connect_radio->setText("Disconnect");
        ui->btn_connect_radio->setEnabled(true);
    }else{
        ui->btn_connect_radio->setText("Connect");
        ui->btn_connect_radio->setEnabled(true);
    }


}

