#ifndef LABELANDSPINBOXWIDGET_H
#define LABELANDSPINBOXWIDGET_H

#include <QWidget>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QObject>
#include <QSpinBox>

class LabelandSpinBoxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LabelandSpinBoxWidget(QWidget *parent = nullptr,
                                   std::string labelText="Label:",
                                   int maximumValue=10,
                                   int minimumValue=0,
                                   int defaultValue=0,
                                   int incrementValue=1);

    void setLabelText(std::string text);
    void setLabelTextQ(QString text);

    void setMaximum(int value);
    void setMinimum(int value);
    void setIncrement(int value);

    int requestSetValue(int value, bool silent=false);
    void setDataSource(int * si, bool silent=false);

    void saveCurrentValue();

    void setEnabled(bool enabled);

    int getValue() { return currentValue;}
    int getMaximum() { return maximum; }
    int getMinimum() { return minimum; }

    bool isSaved();


signals:
    void componentValueChanged(int value,bool silent);

private slots:
    void onSpinBoxChanged(int value);

private:
    QLabel * label;
    QHBoxLayout * main_layout;
    QSpinBox * mainSpinBox;

    int savedValue;
    int currentValue;
    int maximum;
    int minimum;
    int increment;
    bool saved = false;

    int * dataSource;
    bool updateDataSource = false;

    void setValue(int value, bool silent=false);

};

#endif // LABELANDSPINBOXWIDGET_H
