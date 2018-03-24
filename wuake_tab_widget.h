#pragma once

#include <QStackedLayout>
#include <QPushButton>
#include <QTabBar>
#include "wuake_tab_page.h"

class WuakeTabWidgetCorner : public QWidget
{
public:
    WuakeTabWidgetCorner(QWidget* parent = nullptr);
    ~WuakeTabWidgetCorner();

    QPushButton* mBtnClose;
};


class WuakeTabWidget : public QWidget
{
    Q_OBJECT
public:
    WuakeTabWidget(QWidget* parent = nullptr);
    ~WuakeTabWidget();

    bool isValidIndex(int index);

public slots:
    void setCurrentPage(int index);

    void addPage(const QString& title, WuakeTabPage* page);

    void delPage(int index);

    void closeAllTabs();

private:
    QStackedLayout* mPagesLayout;
    QTabBar* mTabBar;

    WuakeTabWidgetCorner* mCornerWidget;

};
