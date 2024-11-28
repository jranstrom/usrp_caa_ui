#ifndef CONTINOUSRECEPTIONRADIOCONTROL_H
#define CONTINOUSRECEPTIONRADIOCONTROL_H

#include "cradioobject.h"
#include <QWidget>
#include "indicatorbuttonwidget.h"
#include "labelandcheckboxwidget.h"
#include "labelandfieldwidget.h"
#include "labelandspinboxwidget.h"
#include "qcustomplot.h"
#include "radiocontrolbasewidget.h"
#include <customfftwobject.h>

class continousReceptionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    explicit continousReceptionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    void onControlClose() override;

    std::string getType() override {return "cont-rx";}


private slots:
    void onToggleReceptionBtnRelease();
    void onCaptureBtnRelease();
    void onTestBtnRelease();

    void onProcessTimerTick();
    void onProcessTimerIntervalChange(int value, bool silent);

private:

    void plotAll();
    void saveCapture();

    std::atomic<bool> isProcessing = false;

    cRadioResponse saveDATfile(std::string filepath,int dataLength);
    cRadioResponse saveCSVfile(std::string filepath, int dataLength);

    double timePlot_max_val = -400;
    double timePlot_min_val = 400;

    double frequencyPlot_max_val = -400;
    double frequencyPlot_min_val = 400;

    bool isReceiving = false;

    std::shared_ptr<customFFTWObject> cfftw = std::make_shared<customFFTWObject>();

    QVBoxLayout * mainLayout;

    QGridLayout * infoSectionLayout;
    LabelandFieldWidget * serialInfoField;
    LabelandFieldWidget * typeInfoField;
    LabelandFieldWidget * addressInfoField;

    QHBoxLayout * controlandCaptureLayout;

    QVBoxLayout * controlSectionLayout;
    QPushButton * toggleReceptionBtn;
    QPushButton * saveCaptureBtn;
    LabelandSpinBoxWidget * fftLengthSpinBox;
    LabelandSpinBoxWidget * processTimerSpinBox;

    QVBoxLayout * captureSectionLayout;

    LabelandFieldWidget * capturePathField;
    QPushButton * browsePathBtn;
    QLineEdit * captureModifierField;
    QLineEdit * captureExtentionField;
    QLabel * captureDotLabel;
    LabelandSpinBoxWidget * captureLengthSpinBox;
    LabelandCheckboxWidget * captureAutoIncrement;



    QHBoxLayout * statusSectionLayout;
    IndicatorButtonWidget * receptionStatusIndicator;
    QLabel * receptionStatusLabel;
    QSpacerItem * statusSectionSpacer;

    QGridLayout * plotSectionLayout;
    QCustomPlot * timePlot;
    QLabel * timePlotTitleLabel;

    QCustomPlot * frequencyPlot;
    QLabel * frequencyPlotTitleLabel;

    QTimer receptionProcessTimer;

    QPushButton * testBtn;

    QSpacerItem * mainSpacer;


};

#endif // CONTINOUSRECEPTIONRADIOCONTROL_H
