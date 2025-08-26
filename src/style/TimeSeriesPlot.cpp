// TimeSeriesPlot.cpp
#include "TimeSeriesPlot.h"
#include <QDebug>

TimeSeriesPlot::TimeSeriesPlot(QWidget *parent)
	: QCustomPlot(parent)
{
	// 设置更新定时器（每5秒触发一次）
	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(5000);
	connect(m_updateTimer, &QTimer::timeout, this, &TimeSeriesPlot::updatePlot);

	// 设置交互检测定时器
	m_interactionTimer = new QTimer(this);
	m_interactionTimer->setSingleShot(true);
	m_interactionTimer->setInterval(3000); // 100ms后恢复更新
	connect(m_interactionTimer, &QTimer::timeout, this, &TimeSeriesPlot::onInteractionTimeout);

	// ============== 字体设置 ==============
	// 尝试使用首选字体
	chartFont.setFamily("Microsoft YaHei UI");

	// 设置字体大小和样式
	chartFont.setPointSize(10);
	chartFont.setBold(false);

	// ============== 样式设置 ==============
	// 设置深灰色背景 (#282A33)
	QColor backgroundColor(40, 42, 51); // #282A33
	setBackground(backgroundColor);

	// 设置坐标轴颜色为白色
	QColor axisColor(Qt::white);

	// 设置坐标轴字体
	xAxis->setLabelFont(chartFont);
	yAxis->setLabelFont(chartFont);
	xAxis->setTickLabelFont(chartFont);
	yAxis->setTickLabelFont(chartFont);

	xAxis->setBasePen(QPen(axisColor, 1));
	yAxis->setBasePen(QPen(axisColor, 1));
	xAxis->setTickPen(QPen(axisColor, 1));
	yAxis->setTickPen(QPen(axisColor, 1));
	xAxis->setSubTickPen(QPen(axisColor, 1));
	yAxis->setSubTickPen(QPen(axisColor, 1));
	xAxis->setTickLabelColor(axisColor);
	yAxis->setTickLabelColor(axisColor);
	xAxis->setLabelColor(axisColor);
	yAxis->setLabelColor(axisColor);

	// 设置网格线颜色（浅灰色）
	QColor gridColor(100, 100, 100);
	xAxis->grid()->setPen(QPen(gridColor, 1, Qt::DotLine));
	yAxis->grid()->setPen(QPen(gridColor, 1, Qt::DotLine));

	// 设置图例样式
	legend->setFont(chartFont); // 设置图例字体
	legend->setBorderPen(QPen(axisColor, 1)); // 白色边框
	legend->setTextColor(axisColor); // 白色文字
	legend->setBrush(backgroundColor); // 深灰色背景
	legend->setSelectedBorderPen(QPen(axisColor, 2)); // 选中时加粗白色边框

	// ============== 坐标轴配置 ==============
	// 配置坐标轴标签
	xAxis->setLabel("时间");

	// 设置X轴为日期时间轴
	QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
	dateTicker->setDateTimeFormat("hh:mm:ss");
	xAxis->setTicker(dateTicker);

	yAxis->setRange(0, 100);


	// 初始时间窗口
	setTimeWindow(m_timeWindow);

	// 启用交互功能 - 仅启用X轴拖动和缩放
	setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	axisRect()->setRangeDragAxes(xAxis, nullptr); // 只拖动X轴
	axisRect()->setRangeZoomAxes(xAxis, nullptr); // 只缩放X轴
	setMouseTracking(true); // 启用鼠标追踪

	// 初始化图例
	legend->setVisible(true);
	legend->setFont(chartFont);
	axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
}

void TimeSeriesPlot::addCurve(const QString &name, const QColor &color)
{
	if (m_curves.contains(name)) return;

	QCPGraph *newCurve = QCustomPlot::addGraph();
	newCurve->setName(name);
	newCurve->setPen(QPen(color, 2));
	newCurve->setLineStyle(QCPGraph::lsLine);
	newCurve->setScatterStyle(QCPScatterStyle::ssDisc);

	m_curves.insert(name, newCurve);
	m_dataBuffers.insert(name, QVector<QCPGraphData>());
}

void TimeSeriesPlot::addDataPoint(const QString &curveName, double value)
{
	if (!m_curves.contains(curveName)) {
		qWarning() << "曲线不存在:" << curveName;
		return;
	}

	// 获取当前时间（自1970年以来的秒数）
	double time = QDateTime::currentDateTime().toSecsSinceEpoch();

	// 添加到缓冲区
	QCPGraphData data;
	data.key = time;
	data.value = value;
	m_dataBuffers[curveName].append(data);
}

