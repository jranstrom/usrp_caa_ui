#ifndef RADIOCONTROLBASEWIDGET_H
#define RADIOCONTROLBASEWIDGET_H

#include <QWidget>
#include "cradioobject.h"

class RadioControlBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RadioControlBaseWidget(QWidget *parent = nullptr,std::shared_ptr<cRadioObject> rad_p=nullptr);
    std::string getSerial(){return sourceRadio->getSerial();}

    std::shared_ptr<cRadioObject> sourceRadio;

    virtual void onControlClose(){};

signals:
    void controlClosed();
    void statusUpdateRequest(std::string message,int code);

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // RADIOCONTROLBASEWIDGET_H