#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>
#include <QResizeEvent>
#include <QtDebug>
#include "wuake_tab_widget.h"


WuakeTabWidgetCorner::WuakeTabWidgetCorner(QWidget *parent) :
    QWidget(parent)
{
    mBtnClose = new QPushButton(this);
    mBtnClose->setFocusPolicy(Qt::NoFocus);
    mBtnClose->setToolTip(tr("Close all tabs"));

    mBtnMinimize = new QPushButton(this);
    mBtnMinimize->setFocusPolicy(Qt::NoFocus);
    mBtnMinimize->setToolTip(tr("Hide window"));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 3, 0);
    layout->setSpacing(3);

    layout->addWidget(mBtnMinimize, 0, Qt::AlignVCenter);
    layout->addWidget(mBtnClose, 0, Qt::AlignVCenter);

    setLayout(layout);

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
}

WuakeTabWidgetCorner::~WuakeTabWidgetCorner()
{
}


WuakeTabContexMenu::WuakeTabContexMenu(WuakeTabWidget *tabWidget) :
    QMenu(tabWidget),
    mTabWidget(tabWidget),
    mTabIndex(-1)
{
    mCloseAct = addAction(tr("Close"));

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onTriggered(QAction*)));
}

WuakeTabContexMenu::~WuakeTabContexMenu()
{
}

void WuakeTabContexMenu::setTabIndex(int index)
{
    mTabIndex = index;
}

void WuakeTabContexMenu::onTriggered(QAction *action)
{
    if (mCloseAct == action) {
        mTabWidget->closePage(mTabIndex);
    }
}



void WuakeTabWidgetCorner::resizeEvent(QResizeEvent *event)
{
    int h = event->size().height();

    setFixedHeight(h);

    h = (int)(h * 0.75);

    QSize iconSize(h, h);
    QString style;

    mBtnMinimize->setFixedSize(iconSize);
    style.clear();
    style += "QPushButton {border-image: url(:/res/images/btn_minimize_normal.png)}";
    style += "QPushButton:hover {border-image: url(:/res/images/btn_minimize_hover.png)}";
    style += "QPushButton:pressed {border-image: url(:/res/images/btn_minimize_pressed.png)}";
    mBtnMinimize->setStyleSheet(style);


    mBtnClose->setFixedSize(iconSize);
    style.clear();
    style += "QPushButton {border-image: url(:/res/images/btn_close_normal.png)}";
    style += "QPushButton:hover {border-image: url(:/res/images/btn_close_hover.png)}";
    style += "QPushButton:pressed {border-image: url(:/res/images/btn_close_pressed.png)}";
    mBtnClose->setStyleSheet(style);
}



WuakeTabWidget::WuakeTabWidget(QWidget *parent) :
    QWidget(parent),
    mIsDestroying(false)
{
    setFocusPolicy(Qt::StrongFocus);
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    mPagesLayout = new QStackedLayout();
    mPagesLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addLayout(mPagesLayout);

    QHBoxLayout* bottomBarLayout = new QHBoxLayout();
    bottomBarLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addLayout(bottomBarLayout);

    mTabBar = new QTabBar(this);
    //mTabBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    mTabBar->setExpanding(false);
    mTabBar->setDrawBase(false);
    mTabBar->setMovable(true);
    mTabBar->setUsesScrollButtons(true);
    bottomBarLayout->addWidget(mTabBar, 1, Qt::AlignLeft|Qt::AlignTop);

    mCornerWidget = new WuakeTabWidgetCorner(this);
    bottomBarLayout->addWidget(mCornerWidget, 0, Qt::AlignTop);

    mContexMenu = new WuakeTabContexMenu(this);
    mTabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    mTabBar->installEventFilter(this);

    connect(mCornerWidget->mBtnClose, SIGNAL(released()), this, SLOT(closeAll()));
    connect(mCornerWidget->mBtnMinimize, SIGNAL(released()), parentWidget(), SLOT(hide()));
    connect(mTabBar, SIGNAL(tabBarClicked(int)), this, SLOT(setCurrentPage(int)));
    connect(mTabBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onMenuRequested(QPoint)));
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

WuakeTabWidget::~WuakeTabWidget()
{
    closeAll();
}

bool WuakeTabWidget::isValidIndex(int index)
{
    if (index >= 0 && index < mTabBar->count()) {
        return true;
    }
    return false;
}

bool WuakeTabWidget::hasTab()
{
    if (mTabBar->count() > 0) {
        return true;
    }
    return false;
}

void WuakeTabWidget::onPageState(WuakePageState state)
{
    WuakeTabPage* page = qobject_cast<WuakeTabPage*>(sender());
    switch (state) {
    case PAGE_STATE_ERROR:
    case PAGE_STATE_CLOSE:
        delPage(page);
        if (mIsDestroying) {
            closeAll();
        }
        break;
    case PAGE_STATE_START:
        if (mTabBar->count() == 1) {
            focusCurrentPage(500);
        }
        break;
    default:
        break;
    }
}

