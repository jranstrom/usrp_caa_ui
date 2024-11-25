#ifndef CONTINOUSRECEPTIONRADIOCONTROL_H
#define CONTINOUSRECEPTIONRADIOCONTROL_H

#include "cradioobject.h"
#include <QWidget>
#include "indicatorbuttonwidget.h"
#include "labelandfieldwidget.h"
#include "qcustomplot.h"
#include "radiocontrolbasewidget.h"
#include <customfftwobject.h>

class continousReceptionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    explicit continousReceptionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    void onControlClose() override;


private slots:
    void onToggleReceptionBtnRelease();
    void onCaptureBtnRelease();
    void onTestBtnRelease();

    void onProcessTimerTick();

private:

    void plotAll();

    bool isReceiving = false;

    std::shared_ptr<customFFTWObject> cfftw = std::make_shared<customFFTWObject>();

    QVBoxLayout * mainLayout;

    QGridLayout * infoSectionLayout;
    LabelandFieldWidget * serialInfoField;
    LabelandFieldWidget * typeInfoField;
    LabelandFieldWidget * addressInfoField;

    QGridLayout * controlSectionLayout;
    QPushButton * toggleReceptionBtn;
    QPushButton * saveCaptureBtn;

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
