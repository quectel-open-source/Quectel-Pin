#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QTcpServer> //监听套接字
#include <QTcpSocket> //通信套接字
#include <QVector>
#include <QTimer>
#include <QThread>
// TCP套接字
class QtTcpSocket;
class QtTcpClient;
class QtcpClass  : public QTcpServer
{
	Q_OBJECT

public:
	QtcpClass(QObject *parent);
	~QtcpClass();
	/////////////////客户端///////////////////////////
	QtTcpClient *TcpClient;
	bool openclient(QString name, QString ip, int port);
	void ClientSend(QString str);
	void CloseClient();
	bool isopenclient = false;
	////////////////////////////////////////////////
	// Tcp客户端容器
	QVector<QtTcpSocket *> tcpSocketVec;
	// 发送信息(参数1: 发送的信息, 参数2: 客户端描述符(分配的TCP客户端ID))
	bool sendMsg(QByteArray msg, int socketDescriptor);
	bool isopen = false;
	//关闭服务器
	void closeServer();
private:
	// TCP客户端连接(有TCP客户端连接自动进入)
	void incomingConnection(qintptr socketDescriptor);
	//线程列表
	QMap<QThread *, quint16> threadList;
signals:
	// 发送信息信号(参数1: 发送的信息, 参数2: 客户端描述符(分配的TCP客户端ID))
	void sendData(QByteArray msg, int socketDescriptor);
	// 发送接受信息(参数1: 接受的信息, 参数2: 客户端描述符(分配的TCP客户端ID))
	void sig_RevMsg(QString msg, int socketDescriptor);
	// 发送TCP客户端断开连接信息(发送给其他对象)
	void sig_DisConnect(int socketDescriptor);
	//发送客户端句柄
	void sig_SendHandle(int socketDescriptor);
private slots:

	// TCP断开槽函数
	void slot_SocketDisconnection(int socketDescriptor);
	// 接受TCP客户端信息槽函数
	void slot_RevSocketMsg(QString msg, int socketDescriptor);
	// 接受发送信息槽函数(外部传入发送信息)
	void slot_SendMsg(QString msg, int socketDescriptor);
	////////客户端////////////
	void slot_RevTcpSocketMsg(QString clientName, const QString msg);
signals:
	void sendtext(QString, QString);
};
// TCP套接字
class QtTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	QtTcpSocket(int socket, QObject *parent);
	~QtTcpSocket();

	// 获取TCP描述符
	int getSocketDescriptor();
	// 发送信息
	bool sendMsg(QByteArray msg);
	void closeserver();
private:
	// 套接字描述符
	int socketDescriptor;

signals:
	// 发送接受TCP信息
	void sig_RevSocketMsg(QString msg, int socketDescriptor);
	// 发送TCP断开连接信息
	void disconnectHost(int socketDescriptor);

private slots:
	// TCP接受信息槽函数
	void recvData();
	// TCP发送信息槽函数
	void sendMsg(QByteArray msg, int id);
	// 断开连接槽函数
	void disconnectToHost();
	
};
class QtTcpClient : public QTcpSocket
{
	Q_OBJECT

public:
	QtTcpClient();
	~QtTcpClient();

	// 连接到服务器
	bool connectToServer(QString clientName, QString ip, int port);
	//发送信息
	void sendMsg(QString msg);
	// 是否停止通信
	void IsStop(bool isstop);

signals:
	// 发送已接受信息
	void sig_RevMsg(QString clientName, QString msg);
	// 发送运行信息
	void sig_RunInfo(QString msg, int i);
	// 发送断开连接信号
	void sig_ConnectState(QString clientName, bool flag);

private slots:
	// 接受信息
	void recvData();
	// 发送信息
	void slot_sendMsg(QString msg);
	// 服务器断开连接
	void disconnectFormServer(QAbstractSocket::SocketError);
	// 定时器槽函数
	void slot_Timeout();
public slots:
	//断开连接
	void slot_Disconnect();

private:
	// 客户端名称
	QString strClientName;
	// 服务器IP
	QString _ip;
	// 服务器端口
	int _port = 0;
	// 线程对象
	//QThread _thread;
	// TCP连接状态
	bool isConnected = false;
	// 停止连接
	bool isStoped = false;
	// 自动连接TCP定时器
	QTimer timer;
};