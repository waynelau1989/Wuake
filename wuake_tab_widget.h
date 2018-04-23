#pragma once

#include <QStackedLayout>
#include <QPushButton>
#include <QTabBar>
#include <QMenu>
#include <QTimer>
#include "wuake_tab_page.h"

class WuakeTabWidgetCorner : public QWidget
{
    Q_OBJECT
public:
    WuakeTabWidgetCorner(QWidget* parent = nullptr);
    ~WuakeTabWidgetCorner();

    QPushButton* mBtnClose;
    QPushButton* mBtnMinimize;

protected:
    void resizeEvent(QResizeEvent* event);
};

class WuakeTabWidget;
class WuakeTabContexMenu : public QMenu
{
    Q_OBJECT
public:
    WuakeTabContexMenu(WuakeTabWidget* tabWidget);
    ~WuakeTabContexMenu();

    void setTabIndex(int index);

protected slots:
    void onTriggered(QAction* action);

private:
    WuakeTabWidget* mTabWidget;
    QAction* mCloseAct;

    int mTabIndex;
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

signals:
    void focusLost();

public slots:
    void setCurrentPage(int index);
    void onPageState(WuakePageState state);
    void closeAll();

private slots:
    void onTimeout();
    void onMenuRequested(const QPoint& pos);
    void onCheckFocus();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    bool event(QEvent *event);

private:
    WuakeTabPage* findPageByIndex(int index);
    WuakeTabPage* currentPage();
    int findIndexByPage(WuakeTabPage* page);

    void focusCurrentPage(int delayMS = 500);

private:
    QStackedLayout* mPagesLayout;
    QTabBar* mTabBar;

    QTimer mTimer;
    QTimer mFocusTimer;

    WuakeTabWidgetCorner* mCornerWidget;
    WuakeTabContexMenu* mContexMenu;

    bool mIsDestroying;
};
