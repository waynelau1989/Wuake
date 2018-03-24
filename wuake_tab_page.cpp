#include <QProcessEnvironment>
#include <QProcess>
#include <QThread>
#include <QVBoxLayout>
#include <Windows.h>
#include "wuake_tab_page.h"

int WuakeTabPage::sCounter = 0;

WuakeTabPage::WuakeTabPage(QWidget *parent) :
    QWidget(parent),
    mHwnd(0)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("ConEmuPID", "-1");
    mProcess = new QProcess(this);
    mProcess->setProcessEnvironment(env);
    mTitleName = "wintty" + sCounter++;
    QStringList params;
    params << "-s" << "100,30";
    params << "-B" << "void";
    params << "-t" << mTitleName;
    params << "--nopin";
    params << "-";
    mProcess->start("C:/Program Files/Git/usr/bin/mintty.exe", params);
    mProcess->waitForStarted();
    mHwnd = ::FindWindow("mintty", mTitleName.toUtf8().data());
    for (int i=0; i<10 && 0 == mHwnd; ++i) {
        QThread::msleep(5);
        mHwnd = ::FindWindow("mintty", mTitleName.toUtf8().data());
    }
    WId wid = (WId)mHwnd;
    mWindow = QWindow::fromWinId(wid);
    mWidget = QWidget::createWindowContainer(mWindow);
    mWidget->setFixedSize(mWindow->size());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mWidget);
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
