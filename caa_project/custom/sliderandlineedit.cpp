#include "sliderandlineedit.h"

SliderAndLineEdit::SliderAndLineEdit(QWidget *parent)
    : QWidget{parent}
{
    InitializeSliderAndLineEdit("Title",3500,-170,"Hz",10,150,true,parent);

}

SliderAndLineEdit::SliderAndLineEdit(std::string title, double upperLimit, double lowerLimit, std::string unit, double valueDisplayModifier, double increment, bool horizontalLayout, QWidget *parent)
{
    InitializeSliderAndLineEdit(title,upperLimit,lowerLimit,unit,valueDisplayModifier,increment,horizontalLayout,parent);
}

void SliderAndLineEdit::InitializeSliderAndLineEdit(std::string title,
                                                    double upperLimit,
                                                    double lowerLimit,
                                                    std::string unit,
                                                    double valueDisplayModifier,
                                                    double increment,
                                                    bool horizontalLayout,
                                                    QWidget *parent)
{

    outerContainer = new QVBoxLayout(this);

    UpperLimit = upperLimit;
    LowerLimit = lowerLimit;
    DisplayValueModifier = valueDisplayModifier;
    Increment = increment;
    Unit = unit;

    Precision = std::ceil(log10(valueDisplayModifier/increment));

    SliderLowerLimit = 0;
    SliderUpperLimit = std::ceil((upperLimit-lowerLimit)/increment); // Number of steps

    mainGroupBox = new QGroupBox(QString::fromStdString(title));
    if(horizontalLayout){
        mainGroupBox->setFixedHeight(100);
        mainGroupBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    }else{
        mainGroupBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    }

    sliderAndLabelContainer = new QWidget();
    lineEditAndLabelContainer = new QWidget();

    mainSlider = new QSlider();
    mainSlider->setMinimum(SliderLowerLimit);
    mainSlider->setMaximum(SliderUpperLimit);


    lowerLimitLabel = new QLabel();
    updateLowerLimitLabel();

    upperLimitLabel = new QLabel();
    updateUpperLimitLabel();

    QHBoxLayout * lineEditAndLabelLayout = new QHBoxLayout(lineEditAndLabelContainer);

    lineEdit = new QLineEdit();
    lineEdit->setMaximumWidth(135);
    lineEdit->setAlignment(Qt::AlignRight);
    lineEditLabel = new QLabel(QString::fromStdString(Unit));
    QSpacerItem * leftSpacer = new QSpacerItem(5,2,QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem * rightSpacer = new QSpacerItem(5,2,QSizePolicy::Expanding, QSizePolicy::Minimum);

    lineEditAndLabelLayout->addItem(leftSpacer);
    lineEditAndLabelLayout->addWidget(lineEdit);
    lineEditAndLabelLayout->addWidget(lineEditLabel);
    lineEditAndLabelLayout->addItem(rightSpacer);

    groupVLayout = new QVBoxLayout(mainGroupBox);

    if(horizontalLayout){

        mainSlider->setOrientation(Qt::Horizontal);

        QHBoxLayout * sliderAndLabelLayout = new QHBoxLayout(sliderAndLabelContainer);
        sliderAndLabelLayout->setSpacing(1);

        sliderAndLabelLayout->addWidget(lowerLimitLabel);
        sliderAndLabelLayout->addWidget(mainSlider);
        sliderAndLabelLayout->addWidget(upperLimitLabel);

        groupVLayout->addWidget(sliderAndLabelContainer);
    }else{
        groupVLayout = new QVBoxLayout(mainGroupBox);
        mainSlider->setOrientation(Qt::Vertical);
    }

    groupVLayout->addWidget(lineEditAndLabelContainer);
    outerContainer->addWidget(mainGroupBox);

    groupVLayout->setSpacing(1);
    groupVLayout->setContentsMargins(1,1,1,1);

    connect(mainSlider,&QSlider::valueChanged,this,&SliderAndLineEdit::onSliderChange);
    connect(lineEdit,&QLineEdit::textEdited,this,&SliderAndLineEdit::onLineEditChange);
    connect(lineEdit,&QLineEdit::editingFinished,this,&SliderAndLineEdit::onLineEditFinishedChange);

    Request2SetComponentValue(lowerLimit);
}

double SliderAndLineEdit::Request2SetComponentValue(double value)
{
    double dvalue = value;
    double svalue;
    if(dvalue > UpperLimit){
        dvalue = UpperLimit;
    }else if(dvalue < LowerLimit){
        dvalue = LowerLimit;
    }else{
        // round to closest increment
        svalue = round((dvalue-LowerLimit)/Increment)*Increment+LowerLimit;

        if(std::abs(dvalue-UpperLimit) > std::abs(dvalue-svalue)){
            dvalue = svalue;
        }else{
            dvalue = UpperLimit;
        }
    }

    SetComponentValue(dvalue,true);

    return dvalue;
}

void SliderAndLineEdit::onSliderChange(int value)
{
    double dvalue = value*Increment+LowerLimit;
    if(dvalue > UpperLimit){
        dvalue = UpperLimit;
    }
    SetComponentValue(dvalue);
}

void SliderAndLineEdit::onLineEditChange(QString value)
{
    isEditing = true;
    bool conversionSuccess;
    double dvalue = value.toDouble(&conversionSuccess)*DisplayValueModifier;
    double svalue = 0;

    if(conversionSuccess){
        if(dvalue > UpperLimit){
            dvalue = UpperLimit;
        }else if(dvalue < LowerLimit){
            dvalue = LowerLimit;
        }else{

            // round to closest increment
            svalue = round((dvalue-LowerLimit)/Increment)*Increment+LowerLimit;

            if(std::abs(dvalue-UpperLimit) > std::abs(dvalue-svalue)){
                //dvalue = svalue;
            }else{
                dvalue = UpperLimit;
            }

        }

        //std::cout << dvalue << std::endl;
        SetComponentValue(dvalue);

    }
}

void SliderAndLineEdit::onLineEditFinishedChange()
{
    isEditing = false;
    SetComponentValue(CurrentValue);
}

void SliderAndLineEdit::updateLowerLimitLabel()
{
    std::stringstream stream;
    int cPrecision = calculateMinPrecision(LowerLimit/DisplayValueModifier);
    stream << std::fixed << std::setprecision(cPrecision) << LowerLimit/DisplayValueModifier;
    std::string s = stream.str() + " " + Unit;
    setLowerLimitLabel(s);
}

void SliderAndLineEdit::updateUpperLimitLabel()
{
    std::stringstream stream;
    int cPrecision = calculateMinPrecision(UpperLimit/DisplayValueModifier);
    stream << std::fixed << std::setprecision(cPrecision) << UpperLimit/DisplayValueModifier;
    std::string s = stream.str() + " " +  Unit;
    setUpperLimitLabel(s);
}

void SliderAndLineEdit::setLowerLimitLabel(std::string labelText)
{
    lowerLimitLabel->setText(QString::fromStdString(labelText));
}

void SliderAndLineEdit::setUpperLimitLabel(std::string labelText)
{
    upperLimitLabel->setText(QString::fromStdString(labelText));
}

void SliderAndLineEdit::SetComponentValue(double value,bool silent)
{
    if(CurrentValue != value){
        CurrentValue = value;
        if(!silent){
            emit componentValueChanged(value);
        }
    }
    disconnect(mainSlider,&QSlider::valueChanged,this,&SliderAndLineEdit::onSliderChange);


    int sliderValue =std::ceil((value-LowerLimit)/Increment);
    mainSlider->setValue(sliderValue);

    if(!isEditing){
        disconnect(lineEdit,&QLineEdit::textEdited,this,&SliderAndLineEdit::onLineEditChange);        
        lineEdit->setText(QString::number(value/DisplayValueModifier,'f',calculateMinPrecision(value/DisplayValueModifier)));
        connect(lineEdit,&QLineEdit::textEdited,this,&SliderAndLineEdit::onLineEditChange);
    }


    connect(mainSlider,&QSlider::valueChanged,this,&SliderAndLineEdit::onSliderChange);

}

int SliderAndLineEdit::calculateMinPrecision(double value)
{
    QString number_str = QString::number(value,'f',15); // maximum precision
    int decimal_pos = number_str.indexOf('.');
    int final_precision = 0;
    if (decimal_pos > -1) {
        while (number_str.endsWith('0')) {
            number_str.chop(1);
        }
        final_precision = number_str.length() - decimal_pos - 1;
    }
    return final_precision;
}
