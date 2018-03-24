#include <QApplication>
#include "wuake_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Dialog w;
    //w.show();
    WuakeWindow ww;
    ww.show();

    return a.exec();
}
