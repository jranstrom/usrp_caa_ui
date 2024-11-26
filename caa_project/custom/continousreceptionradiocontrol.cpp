#include "continousreceptionradiocontrol.h"
#include "cradioobject.h"
#include "labelandfieldwidget.h"
#include "matHF.h"
#include "uhd_clib.h"

continousReceptionRadioControl::continousReceptionRadioControl(QWidget *parent, std::shared_ptr<cRadioObject> rad_p )
    : RadioControlBaseWidget(parent,rad_p)
{

    std::string windowTitle = sourceRadio->getSerial() + " - Continous Reception";
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

    // Control & Capture Section
    QWidget * controlandCaptureContainer = new QWidget;
    controlandCaptureLayout = new QHBoxLayout(controlandCaptureContainer);

    // Control Section IN Control & Capture Section
    QWidget * controlSectionContainer = new QWidget;
    controlSectionContainer->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    controlSectionLayout = new QVBoxLayout(controlSectionContainer);

    QWidget * statusSectionContainer = new QWidget;
    statusSectionContainer->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    statusSectionLayout = new QHBoxLayout(statusSectionContainer);
    receptionStatusIndicator = new IndicatorButtonWidget(this);
    receptionStatusLabel = new QLabel("No ongoing reception",this);
    toggleReceptionBtn = new QPushButton("Start",this);
    connect(toggleReceptionBtn,&QPushButton::released,this,&continousReceptionRadioControl::onToggleReceptionBtnRelease);

    saveCaptureBtn = new QPushButton("Capture",this);
    connect(saveCaptureBtn,&QPushButton::released,this,&continousReceptionRadioControl::onCaptureBtnRelease);

    statusSectionLayout->addWidget(receptionStatusIndicator);
    statusSectionLayout->addWidget(receptionStatusLabel);
    statusSectionLayout->addWidget(toggleReceptionBtn);

    // Plot Control Section IN Control Section
    QWidget * plotControlSectionContainer = new QWidget;
    QGridLayout * plotControlSectionLayout = new QGridLayout(plotControlSectionContainer);

    fftLengthSpinBox = new LabelandSpinBoxWidget(this,"FFT length:",32768,32,1024);
    fftLengthSpinBox->requestSetValue(1024,false);

    processTimerSpinBox = new LabelandSpinBoxWidget(this,"Plot Update (ms):",5000,100,1000);
    processTimerSpinBox->requestSetValue(1000,false);
    connect(processTimerSpinBox,&LabelandSpinBoxWidget::componentValueChanged,this,&continousReceptionRadioControl::onProcessTimerIntervalChange);

    plotControlSectionLayout->addWidget(fftLengthSpinBox,0,0);
    plotControlSectionLayout->addWidget(processTimerSpinBox,0,1);

    controlSectionLayout->addWidget(statusSectionContainer);
    controlSectionLayout->addWidget(plotControlSectionContainer);


    // Capture Section
    QWidget * captureSectionContainer = new QWidget;
    captureSectionLayout = new QVBoxLayout(captureSectionContainer);

    QWidget * captureFileSectionContainer = new QWidget;
    QHBoxLayout * captureFileLayout = new QHBoxLayout(captureFileSectionContainer);
    capturePathField = new LabelandFieldWidget(this,"Filepath:","data/cont_capture/capture_",true);
    capturePathField->setFieldAlignment(Qt::AlignRight);
    captureModifierField = new QLineEdit("0",this);
    captureModifierField->setAlignment(Qt::AlignCenter);
    captureModifierField->setMaximumWidth(30);
    captureModifierField->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    captureDotLabel = new QLabel(".");
    captureDotLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    captureExtentionField = new QLineEdit("dat",this);
    captureExtentionField->setMaximumWidth(40);
    captureExtentionField->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    browsePathBtn = new QPushButton("browser",this);
    browsePathBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    captureFileLayout->addWidget(capturePathField);
    captureFileLayout->addWidget(captureModifierField);
    captureFileLayout->addWidget(captureDotLabel);
    captureFileLayout->addWidget(captureExtentionField);
    captureFileLayout->addWidget(browsePathBtn);

    captureLengthSpinBox = new LabelandSpinBoxWidget(this,"Length",5e5,1,1000);
    captureLengthSpinBox->requestSetValue(1000,false);
    captureLengthSpinBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    captureSectionLayout->addWidget(captureFileSectionContainer);
    captureSectionLayout->addWidget(captureLengthSpinBox);
    captureSectionLayout->addWidget(saveCaptureBtn);

    QFrame * CandCDivider = new QFrame();CandCDivider->setFrameShape(QFrame::VLine);CandCDivider->setFrameShadow(QFrame::Sunken);
    controlandCaptureLayout->addWidget(controlSectionContainer);
    controlandCaptureLayout->addWidget(CandCDivider);
    controlandCaptureLayout->addWidget(captureSectionContainer);


    // Plot Section
    QWidget * plotSectionContainerWidget = new QWidget;
    plotSectionContainerWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    plotSectionLayout = new QGridLayout(plotSectionContainerWidget);

    // timePlotTitleLabel = new QLabel("Time Plot",this);
    // plotSectionLayout->addWidget(timePlotTitleLabel,0,0,Qt::AlignCenter);

    // frequencyPlotTitleLabel = new QLabel("Frequency Spectrum",this);
    // plotSectionLayout->addWidget(frequencyPlotTitleLabel,0,1,Qt::AlignCenter);

    timePlot = new QCustomPlot(this);
    timePlot->setMinimumSize(300,300);
    plotSectionLayout->addWidget(timePlot,0,0);

    frequencyPlot = new QCustomPlot(this);
    frequencyPlot->setMinimumSize(300,300);
    plotSectionLayout->addWidget(frequencyPlot,0,1);

    // Main layout
    testBtn = new QPushButton("Test",this);
    connect(testBtn,&QPushButton::released,this,&continousReceptionRadioControl::onTestBtnRelease);

    mainSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);

    mainLayout->addWidget(infoSectionContainerWidget);
    mainLayout->addWidget(controlandCaptureContainer);
    mainLayout->addWidget(plotSectionContainerWidget);
    mainLayout->addWidget(testBtn);
    //mainLayout->addItem(mainSpacer);

    receptionProcessTimer.setInterval(1000);
    receptionProcessTimer.setSingleShot(false);
    receptionProcessTimer.start();
    connect(&receptionProcessTimer,&QTimer::timeout,this,&continousReceptionRadioControl::onProcessTimerTick);

}

