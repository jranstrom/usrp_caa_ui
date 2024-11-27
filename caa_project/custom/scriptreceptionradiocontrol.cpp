#include "scriptreceptionradiocontrol.h"
#include <filesystem>
#include "uhd_clib.h"


ScriptReceptionRadioControl::ScriptReceptionRadioControl(QWidget *parent, std::shared_ptr<cRadioObject> rad_p)
    : RadioControlBaseWidget(parent, rad_p){

    std::string windowTitle = sourceRadio->getSerial() + " - Script Reception";
    setWindowTitle(QString::fromStdString(windowTitle));

    mainLayout = new QVBoxLayout(this);

    // Information section
    QWidget * infoSectionContainerWidget = new QWidget;
    infoSectionLayout = new QGridLayout(infoSectionContainerWidget);

    serialInfoField = new LabelandFieldWidget(this,"USRP Serial:",sourceRadio->getSerial());
    infoSectionLayout->addWidget(serialInfoField,0,0,1,2);

    typeInfoField = new LabelandFieldWidget(this,"USRP Type:",sourceRadio->getType());
    infoSectionLayout->addWidget(typeInfoField,1,0);

    addressInfoField = new LabelandFieldWidget(this,"Address:",sourceRadio->getAddress());
    infoSectionLayout->addWidget(addressInfoField,1,1);

    mainSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);

    QWidget * scriptEngineContainer = new QWidget;
    scriptEngineLayout = new QGridLayout(scriptEngineContainer);

    scriptEngineField = new LabelandFieldWidget(this,"Engine name:","Engine_1",true);
    scriptNameField = new LabelandFieldWidget(this,"Script name:","adalm_pluto_tx",true);

    QWidget * engineStatusContainer = new QWidget;
    QHBoxLayout * engineStatusLayout = new QHBoxLayout(engineStatusContainer);

    QSpacerItem * engineStatusLeftSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);
    engineConnectedIndidicator = new IndicatorButtonWidget(this);
    engineConnectedIndidicator->setState(1);
    engineConnectBtn = new QPushButton("Connect",this);
    connect(engineConnectBtn,&QPushButton::released,this,&ScriptReceptionRadioControl::onConnectEngineBtnRelease);
    QSpacerItem * engineStatusRightSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);

    engineStatusLayout->addItem(engineStatusLeftSpacer);
    engineStatusLayout->addWidget(engineConnectedIndidicator);
    engineStatusLayout->addWidget(engineConnectBtn);
    engineStatusLayout->addItem(engineStatusRightSpacer);

    QWidget * scriptParamsContainer = new QWidget;
    QHBoxLayout * scriptParamLayout = new QHBoxLayout(scriptParamsContainer);

    QSpacerItem * engineParamLeftSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);
    passToEngineCheckBox = new LabelandCheckboxWidget(this,"Pass param",false);
    paramToEngineSpinBox = new LabelandSpinBoxWidget(this,"Value",100,0,1);
    paramToEngineSpinBox->requestSetValue(1);
    QSpacerItem * engineParamRightSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);

    runScriptBtn = new QPushButton("Run Script",this);
    connect(runScriptBtn,&QPushButton::released,this,&ScriptReceptionRadioControl::onRunScriptBtnRelease);

    scriptParamLayout->addItem(engineParamLeftSpacer);
    scriptParamLayout->addWidget(passToEngineCheckBox);
    scriptParamLayout->addWidget(paramToEngineSpinBox);
    scriptParamLayout->addItem(engineParamRightSpacer);

    scriptEngineLayout->addWidget(scriptEngineField,0,0);
    scriptEngineLayout->addWidget(engineStatusContainer,1,0);
    scriptEngineLayout->addWidget(scriptNameField,0,1);
    scriptEngineLayout->addWidget(scriptParamsContainer,1,1);
    scriptEngineLayout->addWidget(runScriptBtn,2,1);


    // Capture Section
    QWidget * captureFileSectionContainer = new QWidget;
    QHBoxLayout * captureFileLayout = new QHBoxLayout(captureFileSectionContainer);
    capturePathField = new LabelandFieldWidget(this,"Filepath:","data/script_capture/capture_",true);
    capturePathField->setFieldAlignment(Qt::AlignRight);
    captureModifierField = new QLineEdit("1",this);
    captureModifierField->setEnabled(false);
    captureModifierField->setAlignment(Qt::AlignCenter);
    captureModifierField->setMaximumWidth(30);
    captureModifierField->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    captureDotLabel = new QLabel(".");
    captureDotLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    captureExtentionField = new QLineEdit("csv",this);
    captureExtentionField->setMaximumWidth(40);
    captureExtentionField->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    browsePathBtn = new QPushButton("browse",this);
    browsePathBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    captureFileLayout->addWidget(capturePathField);
    captureFileLayout->addWidget(captureModifierField);
    captureFileLayout->addWidget(captureDotLabel);
    captureFileLayout->addWidget(captureExtentionField);
    captureFileLayout->addWidget(browsePathBtn);

    QWidget * captureParamContainer = new QWidget;
    //QHBoxLayout * captureParamLayout = new QHBoxLayout(captureParamContainer);
    QGridLayout * captureParamLayout = new QGridLayout(captureParamContainer);
    captureDelaySpinBox = new LabelandSpinBoxWidget(this,"Capture delay (ms):",5000,0);
    captureDelaySpinBox->requestSetValue(500);
    captureDataLengthSpinBox = new LabelandSpinBoxWidget(this,"Samples/Capture:",10000,0);
    captureDataLengthSpinBox->requestSetValue(1000);
    captureNumberSpinBox = new LabelandSpinBoxWidget(this,"Captures total:",5000,0);
    captureNumberSpinBox->requestSetValue(20);
    connect(captureNumberSpinBox,&LabelandSpinBoxWidget::componentValueChanged,this,&ScriptReceptionRadioControl::onNumberOfCapturesChanged);
    runScriptEachCatpureCheckBox = new LabelandCheckboxWidget(this,"Run Script each capture",false);

    addRandomCFOCheckBox = new LabelandCheckboxWidget(this,"Use random carrier offset",false);
    addRandomPhaseCheckBox = new LabelandCheckboxWidget(this,"Use random phase",false);

    captureParamLayout->addWidget(captureDelaySpinBox,0,0);
    captureParamLayout->addWidget(captureDataLengthSpinBox,0,1);
    captureParamLayout->addWidget(captureNumberSpinBox,1,0);
    captureParamLayout->addWidget(runScriptEachCatpureCheckBox,1,1);
    captureParamLayout->addWidget(addRandomCFOCheckBox,2,0);
    captureParamLayout->addWidget(addRandomPhaseCheckBox,2,1);



    QWidget * captureControlContainer = new QWidget;
    QHBoxLayout * captureControlLayout = new QHBoxLayout(captureControlContainer);

    QSpacerItem * captureControlLeftSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);
    autoCaptureCurrentSpinBox = new LabelandSpinBoxWidget(this,"Current Capture:",captureNumberSpinBox->getValue(),1);
    autoCaptureCurrentSpinBox->requestSetValue(1);
    connect(autoCaptureCurrentSpinBox,&LabelandSpinBoxWidget::componentValueChanged,this,&ScriptReceptionRadioControl::onCurrentCaptureChanged);

    autoCaptureStatusIndicator = new IndicatorButtonWidget(1,0,this);
    autoCaptureBtn = new QPushButton("Auto Capture",this);
    connect(autoCaptureBtn,&QPushButton::released,this,&ScriptReceptionRadioControl::onAutoCaptureBtnRelease);
    QSpacerItem * captureControlRightSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);


    captureControlLayout->addSpacerItem(captureControlLeftSpacer);
    captureControlLayout->addWidget(autoCaptureCurrentSpinBox);
    captureControlLayout->addWidget(autoCaptureStatusIndicator);
    captureControlLayout->addWidget(autoCaptureBtn);
    captureControlLayout->addSpacerItem(captureControlRightSpacer);

    //

    // Control Section
    QWidget * controlSectionContainer = new QWidget;
    //controlSectionContainer->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    controlSectionLayout = new QVBoxLayout(controlSectionContainer);

    QWidget * statusSectionContainer = new QWidget;
    //statusSectionContainer->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    statusSectionLayout = new QHBoxLayout(statusSectionContainer);

    QSpacerItem * statusLeftSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);
    receptionStatusIndicator = new IndicatorButtonWidget(this);
    receptionStatusLabel = new QLabel("No ongoing reception",this);
    toggleReceptionBtn = new QPushButton("Start",this);
    connect(toggleReceptionBtn,&QPushButton::released,this,&ScriptReceptionRadioControl::onToggleReceptionBtnRelease);
    QSpacerItem * statusRightSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Fixed);

    statusSectionLayout->addItem(statusLeftSpacer);
    statusSectionLayout->addWidget(receptionStatusIndicator);
    statusSectionLayout->addWidget(receptionStatusLabel);
    statusSectionLayout->addWidget(toggleReceptionBtn);
    statusSectionLayout->addItem(statusRightSpacer);

    QFrame * IandCDivider = new QFrame();IandCDivider->setFrameShape(QFrame::HLine);IandCDivider->setFrameShadow(QFrame::Sunken);
    QFrame * divider2 = new QFrame();divider2->setFrameShape(QFrame::HLine);divider2->setFrameShadow(QFrame::Sunken);
    QFrame * divider3 = new QFrame();divider3->setFrameShape(QFrame::HLine);divider3->setFrameShadow(QFrame::Sunken);


    mainLayout->addWidget(infoSectionContainerWidget);
    mainLayout->addWidget(IandCDivider);
    mainLayout->addWidget(scriptEngineContainer);
    mainLayout->addWidget(divider2);
    mainLayout->addWidget(captureFileSectionContainer);
    mainLayout->addWidget(captureParamContainer);
    mainLayout->addWidget(captureControlContainer);
    mainLayout->addWidget(divider3);
    mainLayout->addWidget(statusSectionContainer);
    mainLayout->addItem(mainSpacer);

    receptionProcessTimer.setInterval(1000);
    receptionProcessTimer.setSingleShot(false);
    receptionProcessTimer.start();
    connect(&receptionProcessTimer,&QTimer::timeout,this,&ScriptReceptionRadioControl::onReceptionProcessTimerTick);

    autoCaptureTimer.setInterval(100);
    autoCaptureTimer.setSingleShot(false);
    connect(&autoCaptureTimer,&QTimer::timeout,this,&ScriptReceptionRadioControl::onAutoCaptureTimeTick);
}

