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

    void setLabelText(std::string text);
    void setLabelTextQ(QString text);

    int requestSetValue(bool value, bool silent=false);
    void setDataSource(bool * dSource, bool silent=false);

    bool getValue() {return currentValue;}

signals:
    void componentValueChanged(bool value, bool silent);

private slots:
    void onCheckBoxChanged(int value);

private:
    QCheckBox * mainCheckBox;
    QHBoxLayout * main_layout;

    bool currentValue;
    bool * dataSource;
    bool updateDataSource = false;

    void setValue(bool value, bool silent=false);
};

#endif // LABELANDCHECKBOXWIDGET_H
