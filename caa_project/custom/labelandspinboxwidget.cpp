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
    main_layout->setContentsMargins(2,2,2,2);

    label = new QLabel(this);
    setLabelText(labelText);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mainSpinBox = new QSpinBox(this);
    mainSpinBox->setStyleSheet("QSpinBox:disabled { background-color: #FFFFFF; color: #a0a0a0; }");
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

void LabelandSpinBoxWidget::setDataSource(int *si,bool silent)
{
    dataSource = si;
    updateDataSource = true;
    requestSetValue(*si,silent);

}

void LabelandSpinBoxWidget::saveCurrentValue()
{
    savedValue = currentValue;
    saved = true;
}

void LabelandSpinBoxWidget::setEnabled(bool enabled)
{
    if(enabled){
        label->setStyleSheet("QLabel { color: #000000; }");
    }else{
        label->setStyleSheet("QLabel { color: #D3D3D3; }");
    }
    mainSpinBox->setEnabled(enabled);
    //ainSpinBox->update();
}

bool LabelandSpinBoxWidget::isSaved()
{
    if(saved == false){
        return false;
    }

    return (currentValue == savedValue);
}

void LabelandSpinBoxWidget::setValue(int value, bool silent)
{
    if(currentValue != value){
       currentValue = value;
       if(updateDataSource){
           *dataSource = currentValue;
       }
       emit componentValueChanged(value,silent);
    }

}

void LabelandSpinBoxWidget::onSpinBoxChanged(int value)
{
    setValue(value);
}
