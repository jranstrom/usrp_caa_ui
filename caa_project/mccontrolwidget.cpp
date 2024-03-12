#include "mccontrolwidget.h"

MCControlWidget::MCControlWidget(QWidget *parent, int id,std::string mcType)
    : QWidget{parent}
{

    MCType = mcType;

    identifier = id;
    //main_group = new QGroupBox("Unknown Periferal", this);

    main_group_layout = new QVBoxLayout(this);
    main_group_layout->setMargin(10);

    QWidget *widget_info = new QWidget;
    QWidget *widget_control = new QWidget;

    info_layout = new QHBoxLayout(widget_info);
    info_layout->setMargin(10);

    control_layout = new QGridLayout(widget_control);
    control_layout->setMargin(10);

    response_w = new LabelandFieldWidget(this,"Response:","");

    mctype_label = new QLabel(this);
    mctype_label->setAlignment(Qt::AlignCenter);
    mctype_label->setText(QString::fromStdString(MCType));

    main_group_layout->addWidget(mctype_label);
    main_group_layout->addWidget(widget_info);
    main_group_layout->addWidget(response_w);
    main_group_layout->addWidget(widget_control);

    port_w = new LabelandFieldWidget(this,"Port:");
    info_layout->addWidget(port_w);

    mac_w = new LabelandFieldWidget(this,"MAC:");
    info_layout->addWidget(mac_w);

    status_button = new QPushButton("Status", this);
    info_button = new QPushButton("Info", this);
    auto_button = new QPushButton("Auto",this);

    control_layout->addWidget(status_button,0,0);
    control_layout->addWidget(info_button,0,1);
    control_layout->addWidget(auto_button,0,2);

    control_layout->setSpacing(10);

    connect(status_button,&QPushButton::released,this,&MCControlWidget::onStatusButtonReleased);
    connect(info_button,&QPushButton::released,this,&MCControlWidget::onInfoButtonReleased);
    connect(auto_button,&QPushButton::released,this,&MCControlWidget::onAutoButtonReleased);

}

void MCControlWidget::onStatusButtonReleased()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestStatus();
    //std::cout << "Status button released" << std::endl;
}

void MCControlWidget::onInfoButtonReleased()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestInfo();
    //std::cout << "Info button released" << std::endl;
}

void MCControlWidget::onAutoButtonReleased()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestAutoToggle();
}

void MCControlWidget::responseChanged(std::string value)
{
    response_w->setFieldText(value);
    disconnect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
}
