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
	// ȡ��Ӧ�ó��򴰿�
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
	//ȷ����ǰ���Ϊ2D���
	void on_pushButton_sure2D_clicked();
	//ȷ����ǰ���Ϊ�����
	void on_pushButton_sureED_Up_clicked();
	//ȷ����ǰ���Ϊ�����
	void on_pushButton_sureED_Down_clicked();
	//ȷ�ϵ�ǰ���Ϊ��λ���
	void on_pushButton_sureED_Location_clicked();
	//ȷ����ǰ���Ϊ�����
	void on_pushButton_sideCamera_clicked();

private:
	Ui::qg_2dCameraSettingClass ui;

	HTuple m_hLabelID;            //QLabel�ؼ����
	MV_CC_DEVICE_INFO_LIST  m_stDevList;
	BOOL                    m_bOpenDevice;                        // ch:�Ƿ���豸 | en:Whether to open device
	BOOL                    m_bStartGrabbing;                     // ch:�Ƿ�ʼץͼ | en:Whether to start grabbing

	HTuple m_hHalconID = NULL;            //Halcon��ʾ���ھ��
	// ����ָ��
	static qg_2dCameraSetting* cameraWindow;
	//��ǰ�������
	string m_cameraName = "";
public:
	Thread* m_hGrabObject{ nullptr };
	QThread* m_hGrabThread{ nullptr };
	//�����
	std::unique_ptr<CMvCamera> m_pcMyCamera = nullptr;
	std::unique_ptr<Camera3D> m_pcMyCamera_3D = nullptr;
};
