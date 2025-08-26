// TimeSeriesPlot.cpp
#include "TimeSeriesPlot.h"
#include <QDebug>

TimeSeriesPlot::TimeSeriesPlot(QWidget *parent)
	: QCustomPlot(parent)
{
	// ���ø��¶�ʱ����ÿ5�봥��һ�Σ�
	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(5000);
	connect(m_updateTimer, &QTimer::timeout, this, &TimeSeriesPlot::updatePlot);

	// ���ý�����ⶨʱ��
	m_interactionTimer = new QTimer(this);
	m_interactionTimer->setSingleShot(true);
	m_interactionTimer->setInterval(3000); // 100ms��ָ�����
	connect(m_interactionTimer, &QTimer::timeout, this, &TimeSeriesPlot::onInteractionTimeout);

	// ============== �������� ==============
	// ����ʹ����ѡ����
	chartFont.setFamily("Microsoft YaHei UI");

	// ���������С����ʽ
	chartFont.setPointSize(10);
	chartFont.setBold(false);

	// ============== ��ʽ���� ==============
	// �������ɫ���� (#282A33)
	QColor backgroundColor(40, 42, 51); // #282A33
	setBackground(backgroundColor);

	// ������������ɫΪ��ɫ
	QColor axisColor(Qt::white);

	// ��������������
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

	// ������������ɫ��ǳ��ɫ��
	QColor gridColor(100, 100, 100);
	xAxis->grid()->setPen(QPen(gridColor, 1, Qt::DotLine));
	yAxis->grid()->setPen(QPen(gridColor, 1, Qt::DotLine));

	// ����ͼ����ʽ
	legend->setFont(chartFont); // ����ͼ������
	legend->setBorderPen(QPen(axisColor, 1)); // ��ɫ�߿�
	legend->setTextColor(axisColor); // ��ɫ����
	legend->setBrush(backgroundColor); // ���ɫ����
	legend->setSelectedBorderPen(QPen(axisColor, 2)); // ѡ��ʱ�Ӵְ�ɫ�߿�

	// ============== ���������� ==============
	// �����������ǩ
	xAxis->setLabel("ʱ��");

	// ����X��Ϊ����ʱ����
	QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
	dateTicker->setDateTimeFormat("hh:mm:ss");
	xAxis->setTicker(dateTicker);

	yAxis->setRange(0, 100);


	// ��ʼʱ�䴰��
	setTimeWindow(m_timeWindow);

	// ���ý������� - ������X���϶�������
	setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	axisRect()->setRangeDragAxes(xAxis, nullptr); // ֻ�϶�X��
	axisRect()->setRangeZoomAxes(xAxis, nullptr); // ֻ����X��
	setMouseTracking(true); // �������׷��

	// ��ʼ��ͼ��
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
		qWarning() << "���߲�����:" << curveName;
		return;
	}

	// ��ȡ��ǰʱ�䣨��1970��������������
	double time = QDateTime::currentDateTime().toSecsSinceEpoch();

	// ��ӵ�������
	QCPGraphData data;
	data.key = time;
	data.value = value;
	m_dataBuffers[curveName].append(data);
}

void TimeSeriesPlot::setTimeWindow(int seconds)
{
	m_timeWindow = seconds;

	// ���ó�ʼX�᷶Χ
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

	// ������������ - ���Ǹ�������
	for (auto curveIt = m_curves.begin(); curveIt != m_curves.end(); ++curveIt) {
		const QString &name = curveIt.key();
		QCPGraph *curve = curveIt.value();

		if (!m_dataBuffers.contains(name) || m_dataBuffers[name].isEmpty())
			continue;

		// ���������
		curve->data()->add(m_dataBuffers[name]);
		m_dataBuffers[name].clear();
	}

	// ֻ����û���û�����ʱ���·�Χ
	if (!m_userInteracting) {
		// ����X�᷶Χ����ʾ����ʱ�䴰�ڣ�
		xAxis->setRange(currentTime - m_timeWindow/2, currentTime + m_timeWindow / 2);
	}

	// �����ػ�ͼ������ʾ������
	replot();
}

void TimeSeriesPlot::onInteractionTimeout()
{
	// �����������ָ��Զ�����
	m_userInteracting = false;
	qDebug() << "Interactivity ended, restoring auto-update";

	// ���⴦�����û��϶�����ʱ���ָ���������ͼ
	double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
	xAxis->setRange(currentTime - m_timeWindow / 2, currentTime + m_timeWindow / 2);
	replot();
}
// ��д����¼��������û�����
void TimeSeriesPlot::mousePressEvent(QMouseEvent *event)
{
	QCustomPlot::mousePressEvent(event);
	if (event->button() == Qt::LeftButton) {
		m_mousePressed = true;
		m_userInteracting = true;
		m_interactionTimer->stop(); // ֹͣ��ʱ��ʱ��
		qDebug() << "Mouse press - interactivity started";
	}
}

