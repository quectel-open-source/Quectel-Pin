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

    // �������ƺ���
    void startScrolling(int speed = 100);  // ��ʼ�������ٶȣ�����/�룩
    void stopScrolling();                  // ֹͣ����
    bool isScrollingActive()
    {
        return isScrolling;
    };        // ������״̬
    void clearAll(); // ��ȫ�������״̬���ı�
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void scrollText();  // ��ʱ�����õĹ�������

private:
    QTimer* scrollTimer;  // ��ʱ������
    QString originalText; // ԭʼ�ı�����
    int textWidth;        // �ı����ؿ��
    int scrollPos;        // ��ǰ����λ��
    bool isScrolling;     // ����״̬��־
    int scrollOffset;
    int displayUnitWidth;
};

#endif // SMOOTHSCROLLLABEL_H