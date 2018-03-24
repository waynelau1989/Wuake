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
    QWidget(parent)
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

    connect(mCornerWidget->mBtnClose, SIGNAL(released()), this, SLOT(closeAllTabs()));
    connect(mTabBar, SIGNAL(tabBarClicked(int)), this, SLOT(setCurrentPage(int)));
}

WuakeTabWidget::~WuakeTabWidget()
{
    closeAllTabs();
}

bool WuakeTabWidget::isValidIndex(int index)
{
    if (index >= 0 && index < mTabBar->count()) {
        return true;
    }
    return false;
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
    WuakeTabPage* page = (WuakeTabPage*)mTabBar->tabData(index).toULongLong();
    if (page != mPagesLayout->currentWidget()) {
        mPagesLayout->setCurrentWidget(page);
        page->requestFocus();
    }
}

void WuakeTabWidget::addPage(const QString &title, WuakeTabPage *page)
{
    int index = mTabBar->addTab(title);
    mPagesLayout->addWidget(page);
    mTabBar->setTabData(index, QVariant((qint64)page));
}

void WuakeTabWidget::delPage(int index)
{
    if (!isValidIndex(index)) {
        return;
    }
    mTabBar->removeTab(index);
    WuakeTabPage* page = (WuakeTabPage*)mPagesLayout->widget(index);
    page->close();
    mPagesLayout->removeWidget(page);
}

void WuakeTabWidget::closeAllTabs()
{
    while (mTabBar->count() > 0) {
        delPage(mTabBar->count() - 1);
    }
}
