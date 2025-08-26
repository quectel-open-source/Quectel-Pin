#ifndef QG_NGCHART_H
#define QG_NGCHART_H
#pragma execution_character_set("utf-8")

#include <QWidget>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

class qg_NgChart : public QWidget {
    Q_OBJECT

public:
    explicit qg_NgChart(QWidget* parent = nullptr);
    ~qg_NgChart();
    void setNgValue(double tp)
    {
        m_ngValue = tp;
    }
    double getNgValue()
    {
        return m_ngValue;
    }

private:
    void setupChart();
    void setupAxes();
    void setupTimer();
    QDateTime getStartTime();
    void updateData();

    QLineSeries* series;
    QChart* chart;
    QDateTimeAxis* axisX;
    QValueAxis* axisY;
    double m_ngValue = 0;//NGÂÊ
};

#endif // QG_NGCHART_H