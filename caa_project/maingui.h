#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include "custom/radiosysobject.h"
#include "uiobj.h"
#include "tcom_ui.h"
#include <QDateTime>
#include <QTableWidget>
#include <QTimer>
#include "mccontrolwidget.h"
#include "custom/sliderandlineedit.h"

#include "uhd_clib.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class mainGUI;
}
QT_END_NAMESPACE

class mainGUI : public QMainWindow
{
    Q_OBJECT

public:
    mainGUI(QWidget *parent = nullptr);
    ~mainGUI();
    void setRadioSysObject(RadioSysObject * RadObj);

    //void updateTxCarrierFrequency(bool status);
    //void updateRxCarrierFrequency(bool status);

    void updateTxGain(bool status);
    void updateRxGain(bool status);

    void updateTxIPAddress(bool status);
    void updateRxIPAddress(bool status);

    void updateTxPPSSource(bool status);
    void updateRxPPSSource(bool status);

    //void updateTxSamplingRate(bool status);
    //void updateRxSamplingRate(bool status);

    void updateTxREFSource(bool status);
    void updateRxREFSource(bool status);

    void updateTransmitStatus(bool status);
    void updateReceiveStatus(bool status);
    void updateSynchronizationStatus(bool status);

    void updateTxSetupStatus(bool status);
    void updateRxSetupStatus(bool status);


    void updateRxLO_Offset(bool status);

    uiobj uio = uiobj(this);
    Tcom_ui tcom = Tcom_ui(this);

private slots:
    void trackTransmissionProcess();
    void trackReceptionProcess();
    void trackSynchronizationProcess();

    void trackCaptureBufferProcess();

    void updateUSRPSetupChanged(bool val);
    void onUSRPConfigurationChanged(bool val);

    void updateTxUSRPSetupStatus(bool val);
    void updateRxUSRPSetupStatus(bool val);


    void on_buttonGroup_tx_pps_buttonClicked(int val);
    void on_buttonGroup_tx_ref_buttonClicked(int val);
    void on_buttonGroup_rx_pps_buttonClicked(int val);
    void on_buttonGroup_rx_ref_buttonClicked(int val);

    void on_button_load_cfg_released();

    void on_button_transmit_released();

    void on_button_apply_config_released();

    void processing_USRP_setup();
    void processing_automatic_capture();

    void on_button_load_data_released();

    void on_button_tx_stop_released();

    void on_button_apply_config_clicked();

    void on_button_save_cf_released();

    void on_button_receive_released();

    void on_button_rx_stop_released();

    void on_button_write_buffer_to_file_released();

    void on_pushButton_released();

    void updateAvailableDevices(std::vector<std::string> & value);
    void updateConnection(std::string value);
    //void updateResponse(std::string value);
    //void updateMCId(std::string value);
    //void updateMCType(std::string value);

    void on_listWidget_available_devices_itemSelectionChanged();

    void on_button_generate_mc_controls_released();

    void on_button_disconnect_released();

    void updateMCSCycle(int id);

    void userChangedTxCarrierFrequency(double value);
    void userChangedRxCarrierFrequency(double value);
    void userChangedTxSamplingRate(double value);
    void userChangedRxSamplingRate(double value);
    void userChangedTxGain(double value);
    void userChangedRxGain(double value);
    void userChangedLOOffset(double value);
    void userChangedRxIPAddress(std::string value);
    void userChangedTxIPAddress(std::string value);


    void on_pushButton_2_released();

    void on_pushButton_3_released();

    void on_button_capture_synch_released();

    void on_button_set_synch_format_released();

    void on_button_save_synch_capture_released();

    void plot_time_and_freq(std::vector<std::complex<short>> vc_data);

    void on_button_reset_tx_released();

    void on_button_auto_capture_released();

    void onActiveElementSpinBoxChanged(int value,bool noSynchReset);
    void onActiveClassSpinBoxChanged(int value,bool noSynchReset);
    void onActiveSampleSpinBoxChanged(int value,bool noSynchReset);
    void onNumElementSpinBoxChanged(int value);
    void onNumClassesSpinBoxChanged(int value);
    void onSynchCaptureOffsetSpinBoxChanged(int value);
    void onSynchCaptureLengthSpinBoxChanged(int value);


private:
    Ui::mainGUI *ui;
    RadioSysObject * radObj;

    int currentMCControlIdentifier = 0;
    std::vector<MCControlWidget*> mcControlWidgets;

    QTimer processingTimer;

    bool usrpSetupInterrupted;
    bool pendingConfigurationRequest = false;

    int frameFormatStatus = 0; // 0 - never initialized | 1 - initialized and up to date | 2 - initialized but out of date

    bool automaticCaptureRunning = false;
    bool automaticCaptureProcessing = false;
    int validateAutomaticCapture();

    QDateTime transmissionStartTime;
    QDateTime receptionStartTime;
    QDateTime synchronizationStartTime;

    LabelandFieldWidget * rxIPAddressField;
    LabelandFieldWidget * txIPAddressField;

    SliderAndLineEdit * txCarrierSlider;
    SliderAndLineEdit * rxCarrierSlider;

    SliderAndLineEdit * txSamplingRateSlider;
    SliderAndLineEdit * rxSamplingRateSlider;

    SliderAndLineEdit * txGainSlider;
    SliderAndLineEdit * rxGainSlider;

    SliderAndLineEdit * rxLOOffsetSlider;

    void SaveSynchCaptures();
    void SaveSyncCaptures(std::string filepath,std::string message="");

    void SetActiveElementForAllUEs(int value);
    void SetActiveUEForAll(int value);

    void triggerFormatChanged(bool value);

    void SetWidgetColor(QWidget * widg, int colorc);

    void SetWidgetColorState(QWidget * widg, bool state);
    void SetWidgetColorState(QWidget *widg, int state);

    void addStatusUpdate(QString entry,QTableWidget *table, int type=0);

    void hideLayout(QLayout* layout);
    void unhideLayout(QLayout* layout);

    void applyTxConfig();
    void applyRxConfig();

    void removeAllMCControlWidgets();
};
#endif // MAINGUI_H
