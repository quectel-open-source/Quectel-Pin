#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include "qcustomplot.h"

class TimeSeriesPlot : public QCustomPlot
{
	Q_OBJECT
public:
	explicit TimeSeriesPlot(QWidget *parent = nullptr);

	// �������
	void addCurve(const QString &name, const QColor &color = Qt::blue);

	// ������ݵ�
	void addDataPoint(const QString &curveName, double value);

	// ����ʱ�䴰�ڴ�С���룩
	void setTimeWindow(int seconds);

	// ����Y�᷶Χ
	void setYRange(double min, double max);

	// �������߿ɼ���
	void setCurveVisible(const QString &curveName, bool visible);

	// �����������
	void clearAllData();

public slots:
	void startAutoUpdate();
	void stopAutoUpdate();
	void resetView();//�û��ֶ�������ͼ

private slots:
	void updatePlot();

	void onInteractionTimeout();
protected:
	// ��д�¼�������
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

private:
	// �����������
	QFont chartFont;

	QTimer *m_updateTimer;
	QTimer *m_interactionTimer;
	int m_timeWindow = 30; // Ĭ����ʾ30������
	bool m_userInteracting = false;
	bool m_mousePressed = false; // �����갴��״̬����

	QMap<QString, QCPGraph*> m_curves;
	QMap<QString, QVector<QCPGraphData>> m_dataBuffers;
};
