#include "labelandfieldwidget.h"

LabelandFieldWidget::LabelandFieldWidget(QWidget *parent,
                                         std::string labelText,
                                         std::string fieldText,
                                         bool editable)
    : QWidget{parent}
{

    main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(2,2,2,2);

    label = new QLabel(this);
    label->setText(QString::fromStdString(labelText));

    main_layout->addWidget(label);

    lineEdit = new QLineEdit(this);
    lineEdit->setText(QString::fromStdString(fieldText));

    main_layout->addWidget(lineEdit);

    if(!editable){
        connect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextChanged);
    }else{
        connect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextEditing);
        connect(lineEdit,&QLineEdit::editingFinished,this,&LabelandFieldWidget::onFieldTextEditFinished);
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

void LabelandFieldWidget::setFieldText(double value, int precision)
{
    std::string value_str = doubleToString(value,precision);
    setFieldText(value_str);
}

void LabelandFieldWidget::setFieldText(int value)
{
    std::string value_str = std::to_string(value);
    setFieldText(value_str);
}

void LabelandFieldWidget::setFieldText(size_t value)
{
    std::string value_str = std::to_string(value);
    setFieldText(value_str);
}

void LabelandFieldWidget::setEditable(bool is_editable)
{
    if(isEditable != is_editable){
        isEditable = is_editable;
        if(!isEditable){
            connect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextChanged);
            disconnect(lineEdit,&QLineEdit::editingFinished,this,&LabelandFieldWidget::onFieldTextEditFinished);
        }else{
            connect(lineEdit,&QLineEdit::editingFinished,this,&LabelandFieldWidget::onFieldTextEditFinished);
            disconnect(lineEdit,&QLineEdit::textEdited,this,&LabelandFieldWidget::onFieldTextChanged);
        }
    }
}

void LabelandFieldWidget::setDataSource(std::string *ss)
{
    dataSource = ss;
    updateDataSource = true;
    setFieldText(*ss);
}

std::string LabelandFieldWidget::getFieldText()
{
    QString qstring = lineEdit->text();

    return qstring.toStdString();
}

void LabelandFieldWidget::onFieldTextChanged()
{
    lineEdit->setText(QString::fromStdString(fieldText));
}

void LabelandFieldWidget::onFieldTextEditing(QString value)
{
    isEditing = true;
    emit fieldTextEditing(value.toStdString());
}

void LabelandFieldWidget::onFieldTextEditFinished()
{
    isEditing = false;
    if(updateDataSource){
        *dataSource = (lineEdit->text()).toStdString();
    }
    emit fieldTexEditFinished((lineEdit->text()).toStdString());
}

std::string LabelandFieldWidget::doubleToString(double value, int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

