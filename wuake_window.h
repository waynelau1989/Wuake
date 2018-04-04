#pragma once

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QMap>
#include <QPropertyAnimation>
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
public:
    WuakeWindow(QWidget* parent = nullptr);
    ~WuakeWindow();

    void show();
    void hide();

protected:
    void resizeEvent(QResizeEvent* event);

protected slots:
    void onHotkey();

private:
    void initHotkeys();
    void initAnim();
    void updateAnimRect();

private:
    WuakeTabWidget* mTabWidget;
    QMap<HotKeyCode,QString> mHotkeys;

    QPropertyAnimation mShowAnim;
    QPropertyAnimation mHideAnim;
};
