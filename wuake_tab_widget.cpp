#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>
#include "wuake_tab_widget.h"


WuakeTabWidgetCorner::WuakeTabWidgetCorner(QWidget *parent) :
    QWidget(parent)
{
    mBtnClose = new QPushButton(this);
    mBtnClose->setText("C");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(mBtnClose);

    setLayout(layout);

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

WuakeTabWidgetCorner::~WuakeTabWidgetCorner()
{
}


WuakeTabWidget::WuakeTabWidget(QWidget *parent) :
    QWidget(parent),
    mIsDestroying(false)
{
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
    mTabBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    mTabBar->setExpanding(false);
    mTabBar->setDrawBase(false);
    mTabBar->setMovable(true);
    bottomBarLayout->addWidget(mTabBar, 1, Qt::AlignLeft|Qt::AlignTop);

    mCornerWidget = new WuakeTabWidgetCorner(this);
    bottomBarLayout->addWidget(mCornerWidget, 0, Qt::AlignTop);

    connect(mCornerWidget->mBtnClose, SIGNAL(released()), this, SLOT(destroy()));
    connect(mTabBar, SIGNAL(tabBarClicked(int)), this, SLOT(setCurrentPage(int)));
}

WuakeTabWidget::~WuakeTabWidget()
{
    destroy();
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
    case PAGE_STATE_CLOSE:
        delPage(page);
        if (mIsDestroying) {
            destroy();
        }
        break;
    case PAGE_STATE_START:
        break;
    default:
        break;
    }
}

void WuakeTabWidget::setCurrentPage(int index)
{
    qDebug("setCurrentPage:%d", index);
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
    page->requestFocus();
}

void WuakeTabWidget::addPage(const QString &title, WuakeTabPage *page)
{
    int index = mTabBar->addTab(title);
    mPagesLayout->addWidget(page);
    mTabBar->setTabData(index, QVariant((quint64)page));
    connect(page, SIGNAL(stateChanged(WuakePageState)), this, SLOT(onPageState(WuakePageState)));
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

void WuakeTabWidget::destroy()
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
