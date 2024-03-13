#include "indicatorgroupwidget.h"

IndicatorGroupWidget::IndicatorGroupWidget(QWidget *parent,std::string name,int numIndicators)
    : QWidget{parent}
{

    std::vector<int> indicatorStates;
    for(int i=0;i<numIndicators;i++){
        indicatorStates.push_back(0);
    }
    IndicatorGroupWidgetSetup(parent,name,indicatorStates);
}

IndicatorGroupWidget::IndicatorGroupWidget(std::string name, std::vector<int> initialIndicatorStates, QWidget *parent)
{
    IndicatorGroupWidgetSetup(parent,name,initialIndicatorStates);
}

void IndicatorGroupWidget::IndicatorGroupWidgetSetup(QWidget *parent, std::string name, std::vector<int> initialIndicatorStates)
{
    int numberOfIndicators = initialIndicatorStates.size();

    mainLayout = new QVBoxLayout(this);

    const QString name_ = QString::fromStdString(name);
    mainGroupBox = new QGroupBox(name_);
    mainGroupBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    groupLayout = new QHBoxLayout(mainGroupBox);
    //groupLayout->addStretch(1);
    groupLayout->setAlignment(Qt::AlignLeft);


    for(int i=0;i<numberOfIndicators;i++){
        IndicatorButtonWidget * indi = addIndicator(initialIndicatorStates[i]);

        groupLayout->addWidget(indi);
        connect(indi,&IndicatorButtonWidget::indicatorButtonReleased,this,&IndicatorGroupWidget::onIndicatorButtonReleased);
    }

    mainLayout->addWidget(mainGroupBox);
}

IndicatorButtonWidget* IndicatorGroupWidget::addIndicator(int initialState)
{
    int currentNumberOfIndicators = indicators.size();
    IndicatorButtonWidget * newIndicator = new IndicatorButtonWidget(initialState,currentNumberOfIndicators,this);
    indicators.push_back(newIndicator);

    return indicators[currentNumberOfIndicators];
}

int IndicatorGroupWidget::getIndicatorState(int position)
{
    if(checkValidPosition(position)){
        return indicators[position]->getState();
    }else{
        return -1;
    }
}

IndicatorButtonWidget *IndicatorGroupWidget::getIndicator(int position)
{
    if(checkValidPosition(position)){
        return indicators[position];
    }

    return nullptr;
}

void IndicatorGroupWidget::setIndicatorState(int position, int state)
{
    if(checkValidPosition(position)){
        indicators[position]->setState(state);
    }
}

void IndicatorGroupWidget::setIndicatorStates(std::vector<int> states)
{
    for(int i=0;i<indicators.size();i++){
        indicators[i]->setState(states[i]);
    }
}

void IndicatorGroupWidget::setIndicatorStates(std::vector<bool> states)
{
    std::vector<int> newStates;
    for(int i=0;i<indicators.size();i++){
        if(states[i]){
            newStates.push_back(2);
        }else{
            newStates.push_back(1);
        }
    }

    setIndicatorStates(newStates);
}

void IndicatorGroupWidget::onIndicatorButtonReleased(int id)
{
    emit indicatorButtonReleased(id);
}


bool IndicatorGroupWidget::checkValidPosition(int position)
{
    if(position > -1 && position < indicators.size()){
        return true;
    }
    return false;
}
