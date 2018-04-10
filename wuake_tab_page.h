#pragma once

#include <QWidget>
#include <QLabel>
#include <QWindow>
#include <QProcess>
#include <QString>

typedef enum {
    PAGE_STATE_START,
    PAGE_STATE_CLOSE,
    PAGE_STATE_ERROR
} WuakePageState;

class WuakeTabPage : public QWidget
{
    Q_OBJECT
public:
    WuakeTabPage(QWidget* parent = nullptr);
    virtual ~WuakeTabPage();

    virtual void requestFocus();

    virtual void close();

    virtual void startProcess();

    virtual bool processRunning();

    QWindow* pageWindow();

public slots:
    void updateBackground();
    void updatePosition();
    void show();
    void hide();

signals:
    void stateChanged(WuakePageState state);

protected slots:
    void onError(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onStarted();

protected:
    virtual QString className() = 0;
    virtual QString titleName() = 0;

    bool event(QEvent *event);

protected:
    QProcess* mProcess;

private:
    WId mWId;
    HWND mHwnd;

    QLabel* mBgImg;

    QWindow* mWindow;
    QWidget* mWidget;

    bool mIsShowing;
};


class MinttyTabPage : public WuakeTabPage
{
public:
    MinttyTabPage(QWidget* parent = nullptr);
    ~MinttyTabPage();

    void startProcess();

protected:
    QString className();
    QString titleName();

private:
    void findMintty();

private:
    static quint64 sCounter;
    static QString sMinttyPath;

    QString mTitleName;
};
