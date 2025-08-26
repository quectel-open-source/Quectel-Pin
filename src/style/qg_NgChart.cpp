#include "qg_NgChart.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <QTimer>
#include <QChartView>
#include <QVBoxLayout>

//NG率折线图

qg_NgChart::qg_NgChart(QWidget* parent) : QWidget(parent) {
    setStyleSheet("background: #0E1827;"); // 设置深色背景以突出透明图表
    setupChart();
    setupAxes();
    setupTimer();
    //初始化时钟
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
    // 横轴（时间轴）
    axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm");
    axisX->setTitleText("时间");
    // 设置标题字体和大小
    QFont titleFont = axisX->titleFont();
    titleFont.setPointSize(14);  // 设置字体大小
    titleFont.setBold(true);     // 设置加粗
    axisX->setTitleFont(titleFont);

    // 设置标题颜色
    QBrush titleBrush(QColor(255, 255, 255));  // 白色
    axisX->setTitleBrush(titleBrush);
    //series->attachAxis(axisX);

    // 通过代码设置轴颜色、网格线
    axisX->setLabelsColor(Qt::white);            // 轴文本颜色
    axisX->setGridLineColor(Qt::white); // 网格线颜色
    //axisX->setGridLinePen(QPen(Qt::SolidLine));    // 网格线样式
    axisX->setGridLinePen(QPen(Qt::white, 1, Qt::DashLine));    // 网格线样式
   

    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // 纵轴（NG率）
    axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("NG率 (%)");
    // 设置标题字体和大小
    titleFont = axisY->titleFont();
    titleFont.setPointSize(14);  // 设置字体大小
    titleFont.setBold(true);     // 设置加粗
    axisY->setTitleFont(titleFont);

    // 设置标题颜色
    axisY->setTitleBrush(titleBrush);
    //series->attachAxis(axisY);

    // 通过代码设置纵轴样式
    axisY->setLabelsColor(Qt::white);
    axisY->setGridLineColor(Qt::white); // 网格线颜色
    //axisY->setGridLineColor(QColor(255, 255, 255, 100));
    //axisY->setGridLinePen(QPen(Qt::SolidLine));
    axisY->setGridLinePen(QPen(Qt::white, 1, Qt::DashLine));    // 网格线样式

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 设置线条颜色和宽度
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
    timer->start(60000); // 每分钟更新
#endif // DEBUG_LHC

}

QDateTime qg_NgChart::getStartTime() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime startTime(now.date(), QTime(8, 0));
    //QDateTime startTime(now.date(), QTime(14, 00, 00));
    if (now < startTime) startTime = startTime.addDays(-1); // 处理跨天
    return startTime;
}

void qg_NgChart::updateData() {
    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime startTime = getStartTime();
#ifdef DEBUG_LHC
    QDateTime endTime(startTime.date(), QTime(14, 10, 59));
    // 生成模拟数据（0-100%）
    qreal ngRate = QRandomGenerator::global()->bounded(100.0);

    // 添加数据点
    series->append(currentTime.toMSecsSinceEpoch(), ngRate);
#else
    QDateTime endTime(startTime.date(), QTime(23, 59, 59)); // 24小时结束
    series->append(currentTime.toMSecsSinceEpoch(), m_ngValue);
#endif // DEBUG_LHC
    
    // 设置横轴范围为当天8点到第24小时
    axisX->setRange(startTime, endTime);

    //// 自动滚动时间轴
    //axisX->setMin(startTime);
    //axisX->setMax(currentTime);

    // 保留最近2小时数据
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