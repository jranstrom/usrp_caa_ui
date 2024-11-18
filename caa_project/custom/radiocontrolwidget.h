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
#include <qtablewidget.h>

class RadioControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RadioControlWidget(QWidget *parent = nullptr, std::shared_ptr<cRadioObject> cRad_p=nullptr);

    std::string getSerial(){return serial;}

    void pushRadioConfiguration(cRadioConfiguration radConfig_p,int configType=2);
    void pushRadioConfigurationApplyStatus(int statusCode);

signals:
    void loadDefaultConfigurationRequest(std::string serial_p,bool silent);
    void loadFileConfigurationRequest(std::string serial_p,std::string filepath,bool silent);
    void applyConfigurationRequest(std::string serial_p,cRadioConfiguration radConf_p);
    void testRequest(std::string serial_p,bool silent);

private slots:
    void onLoadDefaultConfigurationBtnRelease();
    void onApplyConfigurationBtnRelease();
    void onLoadFileConfigurationBtnRelease();
    void onTestBtnRelease();
    void onConfigurationTableItemChanged(const QModelIndex &parent, int first, int last);

private:

    cRadioConfiguration radConfig;

    std::string serial;
    std::string type;
    std::string uType; // upper case type (for display purposes)
    std::string address;

    QGroupBox * mainGroupBox;

    QVBoxLayout * mainGroupBoxLayout;
    QVBoxLayout * outerContainerLayout;

    QVBoxLayout * basicInfoLayout;

    QHBoxLayout * configurationStatusLayout;
    IndicatorButtonWidget * configurationStatusIndicator;
    QLabel * configurationStatusLabel;
    QPushButton * loadDefaultConfigurationBtn;
    QPushButton * applyConfigurationBtn;

    QHBoxLayout * configurationFileLayout;
    LabelandFieldWidget * fileConfigurationField;
    QPushButton * loadFileConfigurationBtn;

    QHBoxLayout * testSectionLayout;
    QPushButton * testSectionPushBtn;

    QVBoxLayout * configurationTableLayout;
    QTableWidget * configurationTableWidget;

    QHBoxLayout * applyUSRPConfigLayout;


    LabelandFieldWidget * serialField;
    LabelandFieldWidget * typeField;
    LabelandFieldWidget * addressField;

    QFrame *basicInfoDividerLine;

    QSpacerItem * vSpacer;

    void addItemToConfigurationTable(std::string name,std::string unit, std::string value);
    void addItemToConfigurationTable(std::string name,std::string unit, QString value);
};

#endif // RADIOCONTROLWIDGET_H
