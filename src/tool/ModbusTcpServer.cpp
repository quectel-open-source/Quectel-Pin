#include "ModbusTcpServer.h"

ModbusTcpServer::ModbusTcpServer(QObject* parent)
    : QObject(parent), m_server(new QModbusTcpServer(this))
{
    connect(m_server, &QModbusServer::stateChanged,
        [this](QModbusDevice::State state) {
            if (state == QModbusDevice::ConnectedState) {
                emit clientConnected();
            }
            else {
                emit clientDisconnected();
            }
        });

    connect(m_server, &QModbusServer::errorOccurred,
        [this](QModbusDevice::Error error) {
            emit errorOccurred(m_server->errorString());
        });
}

ModbusTcpServer::~ModbusTcpServer()
{
    stopServer();
}

bool ModbusTcpServer::startServer(const QHostAddress& address, quint16 port, int numHoldingRegisters)
{
    if (!m_server) return false;

    // 初始化数据存储
    initDataStorage(numHoldingRegisters);

    // 配置服务器参数
    m_server->setConnectionParameter(QModbusDevice::NetworkAddressParameter, address.toString());
    m_server->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    m_server->setServerAddress(1); // 默认服务器地址

    if (!m_server->connectDevice()) {
        logError("Server start failed: " + m_server->errorString());
        return false;
    }
    return true;
}

void ModbusTcpServer::stopServer()
{
    if (m_server && m_server->state() != QModbusDevice::UnconnectedState) {
        m_server->disconnectDevice();
    }
}

void ModbusTcpServer::initDataStorage(int numHoldingRegisters)
{
    // 初始化保持寄存器
    QModbusDataUnit holdingRegisters(QModbusDataUnit::HoldingRegisters, 0, numHoldingRegisters);
    m_dataMap.insert(holdingRegisters.registerType(), holdingRegisters);
    m_server->setMap(m_dataMap);
}

// 寄存器操作
bool ModbusTcpServer::setHoldingRegister(int address, quint16 value)
{
    if (address < 0 || address >= m_holdingRegisters.size()) {
        logError("Invalid register address: " + QString::number(address));
        return false;
    }

    m_holdingRegisters[address] = value;
    return m_server->setData(QModbusDataUnit::HoldingRegisters, address, value);
}

quint16 ModbusTcpServer::getHoldingRegister(int address) const
{
    if (address < 0 || address >= m_holdingRegisters.size()) {
        logError("Invalid register address: " + QString::number(address));
        return 0;
    }
    return m_holdingRegisters.value(address, 0);
}

// 浮点数支持
bool ModbusTcpServer::setFloatRegister(int startAddress, float value, QSysInfo::Endian byteOrder)
{
    quint16 regs[2];
    memcpy(regs, &value, sizeof(float));

    if (QSysInfo::ByteOrder != byteOrder) {
        std::swap(regs[0], regs[1]);
    }

    return setHoldingRegister(startAddress, regs[0]) &&
        setHoldingRegister(startAddress + 1, regs[1]);
}

float ModbusTcpServer::getFloatRegister(int startAddress, QSysInfo::Endian byteOrder) const
{
    quint16 regHigh = getHoldingRegister(startAddress);
    quint16 regLow = getHoldingRegister(startAddress + 1);

    quint16 regs[2] = { regHigh, regLow };
    if (QSysInfo::ByteOrder != byteOrder) {
        std::swap(regs[0], regs[1]);
    }

    float result;
    memcpy(&result, regs, sizeof(float));
    return result;
}

void ModbusTcpServer::logError(const QString& message) const
{
    machineLog->write(message, Normal);
    //emit errorOccurred(message);
}