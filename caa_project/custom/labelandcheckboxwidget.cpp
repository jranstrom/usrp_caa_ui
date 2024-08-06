#include "labelandcheckboxwidget.h"


LabelandCheckboxWidget::LabelandCheckboxWidget(QWidget *parent, std::string labelText, bool checkedValue)
    : QWidget{parent}
{
    main_layout = new QHBoxLayout(this);
    main_layout->setMargin(2);

    mainCheckBox = new QCheckBox(this);
    setLabelText(labelText);

    main_layout->addWidget(mainCheckBox);

    connect(mainCheckBox,QOverload<int>::of(&QCheckBox::stateChanged),this,&LabelandCheckboxWidget::onCheckBoxChanged);
}

void LabelandCheckboxWidget::setLabelText(std::string text)
{
    setLabelTextQ(QString::fromStdString(text));
}

void LabelandCheckboxWidget::setLabelTextQ(QString text)
{
    mainCheckBox->setText(text);
}

int LabelandCheckboxWidget::requestSetValue(bool value, bool silent)
{
    if(value != currentValue){

        disconnect(mainCheckBox,QOverload<int>::of(&QCheckBox::stateChanged),this,&LabelandCheckboxWidget::onCheckBoxChanged);

        mainCheckBox->setChecked(value);
        setValue(value,silent);

        connect(mainCheckBox,QOverload<int>::of(&QCheckBox::stateChanged),this,&LabelandCheckboxWidget::onCheckBoxChanged);

    }
}

void LabelandCheckboxWidget::setDataSource(bool *dSource, bool silent)
{
    dataSource = dSource;
    updateDataSource = true;
    requestSetValue(*dSource,silent);
}

void LabelandCheckboxWidget::onCheckBoxChanged(int value)
{
    if(value == 2){
        setValue(true);
    }else{
        setValue(false);
    }
}

void LabelandCheckboxWidget::setValue(bool value, bool silent)
{
    if(currentValue != value){
        currentValue = value;
        if(updateDataSource){
            *dataSource = currentValue;
        }
        emit componentValueChanged(value,silent);
    }
}
