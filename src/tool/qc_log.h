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

// ʹ��ö�ٶ�����־����
enum MachineLogType {
    Normal,
    Global,
    Machine,
    Err,
    Mes,
    END // ����ȷ��ö�ٵ�����
};



// ������־��
class QC_Log : public QThread {
    Q_OBJECT

public:
    static QC_Log* instance(); // ��ȡ����ʵ��
    ~QC_Log();

    // д��־
    void write(const QString& msg, MachineLogType type = MachineLogType::Normal);

protected:
    // ��־д���߳�
    void run() override;

private:
    QC_Log(); // ���캯��˽�л�
    // ��־�ڵ�
    struct Node {
        QString str;
        Node* next = nullptr;

        Node(const QString& str) : str(str) {}
        Node() = default;
    };

    // ��־�������
    std::array<Node*, static_cast<int>(MachineLogType::END)> headNodes;
    std::array<Node*, static_cast<int>(MachineLogType::END)> tailNodes;

    // ��־���Ͷ�Ӧ�ļ���
    const char* typeNames[static_cast<int>(MachineLogType::END)] = {
        "normal",
        "global",
        "machine",
        "err",
        "mes"
    };

    QMutex mutex; // �߳�ͬ����
    QSemaphore semaphore; // �ź�������

    bool isClose = false; // �Ƿ�ر���־�߳�
    static QC_Log* uniqueInstance; // ����ʵ��
signals:
    void logUpdate_signal(QString mes,int type);
    void errUpdate_signal(QString mes);
};

#endif // QC_LOG_H
