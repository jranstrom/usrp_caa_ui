#ifndef SCRIPTRECEPTIONRADIOCONTROL_H
#define SCRIPTRECEPTIONRADIOCONTROL_H

#include <QWidget>
#include <qboxlayout.h>
#include "labelandfieldwidget.h"
#include "radiocontrolbasewidget.h"

class ScriptReceptionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    explicit ScriptReceptionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    void onControlClose() override {};

    std::string getType() override {return "script-rx";}

private:

    QVBoxLayout * mainLayout;

    QGridLayout * infoSectionLayout;
    LabelandFieldWidget * serialInfoField;
    LabelandFieldWidget * typeInfoField;
    LabelandFieldWidget * addressInfoField;

    QSpacerItem * mainSpacer;
};

#endif // SCRIPTRECEPTIONRADIOCONTROL_H
