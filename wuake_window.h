#pragma once

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QMap>
#include <QPropertyAnimation>
#include <QSystemTrayIcon>
#include <QAction>
#include "wuake_tab_widget.h"

typedef enum {
    HOTKEY_NONE,
    HOTKEY_SHOW_HIDE,
    HOTKEY_NEW_TAB,
    HOTKEY_SWITCH_LEFT_TAB,
    HOTKEY_SWITCH_RIGHT_TAB,
    HOTKEY_MOVE_ACTIVE_TAB_LEFT,
    HOTKEY_MOVE_ACTIVE_TAB_RIGHT,
} HotKeyCode;



class WuakeWindow : public QDialog
{
    Q_OBJECT
public:
    WuakeWindow(QWidget* parent = nullptr);
    ~WuakeWindow();

public slots:
    void show();
    void hide();

protected:
    void resizeEvent(QResizeEvent* event);

protected slots:
    void onHotkey();
    void onAction(QAction* action);
    void onTrayActive(QSystemTrayIcon::ActivationReason reason);

private:
    void initHotkeys();
    void initAnim();
    void updateAnimRect();

private:
    WuakeTabWidget* mTabWidget;
    QMap<HotKeyCode,QString> mHotkeys;
    QSystemTrayIcon* mTrayIcon;
    QAction* mExitAct;

    QPropertyAnimation mShowAnim;
    QPropertyAnimation mHideAnim;
};
