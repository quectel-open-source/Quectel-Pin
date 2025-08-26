#include "ModbusTcpClient.h"
#include <QModbusDataUnit>
#include <QEventLoop>
#include <QTimer>
#include <QVariant> 
#include "qc_log.h"

//QT �ٷ��Ŀ⣬���������̵߳���

/**
 * @brief ���캯������ʼ��Modbus�ͻ���ʵ��
 */
ModbusTcpClient::ModbusTcpClient(QObject* parent)
    : QObject(parent)
{
    // ����Modbus TCP�ͻ��˶���
    m_client = new QModbusTcpClient(this);
}

/**
 * @brief ����������ȷ���Ͽ�����
 */
ModbusTcpClient::~ModbusTcpClient()
{
    disconnectFromDevice();
}

bool ModbusTcpClient::connectToDevice(const QString& ip, int port)
{
    m_ip = ip;
    m_port = port;
    if (!m_client) {
        emit errorOccurred("�ͻ���δ��ʼ��");
        return false;
    }

    if (m_client->state() == QModbusDevice::ConnectedState) {
        machineLog->write("�豸������", Normal);
        emit errorOccurred("�豸������");
        return true;
    }
    // �������Ӳ���
    //m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant("127.0.0.1"));
    m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant(ip));
    m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, QVariant(port));

    // �����������󣨷�������
    if (!m_client->connectDevice()) {
        emit errorOccurred(m_client->errorString());
        machineLog->write("����ʧ�� ��" + m_client->errorString(), Normal);
        return false;
    }
    machineLog->write("Modbus tcp�豸���ӳɹ�", Normal);
    return true;
}

void ModbusTcpClient::disconnectFromDevice()
{
    if (m_client && m_client->state() != QModbusDevice::UnconnectedState) {
        // �Ͽ����Ӳ��ͷ���Դ
        m_client->disconnectDevice();
    }
}

////��ȡ����Ĵ���
//QVector<quint16> ModbusTcpClient::readHoldingRegisters(int startAddr, int count,
//    int serverAddr, int timeout)
//{
//    QVector<quint16> result;
//    bool timeoutOccurred = false;
//
//    // ǰ���������
//    if (!m_client) {
//        emit errorOccurred("�ͻ���δ��ʼ��");
//        machineLog->write("�ͻ���δ��ʼ��", Normal);
//        return result;
//    }
//    if (m_client->state() != QModbusDevice::ConnectedState) {
//        emit errorOccurred("�豸δ����");
//        machineLog->write("�ͻ���δ��ʼ��", Normal);
//        return result;
//    }
//
//    QEventLoop loop;
//    QTimer timer;
//    timer.setSingleShot(true);
//
//    // ���Ͷ�ȡ����
//    QModbusReply* reply = m_client->sendReadRequest(
//        QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddr, count),
//        serverAddr
//    );
//
//    if (!reply) {
//        emit errorOccurred("������ȡ����ʧ��");
//        machineLog->write("������ȡ����ʧ��", Normal);
//        return result;
//    }
//
//    // �ź�����
//    QObject::connect(&timer, &QTimer::timeout, [&]() {
//        timeoutOccurred = true;
//        loop.quit();
//        });
//
//    QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
//
//    // �����ȴ�
//    timer.start(timeout);
//    loop.exec();
//
//    // ������
//    if (timeoutOccurred) {
//        emit errorOccurred(QString("��ȡ������ʱ��%1ms��").arg(timeout));
//        machineLog->write(QString("��ȡ������ʱ��%1ms��").arg(timeout), Normal);
//        reply->deleteLater();
//        return result;
//    }
//
//    if (reply->error() == QModbusDevice::NoError) {
//        const QModbusDataUnit unit = reply->result();
//        result.reserve(unit.valueCount());
//        for (int i = 0; i < unit.valueCount(); ++i) {
//            result.append(unit.value(i));
//        }
//    }
//    else {
//        emit errorOccurred(QString("��ȡ����%1").arg(reply->errorString()));
//        machineLog->write(QString("��ȡ����%1").arg(reply->errorString()), Normal);
//    }
//
//    reply->deleteLater();
//    return result;
//}


