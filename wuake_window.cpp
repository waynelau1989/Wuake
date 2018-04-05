#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTabBar>
#include <QKeySequence>
#include <QRect>
#include <QMenu>
#include "QHotkey"
#include "wuake_window.h"
#include "wuake_tab_page.h"

WuakeWindow::WuakeWindow(QWidget *parent) :
    QDialog(parent, Qt::ToolTip | Qt::FramelessWindowHint)
    //QDialog(parent)
{
    resize(819, 475);

    mTrayIcon = new QSystemTrayIcon(this);
    mTrayIcon->setIcon(QIcon(":/res/images/app.png"));
    mTrayIcon->setToolTip(tr("Wuake\nDrop-down terminal for Windows"));
    QMenu* menu = new QMenu(this);
    mExitAct = menu->addAction(tr("Exit"));
    mTrayIcon->setContextMenu(menu);
    mTrayIcon->show();

    connect(menu, &QMenu::triggered, this, &WuakeWindow::onAction);
    connect(mTrayIcon, &QSystemTrayIcon::activated, this, &WuakeWindow::onTrayActive);

    mTabWidget = new WuakeTabWidget(this);

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
    if (!isHidden()) return;

    QDialog::show();
    mShowAnim.start();
}

void WuakeWindow::hide()
{
    if (isHidden()) return;
    if (mShowAnim.state() == QAbstractAnimation::Running ||
        mHideAnim.state() == QAbstractAnimation::Running) {
        return;
    }
    mHideAnim.start();
}

void WuakeWindow::initAnim()
{
    updateAnimRect();

    mShowAnim.setTargetObject(this);
    mShowAnim.setPropertyName("geometry");
    mShowAnim.setDuration(150);

    mHideAnim.setTargetObject(this);
    mHideAnim.setPropertyName("geometry");
    mHideAnim.setDuration(150);

    connect(&mHideAnim, &QPropertyAnimation::finished, this, &QDialog::hide);
}

void WuakeWindow::updateAnimRect()
{
    QRect screenSize = QApplication::desktop()->availableGeometry();
    int w = width();
    int h = height();
    int x = (screenSize.right() - w)/2;
    int y = 0;

    QRect rectStart(x, y - h, w, h);
    QRect rectEnd(x, y, w, h);

    mShowAnim.setStartValue(rectStart);
    mShowAnim.setEndValue(rectEnd);

    mHideAnim.setStartValue(rectEnd);
    mHideAnim.setEndValue(rectStart);
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
            QMessageBox::warning(this, tr("Warn"), QString(tr("Register hotkey %1 failed!").arg(it.value())));
        }
    }

}

void WuakeWindow::onHotkey()
{
    QHotkey* hotKey = qobject_cast<QHotkey*>(sender());
    QString hotkeyStr = hotKey->shortcut().toString();
    HotKeyCode keyCode = mHotkeys.key(hotkeyStr, HOTKEY_NONE);

    if (HOTKEY_SHOW_HIDE == keyCode) {
        if (isHidden()) {
            show();
            activateWindow();
        } else {
            hide();
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
