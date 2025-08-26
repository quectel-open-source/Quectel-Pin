#include "QtcpClass.h"

///////////////////////QtTcpSocket��//////////////////////////
QtTcpSocket::QtTcpSocket(int socket, QObject *parent)
	: QTcpSocket(parent)
{
	// �����׽���������
	socketDescriptor = socket;
	// ���׽�����Ϣ���ܺͶϿ��źŲ�
	connect(this, SIGNAL(readyRead()), this, SLOT(recvData()));
	connect(this, SIGNAL(disconnected()), this, SLOT(disconnectToHost()));
}

QtTcpSocket::~QtTcpSocket()
{

}

// ��ȡ�׽���������
int QtTcpSocket::getSocketDescriptor()
{
	return socketDescriptor;
}

// ������Ϣ���ͻ���
bool QtTcpSocket::sendMsg(QByteArray msg)
{
	this->write(msg);
	bool ret = this->flush();

	if (!ret)
	{
		qDebug() << "������Ϣʧ��:" + QString::fromLocal8Bit(msg);
	}
	return ret;
}

// �����׽�����Ϣ
void QtTcpSocket::recvData()
{
	// ��ȡ�ÿͻ���������Ϣ(���ܴ�����������ע���ж�)
	QByteArray buffer = this->readAll();
	// ת����QString
	QString msg = QString::fromLocal8Bit(buffer);
	// �����ѽ�����Ϣ��������
	emit sig_RevSocketMsg(msg, socketDescriptor);
}

// ������Ϣ���ͻ���
void QtTcpSocket::sendMsg(QByteArray msg, int id)
{
	if (id == socketDescriptor)
	{
		this->write(msg);
	}
}

// �ͻ��˶Ͽ�����
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
	// �����߳�
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
	//ȡ�����е�����  
	this->abort();
}

bool QtTcpClient::connectToServer(QString clientName, QString ip, int port)
{
	_ip = ip;
	_port = port;
	strClientName = clientName;
	////ȡ�����е�����  
	//this->abort();
	//���ӷ�����  
	this->connectToHost(QHostAddress(ip), port);
	//�ȴ����ӳɹ�  
	if (!this->waitForConnected(100))
	{
		isConnected = false;
		timer.start(3000);
	}
	else
	{
		isConnected = true;
		//this->sendMsg(QString("������"));//���ӳɹ�����CCD��PLC
		//emit sig_ConnectState(strClientName, true);//True�������ӳɹ�

	}
	return isConnected;
}

// ֹͣͨ��
void  QtTcpClient::IsStop(bool isstop)
{
	//qDebug() << "MES�� " << isstop;
	isStoped = isstop;
	if (!isStoped)
	{
		if (!isConnected)
		{
			connectToServer(strClientName, _ip, _port);
		}
	}
}
//�Ͽ�����
void QtTcpClient::slot_Disconnect()
{
	//ȡ�����е�����  
	this->abort();
	emit sig_RunInfo("�ͻ��˶Ͽ�����", 1);
}

void QtTcpClient::sendMsg(QString msg)
{
	this->write(msg.toLocal8Bit());
	qDebug() << "������Ϣ:" + msg;
	if (!flush())
	{
		emit sig_RunInfo(strClientName + ": ������Ϣʧ�� ", 1);
	}
}

void QtTcpClient::recvData()
{
	QByteArray buffer = this->readAll();
	// ת����QString
	QString msg = QString::fromLocal8Bit(buffer);
	// �����ѽ�����Ϣ����λ��
	emit sig_RevMsg(strClientName, msg);

}

void QtTcpClient::slot_sendMsg(QString msg)
{
	this->write(msg.toLocal8Bit());
	if (!flush())
	{
		emit sig_RunInfo(strClientName + ": ������Ϣʧ�� >> " + msg, 1);
	}
}

void QtTcpClient::disconnectFormServer(QAbstractSocket::SocketError)
{
	if (this->errorString().contains("close"))
	{
		isConnected = false;
		timer.start(3000);
		emit sig_RunInfo(strClientName + ": �������Ͽ�����", 1);
		emit sig_ConnectState(strClientName, false);
	}
}

// ��ʱ���ۺ������Ͽ��Զ�����
void QtTcpClient::slot_Timeout()
{
	//qDebug() << strClientName + "�Զ����ӷ�������....";
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


// TCP�ͻ�������(��TCP�ͻ��������Զ�����)
void QtcpClass::incomingConnection(qintptr socketDescriptor)
{
	// �����µ�TCP�׽��ֶ���
	QtTcpSocket *tcpSocket = new QtTcpSocket(socketDescriptor, 0);

	//����TCP�׽���������
	if (!tcpSocket->setSocketDescriptor(socketDescriptor))
	{
		qDebug() << "set socket descriptor error!";
		return;
	}
	// ��TCP�׽��ֺͷ��������źŲ�
	connect(tcpSocket, SIGNAL(sig_RevSocketMsg(QString, int)), this, SLOT(slot_RevSocketMsg(QString, int)));
	connect(tcpSocket, SIGNAL(disconnectHost(int)), this, SLOT(slot_SocketDisconnection(int)));
	// ��ӵ��׽�������
	tcpSocketVec.append(tcpSocket);
	//���ͽ���Ŀͻ��˾�������õ���
	emit sig_SendHandle(socketDescriptor);

	qDebug() << "�¿ͻ�������: " << socketDescriptor << tcpSocketVec.count();
}
// ���������ܿͻ�����Ϣ
bool QtcpClass::sendMsg(QByteArray msg, int socketDescriptor)
{
	// ���socketDescriptor = -1��Ⱥ�������������ӵĿͻ���
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
			// ������Ϣ��ָ���ͻ���
			if (socketDescriptor == tcpSocketVec[i]->getSocketDescriptor())
			{
				return tcpSocketVec[i]->sendMsg(msg);
			}
		}
	}
	return false;
}

// �ͻ��˴ӷ������Ͽ�
void QtcpClass::slot_SocketDisconnection(int socketDescriptor)
{
	for (int i = 0; i < tcpSocketVec.count(); i++)
	{
		// ����ɾ�����߿ͻ���
		if (socketDescriptor == tcpSocketVec[i]->getSocketDescriptor())
		{
			tcpSocketVec.remove(i);
			break;
		}
	}
	// ���Ϳͻ��˵����źŵ��ⲿ
	emit sig_DisConnect(socketDescriptor);
}
// ���ܿͻ�����Ϣ
void QtcpClass::slot_RevSocketMsg(QString msg, int socketNum)
{
	// �����ѽ��ܵ���Ϣ���ⲿ
	emit sig_RevMsg(msg, socketNum);
}
// ������Ϣ���ͻ���
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
		tcpSocketVec[i]->deleteLater();///>���������ٶ���������Ϣѭ��������һ��event����һ������Ϣѭ���յ����event֮��Ż����ٶ���
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
//���շ�������������Ϣ���ۺ���
void QtcpClass::slot_RevTcpSocketMsg(QString clientName, const QString msg)
{
	emit sendtext(clientName,msg);
}