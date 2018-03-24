#pragma once

#include <QWidget>
#include <QWindow>
#include <QProcess>
#include <QString>

class WuakeTabPage : public QWidget
{
    Q_OBJECT
public:
    WuakeTabPage(QWidget* parent = nullptr);
    ~WuakeTabPage();

    void requestFocus();

    void close();

signals:
    void closed();
    void started();

protected:
    virtual QString className() = 0;
    virtual QString titleName() = 0;

private:
    HWND mHwnd;

    QString mTitleName;

    QProcess* mProcess;
    QWindow* mWindow;
    QWidget* mWidget;
};
