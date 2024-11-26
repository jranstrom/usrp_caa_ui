#include "scriptreceptionradiocontrol.h"


ScriptReceptionRadioControl::ScriptReceptionRadioControl(QWidget *parent, std::shared_ptr<cRadioObject> rad_p)
    : RadioControlBaseWidget(parent, rad_p){

    std::string windowTitle = sourceRadio->getSerial() + " - Script Reception";
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

    mainSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);


    mainLayout->addWidget(infoSectionContainerWidget);
    mainLayout->addItem(mainSpacer);

}
