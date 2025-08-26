#include "SmoothScrollLabel.h"

SmoothScrollLabel::SmoothScrollLabel(QWidget* parent)
    : QLabel(parent), textWidth(0), scrollOffset(0), isScrolling(false)
{
    // 初始化定时器
    scrollTimer = new QTimer(this);
    connect(scrollTimer, &QTimer::timeout, this, &SmoothScrollLabel::scrollText);

    // 设置标签基础属性
    setAlignment(Qt::AlignVCenter);
    setMinimumWidth(100);
}


// 开始滚动效果
void SmoothScrollLabel::startScrolling(int speed)
{
    if (isScrolling) return;

    // 保存原始文本并准备滚动
    originalText = text();
    textWidth = fontMetrics().horizontalAdvance(originalText);

    // 重置滚动参数
    scrollOffset = 0;
    isScrolling = true;

    // 计算显示单元宽度（包含间距）
    displayUnitWidth = textWidth + 40; // 文本间距

    // 根据速度计算时间间隔
    const int pixelsPerInterval = 2; // 每次移动的像素数
    int interval = (speed > 0) ? (1000 * pixelsPerInterval) / speed : 30;

    // 限制最小和最大间隔
    interval = qMax(10, qMin(interval, 200));

    scrollTimer->start(interval);
}

// 停止滚动
void SmoothScrollLabel::stopScrolling()
{
    if (!isScrolling) return;

    scrollTimer->stop();
    isScrolling = false;

    // 恢复原始文本显示
    setText(originalText);
    update();
}

// 滚动文本的核心实现
void SmoothScrollLabel::scrollText()
{
    // 向左滚动（文本表现为向右移动）
    scrollOffset += 2;

    // 当文本滚动超过一个单元宽度时，重置偏移
    if (scrollOffset >= displayUnitWidth) {
        scrollOffset = 0;
    }

    update(); // 触发重绘
}

// 绘制事件处理
void SmoothScrollLabel::paintEvent(QPaintEvent* event)
{
    // 未滚动时使用默认绘制
    if (!isScrolling) {
        QLabel::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    // 绘制背景
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 计算垂直居中位置
    int yPos = (height() - fontMetrics().height()) / 2 + fontMetrics().ascent();

    // 设置文本颜色
    painter.setPen(palette().color(QPalette::WindowText));

    // 计算第一个可见文本单元的起始位置
    int startX = -scrollOffset;

    // 循环绘制足够的文本单元以填满控件
    while (startX < width()) {
        painter.drawText(startX, yPos, originalText);
        startX += displayUnitWidth; // 移到下一个文本单元
    }

    // 在边界处添加淡入淡出效果，使过渡更平滑
    const int fadeWidth = 30; // 淡入淡出区域宽度

    // 左侧淡入
    QLinearGradient leftFade(0, 0, fadeWidth, 0);
    leftFade.setColorAt(0, palette().color(backgroundRole()));
    leftFade.setColorAt(1, Qt::transparent);
    painter.fillRect(0, 0, fadeWidth, height(), leftFade);

    // 右侧淡出
    QLinearGradient rightFade(width() - fadeWidth, 0, width(), 0);
    rightFade.setColorAt(0, Qt::transparent);
    rightFade.setColorAt(1, palette().color(backgroundRole()));
    painter.fillRect(width() - fadeWidth, 0, fadeWidth, height(), rightFade);
}

// 尺寸改变事件
void SmoothScrollLabel::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    // 不需要特殊处理，绘制逻辑会自动适应新尺寸
}

void SmoothScrollLabel::clearAll()
{
    // 1. 停止滚动
    stopScrolling();

    // 2. 清空原始文本
    originalText.clear();
    textWidth = 0;
    scrollOffset = 0;

    // 3. 调用基类清除方法
    QLabel::clear();

    // 4. 强制重绘
    update();
}