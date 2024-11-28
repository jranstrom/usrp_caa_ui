#ifndef RADIOCONTROLWIDGET_H
#define RADIOCONTROLWIDGET_H

#include <QWidget>
#include "buttonwithindicator.h"
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

    void pushRadioConfiguration(std::shared_ptr<cRadioObject> rad);
    void pushRadioConfigurationApplyStatus(int statusCode,cRadioConfiguration radConf_p);

    void changeIndicatorButtonState(int state, std::string type);


signals:
    void loadDefaultConfigurationRequest(std::string serial_p,bool silent);
    void loadFileConfigurationRequest(std::string serial_p,std::string filepath,bool silent);
    void applyConfigurationRequest(std::string serial_p,cRadioConfiguration radConf_p);
    void continousReceptionControlRequest(std::string serial_p,bool silent);
    void continousTransmissionControlRequest(std::string serial_p,bool silent);
    void scriptReceptionControlRequest(std::string serial_p,bool silent);
    void testRequest(std::string serial_p,bool silent);

private slots:
    void onLoadDefaultConfigurationBtnRelease();
    void onApplyConfigurationBtnRelease();
    void onLoadFileConfigurationBtnRelease();
    void onContinousReceptionBtnRelease();
    void onContinousTransmissionBtnRelease();
    void onScriptReceptionBtnRelase();
    void onScriptReceptionIndBtnRelease();
    void onTestBtnRelease();
    void onConfigurationTableInsertItem(const QModelIndex &parent, int first, int last);
    void onConfigurationTableItemChanged(QTableWidgetItem * item);

private:

    std::shared_ptr<cRadioObject> sourceRadio;
    cRadioConfiguration radConfig;
    cRadioConfiguration appliedRadConfig;

    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> radSourceConfig;
    std::unordered_map<std::string,std::shared_ptr<cRadioProperty>> radControlWidgetConfig;

    int configurationStatus = 0; // 1 - loaded; 2 - applied; 3 - changed

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
    QLabel * configurationSectionLabel;

    QHBoxLayout * applyUSRPConfigLayout;

    QGridLayout * radioControlLayout;
    QFrame *radioControlDividerLine;

    ButtonWithIndicator * continousTransmissionIndBtn;
    ButtonWithIndicator * continousReceptionIndBtn;
    ButtonWithIndicator * scriptReceptionIndBtn;

    LabelandFieldWidget * serialField;
    LabelandFieldWidget * typeField;
    LabelandFieldWidget * addressField;

    QFrame *basicInfoDividerLine;

    QSpacerItem * vSpacer;

    void addItemToConfigurationTable(std::string name,std::string unit, std::string value);
    void addItemToConfigurationTable(std::string name,std::string unit, QString value);
};

#endif // RADIOCONTROLWIDGET_H
