#include "QC_Log.h"
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QtCore/qcoreapplication.h>

QC_Log* QC_Log::uniqueInstance = nullptr;

QC_Log::QC_Log() : semaphore(0) {
    // 初始化链表头和尾节点
    for (int i = 0; i < static_cast<int>(MachineLogType::END); ++i) {
        headNodes[i] = tailNodes[i] = new Node();
    }
}

QC_Log::~QC_Log() {
    isClose = true;
    for (int i = 0; i < static_cast<int>(MachineLogType::END); ++i) {
        delete headNodes[i];
    }
    mutex.lock();
    mutex.unlock();
}

QC_Log* QC_Log::instance() {
    if (uniqueInstance == nullptr) {
        uniqueInstance = new QC_Log();
        uniqueInstance->start(); // 启动日志线程
    }
    return uniqueInstance;
}

void QC_Log::write(const QString& msg, MachineLogType type) {
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage = QString("%1 %2").arg(currentDateTime).arg(msg);

    int typeIndex = static_cast<int>(type);

    // 加锁保护链表操作
    mutex.lock();
    tailNodes[typeIndex]->next = new Node(logMessage);
    tailNodes[typeIndex] = tailNodes[typeIndex]->next;
    if (type == Normal)
    {
        emit logUpdate_signal(logMessage, Normal);
    }
    else if (type == Err)
    {
        emit errUpdate_signal(msg);
    }
    else if (type == Mes)
    {
        emit logUpdate_signal(logMessage, Mes);
    }

    mutex.unlock();

    semaphore.release(); // 通知日志线程有新的日志需要处理
}

void QC_Log::run() {
    while (!isClose) {
        semaphore.acquire(); // 等待日志写入信号

        // 遍历所有日志类型
        for (int i = 0; i < static_cast<int>(MachineLogType::END); ++i) {
            mutex.lock();

            // 检查当前日志链表是否有内容
            while (headNodes[i]->next) {
                //QString logFileDir = "./logs";
                QString logFileDir = QCoreApplication::applicationDirPath() + "/logs";
                QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
                QString fileName = QString("%1/%2 %3.log").arg(logFileDir).arg(timeStr).arg(typeNames[i]);

                // 确保日志目录存在
                QDir dir;
                if (!dir.exists(logFileDir)) {
                    dir.mkpath(logFileDir);
                }

                // 打开日志文件
                QFile file(fileName);
                if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
                    QTextStream stream(&file);

                    // 写入日志
                    Node* temp = headNodes[i]->next;
                    delete headNodes[i];
                    headNodes[i] = temp;

                    QString message = headNodes[i]->str;
                    stream << message << "\r\n";

                    file.flush();
                    file.close();
                }
            }
            mutex.unlock();
        }
    }

    // 关闭时写入剩余日志
    for (int i = 0; i < static_cast<int>(MachineLogType::END); ++i) {
        while (headNodes[i]->next) {
            QString logFileDir = "./logs";
            QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
            QString fileName = QString("%1/%2 %3.log").arg(logFileDir).arg(timeStr).arg(typeNames[i]);

            QDir dir;
            if (!dir.exists(logFileDir)) {
                dir.mkpath(logFileDir);
            }

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
                QTextStream stream(&file);

                Node* temp = headNodes[i]->next;
                delete headNodes[i];
                headNodes[i] = temp;

                QString message = headNodes[i]->str;
                stream << message << "\r\n";

                file.flush();
                file.close();
            }
        }
    }
}