void ScriptReceptionRadioControl::onControlClose()
{
    receptionProcessTimer.stop();
    disconnect(&receptionProcessTimer,&QTimer::timeout,this,&ScriptReceptionRadioControl::onReceptionProcessTimerTick);
    disconnect(&autoCaptureTimer,&QTimer::timeout,this,&ScriptReceptionRadioControl::onAutoCaptureTimeTick);
    if(isReceiving == false){
        return;
    }

    cRadioResponse response = sourceRadio->stopContinousReception();

    if(response.code == 0){
        isReceiving = false;
        emit statusUpdateRequest("Reception terminated",-1);
    }else{
        emit statusUpdateRequest(response.message,-1);
    }
}

void ScriptReceptionRadioControl::onToggleReceptionBtnRelease()
{
    cRadioResponse response;
    std::string statusUpdate_m = "";
    int statusUpdateCode = 0;

    if(isReceiving == false){
        response = sourceRadio->startContinousReception();

        if(response.code == 0){
            statusUpdate_m = "Reception started";
            statusUpdateCode = 0;
        }else{
            statusUpdate_m = response.message;
            statusUpdateCode = -1;
        }

    }else{
        response = sourceRadio->stopContinousReception();
        if(response.code == 0){
            statusUpdate_m = "Reception stopped";
            statusUpdateCode = 0;
        }else{
            statusUpdate_m = response.message;
            statusUpdateCode = -1;
        }
    }

    if(statusUpdate_m != ""){
        emit statusUpdateRequest(statusUpdate_m,statusUpdateCode);
    }
}

