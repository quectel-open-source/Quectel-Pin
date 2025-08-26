#include "QC_Log.h"
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QtCore/qcoreapplication.h>

QC_Log* QC_Log::uniqueInstance = nullptr;

QC_Log::QC_Log() : semaphore(0) {
    // ��ʼ������ͷ��β�ڵ�
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
        uniqueInstance->start(); // ������־�߳�
    }
    return uniqueInstance;
}

void QC_Log::write(const QString& msg, MachineLogType type) {
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage = QString("%1 %2").arg(currentDateTime).arg(msg);

    int typeIndex = static_cast<int>(type);

    // ���������������
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

    semaphore.release(); // ֪ͨ��־�߳����µ���־��Ҫ����
}

void QC_Log::run() {
    while (!isClose) {
        semaphore.acquire(); // �ȴ���־д���ź�

        // ����������־����
        for (int i = 0; i < static_cast<int>(MachineLogType::END); ++i) {
            mutex.lock();

            // ��鵱ǰ��־�����Ƿ�������
            while (headNodes[i]->next) {
                //QString logFileDir = "./logs";
                QString logFileDir = QCoreApplication::applicationDirPath() + "/logs";
                QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
                QString fileName = QString("%1/%2 %3.log").arg(logFileDir).arg(timeStr).arg(typeNames[i]);

                // ȷ����־Ŀ¼����
                QDir dir;
                if (!dir.exists(logFileDir)) {
                    dir.mkpath(logFileDir);
                }

                // ����־�ļ�
                QFile file(fileName);
                if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
                    QTextStream stream(&file);

                    // д����־
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

    // �ر�ʱд��ʣ����־
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
