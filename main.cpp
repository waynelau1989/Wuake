#include <QApplication>
#include "wuake_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WuakeWindow ww;
    ww.show();

    return a.exec();
}
