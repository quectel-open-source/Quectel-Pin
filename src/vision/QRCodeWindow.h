#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include "ui_QRCodeWindow.h"
#include <QSettings>
#include <QVector>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QtConcurrent>
#include <qdatetime.h>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include "QRCodeDetect.h"
#include "DataIO.h"

class QRCodeWindow : public QMainWindow
{
	Q_OBJECT

public:
	QRCodeWindow(QWidget *parent = nullptr);
	QRCodeWindow(HObject image, int _processID, int modubleID, std::string _modubleName);
	~QRCodeWindow();
	QPoint whereismouse;
	BOOL keypoint = 0;
	void getmessage(QString value);
private slots:
	void on_toolButton_clicked();
	void on_toolButton_2_clicked();
	void on_toolButton_3_clicked();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
protected:
	virtual void resizeEvent(QResizeEvent* event);
private:
	//��ʼ����ʾ����
	void InitWindow();
	//��ʾͼƬ(hv_FillType ����ѡ��margin����fill)
	void showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color);
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	void GiveParameterToWindow();
	//**********************************************************************************************
	//��ʼ����ά������ʾ���
	void IniTableData_Code2d();
	//���в��Ҷ�ά��ṹ�������ֵ����
	void SetRunParams_Code2d(RunParamsStruct_Code2d &runParams);
	//���ö�ά������ʾ�������
	void SetTableData_Code2d(const ResultParamsStruct_Code2d &resultStruct, qint64 Ct);
	//���¶�ά�������в���������
	void UpdateCode2dDataWindow(const RunParamsStruct_Code2d &runParams);
	//Exe·��
	string FolderPath();
	//����һ������strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
private slots:
	//��ͼƬ
	void on_pBtn_ReadImage_clicked();
	//ͼ������Ӧ
	void on_pBtn_FitImage_clicked();
	//��մ���
	void on_btn_ClearWindow_clicked();
	//�Ƿ���ʾͼ�θ�ѡ��
	void on_ckb_ShowObj_stateChanged(int arg1);
	//***************************************************************************
	//��ά����ۺ���
	void on_pb_Run_clicked();
	//���ƶ�ά����������
	void on_btn_DrawRoi_Code2d_clicked();
	//�����ά�������
	void on_pb_SaveData_clicked();
	//��ȡ��ά�������
	void on_pb_LoadData_clicked();
	//���ROI
	void on_btn_ClearRoi_Code2d_clicked();
	//**************************************************************************

	//������ÿ��Cell����¼�
	void on_tablewidget_Results_Code2d_cellClicked(int row, int column);
	//�������ͷ�ĵ���¼��ۺ���
	void slot_VerticalHeader_Code2d(int);
public slots:
	//����ά��ۺ���
	int slot_FindCode2d();
private:
	Ui::QRCodeWindowClass ui;
	DataIO dataIOC;//������ȡ����

//ˢͼ����*****************************************************************
//��ʾͼ��Ŀؼ�id
	HTuple m_hLabelID;            //QLabel�ؼ����
	HTuple m_hHalconID;            //Halcon��ʾ���ھ��
	//ԭʼͼ��ĳߴ�
	HTuple m_imgWidth, m_imgHeight;
	//ͼƬ·���б�
	HTuple m_imgFiles;

	//��Ҫˢ�µ����ڵ�ͼ��
	HObject ho_ShowObj;
	//�Ƿ���ʾͼ��
	bool isShowObj = true;
	//��ǰͼ��
	HObject ho_Image;
	//���ź��ͼ��
	HObject m_hResizedImg;
	//����ϵ��
	HTuple m_hvScaledRate;
	//���ź�ͼ��Ĵ�С
	HTuple m_scaledHeight, m_scaledWidth;
	//*****************************************************
		//��ά�����������

	QVector<HObject> ho_Code2dRegions;   //��ά���������򼯺�
	HObject SearchRoi_Code2d;     //��ά����������
	RunParamsStruct_Code2d hv_RunParamsCode2d;  //��ά�����в���
	HObject RetRegionShow;
	int RetCountShow = 0;
	//****************************************************************************
public:
	int processId;	//����ID
	string nodeName;	//ģ������
	int processModbuleID;	//ģ��ID
	string ConfigPath;	//�����ļ���·��
	string XmlPath;	//XML·��
	QRCodeDetect* code2dParams1;         //��ά������ָ�����1
};
