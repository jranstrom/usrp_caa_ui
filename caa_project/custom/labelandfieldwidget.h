#ifndef LABELANDFIELDWIDGET_H
#define LABELANDFIELDWIDGET_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>

class LabelandFieldWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LabelandFieldWidget(QWidget *parent = nullptr,
                                 std::string labelText="Label:",
                                 std::string fieldText="", bool editable=false);

    void setLabelText(std::string value);
    void setFieldText(std::string value);
    void setFieldText(double value,int precision=0);
    void setFieldText(int value);
    void setEditable(bool is_editable);

    std::string getFieldText();

signals:

    void fieldTextEditing(std::string value);
    void fieldTexEditFinished(std::string value);

private slots:
    void onFieldTextChanged();
    void onFieldTextEditing(QString value);
    void onFieldTextEditFinished();

private :

    std::string doubleToString(double value,int precision);


    QLabel * label;
    QHBoxLayout * main_layout;
    QLineEdit * lineEdit;

    std::string fieldText = "";

    bool isEditable = false;
    bool isEditing = false;
};

#endif // LABELANDFIELDWIDGET_H