QVector<quint16> ModbusTcpClient::readHoldingRegisters(int startAddr, int count,
    int serverAddr, int timeout)
{
    const int MAX_RETRY = 5; // ������Դ���
    const int RECONNECT_DELAY = 1000; // �����ӳ�(ms)
    int retryCount = 0;
    QVector<quint16> result;

    // ������ѭ��
    while (retryCount < MAX_RETRY) {
        // ǰ���������
        if (!m_client) {
            emit errorOccurred("�ͻ���δ��ʼ��");
            break;
        }

        // �������״̬���Զ�������
        if (m_client->state() != QModbusDevice::ConnectedState) {
            machineLog->write("modbus��⵽���ӶϿ���������������", Normal);
            if (!connectToDevice(m_ip, m_port)) {
                machineLog->write("modbus��������ʧ�ܣ�׼������", Normal);
                retryCount++;
                QThread::msleep(RECONNECT_DELAY);
                continue;
            }
        }

        bool timeoutOccurred = false;
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        // ���Ͷ�ȡ����
        QModbusReply* reply = m_client->sendReadRequest(
            QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddr, count),
            serverAddr
        );

        if (!reply) {
            emit errorOccurred("������ȡ����ʧ��");
            machineLog->write("������ȡ����ʧ�ܣ�׼������", Normal);
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        // �ź�����
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            timeoutOccurred = true;
            loop.quit();
            });

        QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);

        // �����ȴ�
        timer.start(timeout);
        loop.exec();

        // ������
        if (timeoutOccurred) {
            QString errorMsg = QString("��ȡ������ʱ(%1/%2����)").arg(retryCount + 1).arg(MAX_RETRY);
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            reply->deleteLater();
            disconnectFromDevice(); // ��ʱ��Ͽ�����
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();
            result.reserve(unit.valueCount());
            for (int i = 0; i < unit.valueCount(); ++i) {
                result.append(unit.value(i));
            }
            reply->deleteLater();
            return result; // �ɹ���ȡ��ֱ�ӷ���
        }
        else {
            emit errorOccurred(QString("��ȡ����%1").arg(reply->errorString()));
            machineLog->write(QString("��ȡ����%1��׼������").arg(reply->errorString()), Normal);
            reply->deleteLater();

            // ����������������Ͽ�����
            if (reply->error() == QModbusDevice::ConnectionError ||
                reply->error() == QModbusDevice::TimeoutError) {
                disconnectFromDevice();
            }

            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
        }
    }

    // ��������ʧ�ܺ�
    if (retryCount >= MAX_RETRY) {
        emit errorOccurred("��ȡʧ�ܣ�����������Դ���");
        machineLog->write("��ȡʧ�ܣ�����������Դ���", Normal);
    }
    return QVector<quint16>(); // ���ؿս��
}

//��ȡ�����Ĵ���
bool ModbusTcpClient::readHoldingRegister(int regAddr, quint16& output,int serverAddr,int timeout)
{
    QVector<quint16> regs;

    // ����ԭ�ж�Ĵ�����ȡ�������ڲ��߼������䣩
    regs = readHoldingRegisters(regAddr, 1, serverAddr, timeout);

    if (regs.size() != 1) {
        emit errorOccurred("��ȡ�Ĵ���ʧ��");
        machineLog->write("��ȡ�Ĵ���ʧ��", Normal);
        return false;
    }

    output = regs[0];
    return true;
}

//д�����Ĵ���
//bool ModbusTcpClient::writeSingleRegister(int regAddr, quint16 value,
//    int serverAddr, int timeout)
//{
//    bool timeoutOccurred = false;
//
//    // ǰ���������
//    if (!m_client) {
//        emit errorOccurred("�ͻ���δ��ʼ��");
//        machineLog->write("modbusTcp�ͻ���δ��ʼ��", Normal);
//        return false;
//    }
//    if (m_client->state() != QModbusDevice::ConnectedState) {
//        emit errorOccurred("�豸δ����");
//        machineLog->write("modbusTcp�豸δ����", Normal);
//        return false;
//    }
//
//    QEventLoop loop;
//    QTimer timer;
//    timer.setSingleShot(true);
//
//    // ����д������
//    //QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regAddr, { value });
//    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regAddr, 1);
//    writeUnit.setValue(0, value);
//    QModbusReply* reply = m_client->sendWriteRequest(writeUnit, serverAddr);
//
//    if (!reply) {
//        emit errorOccurred("����д������ʧ��");
//        machineLog->write("����д������ʧ��", Normal);
//        return false;
//    }
//
//    // �ź�����
//    QObject::connect(&timer, &QTimer::timeout, [&]() {
//        timeoutOccurred = true;
//        loop.quit();
//        });
//
//    QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
//
//    // �����ȴ�
//    timer.start(timeout);
//    loop.exec();
//
//    // ������
//    if (timeoutOccurred) {
//        emit errorOccurred(QString("д�������ʱ��%1ms��").arg(timeout));
//        machineLog->write(QString("д�������ʱ��%1ms��").arg(timeout), Normal);
//        reply->deleteLater();
//        return false;
//    }
//
//    const bool success = (reply->error() == QModbusDevice::NoError);
//    if (!success) {
//        if (reply->error() == QModbusDevice::ProtocolError) {
//            quint8 exCode = reply->rawResult().exceptionCode();
//            emit errorOccurred(QString("Modbus�쳣�룺0x%1").arg(exCode, 2, 16, QChar('0')));
//            machineLog->write(QString("Modbus�쳣�룺0x%1").arg(exCode, 2, 16, QChar('0')), Normal);
//        }
//        else {
//            emit errorOccurred(reply->errorString());
//        }
//    }
//
//    reply->deleteLater();
//    return success;
//}

