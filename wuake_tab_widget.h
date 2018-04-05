#pragma once

#include <QStackedLayout>
#include <QPushButton>
#include <QTabBar>
#include <QTimer>
#include "wuake_tab_page.h"

class WuakeTabWidgetCorner : public QWidget
{
public:
    WuakeTabWidgetCorner(QWidget* parent = nullptr);
    ~WuakeTabWidgetCorner();

    QPushButton* mBtnClose;
    QPushButton* mBtnMinimize;

protected:
    void resizeEvent(QResizeEvent* event);
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
    void closeAll();

private slots:
    void onTimeout();

private:
    WuakeTabPage* findPageByIndex(int index);
    int findIndexByPage(WuakeTabPage* page);

    void focusCurrentPage(int delayMS = 500);

private:
    QStackedLayout* mPagesLayout;
    QTabBar* mTabBar;

    QTimer mTimer;

    WuakeTabWidgetCorner* mCornerWidget;

    bool mIsDestroying;
};
