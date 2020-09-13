#include <QtDebug>
#include <QApplication>
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTabBar>
#include <QKeySequence>
#include <QRect>
#include <QMenu>
#include <QScreen>
#include "QHotkey"
#include "wuake_window.h"
#include "wuake_tab_page.h"

WuakeWindow::WuakeWindow(QWidget *parent) :
    QDialog(parent, Qt::ToolTip | Qt::FramelessWindowHint),
    mIsShowing(false)
    //QDialog(parent)
{
    //resize(819, 475);

    mTrayIcon = new QSystemTrayIcon(this);
    mTrayIcon->setIcon(QIcon(":/res/images/app.png"));
    mTrayIcon->setToolTip(QString("Wuake\n") + tr("Drop-down terminal for Windows"));
    QMenu* menu = new QMenu(this);
    mExitAct = menu->addAction(tr("Exit"));
    mTrayIcon->setContextMenu(menu);
    mTrayIcon->show();

    connect(menu, &QMenu::triggered, this, &WuakeWindow::onAction);
    connect(mTrayIcon, &QSystemTrayIcon::activated, this, &WuakeWindow::onTrayActive);

    mTabWidget = new WuakeTabWidget(this);

    connect(mTabWidget, &WuakeTabWidget::focusLost, this, &WuakeWindow::hide);

    QVBoxLayout* layoutRoot = new QVBoxLayout(this);
    layoutRoot->setContentsMargins(0, 0, 0, 0);
    layoutRoot->addWidget(mTabWidget);

    initHotkeys();

    initAnim();

    mTabWidget->newTab();
}

WuakeWindow::~WuakeWindow()
{
}

void WuakeWindow::show()
{
    if (mIsShowing) return;

    QDialog::show();

    mShowAnim.start();
}

void WuakeWindow::hide()
{
    if (!mIsShowing) return;
    if (mShowAnim.state() == QAbstractAnimation::Running ||
        mHideAnim.state() == QAbstractAnimation::Running) {
        return;
    }
    mHideAnim.start();
}

void WuakeWindow::onShow()
{
    activateWindow();
    mIsShowing = true;
}

void WuakeWindow::onHide()
{
    mIsShowing = false;
}

void WuakeWindow::initAnim()
{
    updateAnimRect();

    mShowAnim.setTargetObject(this);
    mShowAnim.setPropertyName("pos");
    mShowAnim.setDuration(170);
    mShowAnim.setEasingCurve(QEasingCurve::Linear);

    connect(&mShowAnim, &QPropertyAnimation::finished, this, &WuakeWindow::onShow);

    mHideAnim.setTargetObject(this);
    mHideAnim.setPropertyName("pos");
    mHideAnim.setDuration(170);
    mHideAnim.setEasingCurve(QEasingCurve::Linear);

    connect(&mHideAnim, &QPropertyAnimation::finished, this, &WuakeWindow::onHide);
}

void WuakeWindow::updateAnimRect()
{
    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    if (nullptr == screen) {
        qWarning() << "Cannot find screen!";
        return;
    }

    int w = width();
    int h = height();
    int x = (screen->size().width() - w)/2;
    int y = 0;

    move(x, y);

    QPoint posStart(x, y - h);
    QPoint posEnd(x, y);

    mShowAnim.setStartValue(posStart);
    mShowAnim.setEndValue(posEnd);

    mHideAnim.setStartValue(posEnd);
    mHideAnim.setEndValue(posStart);
}

void WuakeWindow::initHotkeys()
{
    mHotkeys.insert(HOTKEY_SHOW_HIDE, "F12");
    mHotkeys.insert(HOTKEY_NEW_TAB, "Ctrl+Shift+N");
    mHotkeys.insert(HOTKEY_SWITCH_LEFT_TAB, "Shift+Left");
    mHotkeys.insert(HOTKEY_SWITCH_RIGHT_TAB, "Shift+Right");
    mHotkeys.insert(HOTKEY_MOVE_ACTIVE_TAB_LEFT, "Ctrl+Shift+Left");
    mHotkeys.insert(HOTKEY_MOVE_ACTIVE_TAB_RIGHT, "Ctrl+Shift+Right");

    auto it = mHotkeys.constBegin();
    for (; it != mHotkeys.constEnd(); ++it) {
        QHotkey* hotkey = new QHotkey(QKeySequence(it.value()), true, this);
        if (hotkey->isRegistered()) {
            connect(hotkey, &QHotkey::activated, this, &WuakeWindow::onHotkey);
        } else {
            QMessageBox::warning(this, tr("Warn"), tr("Register global hotkey") + QString(" [%1] ").arg(it.value()) + tr("failed!"));
        }
    }

}

void WuakeWindow::onHotkey()
{
    QHotkey* hotKey = qobject_cast<QHotkey*>(sender());
    QString hotkeyStr = hotKey->shortcut().toString();
    HotKeyCode keyCode = mHotkeys.key(hotkeyStr, HOTKEY_NONE);

    if (HOTKEY_SHOW_HIDE == keyCode) {
        if (mIsShowing) {
            hide();
        } else {
            show();
        }
        return;
    }

    if (isHidden()) return;

    switch (keyCode) {
    case HOTKEY_NEW_TAB:
        mTabWidget->newTab();
        break;
    case HOTKEY_SWITCH_LEFT_TAB:
        mTabWidget->switchToLeft();
        break;
    case HOTKEY_SWITCH_RIGHT_TAB:
        mTabWidget->switchToRight();
        break;
    case HOTKEY_MOVE_ACTIVE_TAB_LEFT:
        mTabWidget->moveToLeft();
        break;
    case HOTKEY_MOVE_ACTIVE_TAB_RIGHT:
        mTabWidget->moveToRight();
        break;
        //disable warning in gcc
    case HOTKEY_NONE:
    case HOTKEY_SHOW_HIDE:
        break;
    }
}

void WuakeWindow::onAction(QAction *action)
{
    if (action == mExitAct) {
        mTabWidget->closeAll();
    }
}

void WuakeWindow::onTrayActive(QSystemTrayIcon::ActivationReason reason)
{
    if (QSystemTrayIcon::Trigger == reason) {
        if (isHidden()) {
            show();
        } else {
            hide();
        }
    }
}

void WuakeWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (mShowAnim.state() == QAbstractAnimation::Running ||
        mHideAnim.state() == QAbstractAnimation::Running) {
        return;
    }
    updateAnimRect();
}
