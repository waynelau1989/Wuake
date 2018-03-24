#include <QVBoxLayout>
#include <QTabBar>
#include "wuake_window.h"
#include "wuake_tab_page.h"

WuakeWindow::WuakeWindow(QWidget *parent) :
    //QDialog(parent, Qt::ToolTip | Qt::FramelessWindowHint)
    QDialog(parent)
{
    mTabWidget = new WuakeTabWidget(this);

    QVBoxLayout* layoutRoot = new QVBoxLayout(this);
    layoutRoot->setContentsMargins(0, 0, 0, 0);
    layoutRoot->addWidget(mTabWidget);

    WuakeTabPage* page = new WuakeTabPage(mTabWidget);
    mTabWidget->addPage("tab", page);
    mTabWidget->addPage("tab2", new WuakeTabPage(mTabWidget));
    mTabWidget->addPage("tab3", new WuakeTabPage(mTabWidget));

    QString styleStr;
    //styleStr += "QTabWidget::pane {background-color:red;padding-right:-2px;margin-right:-2px;border:none}";
    //styleStr += "QTabWidget::pane {margin-bottom:-5px;border:none}";
    //mTabWidget->setStyleSheet(styleStr);
}

WuakeWindow::~WuakeWindow()
{
}
