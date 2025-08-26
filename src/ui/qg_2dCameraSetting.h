#pragma once
#pragma execution_character_set("utf-8")

#include <QDialog>
#include "ui_qg_2dCameraSetting.h"
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <MvCamera.h>
#include "Thread.h"
#include "rs_motion.h"

using namespace HalconCpp;

class qg_2dCameraSetting : public QDialog
{
	Q_OBJECT

public:
	qg_2dCameraSetting(QWidget *parent = nullptr);
	~qg_2dCameraSetting();

	int CloseDevice();
	// 取得应用程序窗口
	static qg_2dCameraSetting* getCameraWindow() {
		return cameraWindow;
	}

public slots:
	void on_pushButton_FindDevices_clicked();
	void on_pushButton_OpenDevices_clicked();
	void on_pushButton_CloseDevices_clicked();
	void on_pushButton_StartGrabbing_clicked();
	void slotFlushed();
	void on_pushButton_StopGrabbing_clicked();
	void on_pushButton_ReadParamer_clicked();
	void on_pushButton_WriteParamer_clicked();
	void on_pushButton_StartGrabbing_3D_clicked();
	void on_pushButton_StopGrabbing_3D_clicked();
	void on_pushButton_SaveImg_clicked();
	void slotFlushed_3D();
	//确定当前相机为2D相机
	void on_pushButton_sure2D_clicked();
	//确定当前相机为上相机
	void on_pushButton_sureED_Up_clicked();
	//确定当前相机为下相机
	void on_pushButton_sureED_Down_clicked();
	//确认当前相机为定位相机
	void on_pushButton_sureED_Location_clicked();
	//确定当前相机为侧相机
	void on_pushButton_sideCamera_clicked();

private:
	Ui::qg_2dCameraSettingClass ui;

	HTuple m_hLabelID;            //QLabel控件句柄
	MV_CC_DEVICE_INFO_LIST  m_stDevList;
	BOOL                    m_bOpenDevice;                        // ch:是否打开设备 | en:Whether to open device
	BOOL                    m_bStartGrabbing;                     // ch:是否开始抓图 | en:Whether to start grabbing

	HTuple m_hHalconID = NULL;            //Halcon显示窗口句柄
	// 窗口指针
	static qg_2dCameraSetting* cameraWindow;
	//当前相机名称
	string m_cameraName = "";
public:
	Thread* m_hGrabObject{ nullptr };
	QThread* m_hGrabThread{ nullptr };
	//相机类
	std::unique_ptr<CMvCamera> m_pcMyCamera = nullptr;
	std::unique_ptr<Camera3D> m_pcMyCamera_3D = nullptr;
};
