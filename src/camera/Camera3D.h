#pragma once
#pragma execution_character_set("utf-8")

#include <QSettings>
#include <QVector>
#include <QDebug>
#include <qdatetime.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <qmessagebox.h>
#include "Thread.h"
#include <QObject>

#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <qstring.h>
#include "lvm_sdk.h"
class Camera3D :public QObject
{
	Q_OBJECT
public:
	Camera3D(bool isAction);
	~Camera3D();

	int AllocBuf(int g_grab_lines);
	int run();
	int InitCamera3DLink();
	void DeinitCamera3DLink();
	void StopSubThread(bool stopThread);
	//查找可保存文件路径
	QString findOrCreateFile();
	QImage matToQImage(const cv::Mat& mat);
public:
	lvm_dev_t* dev = NULL;
	lvm_buf_t* buf = NULL;
	QString m_savePath = "1.pcd";
	bool m_isAction = false;
	cv::Mat showImg;
	pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud;
	bool StartFlag = false;
private:
	Thread* m_hGrabObject{ nullptr };
	QThread* m_hGrabThread{ nullptr };
	bool InitCamera3DLinkEnd = false;
	bool RunProgramFlag = false;
signals:
	void signalFlushed_3D();
};

