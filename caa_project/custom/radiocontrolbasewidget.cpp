#include "radiocontrolbasewidget.h"

RadioControlBaseWidget::RadioControlBaseWidget(QWidget *parent,std::shared_ptr<cRadioObject> rad_p)
    : QWidget{parent}
{
    sourceRadio = rad_p;

}

void RadioControlBaseWidget::closeEvent(QCloseEvent *event)
{
    onControlClose();

    emit controlClosed();
}
