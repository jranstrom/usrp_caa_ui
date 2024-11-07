#include "labelandcheckboxwidget.h"


LabelandCheckboxWidget::LabelandCheckboxWidget(QWidget *parent, std::string labelText, bool checkedValue)
    : QWidget{parent}
{
    main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(2,2,2,2);

    mainCheckBox = new QCheckBox(this);
    setLabelText(labelText);

    main_layout->addWidget(mainCheckBox);

    connect(mainCheckBox,QOverload<int>::of(&QCheckBox::stateChanged),this,&LabelandCheckboxWidget::onCheckBoxChanged);
}

void LabelandCheckboxWidget::setEnabled(bool enabled)
{
    if(enabled){
        mainCheckBox->setStyleSheet("QCheckBox { color: #000000; }");
    }else{
        mainCheckBox->setStyleSheet("QCheckBox { color: #D3D3D3; }");
    }
    mainCheckBox->setEnabled(enabled);

    QWidget::setEnabled(enabled);
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

        return 1;

    }

    return 0;
}

void LabelandCheckboxWidget::setDataSource(bool *dSource, bool silent)
{
    dataSource = dSource;
    updateDataSource = true;
    requestSetValue(*dSource,silent);
}

void LabelandCheckboxWidget::saveCurrentValue()
{
    savedValue = currentValue;
    saved = true;
}

int LabelandCheckboxWidget::getCheckBoxState()
{
    return mainCheckBox->checkState();
}

bool LabelandCheckboxWidget::isSaved()
{
    if(saved == false){
        return false;
    }

    return (currentValue == savedValue);
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
