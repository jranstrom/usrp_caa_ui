#include "continoustransmissionradiocontrol.h"
#include "matHF.h"
#include <filesystem>
#include "uhd_clib.h"

continousTransmissionRadioControl::continousTransmissionRadioControl(QWidget *parent, std::shared_ptr<cRadioObject> rad_p)
    : RadioControlBaseWidget(parent, rad_p)
{
    std::string windowTitle = sourceRadio->getSerial() + " - Continous Transmission";
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



    // Status Section
    QWidget * statusSectionContainerWidget = new QWidget;
    statusSectionContainerWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    statusSectionLayout = new QHBoxLayout(statusSectionContainerWidget);
    transmissionStatusIndicator = new IndicatorButtonWidget(this);
    transmissionStatusLabel = new QLabel("n/a");
    statusSectionSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Minimum);
    statusSectionLayout->addWidget(transmissionStatusIndicator);
    statusSectionLayout->addWidget(transmissionStatusLabel);
    statusSectionLayout->addItem(statusSectionSpacer);

    // File Section
    QWidget * fileSectionContainerWidget = new QWidget;
    fileSectionLayout = new QHBoxLayout(fileSectionContainerWidget);
    fileLoadIndicator = new IndicatorButtonWidget(this);
    fileLoadIndicator->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    filepathField = new LabelandFieldWidget(this,"filepath","usrp_default_signals/random_bpsk_osr16.mat",true);
    filepathField->setMinimumWidth(400);
    filepathField->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    connect(filepathField,&LabelandFieldWidget::fieldTextEditing,this,&continousTransmissionRadioControl::onFilepathFieldChanged);


    browseBtn = new QPushButton("Browse",this);
    browseBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    applyBtn = new QPushButton("Apply",this);
    applyBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(applyBtn,&QPushButton::released,this,&continousTransmissionRadioControl::onApplyBtnRelease);

    fileSectionLayout->addWidget(fileLoadIndicator);
    fileSectionLayout->addWidget(filepathField);
    fileSectionLayout->addWidget(browseBtn);
    fileSectionLayout->addWidget(applyBtn);



    // Control Section
    QWidget * controlSectionContainerWidget = new QWidget;
    controlSectionLayout = new QGridLayout(controlSectionContainerWidget);
    startBtn = new QPushButton("Start",this);
    connect(startBtn,&QPushButton::released,this,&continousTransmissionRadioControl::onStartBtnRelease);
    controlSectionLayout->addWidget(startBtn,0,0);

    stopBtn = new QPushButton("Stop",this);
    connect(stopBtn,&QPushButton::released,this,&continousTransmissionRadioControl::onStopBtnRelease);
    controlSectionLayout->addWidget(stopBtn,0,1);

    // Main layout
    testBtn = new QPushButton("Test",this);
    connect(testBtn,&QPushButton::released,this,&continousTransmissionRadioControl::onTestBtnRelease);

    mainLayout->addWidget(infoSectionContainerWidget);
    mainLayout->addWidget(fileSectionContainerWidget);
    mainLayout->addWidget(statusSectionContainerWidget);
    mainLayout->addWidget(controlSectionContainerWidget);
    mainLayout->addWidget(testBtn);

    mainLayout->addItem(mainSpacer);

    transmissionProcessTimer.setInterval(500);
    transmissionProcessTimer.setSingleShot(false);
    transmissionProcessTimer.start();
    connect(&transmissionProcessTimer,&QTimer::timeout,this,&continousTransmissionRadioControl::onProcessTimerTick);

}

void continousTransmissionRadioControl::onControlClose()
{

    disconnect(&transmissionProcessTimer,&QTimer::timeout,this,&continousTransmissionRadioControl::onProcessTimerTick);

    if(transmissionStatus != 1){
        return;
    }
    cRadioResponse response = sourceRadio->stopContinousTransmission();
    if(response.code == 0){
        transmissionStatus = -1;
        emit statusUpdateRequest("Transmission terminated",0);
    }else{
        emit statusUpdateRequest(response.message,-1);
    }
}

