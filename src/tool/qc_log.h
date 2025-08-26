#pragma once

#ifndef QC_LOG_H
#define QC_LOG_H


#include <QThread>
#include <QMutex>
#include <QSemaphore>
#include <QString>
#include <queue>
#include <array>
#define machineLog QC_Log::instance()

// 使用枚举定义日志类型
enum MachineLogType {
    Normal,
    Global,
    Machine,
    Err,
    Mes,
    END // 用于确定枚举的数量
};



// 单例日志类
class QC_Log : public QThread {
    Q_OBJECT

public:
    static QC_Log* instance(); // 获取单例实例
    ~QC_Log();

    // 写日志
    void write(const QString& msg, MachineLogType type = MachineLogType::Normal);

protected:
    // 日志写入线程
    void run() override;

private:
    QC_Log(); // 构造函数私有化
    // 日志节点
    struct Node {
        QString str;
        Node* next = nullptr;

        Node(const QString& str) : str(str) {}
        Node() = default;
    };

    // 日志队列相关
    std::array<Node*, static_cast<int>(MachineLogType::END)> headNodes;
    std::array<Node*, static_cast<int>(MachineLogType::END)> tailNodes;

    // 日志类型对应文件名
    const char* typeNames[static_cast<int>(MachineLogType::END)] = {
        "normal",
        "global",
        "machine",
        "err",
        "mes"
    };

    QMutex mutex; // 线程同步锁
    QSemaphore semaphore; // 信号量控制

    bool isClose = false; // 是否关闭日志线程
    static QC_Log* uniqueInstance; // 单例实例
signals:
    void logUpdate_signal(QString mes,int type);
    void errUpdate_signal(QString mes);
};

#endif // QC_LOG_H
