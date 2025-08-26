#ifndef MODBUSTCPCLIENT_H
#define MODBUSTCPCLIENT_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QModbusTcpClient>

/**
 * @brief Modbus TCP同步客户端类
 * @details 提供阻塞式读写操作，适合在非GUI线程中使用
 */
class ModbusTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit ModbusTcpClient(QObject* parent = nullptr);
    ~ModbusTcpClient();

    /**
     * @brief 连接到Modbus TCP设备
     * @param ip 目标设备IP地址，如"192.168.1.100"
     * @param port 端口号，默认502
     * @return 是否成功发起连接请求
     */
    bool connectToDevice(const QString& ip
        , int port = 502);

    /// @brief 主动断开设备连接
    void disconnectFromDevice();

    /**
     * @brief 同步读取保持寄存器（功能码03）
     * @param startAddr 起始寄存器地址（0-based）
     * @param count 要读取的寄存器数量
     * @param serverAddr 从站地址，默认1
     * @param timeout 超时时间（毫秒），默认3000
     * @return 读取到的寄存器值列表，失败返回空列表
     */
    QVector<quint16> readHoldingRegisters(int startAddr, int count,
        int serverAddr = 1,
        int timeout = 3000);

    /**
    * @brief 读取单个保持寄存器（功能码03）
    * @param startAddr 起始寄存器地址（0-based）
    * @param count 要读取的寄存器数量
    * @param serverAddr 从站地址，默认1
    * @param timeout 超时时间（毫秒），默认3000
    * @return 读取到的寄存器值列表，失败返回空列表
    */
    bool readHoldingRegister(int regAddr, quint16& output,
        int serverAddr = 1,
        int timeout = 3000);

    /**
     * @brief 同步写入单个寄存器（功能码06）
     * @param regAddr 寄存器地址（0-based）
     * @param value 要写入的16位值
     * @param serverAddr 从站地址，默认1
     * @param timeout 超时时间（毫秒），默认3000
     * @return 是否写入成功
     */
    bool writeSingleRegister(int regAddr, quint16 value,
        int serverAddr = 1,
        int timeout = 3000);

    // 添加至类的 public 部分
    /**
     * @brief 读取指定寄存器的单个位状态
     * @param regAddr 寄存器地址（十进制）
     * @param bitPos 位位置（0-15）
     * @param serverAddr 从站地址，默认1
     * @param timeout 超时时间（毫秒），默认3000
     * @return 位状态（true=高电平，false=低电平），失败返回false
     */
    bool readBit(int regAddr, int bitPos, bool& result, int serverAddr = 1, int timeout = 3000);

    /**
     * @brief 写入指定寄存器的单个位值
     * @param regAddr 寄存器地址（十进制）
     * @param bitPos 位位置（0-15）
     * @param value 要写入的值（true=置1，false=置0）
     * @param serverAddr 从站地址，默认1
     * @param timeout 超时时间（毫秒），默认3000
     * @return 是否写入成功
     */
    bool writeBit(int regAddr, int bitPos, bool value,
        int serverAddr = 1, int timeout = 3000);

    //---------------------------- 浮点数扩展函数 ----------------------------
    /**
     * @brief 写入32位浮点数到两个连续的保持寄存器
     *
     * @param startRegAddr    起始寄存器地址（对应文档地址例如40145时，输入转换为144）
     * @param value          要写入的浮点数值
     * @param serverAddr     从站设备地址（默认为1）
     * @param timeout        超时时间（毫秒）
     * @param byteOrder      字节序（默认大端序，与Modbus兼容）
     * @param useMultiWrite  是否使用批量写入（功能码16），若从站不支持功能码16可关闭
     *                       - true:  单次写入两个寄存器（高效，原子性）
     *                       - false: 分两次写入寄存器（兼容旧设备，但存在部分写入风险）
     * @return bool          写入是否成功
     */
    bool writeFloatRegister(int startRegAddr, float value,
        int serverAddr = 1,
        int timeout = 3000,
        QSysInfo::Endian byteOrder = QSysInfo::BigEndian, bool useMultiWrite = true);

    /**
     * @brief 从两个连续的Modbus寄存器读取浮点数
     * @param startRegAddr 起始寄存器地址（例如40145对应参数145）
     * @param output 输出的浮点数值
     * @param serverAddr 从站地址，默认1
     * @param timeout 超时时间（毫秒），默认3000
     * @param byteOrder 字节序（默认大端序，若设备为小端序需调整）
     * @return 是否读取成功
     * @note 浮点数需占用两个连续寄存器（startRegAddr和startRegAddr+1）
     */
    bool readFloatRegister(int startRegAddr, float& output,
        int serverAddr = 1,
        int timeout = 3000,
        QSysInfo::Endian byteOrder = QSysInfo::BigEndian);

    //写多个寄存器
    bool writeMultipleRegisters(int startRegAddr, const QVector<quint16>& values,
        int serverAddr, int timeout);
signals:
    /// @brief 错误发生时发出的信号，携带错误描述
    void errorOccurred(const QString& error);

private:
    QModbusTcpClient* m_client = nullptr; ///< Modbus客户端实例
    QString m_ip = "";
    int m_port = 502;
};

#endif // MODBUSTCPCLIENT_H