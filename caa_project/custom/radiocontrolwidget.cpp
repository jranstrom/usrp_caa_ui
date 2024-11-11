#include "radiocontrolwidget.h"

RadioControlWidget::RadioControlWidget(QWidget *parent, cRadioObject *cRad_p)
{
    radConfig = cRad_p->getConfiguration(); // copy object

    serial = cRad_p->getSerial();
    type = cRad_p->getType();
    address = cRad_p->getAddress();

    uType = type;
    transform(uType.begin(), uType.end(), uType.begin(), ::toupper);

    outerContainerLayout = new QHBoxLayout(this);

    mainGroupBox = new QGroupBox("");
    mainGroupBox->setTitle(QString::fromStdString(uType + " - " + serial));
    mainGroupBox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    mainGroupBoxLayout = new QVBoxLayout(mainGroupBox);

    QWidget *basicInfoWidget = new QWidget;
    basicInfoLayout = new QVBoxLayout(basicInfoWidget);
    basicInfoLayout->setSizeConstraint(QHBoxLayout::SizeConstraint::SetMinimumSize);

    mainGroupBoxLayout->addWidget(basicInfoWidget);

    serialField = new LabelandFieldWidget(this,"Serial:",serial);
    basicInfoLayout->addWidget(serialField);
    serialField->setEnabled(false);

    typeField = new LabelandFieldWidget(this,"Type:",type);
    basicInfoLayout->addWidget(typeField);
    typeField->setEnabled(false);

    addressField = new LabelandFieldWidget(this,"Address:",address);
    basicInfoLayout->addWidget(addressField);
    addressField->setEnabled(false);

    basicInfoDividerLine = new QFrame();
    basicInfoDividerLine->setFrameShape(QFrame::HLine);
    basicInfoDividerLine->setFrameShadow(QFrame::Sunken);

    mainGroupBoxLayout->addWidget(basicInfoDividerLine);

    QWidget *configurationStatusWidget = new QWidget;
    configurationStatusLayout = new QHBoxLayout(configurationStatusWidget);
    configurationStatusLayout->setSizeConstraint(QHBoxLayout::SizeConstraint::SetMinimumSize);
    mainGroupBoxLayout->addWidget(configurationStatusWidget);

    configurationStatusIndicator = new IndicatorButtonWidget(this);
    configurationStatusLabel = new QLabel("No configuration",this);

    QFrame * confVDivider = new QFrame();
    confVDivider->setFrameShape(QFrame::VLine);
    confVDivider->setFrameShadow(QFrame::Sunken);

    loadDefaultConfigurationBtn = new QPushButton("Default",this);
    connect(loadDefaultConfigurationBtn,&QPushButton::released,this,&RadioControlWidget::onLoadDefaultConfigurationBtnRelease);

    QSpacerItem * hSpacerConfStatus = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Minimum);

    configurationStatusLayout->addWidget(configurationStatusIndicator);
    configurationStatusLayout->addWidget(configurationStatusLabel);
    configurationStatusLayout->addWidget(confVDivider);
    configurationStatusLayout->addWidget(loadDefaultConfigurationBtn);
    configurationStatusLayout->addItem(hSpacerConfStatus);

    QWidget *configurationFileWidget = new QWidget;
    configurationFileLayout = new QHBoxLayout(configurationFileWidget);
    configurationFileLayout->setSizeConstraint(QHBoxLayout::SizeConstraint::SetMinimumSize);
    mainGroupBoxLayout->addWidget(configurationFileWidget);

    fileConfigurationField = new LabelandFieldWidget(this,"Filepath:","",true);
    loadFileConfigurationBtn = new QPushButton("Load",this);
    configurationFileLayout->addWidget(fileConfigurationField);
    configurationFileLayout->addWidget(loadFileConfigurationBtn);

    vSpacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);

    mainGroupBoxLayout->addItem(vSpacer);

    outerContainerLayout->addWidget(mainGroupBox);
}

void RadioControlWidget::pushRadioConfiguration(cRadioConfiguration radConfig_p, int configType)
{
    // configType 0 - default; 1 - from file; 2 - not specified
    radConfig = radConfig_p;
    std::string configurationStatusString_t = "Unknown configuration loaded...";
    int indicatorState = 3;
    switch(configType){
    case 0:
        configurationStatusString_t = "Default " + uType + " configuration";
        indicatorState = 2;
        break;
    case 1:
        configurationStatusString_t = "Configuration from file loaded...";
        indicatorState = 2;
     break;
    }

    configurationStatusLabel->setText(QString::fromStdString(configurationStatusString_t));
    configurationStatusIndicator->setState(indicatorState);

    // Update all fields and controls
}

void RadioControlWidget::onLoadDefaultConfigurationBtnRelease()
{
    emit loadDefaultConfigurationRequest(serial,false);
}
