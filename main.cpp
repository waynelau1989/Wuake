#include <QTranslator>
#include <QLocale>
#include "single_application.h"
#include "wuake_window.h"

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
    if (a.isRunning()) {
        return 0;
    }

    QTranslator translator;
    translator.load(QLocale(), "wuake", "_", "translations");
    a.installTranslator(&translator);

    WuakeWindow ww;
    ww.show();

    QObject::connect(&a, &SingleApplication::onNewInstance, &ww, &WuakeWindow::show);

    return a.exec();
}
