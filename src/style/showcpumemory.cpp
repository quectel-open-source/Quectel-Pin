#include "showcpumemory.h"
#include <QThread>
#include <QProcess>
#include <QLabel>
#include <QDateTime>
#include <QStorageInfo>
#include <QTimer>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#elif defined(Q_OS_LINUX)
#include <sys/sysinfo.h>
#include <fstream>
#elif defined(Q_OS_MAC)
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif

#define MB (1024 * 1024)
#define KB (1024)

ShowCpuMemory::ShowCpuMemory(QObject* parent) : QObject(parent)
{
    run = true;
    cpuPercent = 0;
    memoryPercent = 0;
    memoryAll = 0;
    memoryUse = 0;
    diskPercent = 0;
    labMemory = nullptr;

    // 初始化CPU计算所需变量
#ifdef Q_OS_WIN
    lastIdleTime = lastKernelTime = lastUserTime = { 0 };
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    lastTotalTime = lastWorkTime = 0;
#endif
}

ShowCpuMemory::~ShowCpuMemory()
{
    stopMonitoring();
}

void ShowCpuMemory::SetLab(QLabel* lab)
{
    labMemory = lab;
    if (labMemory) {
        labMemory->setObjectName("SystemMonitor");
        labMemory->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
}

void ShowCpuMemory::startMonitoring(int interval)
{
    run = true;

    // 首次获取CPU基准数据
#ifdef Q_OS_WIN
    GetSystemTimes(&lastIdleTime, &lastKernelTime, &lastUserTime);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    getCpuTimes(lastTotalTime, lastWorkTime);
#endif

    // 使用定时器替代循环，更安全
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ShowCpuMemory::updateSystemInfo);
    timer->start(interval);
}

void ShowCpuMemory::stopMonitoring()
{
    run = false;
}

void ShowCpuMemory::updateSystemInfo()
{
    if (!run) return;

    // 获取CPU使用率
    cpuPercent = calculateCpuUsage();

    // 获取内存使用率
    calculateMemoryUsage();

    // 获取硬盘使用率
    calculateDiskUsage();

    // 更新显示
    updateStatusDisplay();
}

int ShowCpuMemory::calculateCpuUsage()
{
    int usage = 0;

#ifdef Q_OS_WIN
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        qint64 idle = compareFileTime(lastIdleTime, idleTime);
        qint64 kernel = compareFileTime(lastKernelTime, kernelTime);
        qint64 user = compareFileTime(lastUserTime, userTime);

        if (kernel + user > 0) {
            usage = static_cast<int>((kernel + user - idle) * 100 / (kernel + user));
        }

        lastIdleTime = idleTime;
        lastKernelTime = kernelTime;
        lastUserTime = userTime;
    }
#elif defined(Q_OS_LINUX)
    qulonglong totalTime, workTime;
    if (getCpuTimes(totalTime, workTime)) {
        if (lastTotalTime > 0 && totalTime > lastTotalTime) {
            qulonglong deltaTotal = totalTime - lastTotalTime;
            qulonglong deltaWork = workTime - lastWorkTime;
            usage = static_cast<int>(deltaWork * 100 / deltaTotal);
        }
        lastTotalTime = totalTime;
        lastWorkTime = workTime;
    }
#elif defined(Q_OS_MAC)
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
        (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
        unsigned long totalTicks = cpuinfo.cpu_ticks[CPU_STATE_USER] +
            cpuinfo.cpu_ticks[CPU_STATE_SYSTEM] +
            cpuinfo.cpu_ticks[CPU_STATE_NICE] +
            cpuinfo.cpu_ticks[CPU_STATE_IDLE];
        unsigned long workTicks = cpuinfo.cpu_ticks[CPU_STATE_USER] +
            cpuinfo.cpu_ticks[CPU_STATE_SYSTEM] +
            cpuinfo.cpu_ticks[CPU_STATE_NICE];

        if (lastTotalTime > 0 && totalTicks > lastTotalTime) {
            unsigned long deltaTotal = totalTicks - lastTotalTime;
            unsigned long deltaWork = workTicks - lastWorkTime;
            usage = static_cast<int>(deltaWork * 100 / deltaTotal);
        }

        lastTotalTime = totalTicks;
        lastWorkTime = workTicks;
    }