void continousTransmissionRadioControl::onStartBtnRelease()
{
    cRadioResponse response = readMatlabTxFile();

    if(response.code == 0){
        response = sourceRadio->startContinousTransmission();

        if(response.code == 0){
            emit statusUpdateRequest("Transmission started",0);
        }else{
            emit statusUpdateRequest(response.message,-1);
        }
    }else{
        emit statusUpdateRequest(response.message,-1);
    }
}

void continousTransmissionRadioControl::onStopBtnRelease()
{
    cRadioResponse response = sourceRadio->stopContinousTransmission();

    if(response.code != 0){
        emit statusUpdateRequest(response.message,0);
    }


    //disconnect(&transmissionProcessTimer,&QTimer::timeout,this,&continousTransmissionRadioControl::onProcessTimerTick);

}

void continousTransmissionRadioControl::onApplyBtnRelease()
{
    if(transmissionStatus == 1){
        emit statusUpdateRequest("Could not apply tx signal, transmission in progress.",-1);
        return;
    }
    cRadioResponse response = readTxFile(filepathField->getFieldText());
    if(response.code == 0){
        filepathField->saveCurrentValue();
        fileLoadIndicator->setState(2); // green - loaded and applied
    }else{
        emit statusUpdateRequest(response.message,-1);
    }
}

void continousTransmissionRadioControl::onFilepathFieldChanged(std::string value)
{
    LabelandFieldWidget * cobj = static_cast<LabelandFieldWidget*>(sender());
    if(cobj->isSaved()){
        fileLoadIndicator->setState(2); // green - loaded and applied
    }else{
        fileLoadIndicator->setState(3); // yellow - not applied
    }
}

void continousTransmissionRadioControl::onTestBtnRelease()
{


}

void continousTransmissionRadioControl::onProcessTimerTick()
{
    int cStatus = sourceRadio->continous_transmission_running ? 1 : -1;
    if(cStatus != transmissionStatus){
        if(cStatus == -1){
            transmissionStatusIndicator->setState(1);
            transmissionStatusLabel->setText("Transmission terminated");
           // transmissionProcessTimer.stop();
        }else{

            transmissionStatusLabel->setText("Transmission in progress");
            transmissionStatusIndicator->setState(2);
        }

        transmissionStatus = cStatus;
    }
}

cRadioResponse continousTransmissionRadioControl::readTxFile(std::string filepath)
{
    cRadioResponse response;
    response.message = "Success";
    response.code =  0;

    if(std::filesystem::exists(filepath) == false){
        response.message = "Could not load tx signal. File: " + filepath + " does not exist.";
        response.code = -1;
        return response;
    }

    std::string fileExtension = std::filesystem::path(filepath).extension().string();

    if(fileExtension == ".mat"){
        response = readMatlabTxFile(filepath,"txSignal");
        if(response.code == 0){
            response.message = "Success loading file " + filepath;
        }
    }else{
        response.message = "Could not load file; No support for desired file type " + fileExtension;
        response.code = -1;
    }

    return response;
}

cRadioResponse continousTransmissionRadioControl::readMatlabTxFile(std::string filepath, std::string vpath)
{
    cRadioResponse response;
    response.message = "Success";
    response.code =  0;

    if(std::filesystem::exists(filepath) == false){
        response.message = "Could not load tx signal. File: " + filepath + " does not exist.";
        response.code = -1;
        return response;
    }

    std::vector<std::complex<double>> txSig;
    try{
        txSig = matHF::read_complex_double_vec(filepath,vpath);
    }catch(...){
        response.message = "Reading " + vpath + " from " + filepath;
        response.code = -2;
        return response;
    }

    std::vector<std::complex<short>> txSig_short = uhd_clib::cvec_conv_double2short(txSig);
    response = sourceRadio->setTransmitSignal(txSig_short);

    return response;
}