void TimeSeriesPlot::setTimeWindow(int seconds)
{
	m_timeWindow = seconds;

	// 设置初始X轴范围
	double now = QDateTime::currentDateTime().toSecsSinceEpoch();
	xAxis->setRange(now - m_timeWindow/2, now+m_timeWindow/2);
}

void TimeSeriesPlot::setYRange(double min, double max)
{
	yAxis->setRange(min, max);
}

void TimeSeriesPlot::setCurveVisible(const QString &curveName, bool visible)
{
	if (m_curves.contains(curveName)) {
		m_curves[curveName]->setVisible(visible);
	}
}

void TimeSeriesPlot::clearAllData()
{
	for (auto curve : m_curves) {
		curve->data()->clear();
	}
	for (auto &buffer : m_dataBuffers) {
		buffer.clear();
	}
	replot();
}

void TimeSeriesPlot::startAutoUpdate()
{
	m_updateTimer->start();
}

void TimeSeriesPlot::stopAutoUpdate()
{
	m_updateTimer->stop();
}

void TimeSeriesPlot::updatePlot()
{
	double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();

	// 更新所有曲线 - 总是更新数据
	for (auto curveIt = m_curves.begin(); curveIt != m_curves.end(); ++curveIt) {
		const QString &name = curveIt.key();
		QCPGraph *curve = curveIt.value();

		if (!m_dataBuffers.contains(name) || m_dataBuffers[name].isEmpty())
			continue;

		// 添加新数据
		curve->data()->add(m_dataBuffers[name]);
		m_dataBuffers[name].clear();
	}

	// 只有在没有用户交互时更新范围
	if (!m_userInteracting) {
		// 更新X轴范围（显示最新时间窗口）
		xAxis->setRange(currentTime - m_timeWindow/2, currentTime + m_timeWindow / 2);
	}

	// 总是重绘图表以显示新数据
	replot();
}

void TimeSeriesPlot::onInteractionTimeout()
{
	// 交互结束，恢复自动更新
	m_userInteracting = false;
	qDebug() << "Interactivity ended, restoring auto-update";

	// 特殊处理：当用户拖动结束时，恢复到最新视图
	double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
	xAxis->setRange(currentTime - m_timeWindow / 2, currentTime + m_timeWindow / 2);
	replot();
}
// 重写鼠标事件，跟踪用户交互
void TimeSeriesPlot::mousePressEvent(QMouseEvent *event)
{
	QCustomPlot::mousePressEvent(event);
	if (event->button() == Qt::LeftButton) {
		m_mousePressed = true;
		m_userInteracting = true;
		m_interactionTimer->stop(); // 停止超时计时器
		qDebug() << "Mouse press - interactivity started";
	}
}

void TimeSeriesPlot::mouseMoveEvent(QMouseEvent *event)
{
	QCustomPlot::mouseMoveEvent(event);
	if (m_mousePressed && !m_userInteracting) {
		// 如果鼠标移动时已经按下但还没标记为交互，则开始交互
		m_userInteracting = true;
		qDebug() << "Mouse drag started - interactivity active";
	}

	static QCPItemTracer *tracer = nullptr;
	static QCPItemText *textLabel = nullptr;

	if (!tracer) {
		tracer = new QCPItemTracer(this);
		tracer->setVisible(false);
		tracer->setStyle(QCPItemTracer::tsCircle);
		tracer->setPen(QPen(Qt::red));
		tracer->setBrush(Qt::red);
		tracer->setSize(10);

		textLabel = new QCPItemText(this);
		textLabel->setFont(chartFont);
		textLabel->setVisible(false);
		textLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
		textLabel->position->setParentAnchor(tracer->position);
		textLabel->position->setCoords(10, -10);
		textLabel->setPen(QPen(Qt::black));
		textLabel->setBrush(QBrush(Qt::white));
		textLabel->setPadding(QMargins(2, 2, 2, 2));
		textLabel->setText("");
	}

	double mouseX = this->xAxis->pixelToCoord(event->pos().x());
	double mouseY = this->yAxis->pixelToCoord(event->pos().y());
	bool foundPoint = false;

	// 遍历所有曲线
	for (int i = 0; i < this->graphCount(); ++i) {
		QCPGraph *graph = this->graph(i);
		if (!graph || !graph->visible())
			continue;

		// 使用新版数据访问API
		auto dataContainer = graph->data();
		if (!dataContainer || dataContainer->isEmpty())
			continue;

		// 查找最接近的点 (使用迭代器)
		double closestDistance = std::numeric_limits<double>::max();
		double closestX = 0;
		double closestY = 0;

		// 获取数据迭代器
		auto it = dataContainer->constBegin();
		auto end = dataContainer->constEnd();

		// 遍历所有数据点
		while (it != end) {
			double distance = std::abs(it->key - mouseX);
			if (distance < closestDistance) {
				closestDistance = distance;
				closestX = it->key;
				closestY = it->value;
			}
			++it;
		}

		// 计算屏幕距离
		QPointF pixelPoint = graph->coordsToPixels(closestX, closestY);
		double screenDistance = QLineF(event->pos(), pixelPoint).length();

		// 检查是否在阈值范围内
		if (screenDistance < 15) {
			tracer->setGraph(graph);
			QString ss = graph->name();
			tracer->setGraphKey(closestX);
			tracer->setVisible(true);

			textLabel->setText(ss + QString(":%1%").arg(closestY, 0, 'f', 2));
			textLabel->setVisible(true);

			foundPoint = true;
			break; // 找到第一个符合条件的点即可
		}
	}

	if (!foundPoint) {
		tracer->setVisible(false);
		textLabel->setVisible(false);
	}

	this->replot();
}

