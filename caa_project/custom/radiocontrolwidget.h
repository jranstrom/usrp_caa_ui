#ifndef RADIOCONTROLWIDGET_H
#define RADIOCONTROLWIDGET_H

#include <QWidget>
#include "custom/cradioobject.h"
#include "custom/radiosysobject.h"
#include "custom/labelandcheckboxwidget.h"
#include "indicatorbuttonwidget.h"
#include "labelandfieldwidget.h"
#include <QGroupBox>
#include <QPushButton>

class RadioControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RadioControlWidget(QWidget *parent = nullptr, cRadioObject * cRad_p=nullptr);

    std::string getSerial(){return serial;}

    void pushRadioConfiguration(cRadioConfiguration radConfig_p,int configType=2);

signals:
    void loadDefaultConfigurationRequest(std::string serial_p,bool silent);

private slots:
    void onLoadDefaultConfigurationBtnRelease();

private:

    cRadioConfiguration radConfig;

    std::string serial;
    std::string type;
    std::string address;

    QGroupBox * mainGroupBox;

    QVBoxLayout * mainGroupBoxLayout;
    QHBoxLayout * outerContainerLayout;

    QVBoxLayout * basicInfoLayout;

    QHBoxLayout * configurationStatusLayout;
    IndicatorButtonWidget * configurationStatusIndicator;
    QLabel * configurationStatusLabel;
    QPushButton * loadDefaultConfigurationBtn;

    QHBoxLayout * configurationFileLayout;
    LabelandFieldWidget * fileConfigurationField;
    QPushButton * loadFileConfigurationBtn;


    LabelandFieldWidget * serialField;
    LabelandFieldWidget * typeField;
    LabelandFieldWidget * addressField;

    QFrame *basicInfoDividerLine;

    QSpacerItem * vSpacer;
};

#endif // RADIOCONTROLWIDGET_H