void WuakeTabWidget::onTimeout()
{
    mTimer.stop();
    parentWidget()->activateWindow();
    WuakeTabPage* page = reinterpret_cast<WuakeTabPage*>(mPagesLayout->currentWidget());
    if (nullptr != page) page->requestFocus();
}

void WuakeTabWidget::onMenuRequested(const QPoint &pos)
{
    qDebug() << "contextMenuRequest";
    currentPage()->enableFocus(true);
    int index = mTabBar->tabAt(pos);
    if (!isValidIndex(index)) return;
    mContexMenu->setTabIndex(index);
    mContexMenu->popup(mTabBar->mapToGlobal(pos));
}

bool WuakeTabWidget::eventFilter(QObject *watched, QEvent *event)
{
    QEvent::Type eventType = event->type();
    //qDebug() << "Tab Event:" << eventType;
    if (mTabBar == watched) {
        if (QEvent::MouseButtonPress == eventType ||
            QEvent::MouseButtonRelease == eventType)
        {
            QMouseEvent* mouseEvent = (QMouseEvent*)(event);
            if (mouseEvent->button() == Qt::RightButton) {
                currentPage()->enableFocus(false);
            }
        } else if (QEvent::Leave == eventType) {
            currentPage()->enableFocus(true);
        }
    }
    return false;
}

void WuakeTabWidget::setCurrentPage(int index)
{
    //qDebug("setCurrentPage:%d", index);
    if (!isValidIndex(index)) {
        return;
    }
    if (mTabBar->currentIndex() != index) {
        mTabBar->setCurrentIndex(index);
    }
    WuakeTabPage* page = findPageByIndex(index);
    if (page != mPagesLayout->currentWidget()) {
        mPagesLayout->setCurrentWidget(page);
    }
    if (page->isEnableFocus()) {
        focusCurrentPage(250);
    }
}

void WuakeTabWidget::addPage(const QString &title, WuakeTabPage *page)
{
    int index = mTabBar->addTab(title);
    mPagesLayout->addWidget(page);
    mTabBar->setTabData(index, QVariant((quint64)page));
    connect(page, SIGNAL(stateChanged(WuakePageState)), this, SLOT(onPageState(WuakePageState)));
    page->startProcess();
    mIsDestroying = false;
    setCurrentPage(index);
}

void WuakeTabWidget::delPageByIndex(int index)
{
    if (!isValidIndex(index)) {
        return;
    }
    WuakeTabPage* page = findPageByIndex(index);
    mTabBar->removeTab(index);
    mPagesLayout->removeWidget(page);
    delete page;

    if (mIsDestroying) return;

    if (mTabBar->count() == index) {
        setCurrentPage(index - 1);
    } else if (mTabBar->count() > 0) {
        setCurrentPage(index);
    }
}

void WuakeTabWidget::delPage(WuakeTabPage *page)
{
    delPageByIndex(findIndexByPage(page));
}

void WuakeTabWidget::closePage(int index)
{
    if (!isValidIndex(index)) {
        return;
    }
    WuakeTabPage* page = findPageByIndex(index);
    page->close();
}


void WuakeTabWidget::newTab()
{
    WuakeTabPage* page = new MinttyTabPage(this);
    addPage("mintty", page);
}

void WuakeTabWidget::switchToLeft()
{
    int index = mTabBar->currentIndex();
    if (--index < 0) {
        index = mTabBar->count() - 1;
    }
    setCurrentPage(index);
}

void WuakeTabWidget::switchToRight()
{
    int index = mTabBar->currentIndex();
    if (++index >= mTabBar->count()) {
        index = 0;
    }
    setCurrentPage(index);
}

void WuakeTabWidget::moveToLeft()
{
    int index = mTabBar->currentIndex();
    if (0 == index) return;
    mTabBar->moveTab(index, index - 1);
}

void WuakeTabWidget::moveToRight()
{
    int index = mTabBar->currentIndex();
    if (index == mTabBar->count() - 1) return;
    mTabBar->moveTab(index, index + 1);
}

void WuakeTabWidget::closeAll()
{
    if (mPagesLayout->isEmpty()) {
        qApp->quit();
        return;
    }
    closePage(mTabBar->count() - 1);
    mIsDestroying = true;
}

WuakeTabPage* WuakeTabWidget::findPageByIndex(int index)
{
    WuakeTabPage* page = (WuakeTabPage*)mTabBar->tabData(index).toULongLong();
    return page;
}

WuakeTabPage* WuakeTabWidget::currentPage()
{
    return (WuakeTabPage*)mPagesLayout->currentWidget();
}

int WuakeTabWidget::findIndexByPage(WuakeTabPage *page)
{
    for (int i=mTabBar->count()-1; i>=0; --i) {
        WuakeTabPage* tmpPage = findPageByIndex(i);
        if (page == tmpPage) {
            return i;
        }
    }
    return -1;
}

void WuakeTabWidget::focusCurrentPage(int delayMS)
{
    if (delayMS < 1) {
        onTimeout();
        return;
    }

    if (mTimer.isActive()) {
        mTimer.stop();
    }
    mTimer.start(delayMS);
}