bool ModbusTcpClient::writeSingleRegister(int regAddr, quint16 value,
    int serverAddr, int timeout)
{
    const int MAX_RETRY = 5; // ������Դ���
    const int RECONNECT_DELAY = 1000; // �����ӳ�(ms)
    int retryCount = 0;

    while (retryCount < MAX_RETRY) {
        // ǰ���������
        if (!m_client) {
            emit errorOccurred("�ͻ���δ��ʼ��");
            machineLog->write("modbusTcp�ͻ���δ��ʼ��", Normal);
            return false;
        }

        // ��鲢�Զ���������
        if (m_client->state() != QModbusDevice::ConnectedState) {
            machineLog->write("��⵽���ӶϿ���������������", Normal);
            if (!connectToDevice(m_ip, m_port)) {
                machineLog->write("��������ʧ�ܣ�׼������", Normal);
                retryCount++;
                QThread::msleep(RECONNECT_DELAY);
                continue;
            }
        }

        bool timeoutOccurred = false;
        bool writeSuccess = false;
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        // ����д������
        QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regAddr, 1);
        writeUnit.setValue(0, value);
        QModbusReply* reply = m_client->sendWriteRequest(writeUnit, serverAddr);

        if (!reply) {
            emit errorOccurred("����д������ʧ��");
            machineLog->write("����д������ʧ�ܣ�׼������", Normal);
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        // �ź�����
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            timeoutOccurred = true;
            loop.quit();
            });

        QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);

        // �����ȴ�
        timer.start(timeout);
        loop.exec();

        // ����ʱ
        if (timeoutOccurred) {
            QString errorMsg = QString("д�������ʱ(%1/%2����)").arg(retryCount + 1).arg(MAX_RETRY);
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            reply->deleteLater();
            disconnectFromDevice(); // ��ʱ��Ͽ�����
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        // ���д����
        const bool success = (reply->error() == QModbusDevice::NoError);

        if (success) {
            reply->deleteLater();
            return true; // д��ɹ�
        }

        // �������
        if (reply->error() == QModbusDevice::ProtocolError) {
            quint8 exCode = reply->rawResult().exceptionCode();
            QString errorMsg = QString("Modbus�쳣��: 0x%1").arg(exCode, 2, 16, QChar('0'));
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            // Э����󲻿ɻָ���������
            reply->deleteLater();
            return false;
        }
        else {
            QString errorMsg = QString("д�����: %1 (׼������)").arg(reply->errorString());
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            // ���Ӵ�����Ͽ�����
            if (reply->error() == QModbusDevice::ConnectionError ||
                reply->error() == QModbusDevice::TimeoutError) {
                disconnectFromDevice();
            }

            reply->deleteLater();
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
        }
    }

    // ��������ʧ��
    emit errorOccurred("д��ʧ��: ����������Դ���");
    machineLog->write("д��ʧ��: ����������Դ���", Normal);
    return false;
}

bool ModbusTcpClient::readBit(int regAddr, int bitPos, bool& result, int serverAddr, int timeout) {
    // ����У��
    if (bitPos < 0 || bitPos > 15) {
        emit errorOccurred(QString("��Чλλ�ã�%1������0-15��").arg(bitPos));
        machineLog->write(QString("��Чλλ�ã�%1������0-15��").arg(bitPos), Normal);
        return false;
    }

    // ��ȡ�����Ĵ���
    QVector<quint16> values = readHoldingRegisters(regAddr, 1, serverAddr, timeout);
    if (values.isEmpty()) {
        return false; // ������Ϣ��ͨ��readHoldingRegisters����
    }

    // ����λ���루ʮ������ʽ��
    quint16 mask = 1 << bitPos;
    result = (values[0] & mask) != 0;
    return true;
}