void ScriptReceptionRadioControl::onRunScriptBtnRelease()
{
    if(engineConnected == true){
        if(passToEngineCheckBox->getValue() == true){
            std::string command = "";
            command = "param_class = " + std::to_string(paramToEngineSpinBox->getValue()) + ";";
            matlabPtr->eval(stringToU16String(command));
        }

        std::string script_name = scriptNameField->getFieldText();
        matlabPtr->eval(stringToU16String(script_name));

        emit statusUpdateRequest("Finished running " + script_name,1);

    }else{
        emit statusUpdateRequest("Must be connected to engine to run script",-1);
    }
}

void ScriptReceptionRadioControl::onConnectEngineBtnRelease()
{
    if(engineConnected == false){
        std::vector<std::u16string> names = matlab::engine::findMATLAB();
        std::vector<std::u16string>::iterator it;
        std::string engineName_t = scriptEngineField->getFieldText();

        it = std::find(names.begin(),names.end(),stringToU16String(engineName_t));
        if(it != names.end()){
            matlabPtr = matlab::engine::connectMATLAB(*it);
        }

        if(matlabPtr){
            matlab::data::ArrayFactory factory;
            matlab::data::CharArray arg = factory.createCharArray("-release");
            matlab::data::CharArray version = matlabPtr->feval(u"version",arg);
            engineConnectedIndidicator->setState(2);
            emit statusUpdateRequest("Connected to " + version.toAscii(),1);
            engineConnected = true;
        }else{
            emit statusUpdateRequest("Could not connect to matlab engine " + engineName_t,-1);
        }
    }
}

