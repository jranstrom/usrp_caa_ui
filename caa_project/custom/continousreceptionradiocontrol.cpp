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

    // Control Section
    QWidget * controlSectionContainerWidget = new QWidget;
    controlSectionLayout = new QGridLayout(controlSectionContainerWidget);

    toggleReceptionBtn = new QPushButton("Start",this);
    connect(toggleReceptionBtn,&QPushButton::released,this,&continousReceptionRadioControl::onToggleReceptionBtnRelease);

    saveCaptureBtn = new QPushButton("Capture",this);

    controlSectionLayout->addWidget(saveCaptureBtn,0,1);
    controlSectionLayout->addWidget(toggleReceptionBtn,0,0);

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
    mainLayout->addWidget(controlSectionContainerWidget);
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
            isReceiving = true;
            toggleReceptionBtn->setText("Stop");
            statusUpdate_m = "Reception started";
            statusUpdateCode = 0;
        }else{
            statusUpdate_m = response.message;
            statusUpdateCode = -1;
        }

    }else{
        response = sourceRadio->stopContinousReception();
        if(response.code == 0){
            isReceiving = false;
            toggleReceptionBtn->setText("Start");
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
        cRadioResponse response = sourceRadio->stopContinousReception();
        if(response.code == 0){
            emit statusUpdateRequest("Transmission stopped",-1);
        }else{
            emit statusUpdateRequest(response.message,-1);
        }
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
        return;
    }

    if(isReceiving == true){
        plotAll();
    }


}

void continousReceptionRadioControl::plotAll()
{
    size_t N = 2048;
    std::vector<std::complex<short>> rxSamples = sourceRadio->getLastReceivedSamples(N);
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
