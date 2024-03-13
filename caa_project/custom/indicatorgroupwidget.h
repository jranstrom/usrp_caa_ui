#ifndef INDICATORGROUPWIDGET_H
#define INDICATORGROUPWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include "indicatorbuttonwidget.h"
#include <vector>
#include <QBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <iostream>

class IndicatorGroupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IndicatorGroupWidget(QWidget *parent = nullptr,std::string name="",int numIndicators=0);
    explicit IndicatorGroupWidget(std::string name="",std::vector<int> initialIndicatorStates = {0},QWidget *parent=nullptr);
    void IndicatorGroupWidgetSetup(QWidget *parent,std::string name,std::vector<int> initialIndicatorStates);
    IndicatorButtonWidget* addIndicator(int initialState);
    int getIndicatorState(int position);
    IndicatorButtonWidget* getIndicator(int position);

    void setIndicatorState(int position,int state);
    void setIndicatorStates(std::vector<int> states);
    void setIndicatorStates(std::vector<bool> states);

    int getNumberOfIndicators() { return indicators.size();}

    std::string getIdentifier() {return identifier;}
    void setIdentifier(std::string value){
        if(value != identifier){
            identifier = value;
        }
    }

signals:
    void indicatorButtonReleased(int id);

private slots:
    void onIndicatorButtonReleased(int id);

private:
    std::vector<IndicatorButtonWidget*> indicators;

    std::string identifier = "";

    QGroupBox *mainGroupBox;
    QVBoxLayout *mainLayout;
    QHBoxLayout * groupLayout;

    bool checkValidPosition(int position);
};

#endif // INDICATORGROUPWIDGET_H