void ScriptReceptionRadioControl::onReceptionProcessTimerTick()
{
    bool cStatus = sourceRadio->continous_reception_running;
    if(cStatus != isReceiving){
        isReceiving = cStatus;
        if(isReceiving == true){
            toggleReceptionBtn->setText("Stop");
            receptionStatusIndicator->setState(2);
            receptionStatusLabel->setText("Reception in progress");
        }else{
            toggleReceptionBtn->setText("Start");
            receptionStatusIndicator->setState(1);
            receptionStatusLabel->setText("No ongoing reception");
        }
        return;
    }
}

void ScriptReceptionRadioControl::onAutoCaptureBtnRelease()
{
    if(autoCaptureRunning == false){
        nextCaptureTime = std::chrono::system_clock::now()+std::chrono::milliseconds(captureDelaySpinBox->getValue());
        autoCaptureTimer.start();
        autoCaptureRunning = true;
    }else{
        autoCaptureRunning = false;
    }
}

void ScriptReceptionRadioControl::onNumberOfCapturesChanged(int value, bool silent)
{
    autoCaptureCurrentSpinBox->setMaximum(value);
}

void ScriptReceptionRadioControl::onCurrentCaptureChanged(int value, bool silent)
{
    captureModifierField->setText(QString::number(value));
}

