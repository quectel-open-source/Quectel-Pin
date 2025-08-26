#include "ModbusTcpClient.h"
#include <QModbusDataUnit>
#include <QEventLoop>
#include <QTimer>
#include <QVariant> 
#include "qc_log.h"

//QT 官方的库，不能在子线程调用

/**
 * @brief 构造函数，初始化Modbus客户端实例
 */
ModbusTcpClient::ModbusTcpClient(QObject* parent)
    : QObject(parent)
{
    // 创建Modbus TCP客户端对象
    m_client = new QModbusTcpClient(this);
}

/**
 * @brief 析构函数，确保断开连接
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
        emit errorOccurred("客户端未初始化");
        return false;
    }

    if (m_client->state() == QModbusDevice::ConnectedState) {
        machineLog->write("设备已连接", Normal);
        emit errorOccurred("设备已连接");
        return true;
    }
    // 设置连接参数
    //m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant("127.0.0.1"));
    m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant(ip));
    m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, QVariant(port));

    // 发起连接请求（非阻塞）
    if (!m_client->connectDevice()) {
        emit errorOccurred(m_client->errorString());
        machineLog->write("请求失败 ：" + m_client->errorString(), Normal);
        return false;
    }
    machineLog->write("Modbus tcp设备连接成功", Normal);
    return true;
}

void ModbusTcpClient::disconnectFromDevice()
{
    if (m_client && m_client->state() != QModbusDevice::UnconnectedState) {
        // 断开连接并释放资源
        m_client->disconnectDevice();
    }
}

////读取多个寄存器
//QVector<quint16> ModbusTcpClient::readHoldingRegisters(int startAddr, int count,
//    int serverAddr, int timeout)
//{
//    QVector<quint16> result;
//    bool timeoutOccurred = false;
//
//    // 前置条件检查
//    if (!m_client) {
//        emit errorOccurred("客户端未初始化");
//        machineLog->write("客户端未初始化", Normal);
//        return result;
//    }
//    if (m_client->state() != QModbusDevice::ConnectedState) {
//        emit errorOccurred("设备未连接");
//        machineLog->write("客户端未初始化", Normal);
//        return result;
//    }
//
//    QEventLoop loop;
//    QTimer timer;
//    timer.setSingleShot(true);
//
//    // 发送读取请求
//    QModbusReply* reply = m_client->sendReadRequest(
//        QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddr, count),
//        serverAddr
//    );
//
//    if (!reply) {
//        emit errorOccurred("创建读取请求失败");
//        machineLog->write("创建读取请求失败", Normal);
//        return result;
//    }
//
//    // 信号连接
//    QObject::connect(&timer, &QTimer::timeout, [&]() {
//        timeoutOccurred = true;
//        loop.quit();
//        });
//
//    QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
//
//    // 启动等待
//    timer.start(timeout);
//    loop.exec();
//
//    // 处理结果
//    if (timeoutOccurred) {
//        emit errorOccurred(QString("读取操作超时（%1ms）").arg(timeout));
//        machineLog->write(QString("读取操作超时（%1ms）").arg(timeout), Normal);
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
//        emit errorOccurred(QString("读取错误：%1").arg(reply->errorString()));
//        machineLog->write(QString("读取错误：%1").arg(reply->errorString()), Normal);
//    }
//
//    reply->deleteLater();
//    return result;
//}


QVector<quint16> ModbusTcpClient::readHoldingRegisters(int startAddr, int count,
    int serverAddr, int timeout)
{
    const int MAX_RETRY = 5; // 最大重试次数
    const int RECONNECT_DELAY = 1000; // 重连延迟(ms)
    int retryCount = 0;
    QVector<quint16> result;

    // 主重试循环
    while (retryCount < MAX_RETRY) {
        // 前置条件检查
        if (!m_client) {
            emit errorOccurred("客户端未初始化");
            break;
        }

        // 检查连接状态（自动重连）
        if (m_client->state() != QModbusDevice::ConnectedState) {
            machineLog->write("modbus检测到连接断开，尝试重新连接", Normal);
            if (!connectToDevice(m_ip, m_port)) {
                machineLog->write("modbus重新连接失败，准备重试", Normal);
                retryCount++;
                QThread::msleep(RECONNECT_DELAY);
                continue;
            }
        }

        bool timeoutOccurred = false;
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        // 发送读取请求
        QModbusReply* reply = m_client->sendReadRequest(
            QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddr, count),
            serverAddr
        );

        if (!reply) {
            emit errorOccurred("创建读取请求失败");
            machineLog->write("创建读取请求失败，准备重试", Normal);
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        // 信号连接
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            timeoutOccurred = true;
            loop.quit();
            });

        QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);

        // 启动等待
        timer.start(timeout);
        loop.exec();

        // 处理结果
        if (timeoutOccurred) {
            QString errorMsg = QString("读取操作超时(%1/%2重试)").arg(retryCount + 1).arg(MAX_RETRY);
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            reply->deleteLater();
            disconnectFromDevice(); // 超时后断开连接
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
            return result; // 成功读取，直接返回
        }
        else {
            emit errorOccurred(QString("读取错误：%1").arg(reply->errorString()));
            machineLog->write(QString("读取错误：%1，准备重试").arg(reply->errorString()), Normal);
            reply->deleteLater();

            // 如果是连接类错误，则断开连接
            if (reply->error() == QModbusDevice::ConnectionError ||
                reply->error() == QModbusDevice::TimeoutError) {
                disconnectFromDevice();
            }

            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
        }
    }

    // 所有重试失败后
    if (retryCount >= MAX_RETRY) {
        emit errorOccurred("读取失败：超过最大重试次数");
        machineLog->write("读取失败：超过最大重试次数", Normal);
    }
    return QVector<quint16>(); // 返回空结果
}

//读取单个寄存器
bool ModbusTcpClient::readHoldingRegister(int regAddr, quint16& output,int serverAddr,int timeout)
{
    QVector<quint16> regs;

    // 调用原有多寄存器读取函数（内部逻辑需适配）
    regs = readHoldingRegisters(regAddr, 1, serverAddr, timeout);

    if (regs.size() != 1) {
        emit errorOccurred("读取寄存器失败");
        machineLog->write("读取寄存器失败", Normal);
        return false;
    }

    output = regs[0];
    return true;
}

//写单个寄存器
//bool ModbusTcpClient::writeSingleRegister(int regAddr, quint16 value,
//    int serverAddr, int timeout)
//{
//    bool timeoutOccurred = false;
//
//    // 前置条件检查
//    if (!m_client) {
//        emit errorOccurred("客户端未初始化");
//        machineLog->write("modbusTcp客户端未初始化", Normal);
//        return false;
//    }
//    if (m_client->state() != QModbusDevice::ConnectedState) {
//        emit errorOccurred("设备未连接");
//        machineLog->write("modbusTcp设备未连接", Normal);
//        return false;
//    }
//
//    QEventLoop loop;
//    QTimer timer;
//    timer.setSingleShot(true);
//
//    // 创建写入请求
//    //QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regAddr, { value });
//    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regAddr, 1);
//    writeUnit.setValue(0, value);
//    QModbusReply* reply = m_client->sendWriteRequest(writeUnit, serverAddr);
//
//    if (!reply) {
//        emit errorOccurred("创建写入请求失败");
//        machineLog->write("创建写入请求失败", Normal);
//        return false;
//    }
//
//    // 信号连接
//    QObject::connect(&timer, &QTimer::timeout, [&]() {
//        timeoutOccurred = true;
//        loop.quit();
//        });
//
//    QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
//
//    // 启动等待
//    timer.start(timeout);
//    loop.exec();
//
//    // 处理结果
//    if (timeoutOccurred) {
//        emit errorOccurred(QString("写入操作超时（%1ms）").arg(timeout));
//        machineLog->write(QString("写入操作超时（%1ms）").arg(timeout), Normal);
//        reply->deleteLater();
//        return false;
//    }
//
//    const bool success = (reply->error() == QModbusDevice::NoError);
//    if (!success) {
//        if (reply->error() == QModbusDevice::ProtocolError) {
//            quint8 exCode = reply->rawResult().exceptionCode();
//            emit errorOccurred(QString("Modbus异常码：0x%1").arg(exCode, 2, 16, QChar('0')));
//            machineLog->write(QString("Modbus异常码：0x%1").arg(exCode, 2, 16, QChar('0')), Normal);
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
    const int MAX_RETRY = 5; // 最大重试次数
    const int RECONNECT_DELAY = 1000; // 重连延迟(ms)
    int retryCount = 0;

    while (retryCount < MAX_RETRY) {
        // 前置条件检查
        if (!m_client) {
            emit errorOccurred("客户端未初始化");
            machineLog->write("modbusTcp客户端未初始化", Normal);
            return false;
        }

        // 检查并自动重新连接
        if (m_client->state() != QModbusDevice::ConnectedState) {
            machineLog->write("检测到连接断开，尝试重新连接", Normal);
            if (!connectToDevice(m_ip, m_port)) {
                machineLog->write("重新连接失败，准备重试", Normal);
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

        // 创建写入请求
        QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regAddr, 1);
        writeUnit.setValue(0, value);
        QModbusReply* reply = m_client->sendWriteRequest(writeUnit, serverAddr);

        if (!reply) {
            emit errorOccurred("创建写入请求失败");
            machineLog->write("创建写入请求失败，准备重试", Normal);
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        // 信号连接
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            timeoutOccurred = true;
            loop.quit();
            });

        QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);

        // 启动等待
        timer.start(timeout);
        loop.exec();

        // 处理超时
        if (timeoutOccurred) {
            QString errorMsg = QString("写入操作超时(%1/%2重试)").arg(retryCount + 1).arg(MAX_RETRY);
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            reply->deleteLater();
            disconnectFromDevice(); // 超时后断开连接
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
            continue;
        }

        // 检查写入结果
        const bool success = (reply->error() == QModbusDevice::NoError);

        if (success) {
            reply->deleteLater();
            return true; // 写入成功
        }

        // 处理错误
        if (reply->error() == QModbusDevice::ProtocolError) {
            quint8 exCode = reply->rawResult().exceptionCode();
            QString errorMsg = QString("Modbus异常码: 0x%1").arg(exCode, 2, 16, QChar('0'));
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            // 协议错误不可恢复，不重试
            reply->deleteLater();
            return false;
        }
        else {
            QString errorMsg = QString("写入错误: %1 (准备重试)").arg(reply->errorString());
            emit errorOccurred(errorMsg);
            machineLog->write(errorMsg, Normal);

            // 连接错误则断开重连
            if (reply->error() == QModbusDevice::ConnectionError ||
                reply->error() == QModbusDevice::TimeoutError) {
                disconnectFromDevice();
            }

            reply->deleteLater();
            retryCount++;
            QThread::msleep(RECONNECT_DELAY);
        }
    }

    // 所有重试失败
    emit errorOccurred("写入失败: 超过最大重试次数");
    machineLog->write("写入失败: 超过最大重试次数", Normal);
    return false;
}

bool ModbusTcpClient::readBit(int regAddr, int bitPos, bool& result, int serverAddr, int timeout) {
    // 参数校验
    if (bitPos < 0 || bitPos > 15) {
        emit errorOccurred(QString("无效位位置：%1（必须0-15）").arg(bitPos));
        machineLog->write(QString("无效位位置：%1（必须0-15）").arg(bitPos), Normal);
        return false;
    }

    // 读取整个寄存器
    QVector<quint16> values = readHoldingRegisters(regAddr, 1, serverAddr, timeout);
    if (values.isEmpty()) {
        return false; // 错误信息已通过readHoldingRegisters发出
    }

    // 计算位掩码（十进制形式）
    quint16 mask = 1 << bitPos;
    result = (values[0] & mask) != 0;
    return true;
}

bool ModbusTcpClient::writeBit(int regAddr, int bitPos, bool value,
    int serverAddr, int timeout) {
    // 参数校验
    if (bitPos < 0 || bitPos > 15) {
        emit errorOccurred(QString("无效位位置：%1（必须0-15）").arg(bitPos));
        machineLog->write(QString("无效位位置：%1（必须0-15）").arg(bitPos), Normal);
        return false;
    }

    // 步骤1：读取当前寄存器值
    QVector<quint16> currentValues = readHoldingRegisters(regAddr, 1, serverAddr, timeout);
    if (currentValues.isEmpty()) {
        return false; // 错误信息已通过readHoldingRegisters发出
    }
    quint16 currentValue = currentValues[0];

    // 步骤2：修改指定位
    quint16 mask = 1 << bitPos;
    quint16 newValue = value ? (currentValue | mask) : (currentValue & ~mask);

    // 步骤3：写入新值
    return writeSingleRegister(regAddr, newValue, serverAddr, timeout);
}

//写浮点
bool ModbusTcpClient::writeFloatRegister(int startRegAddr, float value,
    int serverAddr, int timeout, QSysInfo::Endian byteOrder, bool useMultiWrite)
{
    //quint16 regs[2];
    //memcpy(regs, &value, sizeof(value));  // 安全的内存拷贝

    //// 调整字节序（若系统字节序与设备要求不一致）
    //if (QSysInfo::ByteOrder != byteOrder) {
    //    std::swap(regs[0], regs[1]);
    //}

    //// 分两次写入寄存器
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
                // 可选：回滚高16位（需提前读取旧值）
                // writeSingleRegister(startRegAddr, oldValue, ...);
            }
        }
        return successHigh && successLow;
    }
}

//读浮点
bool ModbusTcpClient::readFloatRegister(int startRegAddr, float& output,int serverAddr,int timeout,QSysInfo::Endian byteOrder)
{
    quint16 regHigh, regLow;

    // 读取高16位寄存器
    if (!readHoldingRegister(startRegAddr, regHigh, serverAddr, timeout)) {
        machineLog->write("读取浮点数高16位失败", Normal);
        return false;
    }

    // 读取低16位寄存器
    if (!readHoldingRegister(startRegAddr + 1, regLow, serverAddr, timeout)) {
        machineLog->write("读取浮点数低16位失败", Normal);
        return false;
    }

    // 调整字节序
    quint16 regs[2] = { regHigh, regLow };
    if (QSysInfo::ByteOrder != byteOrder) {
        std::swap(regs[0], regs[1]);
    }

    // 转换为浮点数
    memcpy(&output, regs, sizeof(float));
    return true;
}

// 新增函数：批量写入多个寄存器（功能码 16）
bool ModbusTcpClient::writeMultipleRegisters(int startRegAddr, const QVector<quint16>& values,
    int serverAddr, int timeout)
{
    if (!m_client || m_client->state() != QModbusDevice::ConnectedState) {
        emit errorOccurred("客户端未连接");
        return false;
    }

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, startRegAddr, values);
    QModbusReply* reply = m_client->sendWriteRequest(writeUnit, serverAddr);

    if (!reply) {
        emit errorOccurred("创建写入请求失败");
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
        emit errorOccurred(QString("批量写入超时（%1ms）").arg(timeout));
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