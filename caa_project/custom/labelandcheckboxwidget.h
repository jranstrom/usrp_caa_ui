#ifndef LABELANDCHECKBOXWIDGET_H
#define LABELANDCHECKBOXWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLayout>
#include <string>

class LabelandCheckboxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LabelandCheckboxWidget(QWidget *parent = nullptr,
                                    std::string labelText="Label:",
                                    bool checkedValue=false);

    void setEnabled(bool enabled);

    void setLabelText(std::string text);
    void setLabelTextQ(QString text);

    int requestSetValue(bool value, bool silent=false);
    void setDataSource(bool * dSource, bool silent=false);

    void saveCurrentValue();
    int getCheckBoxState();

    bool getValue() {return currentValue;}
    bool getSavedValue() {return savedValue;}

    bool isSaved();


signals:
    void componentValueChanged(bool value, bool silent);

private slots:
    void onCheckBoxChanged(int value);

private:
    QCheckBox * mainCheckBox;
    QHBoxLayout * main_layout;

    bool currentValue;
    bool savedValue;
    bool * dataSource;
    bool updateDataSource = false;
    bool saved = false;

    void setValue(bool value, bool silent=false);
};

#endif // LABELANDCHECKBOXWIDGET_H
