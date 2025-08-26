#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include<vtkGenericOpenGLRenderWindow.h>	
#include<QVTKOpenGLNativeWidget.h>

#include <QSettings>
#include <QVector>
#include <QDebug>
#include <qdatetime.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <qmessagebox.h>
#include <thread>

#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <qstring.h>
#include "lvm_sdk.h"


class VisionTools : public QObject
{
	Q_OBJECT

public:
	VisionTools();
	~VisionTools();

	lvm_dev_t *dev = NULL;
	lvm_buf_t* buf = NULL;

	int AllocBuf(int g_grab_lines);
	int LVMGrabCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr GrabCloud,
		cv::Mat &depth_img, lvm_dev_t *dev);

	int run();

	void RunProgram();
	int InitCamera3DLink();
	void DeinitCamera3DLink();
	void StopSubThread(bool stopThread);
	int StartProgram();
	int StopProgram();
	bool getStartFlag()
	{
		return StartFlag;
	};
	void setStartFlag(bool val)
	{
		StartFlag = val;
	};
	int setExpsuretime(int val);
	std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> m_cloud_ori_list;
	pcl::PointCloud<pcl::PointXYZ>::Ptr m_cloud_ori = nullptr; //单个点云
	cv::Mat depth_color_img;
	//初始化成功
	bool m_initSucess = false;
	//保存点云
	bool m_isSaveCloud = false;
	//点云名
	QString m_cloudName;

//private slots:
//	int on_pB_StartProgram_clicked();
//	int on_pB_StopProgram_clicked();

private:
	bool stopSubThreadFlag = false;
	bool InitCamera3DLinkEnd = false;
	bool StartFlag = false;
	bool RunProgramFlag = false;
signals:
	void signalFlushed_3D();
};