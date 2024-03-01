#include "maingui.h"
#include <QApplication>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "custom/radiosysobject.h"

RadioSysObject radObj = RadioSysObject();

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    mainGUI w;
    w.show();

    w.setRadioSysObject(&radObj);




    return a.exec();
}
