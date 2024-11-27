#ifndef BUTTONWITHINDICATOR_H
#define BUTTONWITHINDICATOR_H

#include "indicatorbuttonwidget.h"
#include <QWidget>
#include <qpushbutton.h>

class ButtonWithIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit ButtonWithIndicator(QWidget *parent = nullptr,std::string alignment="",std::string btnText="");

    void setButtonText(std::string text,int state=0);
    void setCurrentState(int state);
    int getCurrentState(){return currentState;}

    std::string getCurrentButtonText(){return buttonText[currentState];}

    void setAlignment(std::string alignment="");

signals:

    void buttonReleased(int state);

private slots:

    void onMainButtonRelease();

private:
    int currentState = 0;
    std::array<std::string, 4> buttonText = {"Disabled", "Off", "On","Pending"};

    QPushButton * mainButton;
    IndicatorButtonWidget * mainIndicator;
    QHBoxLayout * mainLayout;

    QSpacerItem * leftSpacer;
    QSpacerItem * rightSpacer;

protected:
    void setEnabled(bool value);


};

#endif // BUTTONWITHINDICATOR_H
