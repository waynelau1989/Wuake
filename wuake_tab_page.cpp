#include <QProcessEnvironment>
#include <QProcess>
#include <QThread>
#include <QVBoxLayout>
#include <QScreen>
#include <QFileInfo>
#include <QDir>
#include <QMoveEvent>
#include <QtDebug>
#include <QTimer>
#include <Windows.h>
#include "wuake_tab_page.h"

WuakeTabPage::WuakeTabPage(QWidget *parent) :
    QWidget(parent),
    mWindow(nullptr),
    mIsShowing(true)
{
    mProcess = new QProcess(this);

    connect(mProcess, SIGNAL(started()), this, SLOT(onStarted()));
    connect(mProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(mProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));
}

WuakeTabPage::~WuakeTabPage()
{
    if (nullptr != mWindow) {
        delete mWindow;
        mWindow = nullptr;
    }
}

void WuakeTabPage::requestFocus()
{
    qDebug("%s:requestFocus", titleName().toLocal8Bit().data());
    ::SetWindowPos(mHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ::SetActiveWindow(mHwnd);
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

void WuakeTabPage::updatePosition()
{
    QPoint p = mapToGlobal(pos());
    if (nullptr != mWindow && mWindow->position() != p) {
        mWindow->setPosition(p);
    }
}

void WuakeTabPage::updateBackground()
{
    QPixmap pixmap = mWindow->screen()->grabWindow(mWId, 0, 0, mWindow->width(), mWindow->height());
    mBgImg->setPixmap(pixmap);
}

QWindow* WuakeTabPage::pageWindow()
{
    return mWindow;
}

void WuakeTabPage::show()
{
    qDebug() << titleName() << ": Show";
    if (!mIsShowing) {
        ::ShowWindow(mHwnd, SW_RESTORE);
        ::ShowWindow(mHwnd, SW_SHOW);
    }
    mIsShowing = true;
}

void WuakeTabPage::hide()
{
    qDebug() << titleName() << ": Hide";
    if (mIsShowing) {
        updateBackground();
        ::ShowWindow(mHwnd, SW_MINIMIZE);
        ::ShowWindow(mHwnd, SW_HIDE);
    }
    mIsShowing = false;
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

HWND findWindow(const QString& className, const QString& titleName)
{
#ifdef UNICODE
    return ::FindWindow(className.toStdWString().data(), titleName.toStdWString().data());
#else
    return ::FindWindow(className.toLocal8Bit().data(), titleName.toLocal8Bit().data());
#endif

}

void WuakeTabPage::onStarted()
{
    mHwnd = findWindow(className(), titleName());
    for (int i=0; i<100 && 0 == mHwnd; ++i) {
        QThread::msleep(5);
        mHwnd = findWindow(className(), titleName());
    }
    mWId = (WId)mHwnd;
    mWindow = QWindow::fromWinId(mWId);

    DWORD dwExStyle = ::GetWindowLongW(mHwnd, GWL_EXSTYLE);
    dwExStyle ^= WS_EX_TOOLWINDOW;
    ::SetWindowLongW(mHwnd, GWL_EXSTYLE, dwExStyle);

    setFixedSize(mWindow->size());

    //mWidget = QWidget::createWindowContainer(mWindow);
    //mWidget->setFixedSize(mWindow->size());

    mBgImg = new QLabel(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mBgImg);
    //layout->addWidget(mWidget);

    QTimer::singleShot(800, this, SLOT(updateBackground()));

    emit stateChanged(PAGE_STATE_START);
}

bool WuakeTabPage::event(QEvent *event)
{
    if (nullptr == mWindow) return QWidget::event(event);
    QEvent::Type type = event->type();
    /*
    if (QEvent::Paint != type) {
        qDebug("Event Type:%d", event->type());
    }
    */
    if (QEvent::Show == type) {
        show();
        return true;
    } else if (QEvent::Hide == type) {
        hide();
        return true;
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
    QFileInfo minttyInfo(sMinttyPath);
    if (!minttyInfo.exists()) {
        return;
    }

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
    QPoint p = mapToGlobal(pos());
    params << "-p" << QString("%1,%2").arg(p.x()).arg(p.y());
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
}
