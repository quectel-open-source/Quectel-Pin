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

	// 添加曲线
	void addCurve(const QString &name, const QColor &color = Qt::blue);

	// 添加数据点
	void addDataPoint(const QString &curveName, double value);

	// 设置时间窗口大小（秒）
	void setTimeWindow(int seconds);

	// 设置Y轴范围
	void setYRange(double min, double max);

	// 设置曲线可见性
	void setCurveVisible(const QString &curveName, bool visible);

	// 清除所有数据
	void clearAllData();

public slots:
	void startAutoUpdate();
	void stopAutoUpdate();
	void resetView();//用户手动重置视图

private slots:
	void updatePlot();

	void onInteractionTimeout();
protected:
	// 重写事件处理函数
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

private:
	// 创建字体对象
	QFont chartFont;

	QTimer *m_updateTimer;
	QTimer *m_interactionTimer;
	int m_timeWindow = 30; // 默认显示30秒数据
	bool m_userInteracting = false;
	bool m_mousePressed = false; // 添加鼠标按下状态跟踪

	QMap<QString, QCPGraph*> m_curves;
	QMap<QString, QVector<QCPGraphData>> m_dataBuffers;
};
