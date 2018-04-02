#include <QProcessEnvironment>
#include <QProcess>
#include <QThread>
#include <QVBoxLayout>
#include <Windows.h>
#include "wuake_tab_page.h"

WuakeTabPage::WuakeTabPage(QWidget *parent) :
    QWidget(parent)
{
    mProcess = new QProcess(this);

    connect(mProcess, SIGNAL(started()), this, SLOT(onStarted()));
    connect(mProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(mProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
}

WuakeTabPage::~WuakeTabPage()
{
    delete mWindow;
}

void WuakeTabPage::requestFocus()
{
    ::SetFocus(mHwnd);
}

void WuakeTabPage::close()
{
    ::SendMessage(mHwnd, WM_CLOSE, 0, 0);
}

void WuakeTabPage::onError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    emit stateChanged(PAGE_STATE_ERROR);
}

void WuakeTabPage::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    emit stateChanged(PAGE_STATE_CLOSE);
}

void WuakeTabPage::onStarted()
{
    mHwnd = ::FindWindow(className().toLocal8Bit().data(), titleName().toLocal8Bit().data());
    for (int i=0; i<10 && 0 == mHwnd; ++i) {
        QThread::msleep(5);
        mHwnd = ::FindWindow(className().toLocal8Bit().data(), titleName().toLocal8Bit().data());
    }
    WId wid = (WId)mHwnd;
    mWindow = QWindow::fromWinId(wid);
    mWidget = QWidget::createWindowContainer(mWindow);
    mWidget->setFixedSize(mWindow->size());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mWidget);

    emit stateChanged(PAGE_STATE_START);
}

bool WuakeTabPage::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        requestFocus();
    }
    return QWidget::event(event);
}



quint64 MinttyTabPage::sCounter = 0;
MinttyTabPage::MinttyTabPage(QWidget *parent) :
    WuakeTabPage(parent)
{
    mTitleName = QString("%1%2").arg(className()).arg(++sCounter);
    startProcess();
}

MinttyTabPage::~MinttyTabPage()
{
}

void MinttyTabPage::startProcess()
{
    QProcess* process = mProcess;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("ConEmuPID", "-1");
    process->setProcessEnvironment(env);

    QString homeDir = env.value("HOME");
    if (homeDir.isEmpty()) {
        homeDir = env.value("USERPROFILE");
    }

    QStringList params;
    if (!homeDir.isEmpty()) {
        params << "--dir" << homeDir;
    }
    params << "-s" << "100,30";
    params << "-B" << "void";
    params << "-t" << titleName();
    params << "--nopin";
    params << "-";
    process->start("C:/Program Files/Git/usr/bin/mintty.exe", params);
    process->waitForStarted();

}

QString MinttyTabPage::className()
{
    return "mintty";
}

QString MinttyTabPage::titleName()
{
    return mTitleName;
}
