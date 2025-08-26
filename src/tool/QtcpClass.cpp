#include "QtcpClass.h"

///////////////////////QtTcpSocket类//////////////////////////
QtTcpSocket::QtTcpSocket(int socket, QObject *parent)
	: QTcpSocket(parent)
{
	// 设置套接字描述符
	socketDescriptor = socket;
	// 绑定套接字信息接受和断开信号槽
	connect(this, SIGNAL(readyRead()), this, SLOT(recvData()));
	connect(this, SIGNAL(disconnected()), this, SLOT(disconnectToHost()));
}

QtTcpSocket::~QtTcpSocket()
{

}

// 获取套接字描述符
int QtTcpSocket::getSocketDescriptor()
{
	return socketDescriptor;
}

// 发送信息到客户端
bool QtTcpSocket::sendMsg(QByteArray msg)
{
	this->write(msg);
	bool ret = this->flush();

	if (!ret)
	{
		qDebug() << "发送信息失败:" + QString::fromLocal8Bit(msg);
	}
	return ret;
}

// 接受套接字信息
void QtTcpSocket::recvData()
{
	// 读取该客户端所有信息(可能存在连包现象，注意判断)
	QByteArray buffer = this->readAll();
	// 转换成QString
	QString msg = QString::fromLocal8Bit(buffer);
	// 发送已接收信息到服务器
	emit sig_RevSocketMsg(msg, socketDescriptor);
}

// 发送信息到客户端
void QtTcpSocket::sendMsg(QByteArray msg, int id)
{
	if (id == socketDescriptor)
	{
		this->write(msg);
	}
}

// 客户端断开连接
void QtTcpSocket::disconnectToHost()
{
	emit disconnectHost(socketDescriptor);
}

