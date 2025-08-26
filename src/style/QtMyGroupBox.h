#pragma once

#include <QObject>
#include <QGroupBox>
#include <QPainter>
#include <QStyle>
class QtMyGroupBox  : public QGroupBox
{
	Q_OBJECT

public:
	QtMyGroupBox(QWidget *parent);
	~QtMyGroupBox();
public:
	void paintEvent(QPaintEvent *event) override;
	/*�����ڲ���Ӱ�߿�
	painter:���봰�ڻ�ͼ����
	nWidget:�߿���
	pTopLeft:���봰���󶥵�
	pTopRight�����봰���Ҷ���
	pBottomLeft�����봰����׵�
	pButtomRight�����봰���ҵ׵�
	*/
	void getInternalFrame(QPainter& painter,int nWidget,QPoint pTopLeft, QPoint pTopRight, QPoint pBottomLeft, QPoint pButtomRight);
};
