#include <QMessageBox>
#include <QVBoxLayout>
#include <QTabBar>
#include <QKeySequence>
#include "QHotkey"
#include "wuake_window.h"
#include "wuake_tab_page.h"

WuakeWindow::WuakeWindow(QWidget *parent) :
    //QDialog(parent, Qt::ToolTip | Qt::FramelessWindowHint)
    QDialog(parent)
{
    resize(819, 475);
    mTabWidget = new WuakeTabWidget(this);

    QVBoxLayout* layoutRoot = new QVBoxLayout(this);
    layoutRoot->setContentsMargins(0, 0, 0, 0);
    layoutRoot->addWidget(mTabWidget);

    initHotkeys();

    mTabWidget->newTab();
}

WuakeWindow::~WuakeWindow()
{
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
