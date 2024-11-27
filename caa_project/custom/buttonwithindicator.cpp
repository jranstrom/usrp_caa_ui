#include "buttonwithindicator.h"

ButtonWithIndicator::ButtonWithIndicator(QWidget *parent, std::string alignment, std::string btnText)
    : QWidget{parent}
{
    mainLayout = new QHBoxLayout(this);

    buttonText[0] = btnText;
    buttonText[1] = btnText;
    buttonText[2] = btnText;
    buttonText[3] = btnText;

    mainButton = new QPushButton(QString::fromStdString(buttonText[currentState]),this);
    connect(mainButton,&QPushButton::released,this,&ButtonWithIndicator::onMainButtonRelease);

    mainIndicator = new IndicatorButtonWidget(0,0,this);

    leftSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);
    rightSpacer = new QSpacerItem(5,5,QSizePolicy::Expanding,QSizePolicy::Expanding);

    setAlignment(alignment);

}

void ButtonWithIndicator::setButtonText(std::string text, int state)
{
    if(state > -1 && state < 4){
        buttonText[state] = text;
        if(state == currentState){
            mainButton->setText(QString::fromStdString(buttonText[currentState]));
        }
    }

}

void ButtonWithIndicator::setCurrentState(int state)
{
    if(state >= 0 && state < 4){
        currentState = state;
        mainButton->setText(QString::fromStdString(buttonText[currentState]));
        mainIndicator->setState(currentState);
    }else{
        throw std::runtime_error("Error: ButtonWithIndicator::setCurrentState: not a valid state");
    }
}

void ButtonWithIndicator::setAlignment(std::string alignment)
{
    // remove all
    mainLayout->removeItem(leftSpacer);
    mainLayout->removeItem(rightSpacer);

    mainLayout->removeWidget(mainButton);
    mainLayout->removeWidget(mainIndicator);

    if(alignment == "left"){

        mainLayout->addWidget(mainIndicator);
        mainLayout->addWidget(mainButton);
        mainLayout->addItem(rightSpacer);

    }else if(alignment == "center"){

        mainLayout->addItem(leftSpacer);
        mainLayout->addWidget(mainIndicator);
        mainLayout->addWidget(mainButton);
        mainLayout->addItem(rightSpacer);

    }else if(alignment == "right"){

        mainLayout->addItem(leftSpacer);
        mainLayout->addWidget(mainIndicator);
        mainLayout->addWidget(mainButton);

    }else{

        mainLayout->addWidget(mainIndicator);
        mainLayout->addWidget(mainButton);
    }
}

void ButtonWithIndicator::onMainButtonRelease()
{
    emit buttonReleased(currentState);
}

void ButtonWithIndicator::setEnabled(bool value)
{
    mainButton->setEnabled(false);
    QWidget::setEnabled(true);
}

