#include <QProcessEnvironment>
#include <QProcess>
#include <QThread>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QtDebug>
#include <QMessageBox>
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

void WuakeTabPage::startProcess()
{
}

bool WuakeTabPage::processRunning()
{
    if (mProcess->state() == QProcess::NotRunning) {
        return false;
    }
    return true;
}

void WuakeTabPage::onError(QProcess::ProcessError error)
{
    qWarning() << "Process error:" << error;
    QMessageBox::critical(this, tr("Error"), tr("Open program [") + mProcess->program() + tr("] failed!"));
    emit stateChanged(PAGE_STATE_ERROR);
}

void WuakeTabPage::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "exitCode:" << exitCode << ", exitStatus:" << exitStatus;
    emit stateChanged(PAGE_STATE_CLOSE);
}

HWND findWindow(const QString& className, const QString& titleName)
{
    HWND hwnd;
#ifdef UNICODE
    hwnd = ::FindWindow(className.toStdWString().data(), titleName.toStdWString().data());
#else
    hwnd = ::FindWindow(className.toLocal8Bit().data(), titleName.toLocal8Bit().data());
#endif
    return hwnd;
}

void WuakeTabPage::onStarted()
{
    qDebug() << "onStarted";
    mHwnd = findWindow(className(), titleName());
    for (int i=0; i<100 && 0 == mHwnd; ++i) {
        QThread::msleep(5);
        mHwnd = findWindow(className(), titleName());
    }
    WId wid = (WId)mHwnd;
    mWindow = QWindow::fromWinId(wid);
    mWidget = QWidget::createWindowContainer(mWindow);
    mWidget->setFocusPolicy(Qt::StrongFocus);
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
QString MinttyTabPage::sMinttyPath;
MinttyTabPage::MinttyTabPage(QWidget *parent) :
    WuakeTabPage(parent)
{
    mTitleName = QString("%1%2").arg(className()).arg(++sCounter);
    findMintty();
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

    if (!homeDir.isEmpty()) {
        process->setWorkingDirectory(homeDir);
    }

    QStringList params;
    //params << "-s" << "100,30";
    params << "-B" << "void";
    params << "-t" << titleName();
    params << "--nopin";
    params << "-";
    process->start(sMinttyPath, params);
}


QString MinttyTabPage::className()
{
    return "mintty";
}

QString MinttyTabPage::titleName()
{
    return mTitleName;
}

QString findByCmdWhere(const QString& cmd)
{
    QProcess process;
    QList<QString> params;
    params << cmd;
    process.start("where", params);
    process.waitForFinished();
    if (process.exitCode() != 0) return nullptr;

    QByteArray output = process.readAllStandardOutput();
    int pos = output.indexOf("\n");
    if (pos < 0) return nullptr;

    QByteArray path = output.left(pos - 1);
    return QString(path);
}

void MinttyTabPage::findMintty()
{
    if (!sMinttyPath.isEmpty()) return;

    sMinttyPath = findByCmdWhere("mintty.exe");
    if (!sMinttyPath.isEmpty()) return;

    QString gitPath = findByCmdWhere("git.exe");
    if (gitPath.isEmpty()) return;

    QFileInfo gitInfo(gitPath);

    QDir tmp = gitInfo.absoluteDir();
    tmp.cdUp();
    sMinttyPath = tmp.absolutePath() + "/usr/bin/mintty.exe";
    //sMinttyPath = "mintty.exe";
}