void TimeSeriesPlot::mouseReleaseEvent(QMouseEvent *event)
{
	QCustomPlot::mouseReleaseEvent(event);
	if (event->button() == Qt::LeftButton) {
		m_mousePressed = false;

		// 启动超时计时器，等待短时间后恢复自动更新
		m_interactionTimer->start();
		qDebug() << "Mouse released - starting 100ms timeout";
	}
}

// 重写滚轮事件，实现只缩放X轴
void TimeSeriesPlot::wheelEvent(QWheelEvent *event)
{
	// 标记交互状态
	m_userInteracting = true;
	m_interactionTimer->stop(); // 停止超时计时器

	// 检查是否按住Ctrl键（可选功能）
	bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

	// 默认行为：只缩放X轴
	if (!ctrlPressed) {
		//// 只缩放X轴
		//double factor = (event->angleDelta().y() > 0) ? 1.15 : 1 / 1.15;

		//// 获取鼠标位置
		//QPoint pos = event->pos();

		//// 计算以鼠标为中心的新范围
		//double centerX = xAxis->pixelToCoord(pos.x());
		//double currentWidth = xAxis->range().size();
		//double newWidth = currentWidth * factor;

		//// 确保缩放后不会显示无效范围
		//if (newWidth < 1.0) newWidth = 1.0; // 最小1秒范围
		//if (newWidth > 3600.0 * 24) newWidth = 3600.0 * 24; // 最大24小时范围

		//xAxis->setRange(centerX - newWidth / 2, centerX + newWidth / 2);
		//replot();
		//event->accept();

			   // 计算缩放因子
		double factor = (event->angleDelta().y() > 0) ? 1.5 : 1 / 1.5;

		// 获取当前X轴范围
		double currentMin = xAxis->range().lower;
		double currentMax = xAxis->range().upper;
		double currentWidth = currentMax - currentMin;

		// 计算X轴中心点
		double centerX = (currentMin + currentMax) / 2.0;

		// 计算以X轴中心为中心的新范围
		double newWidth = currentWidth * factor;

		// 确保缩放后不会显示无效范围
		if (newWidth < 1.0) newWidth = 1.0; // 最小1秒范围
		if (newWidth > 3600.0 * 24) newWidth = 3600.0 * 24; // 最大24小时范围

		// 设置新范围，保持中心点不变
		xAxis->setRange(centerX - newWidth / 2.0, centerX + newWidth / 2.0);

		replot();
		event->accept();
	}
	else {
		// Ctrl按下时使用默认缩放行为
		QCustomPlot::wheelEvent(event);
	}

	// 启动超时计时器
	m_interactionTimer->start();
	qDebug() << "Wheel event - interactivity active, timeout started";
}

// 添加重置视图方法
void TimeSeriesPlot::resetView()
{
	double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
	xAxis->setRange(currentTime - m_timeWindow / 2, currentTime + m_timeWindow / 2);
	replot();
	m_userInteracting = false;
	m_interactionTimer->stop();
	qDebug() << "View reset to latest data";
}