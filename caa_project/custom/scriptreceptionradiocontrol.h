#ifndef SCRIPTRECEPTIONRADIOCONTROL_H
#define SCRIPTRECEPTIONRADIOCONTROL_H

#include <QWidget>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include "indicatorbuttonwidget.h"
#include "labelandcheckboxwidget.h"
#include "labelandfieldwidget.h"
#include "labelandspinboxwidget.h"
#include "radiocontrolbasewidget.h"
#include "MatlabEngine.hpp"

class ScriptReceptionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    explicit ScriptReceptionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    void onControlClose() override;

    std::string getType() override {return "script-rx";}

private slots:
    void onToggleReceptionBtnRelease();
    void onRunScriptBtnRelease();
    void onConnectEngineBtnRelease();
    void onReceptionProcessTimerTick();
private:

    bool isReceiving = false;

    std::unique_ptr<matlab::engine::MATLABEngine> matlabPtr;
    bool engineConnected = false;
    std::u16string stringToU16String(const std::string &str);

    QVBoxLayout * mainLayout;

     QGridLayout * infoSectionLayout;
    LabelandFieldWidget * serialInfoField;
    LabelandFieldWidget * typeInfoField;
    LabelandFieldWidget * addressInfoField;

    LabelandFieldWidget * capturePathField;
    QPushButton * browsePathBtn;
    QLineEdit * captureModifierField;
    QLineEdit * captureExtentionField;
    QLabel * captureDotLabel;
    LabelandSpinBoxWidget * captureDelaySpinBox;

    QHBoxLayout * statusSectionLayout;
    QHBoxLayout * controlandCaptureLayout;
    QLabel * receptionStatusLabel;
    IndicatorButtonWidget * receptionStatusIndicator;

    QGridLayout * scriptEngineLayout;
    LabelandFieldWidget * scriptEngineField;
    LabelandFieldWidget * scriptNameField;
    IndicatorButtonWidget * engineConnectedIndidicator;
    QPushButton * engineConnectBtn;
    LabelandCheckboxWidget * passToEngineCheckBox;
    LabelandSpinBoxWidget * paramToEngineSpinBox;
    QPushButton * runScriptBtn;

    QVBoxLayout * controlSectionLayout;
    QPushButton * toggleReceptionBtn;
    QPushButton * saveCaptureBtn;
    LabelandSpinBoxWidget * fftLengthSpinBox;
    LabelandSpinBoxWidget * processTimerSpinBox;

    QTimer receptionProcessTimer;

    QSpacerItem * mainSpacer;
};

#endif // SCRIPTRECEPTIONRADIOCONTROL_H
