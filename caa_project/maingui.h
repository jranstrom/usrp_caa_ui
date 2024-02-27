#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include "custom/radiosysobject.h"
#include "uiobj.h"

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

    void updateTransmitStatus(bool status);
    void updateTxSetupStatus(bool status);
    void updateRxSetupStatus(bool status);

    uiobj *uio = new uiobj(this);

private slots:
    void on_button_rx_test_connection_released();
    void on_button_tx_test_connection_released();

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

    void on_button_load_data_released();

    void on_button_tx_stop_released();

    void on_button_apply_config_clicked();

    void on_button_save_cf_released();

private:
    Ui::mainGUI *ui;
    RadioSysObject * radObj;

    void SetWidgetColor(QWidget * widg, int colorc);

    void applyTxConfig();
    void applyRxConfig();
};
#endif // MAINGUI_H
