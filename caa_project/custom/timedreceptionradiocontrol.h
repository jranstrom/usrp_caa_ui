#ifndef TIMEDRECEPTIONRADIOCONTROL_H
#define TIMEDRECEPTIONRADIOCONTROL_H

#include <QWidget>
#include "radiocontrolbasewidget.h"

class TimedReceptionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    TimedReceptionRadioControl(QWidget *parent=nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr );

    std::string getType() override {return "timed-rx";}
    RadioControlWidget::RadioControlType getEnumType() override {return RadioControlWidget::RadioControlType::RxTIMED;}
};

#endif // TIMEDRECEPTIONRADIOCONTROL_H
