#ifndef SMOOTHSCROLLLABEL_H
#define SMOOTHSCROLLLABEL_H

#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QStyleOption>

class SmoothScrollLabel : public QLabel
{
    Q_OBJECT

public:
    explicit SmoothScrollLabel(QWidget* parent = nullptr);

    // 滚动控制函数
    void startScrolling(int speed = 100);  // 开始滚动（速度：像素/秒）
    void stopScrolling();                  // 停止滚动
    bool isScrollingActive()
    {
        return isScrolling;
    };        // 检查滚动状态
    void clearAll(); // 完全清除滚动状态和文本
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void scrollText();  // 定时器调用的滚动函数

private:
    QTimer* scrollTimer;  // 定时器对象
    QString originalText; // 原始文本内容
    int textWidth;        // 文本像素宽度
    int scrollPos;        // 当前滚动位置
    bool isScrolling;     // 滚动状态标志
    int scrollOffset;
    int displayUnitWidth;
};

#endif // SMOOTHSCROLLLABEL_H