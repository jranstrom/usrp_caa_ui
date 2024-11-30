#ifndef CONTINOUSTRANSMISSIONRADIOCONTROL_H
#define CONTINOUSTRANSMISSIONRADIOCONTROL_H

#include <QWidget>
#include <qboxlayout.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include "indicatorbuttonwidget.h"
#include "labelandfieldwidget.h"
#include "radiocontrolbasewidget.h"

class continousTransmissionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    explicit continousTransmissionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    void onControlClose() override;

    std::string getType() override {return "cont-tx";}
    RadioControlWidget::RadioControlType getEnumType() override {return RadioControlWidget::RadioControlType::TxCONT;}

private slots:
    void onStartBtnRelease();
    void onStopBtnRelease();
    void onApplyBtnRelease();
    void onBrowseBtnRelease(){};

    void onFilepathFieldChanged(std::string value);

    void onTestBtnRelease();

    void onProcessTimerTick();


private:
    cRadioResponse readTxFile(std::string filepath);
    cRadioResponse readMatlabTxFile(std::string filepath="usrp_default_signals/random_bpsk_osr16.mat", std::string vpath="txSignal");
    cRadioResponse readDATtxFile(std::string filepath="");
    cRadioResponse readCSVtxFile(std::string filepath="");

    int transmissionStatus = 0;

    QVBoxLayout * mainLayout;

    QGridLayout * infoSectionLayout;
    LabelandFieldWidget * serialInfoField;
    LabelandFieldWidget * typeInfoField;
    LabelandFieldWidget * addressInfoField;

    QHBoxLayout * fileSectionLayout;
    IndicatorButtonWidget * fileLoadIndicator;
    LabelandFieldWidget * filepathField;
    QPushButton * browseBtn;
    QPushButton *  applyBtn;

    QHBoxLayout * statusSectionLayout;
    IndicatorButtonWidget * transmissionStatusIndicator;
    QLabel * transmissionStatusLabel;
    QSpacerItem * statusSectionSpacer;

    QTimer transmissionProcessTimer;


    QGridLayout * controlSectionLayout;
    QPushButton * startBtn;
    QPushButton * stopBtn;

    QPushButton * testBtn;

     QSpacerItem * mainSpacer;
};

#endif // CONTINOUSTRANSMISSIONRADIOCONTROL_H
