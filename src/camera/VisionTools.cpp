#include "VisionTools.h"
#include "qc_log.h"
#include <QtCore/qdir.h>
#include <QtCore/qcoreapplication.h>
#include "rs_motion.h"
#pragma execution_character_set("utf-8")

VisionTools::VisionTools()
{
}

VisionTools::~VisionTools()
{
	DeinitCamera3DLink();
}


int VisionTools::StartProgram()
{
	machineLog->write("- - - ->>>>>>>>开始执行采集......", Normal);
	StartFlag = true;
	stopSubThreadFlag = false;
	std::function<void()> run = std::bind(&VisionTools::run, this);
	std::thread t1(run);
	t1.detach();
	return 0;
}

int VisionTools::StopProgram()
{
	StopSubThread(true);
	machineLog->write("- - - ->>>>>>>>停止执行", Normal);
	return 0;
}

int VisionTools::run()
{
	//AllocBuf(LSM->m_Paramer.ScanRow);
	RunProgram();

	//for (;;)
	//{
	//	if (stopSubThreadFlag)
	//	{
	//		stopSubThreadFlag = false;
	//		break;
	//	}
	//	else
	//	{
	//		RunProgram();
	//	}
	//}
	return 0;
}

void VisionTools::StopSubThread(bool stopThread)
{
	stopSubThreadFlag = stopThread;
	while (InitCamera3DLinkEnd == false)
	{
		if (StartFlag == false)
		{
			return;
		}
	}
	//改为只停线程不关设备
	//DeinitCamera3DLink();
	StartFlag = false;
}

int VisionTools::InitCamera3DLink()
{
	machineLog->write("- - - ->>>>>>>>开始执行，初始化中......", Normal);
	stopSubThreadFlag = false;
	InitCamera3DLinkEnd = false;
	m_initSucess = false;

	char connect_ip[16] = "192.168.10.13";
	int connect_dev_type = 3;

	int ret = lvm_init_sdk(NULL, "./LOG");
	if (ret != 0)
	{
		qDebug() << "sdk init failed!";
		InitCamera3DLinkEnd = true;
		return 0;
	}
	if (dev)
	{
		//防止之前有未清空的内存
		lvm_destroy_dev(dev);
		dev = NULL;
	}
	dev = lvm_create_dev(connect_ip, connect_dev_type);
	if (!dev)
	{
		//qDebug() << "dev create failed!";
		machineLog->write("dev create failed!", Normal);
		machineLog->write("3D相机初始化开机失败", Err);
		InitCamera3DLinkEnd = true;
		return 0;
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
		machineLog->write("dev connect failed! ret = " + QString::number(ret), Normal);
		machineLog->write("3D相机初始化连接失败", Err);
		InitCamera3DLinkEnd = true;
		return 0;
	}

	else if (DEV_PARAM_DO_NOT_MATCH == ret)
	{
		qDebug() << "firmware version is not correct!";
		InitCamera3DLinkEnd = true;
		return 0;
	}

	InitCamera3DLinkEnd = true;
	m_initSucess = true;
	machineLog->write("- - - ->>>>>>>>相机IP连接成功，初始化完成。", Normal);
	return 0;
}

void VisionTools::DeinitCamera3DLink()
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
	dev = NULL;
	lvm_deinit_sdk();
	machineLog->write("- - - ->>>>>>>>相机取图句柄释放完成，相机关闭连接成功。", Normal);
}


void VisionTools::RunProgram()
{
	if (dev == NULL || dev->error_code == 40035 || dev->error_code == 40006 || dev->error_code == 40002)
	{
		machineLog->write("- - - ->>>>>>>>相机连接失败，请检查相机状态和IP设置。", Normal);
		machineLog->write("3D相机连接失败，请检查相机状态和IP设置", Err);
		stopSubThreadFlag = true;
	}
	else
	{
		if (stopSubThreadFlag)
		{
			machineLog->write("- - - ->>>>>>>>stopSubThreadFlag true。", Normal);
			return;
		}
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_ori(new pcl::PointCloud<pcl::PointXYZ>);
		cv::Mat depth_img;
		LVMGrabCloud(cloud_ori, depth_img, dev); 
		machineLog->write("- - - ->>>>>>>>点云数量 = " + QString::number(cloud_ori->size()), Normal);
		m_cloud_ori = cloud_ori;
		if (!cloud_ori->empty())
		{
			m_cloud_ori_list.emplace_back(cloud_ori);
		}
		if (depth_img.empty())
		{
			machineLog->write("- - - ->>>>>>>>深度图读取失败", Normal);
			machineLog->write("3D深度图读取失败", Err);
			return;
		}
		else
		{
			if (!depth_color_img.empty())
			{
				depth_color_img.release();
			}
			depth_color_img = depth_img.clone();
		}

	}
}

int VisionTools::AllocBuf(int g_grab_lines)
{
	machineLog->write("3D扫描行数 ：" + QString::number(g_grab_lines), Normal);
	if (buf)
	{
		//先清除之前的内存
		lvm_free_buf(buf);
		buf = NULL;
	}
	buf = lvm_alloc_depth_map_buf(dev, 1, 0, g_grab_lines, 2);
	if (buf == NULL)
	{
		machineLog->write("3D扫描行数设置缓存失败", Normal);
		machineLog->write("3D扫描行数设置缓存失败", Err);
		return 1;
	}
	int ret1 = lvm_bind_buf(dev, buf);
	if (ret1 != 0 && ret1 != 40019)
	{
		machineLog->write("3D扫描行数设置失败", Normal);
		machineLog->write("3D扫描行数设置失败", Err);
		return 1;
	}
}