void QtTcpSocket::closeserver()
{
	this->disconnectFromHost();
	this->close();
}
/////////////////client//////////////////////////
QtTcpClient::QtTcpClient()
{
	// 开启线程
	//this->moveToThread(&_thread);
	//_thread.start();

	timer.setSingleShot(true);
	connect(this, SIGNAL(readyRead()), this, SLOT(recvData()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(disconnectFormServer(QAbstractSocket::SocketError)));
	connect(&timer, SIGNAL(timeout()), this, SLOT(slot_Timeout()));
}

QtTcpClient::~QtTcpClient()
{
	timer.stop();
	//取消已有的连接  
	this->abort();
}

bool QtTcpClient::connectToServer(QString clientName, QString ip, int port)
{
	_ip = ip;
	_port = port;
	strClientName = clientName;
	////取消已有的连接  
	//this->abort();
	//连接服务器  
	this->connectToHost(QHostAddress(ip), port);
	//等待连接成功  
	if (!this->waitForConnected(100))
	{
		isConnected = false;
		timer.start(3000);
	}
	else
	{
		isConnected = true;
		//this->sendMsg(QString("已连接"));//连接成功后发送CCD给PLC
		//emit sig_ConnectState(strClientName, true);//True代表连接成功

	}
	return isConnected;
}

// 停止通信
void  QtTcpClient::IsStop(bool isstop)
{
	//qDebug() << "MES： " << isstop;
	isStoped = isstop;
	if (!isStoped)
	{
		if (!isConnected)
		{
			connectToServer(strClientName, _ip, _port);
		}
	}
}
//断开连接
void QtTcpClient::slot_Disconnect()
{
	//取消已有的连接  
	this->abort();
	emit sig_RunInfo("客户端断开连接", 1);
}

void QtTcpClient::sendMsg(QString msg)
{
	this->write(msg.toLocal8Bit());
	qDebug() << "发送信息:" + msg;
	if (!flush())
	{
		emit sig_RunInfo(strClientName + ": 发送信息失败 ", 1);
	}
}

void QtTcpClient::recvData()
{
	QByteArray buffer = this->readAll();
	// 转换成QString
	QString msg = QString::fromLocal8Bit(buffer);
	// 发送已接收信息到上位机
	emit sig_RevMsg(strClientName, msg);

}

void QtTcpClient::slot_sendMsg(QString msg)
{
	this->write(msg.toLocal8Bit());
	if (!flush())
	{
		emit sig_RunInfo(strClientName + ": 发送信息失败 >> " + msg, 1);
	}
}

void QtTcpClient::disconnectFormServer(QAbstractSocket::SocketError)
{
	if (this->errorString().contains("close"))
	{
		isConnected = false;
		timer.start(3000);
		emit sig_RunInfo(strClientName + ": 服务器断开连接", 1);
		emit sig_ConnectState(strClientName, false);
	}
}

// 定时器槽函数，断开自动重连
void QtTcpClient::slot_Timeout()
{
	//qDebug() << strClientName + "自动连接服务器中....";
	if (!isConnected)
	{
		connectToServer(strClientName, _ip, _port);
	}
}


////////////////////////////////////////////////////////////////

QtcpClass::QtcpClass(QObject *parent)
	:QTcpServer(parent)
{
	TcpClient = new QtTcpClient();
	connect(TcpClient, SIGNAL(sig_RevMsg(QString, QString)), this, SLOT(slot_RevTcpSocketMsg(QString, QString)));
	connect(TcpClient, SIGNAL(sig_RunInfo(QString, int)), this, SLOT(slot_RunInfo(QString, int)));
	connect(this, SIGNAL(sig_sendMsg(QString)), TcpClient, SLOT(slot_sendMsg(QString)));
}

QtcpClass::~QtcpClass()
{
	if (TcpClient) {
		TcpClient->disconnectFromHost(); 
		delete TcpClient;                
		TcpClient = nullptr;             
	}

}


// TCP客户端连接(有TCP客户端连接自动进入)
void QtcpClass::incomingConnection(qintptr socketDescriptor)
{
	// 创建新的TCP套接字对象
	QtTcpSocket *tcpSocket = new QtTcpSocket(socketDescriptor, 0);

	//设置TCP套接字描述符
	if (!tcpSocket->setSocketDescriptor(socketDescriptor))
	{
		qDebug() << "set socket descriptor error!";
		return;
	}
	// 绑定TCP套接字和服务器的信号槽
	connect(tcpSocket, SIGNAL(sig_RevSocketMsg(QString, int)), this, SLOT(slot_RevSocketMsg(QString, int)));
	connect(tcpSocket, SIGNAL(disconnectHost(int)), this, SLOT(slot_SocketDisconnection(int)));
	// 添加到套接字容器
	tcpSocketVec.append(tcpSocket);
	//发送接入的客户端句柄到调用的类
	emit sig_SendHandle(socketDescriptor);

	qDebug() << "新客户端连接: " << socketDescriptor << tcpSocketVec.count();
}
// 服务器接受客户端信息
bool QtcpClass::sendMsg(QByteArray msg, int socketDescriptor)
{
	// 如果socketDescriptor = -1，群发送所有已连接的客户端
	if (socketDescriptor == -1)
	{
		int ngCount = 0;
		for (int i = 0; i < tcpSocketVec.count(); i++)
		{
			if (!tcpSocketVec[i]->sendMsg(msg))
			{
				ngCount += 1;
			}
			if (i == tcpSocketVec.count() - 1)
			{
				if (ngCount == 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				continue;
			}
		}
	}
	else
	{
		for (int i = 0; i < tcpSocketVec.count(); i++)
		{
			// 发送信息到指定客户端
			if (socketDescriptor == tcpSocketVec[i]->getSocketDescriptor())
			{
				return tcpSocketVec[i]->sendMsg(msg);
			}
		}
	}
	return false;
}

// 客户端从服务器断开
void QtcpClass::slot_SocketDisconnection(int socketDescriptor)
{
	for (int i = 0; i < tcpSocketVec.count(); i++)
	{
		// 容器删除掉线客户端
		if (socketDescriptor == tcpSocketVec[i]->getSocketDescriptor())
		{
			tcpSocketVec.remove(i);
			break;
		}
	}
	// 发送客户端掉线信号到外部
	emit sig_DisConnect(socketDescriptor);
}
// 接受客户端信息
void QtcpClass::slot_RevSocketMsg(QString msg, int socketNum)
{
	// 发送已接受的信息到外部
	emit sig_RevMsg(msg, socketNum);
}
// 发送信息到客户端
void QtcpClass::slot_SendMsg(QString msg, int socketNum)
{
	//emit sendData(msg, socketNum);
	sendMsg(msg.toLocal8Bit(), socketNum);
}
void  QtcpClass::closeServer()
{
	for (int i = 0; i < tcpSocketVec.size(); i++)
	{
		tcpSocketVec[i]->close();
		tcpSocketVec[i]->disconnectFromHost();
		tcpSocketVec[i]->deleteLater();///>不立即销毁对象，向主消息循环发送了一个event，下一次主消息循环收到这个event之后才会销毁对象
	}
	tcpSocketVec.clear();
}

bool QtcpClass::openclient(QString name,QString ip,int port)
{
	if (!TcpClient->connectToServer(name, ip, port))
	{
		return false;
	}
	isopenclient = true;
	return true;
}
void QtcpClass::ClientSend(QString str)
{
	if (isopenclient == false)
	{
		return;
	}
	TcpClient->sendMsg(str);
}
void QtcpClass::CloseClient()
{
	if (isopenclient == false)
	{
		return;
	}
	TcpClient->close();
	isopenclient = false;
}
//接收服务器发来的信息，槽函数
void QtcpClass::slot_RevTcpSocketMsg(QString clientName, const QString msg)
{
	emit sendtext(clientName,msg);
}