void TimeSeriesPlot::mouseMoveEvent(QMouseEvent *event)
{
	QCustomPlot::mouseMoveEvent(event);
	if (m_mousePressed && !m_userInteracting) {
		// �������ƶ�ʱ�Ѿ����µ���û���Ϊ��������ʼ����
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

	// ������������
	for (int i = 0; i < this->graphCount(); ++i) {
		QCPGraph *graph = this->graph(i);
		if (!graph || !graph->visible())
			continue;

		// ʹ���°����ݷ���API
		auto dataContainer = graph->data();
		if (!dataContainer || dataContainer->isEmpty())
			continue;

		// ������ӽ��ĵ� (ʹ�õ�����)
		double closestDistance = std::numeric_limits<double>::max();
		double closestX = 0;
		double closestY = 0;

		// ��ȡ���ݵ�����
		auto it = dataContainer->constBegin();
		auto end = dataContainer->constEnd();

		// �����������ݵ�
		while (it != end) {
			double distance = std::abs(it->key - mouseX);
			if (distance < closestDistance) {
				closestDistance = distance;
				closestX = it->key;
				closestY = it->value;
			}
			++it;
		}

		// ������Ļ����
		QPointF pixelPoint = graph->coordsToPixels(closestX, closestY);
		double screenDistance = QLineF(event->pos(), pixelPoint).length();

		// ����Ƿ�����ֵ��Χ��
		if (screenDistance < 15) {
			tracer->setGraph(graph);
			QString ss = graph->name();
			tracer->setGraphKey(closestX);
			tracer->setVisible(true);

			textLabel->setText(ss + QString(":%1%").arg(closestY, 0, 'f', 2));
			textLabel->setVisible(true);

			foundPoint = true;
			break; // �ҵ���һ�����������ĵ㼴��
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

		// ������ʱ��ʱ�����ȴ���ʱ���ָ��Զ�����
		m_interactionTimer->start();
		qDebug() << "Mouse released - starting 100ms timeout";
	}
}

// ��д�����¼���ʵ��ֻ����X��
void TimeSeriesPlot::wheelEvent(QWheelEvent *event)
{
	// ��ǽ���״̬
	m_userInteracting = true;
	m_interactionTimer->stop(); // ֹͣ��ʱ��ʱ��

	// ����Ƿ�סCtrl������ѡ���ܣ�
	bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

	// Ĭ����Ϊ��ֻ����X��
	if (!ctrlPressed) {
		//// ֻ����X��
		//double factor = (event->angleDelta().y() > 0) ? 1.15 : 1 / 1.15;

		//// ��ȡ���λ��
		//QPoint pos = event->pos();

		//// ���������Ϊ���ĵ��·�Χ
		//double centerX = xAxis->pixelToCoord(pos.x());
		//double currentWidth = xAxis->range().size();
		//double newWidth = currentWidth * factor;

		//// ȷ�����ź󲻻���ʾ��Ч��Χ
		//if (newWidth < 1.0) newWidth = 1.0; // ��С1�뷶Χ
		//if (newWidth > 3600.0 * 24) newWidth = 3600.0 * 24; // ���24Сʱ��Χ

		//xAxis->setRange(centerX - newWidth / 2, centerX + newWidth / 2);
		//replot();
		//event->accept();

			   // ������������
		double factor = (event->angleDelta().y() > 0) ? 1.5 : 1 / 1.5;

		// ��ȡ��ǰX�᷶Χ
		double currentMin = xAxis->range().lower;
		double currentMax = xAxis->range().upper;
		double currentWidth = currentMax - currentMin;

		// ����X�����ĵ�
		double centerX = (currentMin + currentMax) / 2.0;

		// ������X������Ϊ���ĵ��·�Χ
		double newWidth = currentWidth * factor;

		// ȷ�����ź󲻻���ʾ��Ч��Χ
		if (newWidth < 1.0) newWidth = 1.0; // ��С1�뷶Χ
		if (newWidth > 3600.0 * 24) newWidth = 3600.0 * 24; // ���24Сʱ��Χ

		// �����·�Χ���������ĵ㲻��
		xAxis->setRange(centerX - newWidth / 2.0, centerX + newWidth / 2.0);

		replot();
		event->accept();
	}
	else {
		// Ctrl����ʱʹ��Ĭ��������Ϊ
		QCustomPlot::wheelEvent(event);
	}

	// ������ʱ��ʱ��
	m_interactionTimer->start();
	qDebug() << "Wheel event - interactivity active, timeout started";
}

// ���������ͼ����
void TimeSeriesPlot::resetView()
{
	double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
	xAxis->setRange(currentTime - m_timeWindow / 2, currentTime + m_timeWindow / 2);
	replot();
	m_userInteracting = false;
	m_interactionTimer->stop();
	qDebug() << "View reset to latest data";
}