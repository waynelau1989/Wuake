#include "single_application.h"

SingleApplication::SingleApplication(int argc, char *argv[]) :
    QApplication(argc, argv),
    mServerName("Wuake"),
    mIsRunning(false)
{
    mServerName = "Wuake";
    mClient.connectToServer(mServerName);
    if (mClient.waitForConnected(500)) {
        mIsRunning = true;
        return;
    }

    mServer.listen(mServerName);
    connect(&mServer, SIGNAL(newConnection()), this, SIGNAL(onNewInstance()));
}

bool SingleApplication::isRunning()
{
    return mIsRunning;
}
