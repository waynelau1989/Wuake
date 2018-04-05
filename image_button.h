#pragma once

#include <QLabel>

class ImageButton : public QLabel
{
public:
    ImageButton(QWidget* parent = nullptr);

    void setNormalImg(const QString& url);
    void setHoverImg(const QString& url);
    void setPressedImg(const QString& url);

protected:
    void resizeEvent(QResizeEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    QString mNormalUrl;
    QString mHoverUrl;
    QString mPressedUrl;
};