bool ModbusTcpClient::writeBit(int regAddr, int bitPos, bool value,
    int serverAddr, int timeout) {
    // ����У��
    if (bitPos < 0 || bitPos > 15) {
        emit errorOccurred(QString("��Чλλ�ã�%1������0-15��").arg(bitPos));
        machineLog->write(QString("��Чλλ�ã�%1������0-15��").arg(bitPos), Normal);
        return false;
    }

    // ����1����ȡ��ǰ�Ĵ���ֵ
    QVector<quint16> currentValues = readHoldingRegisters(regAddr, 1, serverAddr, timeout);
    if (currentValues.isEmpty()) {
        return false; // ������Ϣ��ͨ��readHoldingRegisters����
    }
    quint16 currentValue = currentValues[0];

    // ����2���޸�ָ��λ
    quint16 mask = 1 << bitPos;
    quint16 newValue = value ? (currentValue | mask) : (currentValue & ~mask);

    // ����3��д����ֵ
    return writeSingleRegister(regAddr, newValue, serverAddr, timeout);
}

//д����
bool ModbusTcpClient::writeFloatRegister(int startRegAddr, float value,
    int serverAddr, int timeout, QSysInfo::Endian byteOrder, bool useMultiWrite)
{
    //quint16 regs[2];
    //memcpy(regs, &value, sizeof(value));  // ��ȫ���ڴ濽��

    //// �����ֽ�����ϵͳ�ֽ������豸Ҫ��һ�£�
    //if (QSysInfo::ByteOrder != byteOrder) {
    //    std::swap(regs[0], regs[1]);
    //}

    //// ������д��Ĵ���
    //bool successHigh = writeSingleRegister(startRegAddr, regs[0], serverAddr, timeout);
    //bool successLow = writeSingleRegister(startRegAddr + 1, regs[1], serverAddr, timeout);

    //return successHigh && successLow;


    quint16 regs[2];
    memcpy(regs, &value, sizeof(float));
    //if (QSysInfo::ByteOrder != byteOrder) {
    //    std::swap(regs[0], regs[1]);
    //}

    if (useMultiWrite) {
        QVector<quint16> values = { regs[0], regs[1] };
        return writeMultipleRegisters(startRegAddr, values, serverAddr, timeout);
    }
    else {
        bool successHigh = writeSingleRegister(startRegAddr, regs[0], serverAddr, timeout);
        bool successLow = false;
        if (successHigh) {
            successLow = writeSingleRegister(startRegAddr + 1, regs[1], serverAddr, timeout);
            if (!successLow) {
                // ��ѡ���ع���16λ������ǰ��ȡ��ֵ��
                // writeSingleRegister(startRegAddr, oldValue, ...);
            }
        }
        return successHigh && successLow;
    }
}

//������
bool ModbusTcpClient::readFloatRegister(int startRegAddr, float& output,int serverAddr,int timeout,QSysInfo::Endian byteOrder)
{
    quint16 regHigh, regLow;

    // ��ȡ��16λ�Ĵ���
    if (!readHoldingRegister(startRegAddr, regHigh, serverAddr, timeout)) {
        machineLog->write("��ȡ��������16λʧ��", Normal);
        return false;
    }

    // ��ȡ��16λ�Ĵ���
    if (!readHoldingRegister(startRegAddr + 1, regLow, serverAddr, timeout)) {
        machineLog->write("��ȡ��������16λʧ��", Normal);
        return false;
    }

    // �����ֽ���
    quint16 regs[2] = { regHigh, regLow };
    if (QSysInfo::ByteOrder != byteOrder) {
        std::swap(regs[0], regs[1]);
    }

    // ת��Ϊ������
    memcpy(&output, regs, sizeof(float));
    return true;
}

// ��������������д�����Ĵ����������� 16��
bool ModbusTcpClient::writeMultipleRegisters(int startRegAddr, const QVector<quint16>& values,
    int serverAddr, int timeout)
{
    if (!m_client || m_client->state() != QModbusDevice::ConnectedState) {
        emit errorOccurred("�ͻ���δ����");
        return false;
    }

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, startRegAddr, values);
    QModbusReply* reply = m_client->sendWriteRequest(writeUnit, serverAddr);

    if (!reply) {
        emit errorOccurred("����д������ʧ��");
        return false;
    }

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    bool timeoutOccurred = false;

    connect(&timer, &QTimer::timeout, [&]() {
        timeoutOccurred = true;
        loop.quit();
        });
    connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);

    timer.start(timeout);
    loop.exec();

    if (timeoutOccurred) {
        emit errorOccurred(QString("����д�볬ʱ��%1ms��").arg(timeout));
        reply->deleteLater();
        return false;
    }

    const bool success = (reply->error() == QModbusDevice::NoError);
    if (!success) {
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
    return success;
}