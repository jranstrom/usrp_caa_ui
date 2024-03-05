#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include "custom/radiosysobject.h"
#include "uiobj.h"
#include <QDateTime>
#include <QTableWidget>
#include <QTimer>

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

    void updateTxCarrierFrequency(bool status);
    void updateRxCarrierFrequency(bool status);

    void updateTxGain(bool status);
    void updateRxGain(bool status);

    void updateTxIPAddress(bool status);
    void updateRxIPAddress(bool status);

    void updateTxPPSSource(bool status);
    void updateRxPPSSource(bool status);

    void updateTxSamplingRate(bool status);
    void updateRxSamplingRate(bool status);

    void updateTxREFSource(bool status);
    void updateRxREFSource(bool status);

    void updateTransmitStatus(bool status);
    void updateReceiveStatus(bool status);
    void updateTxSetupStatus(bool status);
    void updateRxSetupStatus(bool status);


    void updateRxLO_Offset(bool status);

    uiobj uio = uiobj(this);

private slots:
    void on_button_rx_test_connection_released();
    void on_button_tx_test_connection_released();

    void trackTransmissionProcess();
    void trackReceptionProcess();

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

    void on_vslider_tx_gain_valueChanged(int value);

    void on_lineEdit_tx_gain_textEdited(const QString &arg1);

    void on_vslider_rx_gain_valueChanged(int value);

    void on_lineEdit_rx_gain_textEdited(const QString &arg1);

    void on_hslider_tx_fc_valueChanged(int value);

    void on_lineEdit_tx_fc_editingFinished();

    void on_lineEdit_rx_fc_editingFinished();

    void on_hslider_rx_fc_valueChanged(int value);

    void on_button_transmit_released();

    void on_button_apply_config_released();

    void processing_USRP_setup();

    void on_button_load_data_released();

    void on_button_tx_stop_released();

    void on_button_apply_config_clicked();

    void on_button_save_cf_released();

    void on_lineEdit_tx_ip_editingFinished();

    void on_lineEdit_rx_ip_editingFinished();

    void on_button_receive_released();

    void on_button_rx_stop_released();

    void on_button_write_buffer_to_file_released();

    void on_lineEdit_lo_offset_textEdited(const QString &arg1);

    void on_vslider_lo_offset_valueChanged(int value);

    void on_lineEdit_rx_rs_textEdited(const QString &arg1);

    void on_lineEdit_tx_rs_textEdited(const QString &arg1);

    void on_hslider_rx_rs_valueChanged(int value);

    void on_hslider_tx_rs_valueChanged(int value);

private:
    Ui::mainGUI *ui;
    RadioSysObject * radObj;

    QTimer processingTimer;

    bool usrpSetupInterrupted;
    bool pendingConfigurationRequest = false;

    QDateTime transmissionStartTime;
    QDateTime receptionStartTime;

    void SetWidgetColor(QWidget * widg, int colorc);

    void addStatusUpdate(QString entry,QTableWidget *table);

    void applyTxConfig();
    void applyRxConfig();
};
#endif // MAINGUI_H
