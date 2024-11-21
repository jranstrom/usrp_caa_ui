#ifndef CONTINOUSRECEPTIONRADIOCONTROL_H
#define CONTINOUSRECEPTIONRADIOCONTROL_H

#include "cradioobject.h"
#include <QWidget>
#include "labelandfieldwidget.h"
#include "qcustomplot.h"

class continousReceptionRadioControl : public QWidget
{
    Q_OBJECT
public:
    explicit continousReceptionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    std::string getSerial(){return sourceRadio->getSerial();}

private slots:
    void onToggleReceptionBtnRelease();
    void onCaptureBtnRelease();
    void onTestBtnRelease();

private:
    std::shared_ptr<cRadioObject> sourceRadio;

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

signals:
    void controlClosed();
    void statusUpdateRequest(std::string message,int code);

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // CONTINOUSRECEPTIONRADIOCONTROL_H
