#include "scriptreceptionradiocontrol.h"


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
    passToEngineCheckBox = new LabelandCheckboxWidget(this,"Pass param:",false);
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


    mainLayout->addWidget(infoSectionContainerWidget);
    mainLayout->addWidget(IandCDivider);
    mainLayout->addWidget(scriptEngineContainer);
    mainLayout->addWidget(divider2);
    mainLayout->addWidget(statusSectionContainer);
    mainLayout->addItem(mainSpacer);

    receptionProcessTimer.setInterval(1000);
    receptionProcessTimer.setSingleShot(false);
    receptionProcessTimer.start();
    connect(&receptionProcessTimer,&QTimer::timeout,this,&ScriptReceptionRadioControl::onReceptionProcessTimerTick);

}

void ScriptReceptionRadioControl::onControlClose()
{
    receptionProcessTimer.stop();
    disconnect(&receptionProcessTimer,&QTimer::timeout,this,&ScriptReceptionRadioControl::onReceptionProcessTimerTick);

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

std::u16string ScriptReceptionRadioControl::stringToU16String(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> convert32;

    auto u32string = convert32.from_bytes(str);
    std::u16string u16str(u32string.begin(),u32string.end());
    return u16str;
}
