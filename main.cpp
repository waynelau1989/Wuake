#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "wuake_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QLocale(), "wuake", "_", "translations");
    a.installTranslator(&translator);

    WuakeWindow ww;
    ww.show();

    return a.exec();
}
