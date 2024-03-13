#ifndef SLIDERANDLINEEDIT_H
#define SLIDERANDLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QBoxLayout>
#include <vector>
#include <QGroupBox>
#include <QSpacerItem>

#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

class SliderAndLineEdit : public QWidget
{
    Q_OBJECT
public:
    explicit SliderAndLineEdit(QWidget *parent = nullptr);
    explicit SliderAndLineEdit(std::string title="",
                               double upperLimit=500,
                               double lowerLimit=0,
                               std::string unit="",
                               double valueDisplayModifier=1,
                               double increment=1,
                               bool horizontalLayout=true,
                               QWidget * parent=nullptr);

    void InitializeSliderAndLineEdit(std::string title,
                                    double upperLimit,
                                    double lowerLimit,
                                    std::string unit,
                                    double valueDisplayModifier,
                                    double increment,
                                    bool horizontalLayout,
                                    QWidget * parent);

    double Request2SetComponentValue(double value);

signals:

    void componentValueChanged(double value);

private slots:
    void onSliderChange(int value);
    void onLineEditChange(QString value);
    void onLineEditFinishedChange();
private:

    int Precision = 1;

    QVBoxLayout * outerContainer;
    QGroupBox * mainGroupBox;
    QHBoxLayout * groupHLayout;
    QVBoxLayout * groupVLayout;

    QWidget * sliderAndLabelContainer;
    QWidget * lineEditAndLabelContainer;

    QSlider * mainSlider;

    QLineEdit * lineEdit;
    bool isEditing = false;

    QLabel * lineEditLabel;

    QLabel * lowerLimitLabel;
    QLabel * upperLimitLabel;

    double UpperLimit = 10;
    double LowerLimit = 0;

    std::string Unit = "Hz";

    double CurrentValue = 0;

    int SliderUpperLimit = 10;
    int SliderLowerLimit = 0;

    double Increment = 1;
    double ConvertionFactor = 1;

    double DisplayValueModifier = 1;

    void updateLowerLimitLabel();
    void updateUpperLimitLabel();

    void setLowerLimitLabel(std::string labelText);
    void setUpperLimitLabel(std::string labelText);

    void SetComponentValue(double value,bool silent=false);

};

#endif // SLIDERANDLINEEDIT_H
