#include "SmoothScrollLabel.h"

SmoothScrollLabel::SmoothScrollLabel(QWidget* parent)
    : QLabel(parent), textWidth(0), scrollOffset(0), isScrolling(false)
{
    // ��ʼ����ʱ��
    scrollTimer = new QTimer(this);
    connect(scrollTimer, &QTimer::timeout, this, &SmoothScrollLabel::scrollText);

    // ���ñ�ǩ��������
    setAlignment(Qt::AlignVCenter);
    setMinimumWidth(100);
}


// ��ʼ����Ч��
void SmoothScrollLabel::startScrolling(int speed)
{
    if (isScrolling) return;

    // ����ԭʼ�ı���׼������
    originalText = text();
    textWidth = fontMetrics().horizontalAdvance(originalText);

    // ���ù�������
    scrollOffset = 0;
    isScrolling = true;

    // ������ʾ��Ԫ��ȣ�������ࣩ
    displayUnitWidth = textWidth + 40; // �ı����

    // �����ٶȼ���ʱ����
    const int pixelsPerInterval = 2; // ÿ���ƶ���������
    int interval = (speed > 0) ? (1000 * pixelsPerInterval) / speed : 30;

    // ������С�������
    interval = qMax(10, qMin(interval, 200));

    scrollTimer->start(interval);
}

// ֹͣ����
void SmoothScrollLabel::stopScrolling()
{
    if (!isScrolling) return;

    scrollTimer->stop();
    isScrolling = false;

    // �ָ�ԭʼ�ı���ʾ
    setText(originalText);
    update();
}

// �����ı��ĺ���ʵ��
void SmoothScrollLabel::scrollText()
{
    // ����������ı�����Ϊ�����ƶ���
    scrollOffset += 2;

    // ���ı���������һ����Ԫ���ʱ������ƫ��
    if (scrollOffset >= displayUnitWidth) {
        scrollOffset = 0;
    }

    update(); // �����ػ�
}

// �����¼�����
void SmoothScrollLabel::paintEvent(QPaintEvent* event)
{
    // δ����ʱʹ��Ĭ�ϻ���
    if (!isScrolling) {
        QLabel::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    // ���Ʊ���
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // ���㴹ֱ����λ��
    int yPos = (height() - fontMetrics().height()) / 2 + fontMetrics().ascent();

    // �����ı���ɫ
    painter.setPen(palette().color(QPalette::WindowText));

    // �����һ���ɼ��ı���Ԫ����ʼλ��
    int startX = -scrollOffset;

    // ѭ�������㹻���ı���Ԫ�������ؼ�
    while (startX < width()) {
        painter.drawText(startX, yPos, originalText);
        startX += displayUnitWidth; // �Ƶ���һ���ı���Ԫ
    }

    // �ڱ߽紦��ӵ��뵭��Ч����ʹ���ɸ�ƽ��
    const int fadeWidth = 30; // ���뵭��������

    // ��൭��
    QLinearGradient leftFade(0, 0, fadeWidth, 0);
    leftFade.setColorAt(0, palette().color(backgroundRole()));
    leftFade.setColorAt(1, Qt::transparent);
    painter.fillRect(0, 0, fadeWidth, height(), leftFade);

    // �Ҳ൭��
    QLinearGradient rightFade(width() - fadeWidth, 0, width(), 0);
    rightFade.setColorAt(0, Qt::transparent);
    rightFade.setColorAt(1, palette().color(backgroundRole()));
    painter.fillRect(width() - fadeWidth, 0, fadeWidth, height(), rightFade);
}

// �ߴ�ı��¼�
void SmoothScrollLabel::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    // ����Ҫ���⴦�������߼����Զ���Ӧ�³ߴ�
}

void SmoothScrollLabel::clearAll()
{
    // 1. ֹͣ����
    stopScrolling();

    // 2. ���ԭʼ�ı�
    originalText.clear();
    textWidth = 0;
    scrollOffset = 0;

    // 3. ���û����������
    QLabel::clear();

    // 4. ǿ���ػ�
    update();
}