#include "mccontrolwidget.h"

MCControlWidget::MCControlWidget(std::string mcPort,Tcom_ui * tcom_ptr, int id, std::string mcType, QWidget *parent)
{
    InitializeMCControlWidget(id,mcType,mcPort,tcom_ptr,parent);
}

void MCControlWidget::InitializeMCControlWidget(int id, std::string mcType, std::string mcPort,Tcom_ui * tcom_ptr, QWidget *parent)
{
    MCType = mcType;
    identifier = id;
    port = mcPort;
    tcom = tcom_ptr;

    outerContainerLayout = new QHBoxLayout(this);
    mainGroupBox = new QGroupBox("");
    mainGroupBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    mainGroupBoxLayout = new QVBoxLayout(mainGroupBox);
    mainGroupBoxLayout->addStretch(1);

    QWidget *widget_info = new QWidget;
    QWidget *widget_control = new QWidget;

    infoLayout = new QHBoxLayout(widget_info);
    infoLayout->setMargin(2);

    controlLayout = new QGridLayout(widget_control);
    controlLayout->setMargin(2);

    response_w = new LabelandFieldWidget(this,"Response:","");

    //mctype_label = new QLabel(this);
    //mctype_label->setAlignment(Qt::AlignCenter);
    //mctype_label->setText(QString::fromStdString(MCType));
    //mainGroupBoxLayout->addWidget(mctype_label);

    mainGroupBoxLayout->addWidget(widget_info);
    mainGroupBoxLayout->addWidget(response_w);
    mainGroupBoxLayout->addWidget(widget_control);

    // Port field
    port_w = new LabelandFieldWidget(this,"Port:",port);
    infoLayout->addWidget(port_w);
    //port_w->setVisible(false);

    mac_w = new LabelandFieldWidget(this,"MAC:");
    //infoLayout->addWidget(mac_w);
    mac_w->setVisible(false);

    // Define control buttons
    status_button = new QPushButton("Status");
    info_button = new QPushButton("Info");
    auto_button = new QPushButton("Auto");
    debug_button = new QPushButton("Debug");
    reset_button = new QPushButton("Reset");

    // Add control buttons to layout
    controlLayout->addWidget(status_button,0,0);
    controlLayout->addWidget(info_button,0,1);
    controlLayout->addWidget(debug_button,0,2);
    controlLayout->addWidget(auto_button,0,3);
    controlLayout->addWidget(reset_button,1,3);

    if(MCType == "Element Switch"){
        element_group_w = new IndicatorGroupWidget("Elements",{0,0,0,0});
        controlLayout->addWidget(element_group_w,1,0,1,2);
        connect(element_group_w,&IndicatorGroupWidget::indicatorButtonReleased,this,&MCControlWidget::onElementButtonReleased);
    }

    if(MCType == "UE Switch"){
        // Get number of UEs connected
        if(tcom != nullptr){
            //connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
            tcom->requestToConnect(port,true);
            std::string config = tcom->requestConfiguration(true);

            int L = getNumberAfterMatch(config,"UES");
            std::vector<int> initialStates;
            for(int i = 0;i<L;i++){
                initialStates.push_back(0);
            }

            cycle_button = new QPushButton("Cycle");

            connect(cycle_button,&QPushButton::released,this,&MCControlWidget::onCycleButtonReleased);

            ue_group_w = new IndicatorGroupWidget("UEs",initialStates);
            connect(ue_group_w,&IndicatorGroupWidget::indicatorButtonReleased,this,&MCControlWidget::onUEButtonReleased);

            controlLayout->addWidget(ue_group_w,1,0,1,2);
            controlLayout->addWidget(cycle_button,1,2);
        }
    }


    controlLayout->setSpacing(10);

    outerContainerLayout->addWidget(mainGroupBox);

    connect(status_button,&QPushButton::released,this,&MCControlWidget::onStatusButtonReleased);
    connect(info_button,&QPushButton::released,this,&MCControlWidget::onInfoButtonReleased);
    connect(auto_button,&QPushButton::released,this,&MCControlWidget::onAutoButtonReleased);
    connect(debug_button,&QPushButton::released,this,&MCControlWidget::onDebugButtonReleased);
    connect(reset_button,&QPushButton::released,this,&MCControlWidget::onResetButtonReleased);
}

void MCControlWidget::requestRead()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true,100); // short timeout, so returns quickly
    bool hasMessage = tcom->requestRead();
    if(!hasMessage){
        disconnect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    }
}

void MCControlWidget::requestSelectElement(int value)
{
    tcom->requestToConnect(port,true);
    tcom->requestAllUEsOFF();
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestELToggle(value);
}

void MCControlWidget::requestSelectUE(int value)
{
    tcom->requestToConnect(port,true);
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestUESelect(value);

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

void MCControlWidget::onDebugButtonReleased()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestDebugToggle();
}

void MCControlWidget::onCycleButtonReleased()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestCycle();
    emit cycleButtonReleased(identifier);
}

void MCControlWidget::onResetButtonReleased()
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestReset();
    emit cycleButtonReleased(identifier);
}

void MCControlWidget::responseChanged(std::string value)
{
    response_w->setFieldText(value);

    configureMCfromResponse(value);
    // set control

    disconnect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
}

void MCControlWidget::onElementButtonReleased(int id)
{
    // Toggle element
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);
    tcom->requestELToggle(id+1);
}

void MCControlWidget::onUEButtonReleased(int id)
{
    connect(tcom,&Tcom_ui::responseChanged,this,&MCControlWidget::responseChanged);
    tcom->requestToConnect(port,true);

    if(ue_group_w->getIndicator(id)->getState() == 2){
        tcom->requestAllUEsOFF();
    }else{
        tcom->requestUESelect(id+1);
    }
}

void MCControlWidget::configureMCfromResponse(std::string value)
{
    if(MCType == "Element Switch"){
        size_t pos = value.find("caae");
        int caae_config = 0;
        int L = element_group_w->getNumberOfIndicators(); //get number of elements

        std::vector<bool> elementStates;

        if (pos != std::string::npos) {
            pos += 4;
            std::string number = "";

            while(isdigit(value[pos])){
                number += value[pos++];
            }

            caae_config = std::stoi(number);
            for(int i=0;i<L;i++){
                elementStates.push_back(static_cast<bool>((caae_config >> i) & 1));
            }

            element_group_w->setIndicatorStates(elementStates);
        }

    }else if(MCType == "UE Switch"){
        int activeUE = getNumberAfterMatch(value,"ue");
        for(int i=0;i<ue_group_w->getNumberOfIndicators();i++){
            if(activeUE-1 == i){
                ue_group_w->setIndicatorState(i,2);
            }else{
                ue_group_w->setIndicatorState(i,1);
            }
        }
    }
}

int MCControlWidget::getNumberAfterMatch(std::string originalString, std::string substring)
{
    int result = -1;
    size_t pos = originalString.find(substring);
    if (pos != std::string::npos) {
        pos += substring.size();
        std::string number = "";
        while(pos < originalString.size() && isdigit(originalString[pos])){
            number += originalString[pos++];
        }

        try{
            result = std::stoi(number);
        }catch(...){
            result = -1;
        }
    }

    return result;

}
