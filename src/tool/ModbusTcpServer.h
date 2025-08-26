#pragma once

#include <QObject>
#include <QModbusTcpServer>
#include <QModbusDataUnitMap>
#include <QHostAddress>
#include "qc_log.h"

class ModbusTcpServer : public QObject
{
    Q_OBJECT

public:
    explicit ModbusTcpServer(QObject* parent = nullptr);
    ~ModbusTcpServer();

    bool startServer(const QHostAddress& address = QHostAddress::Any,
        quint16 port = 502,
        int numHoldingRegisters = 1000);
    void stopServer();

    // �Ĵ�������
    bool setHoldingRegister(int address, quint16 value);
    quint16 getHoldingRegister(int address) const;

    // ������֧��
    bool setFloatRegister(int startAddress, float value, QSysInfo::Endian byteOrder = QSysInfo::BigEndian);
    float getFloatRegister(int startAddress, QSysInfo::Endian byteOrder = QSysInfo::BigEndian) const;

private:
    QModbusTcpServer* m_server;
    QModbusDataUnitMap m_dataMap;
    QHash<int, quint16> m_holdingRegisters; // ���ּĴ����洢

    void initDataStorage(int numHoldingRegisters);
    void logError(const QString& message) const;

signals:
    void clientConnected();
    void clientDisconnected();
    void errorOccurred(const QString& error);
};