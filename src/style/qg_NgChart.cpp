#include "qg_NgChart.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <QTimer>
#include <QChartView>
#include <QVBoxLayout>

//NG������ͼ

qg_NgChart::qg_NgChart(QWidget* parent) : QWidget(parent) {
    setStyleSheet("background: #0E1827;"); // ������ɫ������ͻ��͸��ͼ��
    setupChart();
    setupAxes();
    setupTimer();
    //��ʼ��ʱ��
    updateData();
}

qg_NgChart::~qg_NgChart()
{
    if (chart) {
        delete chart;
        chart = nullptr;
    }
}

void qg_NgChart::setupChart() {
    series = new QLineSeries();
    chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setBackgroundBrush(Qt::transparent);
    chart->setBackgroundRoundness(0);
    chart->setMargins(QMargins(0, 0, 0, 0));

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    //setAttribute(Qt::WA_TranslucentBackground);
    //chartView->setStyleSheet("background: transparent");
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet(R"(
        QChartView {
            background: transparent;
            border: none;
        }
    )");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);
}

void qg_NgChart::setupAxes() {
    // ���ᣨʱ���ᣩ
    axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm");
    axisX->setTitleText("ʱ��");
    // ���ñ�������ʹ�С
    QFont titleFont = axisX->titleFont();
    titleFont.setPointSize(14);  // ���������С
    titleFont.setBold(true);     // ���üӴ�
    axisX->setTitleFont(titleFont);

    // ���ñ�����ɫ
    QBrush titleBrush(QColor(255, 255, 255));  // ��ɫ
    axisX->setTitleBrush(titleBrush);
    //series->attachAxis(axisX);

    // ͨ��������������ɫ��������
    axisX->setLabelsColor(Qt::white);            // ���ı���ɫ
    axisX->setGridLineColor(Qt::white); // ��������ɫ
    //axisX->setGridLinePen(QPen(Qt::SolidLine));    // ��������ʽ
    axisX->setGridLinePen(QPen(Qt::white, 1, Qt::DashLine));    // ��������ʽ
   

    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // ���ᣨNG�ʣ�
    axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("NG�� (%)");
    // ���ñ�������ʹ�С
    titleFont = axisY->titleFont();
    titleFont.setPointSize(14);  // ���������С
    titleFont.setBold(true);     // ���üӴ�
    axisY->setTitleFont(titleFont);

    // ���ñ�����ɫ
    axisY->setTitleBrush(titleBrush);
    //series->attachAxis(axisY);

    // ͨ����������������ʽ
    axisY->setLabelsColor(Qt::white);
    axisY->setGridLineColor(Qt::white); // ��������ɫ
    //axisY->setGridLineColor(QColor(255, 255, 255, 100));
    //axisY->setGridLinePen(QPen(Qt::SolidLine));
    axisY->setGridLinePen(QPen(Qt::white, 1, Qt::DashLine));    // ��������ʽ

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // ����������ɫ�Ϳ��
    QPen pen(Qt::yellow);
    pen.setWidth(2);
    series->setPen(pen);
}

void qg_NgChart::setupTimer() {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &qg_NgChart::updateData);
#ifdef DEBUG_LHC
    timer->start(10000);
#else
    timer->start(60000); // ÿ���Ӹ���
#endif // DEBUG_LHC

}

QDateTime qg_NgChart::getStartTime() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime startTime(now.date(), QTime(8, 0));
    //QDateTime startTime(now.date(), QTime(14, 00, 00));
    if (now < startTime) startTime = startTime.addDays(-1); // �������
    return startTime;
}

void qg_NgChart::updateData() {
    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime startTime = getStartTime();
#ifdef DEBUG_LHC
    QDateTime endTime(startTime.date(), QTime(14, 10, 59));
    // ����ģ�����ݣ�0-100%��
    qreal ngRate = QRandomGenerator::global()->bounded(100.0);

    // ������ݵ�
    series->append(currentTime.toMSecsSinceEpoch(), ngRate);
#else
    QDateTime endTime(startTime.date(), QTime(23, 59, 59)); // 24Сʱ����
    series->append(currentTime.toMSecsSinceEpoch(), m_ngValue);
#endif // DEBUG_LHC
    
    // ���ú��᷶ΧΪ����8�㵽��24Сʱ
    axisX->setRange(startTime, endTime);

    //// �Զ�����ʱ����
    //axisX->setMin(startTime);
    //axisX->setMax(currentTime);

    // �������2Сʱ����
    qint64 minTime = currentTime.addSecs(-7200).toMSecsSinceEpoch();
    qint64 maxTime = endTime.toMSecsSinceEpoch();
    QVector<QPointF> points = series->pointsVector();
    points.erase(std::remove_if(points.begin(), points.end(),
        [minTime, maxTime](const QPointF& p) {
            return p.x() < minTime || p.x() > maxTime;
        }),
        points.end());
    series->replace(points);
}