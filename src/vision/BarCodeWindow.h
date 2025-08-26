#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include "ui_BarCodeWindow.h"
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
#include "BarCodeDetect.h"
#include "DataIO.h"

class BarCodeWindow : public QMainWindow
{
	Q_OBJECT

public:
	BarCodeWindow(QWidget *parent = nullptr);
	BarCodeWindow(HObject image, int _processID, int modubleID, std::string _modubleName);
	~BarCodeWindow();
	QPoint whereismouse;
	BOOL keypoint = 0;
	void getmessage(QString value);
	ResultParamsStruct_BarCode getResult() {
		return mResultParams;
	};
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
	//���в���һά��ṹ�������ֵ����
	void SetRunParams_BarCode(RunParamsStruct_BarCode &runParams);
	//��ʼ��һά������ʾ���
	void IniTableData_BarCode();
	//����һά������ʾ�������
	void SetTableData_BarCode(const ResultParamsStruct_BarCode &resultStruct, qint64 Ct);
	//����һά�������в���������
	void UpdateBarCodeDataWindow(const RunParamsStruct_BarCode &runParams);
	//Exe·��
	string FolderPath();
	//����һ������strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	//******************************************************************************************************
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
	//һά����ۺ���
	void on_pb_Run_clicked();
	//����һά����������
	void on_btn_DrawRoi_BarCode_clicked();
	//����һά�������
	void on_pb_SaveData_clicked();
	//��ȡһά�������
	void on_pb_LoadData_clicked();
	//���ROI
	void on_btn_ClearRoi_BarCode_clicked();
	//**************************************************************************

	//������ÿ��Cell����¼�
	void on_tablewidget_Results_BarCode_cellClicked(int row, int column);
	//�������ͷ�ĵ���¼��ۺ���
	void slot_VerticalHeader_BarCode(int);
public slots:
	//���һά��ۺ���
	int slot_FindBarCode();
private:
	Ui::BarCodeWindowClass ui;
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
		//�������������

	QVector<HObject> ho_BarCodeRegions;   //һά���������򼯺�
	HObject SearchRoi_BarCode;     //һά����������
	RunParamsStruct_BarCode hv_RunParamsBarCode;  //һά�����в���
	HObject RetRegionShow;
	int RetCountShow = 0;
	//����������erik �¼ӣ�
	ResultParamsStruct_BarCode mResultParams;
	//****************************************************************************
public:
	int processId;	//����ID
	string nodeName;	//ģ������
	int processModbuleID;	//ģ��ID
	string ConfigPath;	//�����ļ���·��
	string XmlPath;	//XML·��
	BarCodeDetect* barCodeParams1;        //һά������ָ�����1
};
