#ifndef CONTINOUSRECEPTIONRADIOCONTROL_H
#define CONTINOUSRECEPTIONRADIOCONTROL_H

#include "cradioobject.h"
#include <QWidget>
#include "labelandfieldwidget.h"
#include "qcustomplot.h"
#include "radiocontrolbasewidget.h"

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

private:

    bool isReceiving = false;

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

    QPushButton * testBtn;

    QSpacerItem * mainSpacer;


};

#endif // CONTINOUSRECEPTIONRADIOCONTROL_H
