#include <QTranslator>
#include <QLocale>
#include "single_application.h"
#include "wuake_window.h"

int main(int argc, char *argv[])
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

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