int VisionTools::LVMGrabCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr GrabCloud,
	cv::Mat &depth_img, lvm_dev_t *dev)
{
	//QDateTime startTime = QDateTime::currentDateTime();
	if (stopSubThreadFlag)
	{
		return 0;
	}
	int timeout = 20000;
	//int timeout = 2000;

	//QDateTime endTime = QDateTime::currentDateTime();
	//qint64 intervalTimeMS = startTime.msecsTo(endTime);
	//qDebug()<<"相机申请内存时间:" + QString::number(intervalTimeMS) + "ms";

	while (true)
	{
		if (stopSubThreadFlag || LSM->m_isEmergency)
		{
			break;
		}
		lvm_point_cloud_t pc;
		//QDateTime startTime = QDateTime::currentDateTime();
		machineLog->write("1111111111111111", Normal);
		lvm_depth_map_t* depth_map = (lvm_depth_map_t*)lvm_grab_frame(dev, timeout);
		machineLog->write("22222222222222", Normal);
		if (depth_map)
		{
			machineLog->write("depth_map --------------", Normal);
			depth_img = cv::Mat(depth_map->head.height, depth_map->head.width, CV_16UC1, depth_map->data);
			cv::Mat src_gray_f;
			depth_img.convertTo(src_gray_f, CV_32F);
			cv::Mat img = cv::Mat::zeros(depth_img.size(), CV_32FC1);
			cv::normalize(src_gray_f, img, 1.0, 0, cv::NORM_MINMAX);
			cv::Mat result = img * 255;
			result.convertTo(img, CV_8UC1);
			cv::applyColorMap(img, depth_img, 2);
			//emit signalFlushed_3D();

			pc.p = (lvm_point_t*)malloc(depth_map->head.width * depth_map->head.height * sizeof(lvm_point_t));
			if (CORRECT == lvm_convert_depth_map_to_pcld(depth_map, &pc, depth_map->head.width * depth_map->head.height))
			{
				pc.head.width = depth_map->head.width;
				pc.head.height = depth_map->head.height;
				if (m_isSaveCloud)
				{
					QString exePath = QCoreApplication::applicationDirPath();
					QDir photoDir(exePath + "/3D_Photo");

					if (!photoDir.exists()) {
						if (!photoDir.mkpath(".")) {
							continue;
						}
					}
					auto filePath = photoDir.filePath(m_cloudName + ".pcd").toUtf8().data();
					if (CORRECT == lvm_save_pcld(dev, filePath, &pc))
					{

						printf("保存图像成功");
					}
					//if (CORRECT == lvm_save_pcld(dev, "pcld.pcd", &pc))
					//{

					//	printf("保存图像成功");
					//}
				}
				
				pcl::PointCloud<pcl::PointXYZ>::Ptr Cloud(new pcl::PointCloud<pcl::PointXYZ>);

				Cloud->width = pc.head.width* pc.head.height;
				Cloud->height = 1;
				Cloud->points.resize(pc.head.width * pc.head.height);
				for (int i = 0; i < Cloud->points.size(); i++)
				{
					Cloud->points[i].x = pc.p[i].x;
					Cloud->points[i].y = pc.p[i].y;
					Cloud->points[i].z = pc.p[i].z;
				}

				for (int i = 0; i < Cloud->points.size(); i++)
				{
					pcl::PointXYZ P0;
					if (!(Cloud->points[i].x == 0 && Cloud->points[i].y == 0 && Cloud->points[i].z == 0))
					{
						P0.x = Cloud->points[i].x;
						P0.y = Cloud->points[i].y;
						P0.z = Cloud->points[i].z;
						GrabCloud->points.push_back(P0);
					}
				}
				GrabCloud->width = GrabCloud->points.size();
				GrabCloud->height = 1;
				GrabCloud->is_dense = false;
				free(pc.p);



				//GrabCloud->width = pc.head.width * pc.head.height;
				//GrabCloud->height = 1;
				//GrabCloud->points.resize(pc.head.width * pc.head.height);
				//for (int i = 0; i < GrabCloud->points.size(); i++)
				//{
				//	GrabCloud->points[i].x = pc.p[i].x;
				//	GrabCloud->points[i].y = pc.p[i].y;
				//	GrabCloud->points[i].z = pc.p[i].z;
				//}
				//free(pc.p);


				//QDateTime endTime = QDateTime::currentDateTime();
				//qint64 intervalTimeMS = startTime.msecsTo(endTime);
				//qDebug()<<"相机采图运行时间:" + QString::number(intervalTimeMS) + "ms";
				break;
			}
			else
			{
				//qDebug() << "convert to depth map fail!";
				machineLog->write("convert to depth map fail!", Normal);
				free(pc.p);
				break;
			}
		}
		else
		{
			//qDebug() << "grab_frame timeout!";
			machineLog->write("grab_frame timeout!", Normal);
			break;
		}
	}

	return 0;
}

//设置曝光时间
int VisionTools::setExpsuretime(int val)
{
	if(dev == NULL)
		return -1;
	if(val <= 0)
		return -1;
	dev->config_param->expsure_time = val;
}

