#include "labelandfieldwidget.h"

LabelandFieldWidget::LabelandFieldWidget(QWidget *parent,
                                         std::string labelText,
                                         std::string fieldText,
                                         bool editable)
    : QWidget{parent}
{

    main_layout = new QHBoxLayout(this);
    main_layout->setMargin(2);

    label = new QLabel(this);
    label->setText(QString::fromStdString(labelText));

    main_layout->addWidget(label);

    lineEdit = new QLineEdit(this);
    lineEdit->setText(QString::fromStdString(fieldText));

    main_layout->addWidget(lineEdit);

    if(!editable){
        connect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextChanged);
    }
}

void LabelandFieldWidget::setLabelText(std::string value)
{
    label->setText(QString::fromStdString(value));
}

void LabelandFieldWidget::setFieldText(std::string value)
{
    if(value != fieldText){
        fieldText = value;
        lineEdit->setText(QString::fromStdString(value));
    }
}

void LabelandFieldWidget::setEditable(bool is_editable)
{
    if(isEditable != is_editable){
        isEditable = is_editable;
        if(!isEditable){
            connect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextChanged);
        }else{
            disconnect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextChanged);
        }
    }
}

void LabelandFieldWidget::onFieldTextChanged()
{
    lineEdit->setText(QString::fromStdString(fieldText));
}
