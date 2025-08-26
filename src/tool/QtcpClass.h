#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QTcpServer> //�����׽���
#include <QTcpSocket> //ͨ���׽���
#include <QVector>
#include <QTimer>
#include <QThread>
// TCP�׽���
class QtTcpSocket;
class QtTcpClient;
class QtcpClass  : public QTcpServer
{
	Q_OBJECT

public:
	QtcpClass(QObject *parent);
	~QtcpClass();
	/////////////////�ͻ���///////////////////////////
	QtTcpClient *TcpClient;
	bool openclient(QString name, QString ip, int port);
	void ClientSend(QString str);
	void CloseClient();
	bool isopenclient = false;
	////////////////////////////////////////////////
	// Tcp�ͻ�������
	QVector<QtTcpSocket *> tcpSocketVec;
	// ������Ϣ(����1: ���͵���Ϣ, ����2: �ͻ���������(�����TCP�ͻ���ID))
	bool sendMsg(QByteArray msg, int socketDescriptor);
	bool isopen = false;
	//�رշ�����
	void closeServer();
private:
	// TCP�ͻ�������(��TCP�ͻ��������Զ�����)
	void incomingConnection(qintptr socketDescriptor);
	//�߳��б�
	QMap<QThread *, quint16> threadList;
signals:
	// ������Ϣ�ź�(����1: ���͵���Ϣ, ����2: �ͻ���������(�����TCP�ͻ���ID))
	void sendData(QByteArray msg, int socketDescriptor);
	// ���ͽ�����Ϣ(����1: ���ܵ���Ϣ, ����2: �ͻ���������(�����TCP�ͻ���ID))
	void sig_RevMsg(QString msg, int socketDescriptor);
	// ����TCP�ͻ��˶Ͽ�������Ϣ(���͸���������)
	void sig_DisConnect(int socketDescriptor);
	//���Ϳͻ��˾��
	void sig_SendHandle(int socketDescriptor);
private slots:

	// TCP�Ͽ��ۺ���
	void slot_SocketDisconnection(int socketDescriptor);
	// ����TCP�ͻ�����Ϣ�ۺ���
	void slot_RevSocketMsg(QString msg, int socketDescriptor);
	// ���ܷ�����Ϣ�ۺ���(�ⲿ���뷢����Ϣ)
	void slot_SendMsg(QString msg, int socketDescriptor);
	////////�ͻ���////////////
	void slot_RevTcpSocketMsg(QString clientName, const QString msg);
signals:
	void sendtext(QString, QString);
};
// TCP�׽���
class QtTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	QtTcpSocket(int socket, QObject *parent);
	~QtTcpSocket();

	// ��ȡTCP������
	int getSocketDescriptor();
	// ������Ϣ
	bool sendMsg(QByteArray msg);
	void closeserver();
private:
	// �׽���������
	int socketDescriptor;

signals:
	// ���ͽ���TCP��Ϣ
	void sig_RevSocketMsg(QString msg, int socketDescriptor);
	// ����TCP�Ͽ�������Ϣ
	void disconnectHost(int socketDescriptor);

private slots:
	// TCP������Ϣ�ۺ���
	void recvData();
	// TCP������Ϣ�ۺ���
	void sendMsg(QByteArray msg, int id);
	// �Ͽ����Ӳۺ���
	void disconnectToHost();
	
};
class QtTcpClient : public QTcpSocket
{
	Q_OBJECT

public:
	QtTcpClient();
	~QtTcpClient();

	// ���ӵ�������
	bool connectToServer(QString clientName, QString ip, int port);
	//������Ϣ
	void sendMsg(QString msg);
	// �Ƿ�ֹͣͨ��
	void IsStop(bool isstop);

signals:
	// �����ѽ�����Ϣ
	void sig_RevMsg(QString clientName, QString msg);
	// ����������Ϣ
	void sig_RunInfo(QString msg, int i);
	// ���ͶϿ������ź�
	void sig_ConnectState(QString clientName, bool flag);

private slots:
	// ������Ϣ
	void recvData();
	// ������Ϣ
	void slot_sendMsg(QString msg);
	// �������Ͽ�����
	void disconnectFormServer(QAbstractSocket::SocketError);
	// ��ʱ���ۺ���
	void slot_Timeout();
public slots:
	//�Ͽ�����
	void slot_Disconnect();

private:
	// �ͻ�������
	QString strClientName;
	// ������IP
	QString _ip;
	// �������˿�
	int _port = 0;
	// �̶߳���
	//QThread _thread;
	// TCP����״̬
	bool isConnected = false;
	// ֹͣ����
	bool isStoped = false;
	// �Զ�����TCP��ʱ��
	QTimer timer;
};