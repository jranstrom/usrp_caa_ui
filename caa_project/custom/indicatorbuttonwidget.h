#ifndef INDICATORBUTTONWIDGET_H
#define INDICATORBUTTONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <string>
#include <QLayout>

class IndicatorButtonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IndicatorButtonWidget(QWidget *parent = nullptr);
    explicit IndicatorButtonWidget(int initialState,int id=0,QWidget *parent = nullptr);

    void InitializeIndicatorButtonWidget(int initalState,int id,QWidget * parent);

    void setState(int state); // 0 - gray (disabled); 1 - red; 2 - green; 3 - yellow
    int getState() {return currentState;}
    std::string getStateName();

    int getId() {return identifier;}

private slots:
    void onIndicatorButtonReleased();

signals:
    void indicatorButtonReleased(int id);

private:
    int currentState = -1;
    int identifier = 0;
    QPushButton * indicator;
    QHBoxLayout * mainLayout;

    static const int numStates = 4;
    std::string state_names[numStates] = {"disabled","off","on","pending"};


    void setWidgetColor(QWidget *widg,int colorc);
    bool checkValidState(int state);

};

#endif // INDICATORBUTTONWIDGET_H