#endif

    return qBound(0, usage, 100);
}

void ShowCpuMemory::calculateMemoryUsage()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        memoryPercent = statex.dwMemoryLoad;
        memoryAll = statex.ullTotalPhys / MB;
        memoryUse = (statex.ullTotalPhys - statex.ullAvailPhys) / MB;
    }
#elif defined(Q_OS_LINUX)
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        memoryAll = (info.totalram * info.mem_unit) / MB;
        memoryUse = memoryAll - (info.freeram * info.mem_unit) / MB;
        memoryPercent = static_cast<int>((memoryUse * 100.0) / memoryAll);
    }
#elif defined(Q_OS_MAC)
    vm_size_t pageSize;
    mach_port_t machPort;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vmStats;

    machPort = mach_host_self();
    count = sizeof(vmStats) / sizeof(natural_t);
    if (host_page_size(machPort, &pageSize) == KERN_SUCCESS &&
        host_statistics64(machPort, HOST_VM_INFO,
            (host_info64_t)&vmStats, &count) == KERN_SUCCESS) {
        qulonglong freeMemory = vmStats.free_count * pageSize;
        qulonglong usedMemory = (vmStats.active_count + vmStats.inactive_count +
            vmStats.wire_count) * pageSize;
        qulonglong totalMemory = usedMemory + freeMemory;

        memoryAll = totalMemory / MB;
        memoryUse = usedMemory / MB;
        memoryPercent = static_cast<int>((usedMemory * 100.0) / totalMemory);
    }
#endif
}

void ShowCpuMemory::calculateDiskUsage()
{
    qint64 totalBytes = 0;
    qint64 usedBytes = 0;

    QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();
    foreach(const QStorageInfo & volume, volumes) {
        if (volume.isValid() && volume.isReady() && !volume.rootPath().isEmpty()) {
            totalBytes += volume.bytesTotal();
            usedBytes += volume.bytesTotal() - volume.bytesAvailable();
        }
    }

    if (totalBytes > 0) {
        diskPercent = static_cast<int>((usedBytes * 100.0) / totalBytes);
    }
}

void ShowCpuMemory::updateStatusDisplay()
{
    if (!labMemory) return;

    QString status = QString("CPU: %1% | 内存: %2% (%3MB/%4MB) | 硬盘: %5% | %6")
        .arg(cpuPercent)
        .arg(memoryPercent)
        .arg(memoryUse)
        .arg(memoryAll)
        .arg(diskPercent)
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    labMemory->setText(status);
    emit statusUpdated(status);
}

#ifdef Q_OS_WIN
qint64 ShowCpuMemory::compareFileTime(FILETIME a, FILETIME b)
{
    ULARGE_INTEGER aTime, bTime;
    aTime.LowPart = a.dwLowDateTime;
    aTime.HighPart = a.dwHighDateTime;
    bTime.LowPart = b.dwLowDateTime;
    bTime.HighPart = b.dwHighDateTime;
    return bTime.QuadPart - aTime.QuadPart;
}
#elif defined(Q_OS_LINUX)
bool ShowCpuMemory::getCpuTimes(qulonglong& totalTime, qulonglong& workTime)
{
    std::ifstream file("/proc/stat");
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line);
    if (line.substr(0, 3) != "cpu") return false;

    std::istringstream iss(line.substr(5));
    qulonglong user, nice, system, idle, iowait, irq, softirq;
    iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq;

    totalTime = user + nice + system + idle + iowait + irq + softirq;
    workTime = user + nice + system;

    return true;
}
#endif