void continousReceptionRadioControl::onControlClose()
{
    receptionProcessTimer.stop();
    disconnect(&receptionProcessTimer,&QTimer::timeout,this,&continousReceptionRadioControl::onProcessTimerTick);

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

void continousReceptionRadioControl::onToggleReceptionBtnRelease()
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

void continousReceptionRadioControl::onCaptureBtnRelease()
{
    // Terminate reception...
    if(isReceiving){
        saveCapture();
    }else{
        emit statusUpdateRequest("Reception must be running",-1);
    }
}

void continousReceptionRadioControl::onTestBtnRelease()
{

}

void continousReceptionRadioControl::onProcessTimerTick()
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

    if(isReceiving == true){
        plotAll();
    }


}

void continousReceptionRadioControl::onProcessTimerIntervalChange(int value, bool silent)
{
    receptionProcessTimer.stop();
    receptionProcessTimer.setInterval(value);
    receptionProcessTimer.start();
}

void continousReceptionRadioControl::plotAll()
{
    size_t N = static_cast<size_t>(fftLengthSpinBox->getValue());

    cRadioResponse response;
    response.message = "Success";
    response.code = 0;

    std::vector<std::complex<short>> rxSamples = sourceRadio->getLastReceivedSamples(N,response);
    std::vector<std::complex<double>> rxSamples_double(N);
    // Test to plot
    QVector<double> x(N),y(N);

    double sample_rate = (sourceRadio->getAppliedConfiguation())["rx-sampling-rate"]->getPropertyValueDouble();
    sample_rate = sample_rate/1e3;

    double y_r;
    double y_i;

    double nfactor =  std::pow(2, 15);

    if(rxSamples.empty() == false){
        for(int i =0;i<N;i++){
            y_r = static_cast<double>(std::real(rxSamples[i]))/nfactor;
            y_i = static_cast<double>(std::imag(rxSamples[i]))/nfactor;

            rxSamples_double[i] = std::complex<double>(y_r,y_i);

            y[i] = 10*std::log10(std::pow(y_r,2)+std::pow(y_i,2));
            x[i] = i;

            if(timePlot_max_val < y[i]){
                timePlot_max_val = y[i];
            }

            if(timePlot_min_val > y[i] && std::isinf(y[i]) == false){
                timePlot_min_val = y[i];
            }
        }

        timePlot->addGraph();
        timePlot->graph(0)->setData(x, y);
        timePlot->xAxis->setLabel("Sample");
        timePlot->yAxis->setLabel("Amplitude");
        timePlot->yAxis->setRange(timePlot_min_val-5,timePlot_max_val+5);
        timePlot->xAxis->setRange(0,N);
        timePlot->replot();
    }

    double Y_r;
    double Y_i;
    QVector<double> X(N),Y(N);
    std::vector<double> Y_t(N);

    rxSamples_double = cfftw->fft_w_zpadd(rxSamples_double,N,false);
    for(int i =0;i<N;i++){
        Y_r = std::real(rxSamples_double[i]);
        Y_i = std::imag(rxSamples_double[i]);


        Y_t[i] = 10*std::log10(std::pow(Y_r,2)+std::pow(Y_i,2));
        X[i] = ((static_cast<double>(i))/N-0.5)*sample_rate;;

        if(frequencyPlot_max_val < Y_t[i]){
            frequencyPlot_max_val = Y_t[i];
        }

        if(frequencyPlot_min_val > Y_t[i] && std::isinf(Y_t[i]) == false){
            frequencyPlot_min_val = Y_t[i];
        }
    }

    uhd_clib::fft_shift(Y_t);

    //Y = QVector::from
    Y = QVector<double>(Y_t.begin(),Y_t.end());

    frequencyPlot->addGraph();
    frequencyPlot->graph(0)->setData(X,Y);
    frequencyPlot->yAxis->setRange(frequencyPlot_min_val-5,frequencyPlot_max_val+10);
    frequencyPlot->xAxis->setRange(X[0],X[N-1]);
    frequencyPlot->replot();
}

