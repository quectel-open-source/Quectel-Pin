#ifndef MODBUSTCPCLIENT_H
#define MODBUSTCPCLIENT_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QModbusTcpClient>

/**
 * @brief Modbus TCPͬ���ͻ�����
 * @details �ṩ����ʽ��д�������ʺ��ڷ�GUI�߳���ʹ��
 */
class ModbusTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit ModbusTcpClient(QObject* parent = nullptr);
    ~ModbusTcpClient();

    /**
     * @brief ���ӵ�Modbus TCP�豸
     * @param ip Ŀ���豸IP��ַ����"192.168.1.100"
     * @param port �˿ںţ�Ĭ��502
     * @return �Ƿ�ɹ�������������
     */
    bool connectToDevice(const QString& ip
        , int port = 502);

    /// @brief �����Ͽ��豸����
    void disconnectFromDevice();

    /**
     * @brief ͬ����ȡ���ּĴ�����������03��
     * @param startAddr ��ʼ�Ĵ�����ַ��0-based��
     * @param count Ҫ��ȡ�ļĴ�������
     * @param serverAddr ��վ��ַ��Ĭ��1
     * @param timeout ��ʱʱ�䣨���룩��Ĭ��3000
     * @return ��ȡ���ļĴ���ֵ�б�ʧ�ܷ��ؿ��б�
     */
    QVector<quint16> readHoldingRegisters(int startAddr, int count,
        int serverAddr = 1,
        int timeout = 3000);

    /**
    * @brief ��ȡ�������ּĴ�����������03��
    * @param startAddr ��ʼ�Ĵ�����ַ��0-based��
    * @param count Ҫ��ȡ�ļĴ�������
    * @param serverAddr ��վ��ַ��Ĭ��1
    * @param timeout ��ʱʱ�䣨���룩��Ĭ��3000
    * @return ��ȡ���ļĴ���ֵ�б�ʧ�ܷ��ؿ��б�
    */
    bool readHoldingRegister(int regAddr, quint16& output,
        int serverAddr = 1,
        int timeout = 3000);

    /**
     * @brief ͬ��д�뵥���Ĵ�����������06��
     * @param regAddr �Ĵ�����ַ��0-based��
     * @param value Ҫд���16λֵ
     * @param serverAddr ��վ��ַ��Ĭ��1
     * @param timeout ��ʱʱ�䣨���룩��Ĭ��3000
     * @return �Ƿ�д��ɹ�
     */
    bool writeSingleRegister(int regAddr, quint16 value,
        int serverAddr = 1,
        int timeout = 3000);

    // �������� public ����
    /**
     * @brief ��ȡָ���Ĵ����ĵ���λ״̬
     * @param regAddr �Ĵ�����ַ��ʮ���ƣ�
     * @param bitPos λλ�ã�0-15��
     * @param serverAddr ��վ��ַ��Ĭ��1
     * @param timeout ��ʱʱ�䣨���룩��Ĭ��3000
     * @return λ״̬��true=�ߵ�ƽ��false=�͵�ƽ����ʧ�ܷ���false
     */
    bool readBit(int regAddr, int bitPos, bool& result, int serverAddr = 1, int timeout = 3000);

    /**
     * @brief д��ָ���Ĵ����ĵ���λֵ
     * @param regAddr �Ĵ�����ַ��ʮ���ƣ�
     * @param bitPos λλ�ã�0-15��
     * @param value Ҫд���ֵ��true=��1��false=��0��
     * @param serverAddr ��վ��ַ��Ĭ��1
     * @param timeout ��ʱʱ�䣨���룩��Ĭ��3000
     * @return �Ƿ�д��ɹ�
     */
    bool writeBit(int regAddr, int bitPos, bool value,
        int serverAddr = 1, int timeout = 3000);

    //---------------------------- ��������չ���� ----------------------------
    /**
     * @brief д��32λ�����������������ı��ּĴ���
     *
     * @param startRegAddr    ��ʼ�Ĵ�����ַ����Ӧ�ĵ���ַ����40145ʱ������ת��Ϊ144��
     * @param value          Ҫд��ĸ�����ֵ
     * @param serverAddr     ��վ�豸��ַ��Ĭ��Ϊ1��
     * @param timeout        ��ʱʱ�䣨���룩
     * @param byteOrder      �ֽ���Ĭ�ϴ������Modbus���ݣ�
     * @param useMultiWrite  �Ƿ�ʹ������д�루������16��������վ��֧�ֹ�����16�ɹر�
     *                       - true:  ����д�������Ĵ�������Ч��ԭ���ԣ�
     *                       - false: ������д��Ĵ��������ݾ��豸�������ڲ���д����գ�
     * @return bool          д���Ƿ�ɹ�
     */
    bool writeFloatRegister(int startRegAddr, float value,
        int serverAddr = 1,
        int timeout = 3000,
        QSysInfo::Endian byteOrder = QSysInfo::BigEndian, bool useMultiWrite = true);

    /**
     * @brief ������������Modbus�Ĵ�����ȡ������
     * @param startRegAddr ��ʼ�Ĵ�����ַ������40145��Ӧ����145��
     * @param output ����ĸ�����ֵ
     * @param serverAddr ��վ��ַ��Ĭ��1
     * @param timeout ��ʱʱ�䣨���룩��Ĭ��3000
     * @param byteOrder �ֽ���Ĭ�ϴ�������豸ΪС�����������
     * @return �Ƿ��ȡ�ɹ�
     * @note ��������ռ�����������Ĵ�����startRegAddr��startRegAddr+1��
     */
    bool readFloatRegister(int startRegAddr, float& output,
        int serverAddr = 1,
        int timeout = 3000,
        QSysInfo::Endian byteOrder = QSysInfo::BigEndian);

    //д����Ĵ���
    bool writeMultipleRegisters(int startRegAddr, const QVector<quint16>& values,
        int serverAddr, int timeout);
signals:
    /// @brief ������ʱ�������źţ�Я����������
    void errorOccurred(const QString& error);

private:
    QModbusTcpClient* m_client = nullptr; ///< Modbus�ͻ���ʵ��
    QString m_ip = "";
    int m_port = 502;
};

#endif // MODBUSTCPCLIENT_H