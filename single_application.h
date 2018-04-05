#pragma once

#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>


class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int argc, char* argv[]);

    bool isRunning();

signals:
    void onNewInstance();

private:
    QLocalServer mServer;
    QLocalSocket mClient;
    QString mServerName;

    bool mIsRunning;
};
