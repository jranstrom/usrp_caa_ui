#include "radiocontrolwidget.h"
#include <qheaderview.h>

RadioControlWidget::RadioControlWidget(QWidget *parent, std::shared_ptr<cRadioObject> cRad_p)
{
    radConfig = cRad_p->getConfiguration(); // copy object

    serial = cRad_p->getSerial();
    type = cRad_p->getType();
    address = cRad_p->getAddress();

    uType = type;
    transform(uType.begin(), uType.end(), uType.begin(), ::toupper);

    outerContainerLayout = new QVBoxLayout(this);
    outerContainerLayout->setSizeConstraint(QVBoxLayout::SetMinAndMaxSize);

    mainGroupBox = new QGroupBox("");
    mainGroupBox->setTitle(QString::fromStdString(uType + " - " + serial));
    mainGroupBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    mainGroupBoxLayout = new QVBoxLayout(mainGroupBox);
    mainGroupBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget *basicInfoWidget = new QWidget;
    basicInfoLayout = new QVBoxLayout(basicInfoWidget);
   // basicInfoLayout->setSizeConstraint(QHBoxLayout::SizeConstraint::SetMinimumSize);

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

    QWidget *configurationStatusWidget = new QWidget;
    configurationStatusLayout = new QHBoxLayout(configurationStatusWidget);
    configurationStatusWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    //configurationStatusLayout->setSizeConstraint(QHBoxLayout::SizeConstraint::SetMinimumSize);

    configurationStatusIndicator = new IndicatorButtonWidget(this);
    configurationStatusLabel = new QLabel("No configuration",this);

    QFrame * confVDivider = new QFrame();
    confVDivider->setFrameShape(QFrame::VLine);
    confVDivider->setFrameShadow(QFrame::Sunken);

    loadDefaultConfigurationBtn = new QPushButton("Default",this);
    connect(loadDefaultConfigurationBtn,&QPushButton::released,this,&RadioControlWidget::onLoadDefaultConfigurationBtnRelease);

    applyConfigurationBtn = new QPushButton("Apply",this);
    connect(applyConfigurationBtn,&QPushButton::released,this,&RadioControlWidget::onApplyConfigurationBtnRelease);

    QSpacerItem * hSpacerConfStatus = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Minimum);

    configurationStatusLayout->addWidget(configurationStatusIndicator);
    configurationStatusLayout->addWidget(configurationStatusLabel);
    configurationStatusLayout->addWidget(confVDivider);
    configurationStatusLayout->addWidget(loadDefaultConfigurationBtn);
    configurationStatusLayout->addWidget(applyConfigurationBtn);
    configurationStatusLayout->addItem(hSpacerConfStatus);

    QWidget *configurationFileWidget = new QWidget;
    configurationFileLayout = new QHBoxLayout(configurationFileWidget);
    configurationFileWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    //configurationFileLayout->setSizeConstraint(QHBoxLayout::SizeConstraint::SetMinimumSize);


    fileConfigurationField = new LabelandFieldWidget(this,"Filepath:","",true);
    loadFileConfigurationBtn = new QPushButton("Load",this);
    connect(loadFileConfigurationBtn,&QPushButton::released,this,&RadioControlWidget::onLoadFileConfigurationBtnRelease);

    configurationFileLayout->addWidget(fileConfigurationField);
    configurationFileLayout->addWidget(loadFileConfigurationBtn);

    QWidget * testWidget = new QWidget;
    testSectionLayout = new QHBoxLayout(testWidget);

    testSectionPushBtn = new QPushButton("Test",this);
    connect(testSectionPushBtn,&QPushButton::released,this,&RadioControlWidget::onTestBtnRelease);
    testSectionLayout->addWidget(testSectionPushBtn);

   // mainGroupBoxLayout->addWidget(testWidget);

    QWidget * configurationTableContainerWidget = new QWidget;
    configurationTableWidget = new QTableWidget(0,3,this);
    configurationTableWidget->setMinimumWidth(100);

    configurationTableWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    configurationTableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Type" << "Value");

    QFont headerFont = configurationTableWidget->horizontalHeader()->font();
    headerFont.setPointSize(10);
    configurationTableWidget->horizontalHeader()->setFont(headerFont);

    configurationTableWidget->setColumnWidth(1, 30);
    configurationTableWidget->setColumnWidth(2, 110);

    configurationTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    configurationTableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);

    int headerHeigth = configurationTableWidget->horizontalHeader()->height();
    configurationTableWidget->setMinimumHeight(headerHeigth+2);
    configurationTableWidget->setMaximumHeight(headerHeigth+2);

    QFont font = configurationTableWidget->font();  // Get the current font of the table
    font.setPointSize(9);
    configurationTableWidget->setFont(font);

    QAbstractItemModel* model = configurationTableWidget->model();
    connect(model,&QAbstractTableModel::rowsInserted,this,&RadioControlWidget::onConfigurationTableItemChanged);

    vSpacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Expanding);

    //mainGroupBoxLayout->addItem(vSpacer);
    //mainGroupBoxLayout->addStretch(1);

    mainGroupBoxLayout->addWidget(basicInfoWidget);

    mainGroupBoxLayout->addWidget(basicInfoDividerLine);
    mainGroupBoxLayout->addWidget(configurationTableWidget);
    mainGroupBoxLayout->addWidget(configurationStatusWidget);

    mainGroupBoxLayout->addWidget(configurationTableContainerWidget);
    mainGroupBoxLayout->addWidget(configurationFileWidget);
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
        configurationStatusString_t = "Default configuration loaded";
        indicatorState = 3;
        break;
    case 1:
        configurationStatusString_t = "File configuration loaded";
        indicatorState = 3;
     break;
    }

    configurationStatusLabel->setText(QString::fromStdString(configurationStatusString_t));
    configurationStatusIndicator->setState(indicatorState);

    configurationTableWidget->setRowCount(0);

    addItemToConfigurationTable("Rx Antenna","n/a",radConfig_p.rxAntenna);
    addItemToConfigurationTable("Rx Gain","dB",QString::number(radConfig_p.rxGain, 'f', 3));
    addItemToConfigurationTable("Rx Carrier Frequency","GHz",QString::number(radConfig_p.rxCarrierFrequency/1e9, 'f', 3));
    //addItemToConfigurationTable("Rx Gain","GHz",std::to_string(radConfig_p.);
    addItemToConfigurationTable("Rx Sampling rate","MHz",QString::number(radConfig_p.rxSamplingRate/1e6, 'f', 3));
   // addItemToConfigurationTable("Rx Sampling rate","MHz",QString::number(radConfig_p. 'f', 3));

    addItemToConfigurationTable("Tx Antenna","n/a",radConfig_p.txAntenna);
    addItemToConfigurationTable("Tx Gain","dB",QString::number(radConfig_p.txGain, 'f', 3));
    addItemToConfigurationTable("Tx Carrier Frequency","GHz",QString::number(radConfig_p.txCarrierFrequency/1e9, 'f', 3));
    addItemToConfigurationTable("Tx Sampling rate","MHz",QString::number(radConfig_p.txSamplingRate/1e6, 'f', 3));

    addItemToConfigurationTable("PPS Source","n/a",radConfig_p.ppsSource);
    addItemToConfigurationTable("Ref Source","n/a",radConfig_p.refSource);


    // Update all fields and controls
}

