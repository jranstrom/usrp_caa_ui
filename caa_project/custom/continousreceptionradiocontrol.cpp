#include "continousreceptionradiocontrol.h"
#include "cradioobject.h"
#include "labelandfieldwidget.h"

continousReceptionRadioControl::continousReceptionRadioControl(QWidget *parent, std::shared_ptr<cRadioObject> rad_p )
    : QWidget{parent}
{
    sourceRadio = rad_p;

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
            statusUpdate_m = "Transmission started";
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
            statusUpdate_m = "Transmission stopped";
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

}

void continousReceptionRadioControl::onTestBtnRelease()
{
    size_t N = 2048;
    std::vector<std::complex<short>> rxSamples = sourceRadio->getLastReceivedSamples(N);
    // Test to plot
    QVector<double> x(N),y(N);
    double max_val = -400;
    double min_val = 400;

    double y_r;
    double y_i;

    double nfactor =  std::pow(2, 15);

    if(rxSamples.empty() == false){
        for(int i =0;i<N;i++){
            y_r = static_cast<double>(std::real(rxSamples[i]))/nfactor;
            y_i = static_cast<double>(std::imag(rxSamples[i]))/nfactor;

            y[i] = 10*std::log10(std::pow(y_r,2)+std::pow(y_i,2));
            x[i] = i;

            if(max_val < y[i]){
                max_val = y[i];
            }

            if(min_val > y[i]){
                min_val = y[i];
            }
        }

        min_val = -100;
        max_val = 10;
        timePlot->addGraph();
        timePlot->graph(0)->setData(x, y);
        timePlot->xAxis->setLabel("Sample");
        timePlot->yAxis->setLabel("Amplitude");
        timePlot->yAxis->setRange(min_val,max_val);
        timePlot->xAxis->setRange(0,N);
        timePlot->replot();
    }
}

void continousReceptionRadioControl::closeEvent(QCloseEvent *event)
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

    emit controlClosed();
}
