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
    bool hasTab();

    void addPage(const QString& title, WuakeTabPage* page);
    void delPageByIndex(int index);
    void delPage(WuakeTabPage* page);
    void closePage(int index);

    void newTab();
    void switchToLeft();
    void switchToRight();
    void moveToLeft();
    void moveToRight();

public slots:
    void setCurrentPage(int index);
    void onPageState(WuakePageState state);
    void destroy();

private:
    WuakeTabPage* findPageByIndex(int index);
    int findIndexByPage(WuakeTabPage* page);

private:
    QStackedLayout* mPagesLayout;
    QTabBar* mTabBar;

    WuakeTabWidgetCorner* mCornerWidget;

    bool mIsDestroying;
};
