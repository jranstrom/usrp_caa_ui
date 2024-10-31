#include "indicatorbuttonwidget.h"

IndicatorButtonWidget::IndicatorButtonWidget(QWidget *parent)
{
    InitializeIndicatorButtonWidget(0,0,parent);
}

IndicatorButtonWidget::IndicatorButtonWidget(int initialState,int id,QWidget *parent)
    : QWidget{parent}
{
    InitializeIndicatorButtonWidget(initialState,id,parent);
}

void IndicatorButtonWidget::InitializeIndicatorButtonWidget(int initalState, int id, QWidget *parent)
{
    identifier = id;
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(2,2,2,2);
    indicator = new QPushButton("");
    indicator->setMaximumWidth(25);
    mainLayout->addWidget(indicator);

    connect(indicator,&QPushButton::released,this,&IndicatorButtonWidget::onIndicatorButtonReleased);

    setState(initalState);
}

void IndicatorButtonWidget::setState(int state)
{
    if(state != currentState){
        if(state == 0){
            setWidgetColor(indicator,12631996);
            indicator->setEnabled(false);
        }else{
            indicator->setEnabled(true);
        }
        if(state == 1){
            setWidgetColor(indicator,16146769);
        }else if(state == 2){
            setWidgetColor(indicator,9433252);
        }else if(state == 3){
            setWidgetColor(indicator,16380011);
        }

        currentState = state;
    }
}

std::string IndicatorButtonWidget::getStateName()
{
    if(checkValidState(currentState)){
        return state_names[currentState];
    }

    return "";
}

void IndicatorButtonWidget::onIndicatorButtonReleased()
{
    emit indicatorButtonReleased(identifier);
}

void IndicatorButtonWidget::setWidgetColor(QWidget *widg, int colorc)
{
    widg->setAutoFillBackground(true);
    QPalette palette = widg->palette();
    palette.setColor(QPalette::Button, QColor(colorc));
    widg->setPalette(palette);
    widg->show();
}

bool IndicatorButtonWidget::checkValidState(int state)
{
    bool valid = false;
    if(state > -1 && state < numStates){
        valid = true;
    }
    return valid;
}
