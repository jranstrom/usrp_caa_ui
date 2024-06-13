#include "labelandspinboxwidget.h"

LabelandSpinBoxWidget::LabelandSpinBoxWidget(QWidget *parent,
                                             std::string labelText,
                                             int maximumValue,
                                             int minimumValue,
                                             int defaultValue,
                                             int incrementValue)
    : QWidget{parent}
{
    main_layout = new QHBoxLayout(this);
    main_layout->setMargin(2);

    label = new QLabel(this);
    setLabelText(labelText);

    mainSpinBox = new QSpinBox(this);
    setMaximum(maximumValue);
    setMinimum(minimumValue);
    setIncrement(incrementValue);


    main_layout->addWidget(label);
    main_layout->addWidget(mainSpinBox);

    connect(mainSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),this,&LabelandSpinBoxWidget::onSpinBoxChanged);

    //connect(mainSpinBox,&QSpinBox::valueChanged,this,&LabelandSpinBoxWidget::onSpinBoxChanged);
}

void LabelandSpinBoxWidget::setLabelText(std::string text)
{
    setLabelTextQ(QString::fromStdString(text));
}

void LabelandSpinBoxWidget::setLabelTextQ(QString text)
{
    label->setText(text);
}

void LabelandSpinBoxWidget::setMaximum(int value)
{
    if(currentValue > value){
        requestSetValue(value,true);
    }
    mainSpinBox->setMaximum(value);
    maximum = value;
}

void LabelandSpinBoxWidget::setMinimum(int value)
{
    if(currentValue < value){
        requestSetValue(value,true);
    }
    mainSpinBox->setMinimum(value);
    minimum = value;
}

void LabelandSpinBoxWidget::setIncrement(int value)
{
    mainSpinBox->setSingleStep(value);
}

int LabelandSpinBoxWidget::requestSetValue(int value, bool silent)
{

    // Adjust value to be within allowable range
    int adjusted_value = ((value-minimum) % (maximum-minimum+1))+minimum;

    if(adjusted_value != currentValue){

        // Disconnect to avoid loop-around
       disconnect(mainSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),this,&LabelandSpinBoxWidget::onSpinBoxChanged);

       mainSpinBox->setValue(adjusted_value);
       setValue(adjusted_value,silent);

        // Reconnect to enable triggering on spinbox change
        connect(mainSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),this,&LabelandSpinBoxWidget::onSpinBoxChanged);
    }

   return adjusted_value;
}

void LabelandSpinBoxWidget::setValue(int value, bool silent)
{
    if(currentValue != value){
       currentValue = value;
        emit componentValueChanged(value,silent);
    }

}

void LabelandSpinBoxWidget::onSpinBoxChanged(int value)
{
    setValue(value);
}
