#ifndef TIMEDTRANSMISSIONRADIOCONTROL_H
#define TIMEDTRANSMISSIONRADIOCONTROL_H

#include <QWidget>
#include <qboxlayout.h>
#include "radiocontrolbasewidget.h"

class TimedTransmissionRadioControl : public RadioControlBaseWidget
{
    Q_OBJECT
public:
    TimedTransmissionRadioControl(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> rad_p=nullptr);

    std::string getType() override {return "timed-tx";}
    RadioControlWidget::RadioControlType getEnumType() override {return RadioControlWidget::RadioControlType::TxTIMED;}
private:

    QVBoxLayout * mainLayout;

};

#endif // TIMEDTRANSMISSIONRADIOCONTROL_H