void ScriptReceptionRadioControl::onAutoCaptureTimeTick()
{
    if(autoCaptureRunningPrev != autoCaptureRunning){
        // status changed

        if(autoCaptureRunning == true){
            autoCaptureStatusIndicator->setState(2);
            autoCaptureBtn->setText("Stop Capture");
        }else{
            autoCaptureStatusIndicator->setState(1);
            autoCaptureBtn->setText("Auto Capture");
        }

        autoCaptureRunningPrev = autoCaptureRunning;
    }

    if(autoCaptureRunning == false){

        return;
    }

    if(std::chrono::system_clock::now() > nextCaptureTime){

        saveCapture(); // save
        int currentCaptureVal = autoCaptureCurrentSpinBox->getValue();

        if(currentCaptureVal == captureNumberSpinBox->getValue()){
            autoCaptureRunning = false;
            autoCaptureCurrentSpinBox->requestSetValue(autoCaptureCurrentSpinBox->getMinimum());
        }else{
            autoCaptureCurrentSpinBox->requestSetValue(currentCaptureVal+1); //

            if(runScriptEachCatpureCheckBox->getValue() == true){
                onRunScriptBtnRelease(); // run script
            }
            nextCaptureTime = std::chrono::system_clock::now()+std::chrono::milliseconds(captureDelaySpinBox->getValue());
        }


    }
}

void ScriptReceptionRadioControl::saveCapture()
{
    std::string fileExtension = captureExtentionField->text().toStdString();
    std::string modifier = captureModifierField->text().toStdString();
    std::string currentFilepath = capturePathField->getFieldText();
    std::string combinedPath = currentFilepath + modifier + "." + fileExtension;

    std::filesystem::path fullPath(combinedPath);
    std::filesystem::path directoryPath = fullPath.parent_path();

    cRadioResponse response;
    response.code = 0;
    response.message = "Success";

    bool isValid = false;

    if(fileExtension == "dat" || fileExtension == "csv"){
        isValid = true;
    }else{
        response.code =-1;
        response.message = "Missing support for file extension " + fileExtension;
    }

    // Create directory if it does not exist
    if(isValid == true && std::filesystem::exists(directoryPath) == false){
        std::filesystem::create_directory(directoryPath);
    }

    if(fileExtension == "csv"){
        response = saveCSVfile(combinedPath,captureDataLengthSpinBox->getValue());
    }

    emit statusUpdateRequest(response.message,response.code);
}

cRadioResponse ScriptReceptionRadioControl::saveCSVfile(std::string filepath, int dataLength)
{
    cRadioResponse response;
    response.message = "Success";
    response.code = 0;

    std::vector<std::complex<short>> rxSamples_short;

    do{
        rxSamples_short = sourceRadio->getLastReceivedSamples(dataLength,response);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }while(response.code == -3);

    if(response.code < 0){
        response.code = -1;
        return response;
    }

    std::vector<std::complex<double>> rxSamples_double = uhd_clib::cvec_conv_short2double(rxSamples_short);
    const double pi = 3.14159265359;
    double fs = sourceRadio->getStagedConfiguation()["rx-sampling-rate"]->getPropertyValueDouble();
    const std::complex<double> j = std::complex<double>(0.0,1.0);

    if(addRandomCFOCheckBox->getValue() == true){
        double rand_cfo_hz = ((rand() / double(RAND_MAX))-0.5)*2*fs/4;
        for(int i=0;i<dataLength;i++){
            double n = static_cast<double>(i);
            rxSamples_double[i] = rxSamples_double[i]*std::exp(2*pi*j*rand_cfo_hz*n/fs);
        }
    }

    if(addRandomPhaseCheckBox->getValue() == true){
        double rand_phase = (rand() / double(RAND_MAX))*2*pi;
        for(int i=0;i<dataLength;i++){
            double n = static_cast<double>(i);
            rxSamples_double[i] = rxSamples_double[i]*std::exp(j*rand_phase);
        }
    }

    std::ofstream outfile;
    outfile.open(filepath);
    if(outfile.is_open()){
        for(int i =0;i<dataLength;i++){
            outfile << std::real(rxSamples_double[i]) << "," << std::imag(rxSamples_double[i]) << "\n";
        }

        outfile.close();
    }else{
        response.message = "Could not open file " + filepath;
        response.code = -1;
    }
}

std::u16string ScriptReceptionRadioControl::stringToU16String(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> convert32;

    auto u32string = convert32.from_bytes(str);
    std::u16string u16str(u32string.begin(),u32string.end());
    return u16str;
}
