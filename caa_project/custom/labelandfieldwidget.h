#ifndef LABELANDFIELDWIDGET_H
#define LABELANDFIELDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>

class LabelandFieldWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LabelandFieldWidget(QWidget *parent = nullptr,std::string labelText="Label:", std::string fieldText="");

    void setLabelText(std::string value);
    void setFieldText(std::string value);

signals:

private slots:
    void onFieldTextChanged();

private :
    QLabel * label;
    QHBoxLayout * main_layout;
    QLineEdit * lineEdit;

    std::string fieldText = "";
};

#endif // LABELANDFIELDWIDGET_H