void continousReceptionRadioControl::saveCapture()
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

    if(fileExtension == "dat"){
        response = saveDATfile(combinedPath,captureLengthSpinBox->getValue());
    }else if(fileExtension == "csv"){
        response = saveCSVfile(combinedPath,captureLengthSpinBox->getValue());
    }

    if(response.code == 0){
        response.message = "Success saving capture to file " + combinedPath;
    }

    emit statusUpdateRequest(response.message,response.code);

}

cRadioResponse continousReceptionRadioControl::saveDATfile(std::string filepath, int dataLength)
{
    cRadioResponse response;
    response.message = "Success";
    response.code = 0;

    std::vector<std::complex<short>> rxSamples;

    do{
        rxSamples = sourceRadio->getLastReceivedSamples(dataLength,response);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }while(response.code == -3);

    if(response.code < 0){
        response.code = -1;
        return response;
    }

    std::ofstream outfile;

    outfile.open(filepath.c_str(),std::ofstream::binary);
    std::complex<short> * buffer;

    try{
        buffer = new std::complex<short>[dataLength];
    }catch(std::bad_alloc& exc){
        response.message = "Could not allocate for buffer";
        response.code = -1;
    }

    for(int i=0; i< dataLength;++i){
        buffer[i] = rxSamples[i];
    }

    if(outfile.is_open()){
        outfile.write((const char *) buffer,dataLength*sizeof(std::complex<short>));
    }else{
        response.message = "Could not open file " + filepath;
        response.code = -1;
    }
    delete[] buffer;

    return response;
}

cRadioResponse continousReceptionRadioControl::saveCSVfile(std::string filepath, int dataLength)
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
