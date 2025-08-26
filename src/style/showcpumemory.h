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
    void startMonitoring(int interval = 1000); // Ĭ��1��ˢ��
    void stopMonitoring();

signals:
    void statusUpdated(const QString& message); // ����״̬�����ź�
    void hardDiskSignal(QString hardDiskValue); // ����ԭ���ź�

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

    // CPU�����������
#ifdef Q_OS_WIN
    FILETIME lastIdleTime;
    FILETIME lastKernelTime;
    FILETIME lastUserTime;
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    qulonglong lastTotalTime;
    qulonglong lastWorkTime;
#endif
};