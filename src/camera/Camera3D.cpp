#include "Camera3D.h"
#include "qc_log.h"
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>

Camera3D::Camera3D(bool isAction)
	:m_isAction(isAction)
{
	//开启采集线程
	if (m_hGrabObject == nullptr)
	{
		m_hGrabObject = new Thread();
	}
	if (m_hGrabThread == nullptr)
	{
		m_hGrabThread = new QThread();
		m_hGrabObject->moveToThread(m_hGrabThread);
	}
};

Camera3D::~Camera3D()
{
	if (m_hGrabObject)
	{
		m_hGrabObject->bExit = true;
		m_hGrabObject->deleteLater();
		m_hGrabObject = nullptr;
	}

	if (m_hGrabThread)
	{
		m_hGrabThread->quit();
		m_hGrabThread->wait();
		m_hGrabThread->deleteLater();
		m_hGrabThread = nullptr;
	}
	DeinitCamera3DLink();
};

int Camera3D::run()
{
	InitCamera3DLinkEnd = false;

	int rtn = InitCamera3DLink();
	if (rtn != 0)
		return rtn;


	rtn = AllocBuf(2800);
	if (rtn != 0)
		return rtn;

	if (dev == NULL || dev->error_code == 40035 || dev->error_code == 40006 || dev->error_code == 40002)
	{
		machineLog->write("- - - ->>>>>>>>相机连接失败，请检查相机状态和IP设置。",Normal);
		m_hGrabObject->bExit = true;
	}
	else
	{
		if (!m_hGrabThread->isRunning())
		{
			//更新保存路径
			m_savePath = findOrCreateFile();
			m_hGrabObject->bExit = false;
			m_hGrabThread->start();
			QMetaObject::invokeMethod(m_hGrabObject, &Thread::cameraGrabbing_3D, Qt::QueuedConnection);
		}
	}

	return 0;
}

//3D相机初始化
int Camera3D::InitCamera3DLink()
{
	char connect_ip[16] = "192.168.10.13";
	int connect_dev_type = 3;

	int ret = lvm_init_sdk(NULL, "./LOG");
	if (ret != 0)
	{
		qDebug() << "3D camera sdk init failed!";
		//InitCamera3DLinkEnd = true;
		return 0;
	}
	dev = lvm_create_dev(connect_ip, connect_dev_type);
	if (!dev)
	{
		qDebug() << "dev create failed!";
		//InitCamera3DLinkEnd = true;
		return -1;
	}
	for (int i = 0; i < 10; i++)
	{
		ret = lvm_connect_dev(dev);
		if (ret == 0)
		{
			break;
		}
		Sleep(1);
	}
	if (ret != 0)
	{
		qDebug() << "dev connect failed!";
		//InitCamera3DLinkEnd = true;
		return -1;
	}

	else if (DEV_PARAM_DO_NOT_MATCH == ret)
	{
		qDebug() << "firmware version is not correct!";
		//InitCamera3DLinkEnd = true;
		return -1;
	}

	InitCamera3DLinkEnd = true;
	machineLog->write("- - - ->>>>>>>>相机IP连接成功，初始化完成", Normal);
	return 0;
}

//查找可保存文件路径
QString Camera3D::findOrCreateFile()
{
	QString exePath = QCoreApplication::applicationDirPath();
	QDir photoDir(exePath + "/3D_Photo");

	if (!photoDir.exists()) {
		if (!photoDir.mkpath(".")) {
			qDebug() << "无法创建 3D_Photo 文件夹";
			return QString();
		}
	}

	// 从 1.pcd 开始查找
	int fileIndex = 1;
	QString fileName;
	do {
		fileName = photoDir.filePath(QString::number(fileIndex) + ".pcd");
		fileIndex++;
	} while (QFile::exists(fileName));  // 如果文件已存在，则继续增加索引

	// 返回最终的文件路径
	return fileName;
}


int Camera3D::AllocBuf(int g_grab_lines)
{
	buf = lvm_alloc_depth_map_buf(dev, 1, 0, g_grab_lines, 2);
	int ret1 = lvm_bind_buf(dev, buf);
	if (ret1 != 0 && ret1 != 40019)
	{
		qDebug() << "capture param set failed!";
		return -1;
	}
	return 0;
}

void Camera3D::StopSubThread(bool stopThread)
{
	if (m_hGrabObject)
	{
		m_hGrabObject->bExit = stopThread;
	}

	if (InitCamera3DLinkEnd == false && StartFlag == false)
	{
		return;
	}
	DeinitCamera3DLink();
}

void Camera3D::DeinitCamera3DLink()
{
	StartFlag = false;
	InitCamera3DLinkEnd = false;

	lvm_free_buf(buf);
	buf = NULL;

	int ret_re;
	ret_re = lvm_disconnect_dev(dev);
	if (ret_re != 0)
	{
		qDebug() << "断联失败";
		return;
	}
	lvm_destroy_dev(dev);
	dev = nullptr;
	lvm_deinit_sdk();
	machineLog->write("- - - ->>>>>>>>相机取图句柄释放完成，相机关闭连接成功", Normal);
}

// 将cv::Mat 转换为 QImage
QImage Camera3D::matToQImage(const cv::Mat& mat)
{
	if (mat.type() == CV_8UC1) {
		// 灰度图
		return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
	}
	else if (mat.type() == CV_8UC3) {
		// BGR 彩色图
		cv::Mat rgbMat;
		cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
		return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
	}
	return QImage();
}