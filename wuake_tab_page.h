#pragma once

#include <QWidget>
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

    void enableFocus(bool enable);

    bool isEnableFocus();

    bool isFocused();

signals:
    void stateChanged(WuakePageState state);

protected slots:
    void onError(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onStarted();

protected:
    bool event(QEvent *event);
    virtual QString className() = 0;
    virtual QString titleName() = 0;

protected:
    QProcess* mProcess;

private:
    HWND mHwnd;

    QWindow* mWindow;
    QWidget* mWidget;

    bool mEnFocus;
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
