#pragma once
#pragma execution_character_set("utf-8")

#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QStorageInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class ShowCpuMemory : public QObject
{
    Q_OBJECT

public:
    explicit ShowCpuMemory(QObject* parent = nullptr);
    ~ShowCpuMemory();

    void SetLab(QLabel* lab);
    void startMonitoring(int interval = 1000); // 默认1秒刷新
    void stopMonitoring();

signals:
    void statusUpdated(const QString& message); // 新增状态更新信号
    void hardDiskSignal(QString hardDiskValue); // 保留原有信号

private slots:
    void updateSystemInfo();

private:
    int calculateCpuUsage();
    void calculateMemoryUsage();
    void calculateDiskUsage();
    void updateStatusDisplay();

#ifdef Q_OS_WIN
    qint64 compareFileTime(FILETIME a, FILETIME b);
#elif defined(Q_OS_LINUX)
    bool getCpuTimes(qulonglong& totalTime, qulonglong& workTime);
#endif

private:
    bool run;
    int cpuPercent;
    int memoryPercent;
    int memoryAll;
    int memoryUse;
    int diskPercent;
    QLabel* labMemory;
    QTimer* monitorTimer;

    // CPU计算所需变量
#ifdef Q_OS_WIN
    FILETIME lastIdleTime;
    FILETIME lastKernelTime;
    FILETIME lastUserTime;
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    qulonglong lastTotalTime;
    qulonglong lastWorkTime;
#endif
};