void RadioControlWidget::pushRadioConfigurationApplyStatus(int statusCode)
{
    std::string configurationStatusString_t = "Unknown error applying configuration";
    int indicatorState = 1;
    switch(statusCode){
    case 0:
        configurationStatusString_t = serial + " configured";
        indicatorState = 2;
        break;
    case -1:
        configurationStatusString_t = "Error";
        indicatorState = 1;
    }

    configurationStatusLabel->setText(QString::fromStdString(configurationStatusString_t));
    configurationStatusIndicator->setState(indicatorState);
}

void RadioControlWidget::onLoadDefaultConfigurationBtnRelease()
{
    emit loadDefaultConfigurationRequest(serial,false);
}

void RadioControlWidget::onApplyConfigurationBtnRelease()
{
    emit applyConfigurationRequest(serial,radConfig);
}

void RadioControlWidget::onLoadFileConfigurationBtnRelease()
{
    emit loadFileConfigurationRequest(serial,fileConfigurationField->getFieldText(),false);
}

void RadioControlWidget::onTestBtnRelease()
{
    configurationTableWidget->insertRow(0);
    //emit testRequest(serial,false);
}

void RadioControlWidget::onConfigurationTableItemChanged(const QModelIndex &parent, int first, int last)
{
    int rows = configurationTableWidget->rowCount();
    int rowHeight = configurationTableWidget->rowHeight(0);
    int headerHeight = configurationTableWidget->horizontalHeader()->height();

    //std::cout<< "Before: " << configurationTableWidget->minimumHeight() << " :: " << configurationTableWidget->height() << " :: " << configurationTableWidget->maximumHeight() << std::endl;

    int minimumHeight = 30;
    int maximumHeight = rowHeight*rows+headerHeight;

    configurationTableWidget->setMinimumHeight(minimumHeight);
    configurationTableWidget->setMaximumHeight(maximumHeight);

    //std::cout << "After: " << configurationTableWidget->minimumHeight() << " :: " << configurationTableWidget->height() << " :: " << configurationTableWidget->maximumHeight() << std::endl;
    //std::cout << mainGroupBox->maximumHeight() << std::endl;
    configurationTableWidget->adjustSize();
}

void RadioControlWidget::addItemToConfigurationTable(std::string name, std::string unit, std::string value)
{
    addItemToConfigurationTable(name,unit,QString::fromStdString(value));
    //std::cout<< "Viewport : " << configurationTableWidget->viewport()->minimumHeight() << std::endl;
}

void RadioControlWidget::addItemToConfigurationTable(std::string name, std::string unit, QString value)
{
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(name));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(unit));
    QTableWidgetItem *item3 = new QTableWidgetItem(value);

    configurationTableWidget->insertRow(0);

    // Add items to the table widget
    configurationTableWidget->setItem(0, 0, item1); // Row 0, Column 0
    configurationTableWidget->setItem(0, 1, item2); // Row 0, Column 1
    configurationTableWidget->setItem(0, 2, item3); // Row 0, Column 1
}

