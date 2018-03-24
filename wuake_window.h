#pragma once

#include <QWidget>
#include <QDialog>
#include "wuake_tab_widget.h"

class WuakeWindow : public QDialog
{
public:
    WuakeWindow(QWidget* parent = nullptr);
    ~WuakeWindow();

private:
    WuakeTabWidget* mTabWidget;
};
