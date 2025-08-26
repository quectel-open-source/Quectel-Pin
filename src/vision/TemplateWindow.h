#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include "ui_TemplateWindow.h"
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
#include "TemplateDetect.h"
#include "DataIO.h"

class TemplateWindow : public QMainWindow
{
	Q_OBJECT

public:
	TemplateWindow(QWidget *parent = nullptr);
	TemplateWindow(HObject image, int _processID, int modubleID, std::string _modubleName);
	~TemplateWindow();
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
	//ģ��ƥ������ṹ�������ֵ����
	void SetRunParams_Template(RunParamsStruct_Template &runParams);
	//��ʼ��ģ����ʾ���
	void IniTableData_Template();
	//����ģ����ʾ�������
	void SetTableData_Template(const ResultParamsStruct_Template &resultStruct, qint64 Ct);
	//����ģ��ѵ�����������в�������
	void UpdateTemplateDataWindow(const RunParamsStruct_Template &RunParams);
	//����	roi����
	void DrawRoiFunc(HObject &OutRegion);
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
	//��ʾģ��ͼ
	void DisplyModelImg(const HObject &image);
	//***************************************************************************
	//ģ��ƥ���òۺ���
	//Roi������״ѡ��
	void on_cmb_RoiShape_Template_activated(const QString &arg1);
	//����ģ��ѵ������
	void on_btn_DrawTrainRoi_Template_clicked();
	//����ģ����������
	void on_btn_DrawSearchRoi_Template_clicked();
	//������ģ����
	void on_btn_DrawShaddowRoi_Template_clicked();
	//�ϲ�ģ������Roi
	void on_btn_UnionRoi_Template_clicked();
	//���ģ������Roi
	void on_btn_ClearRoi_Template_clicked();
	//ѵ��ģ��
	void on_btn_Train_Template_clicked();
	//����ģ��
	void on_pb_Run_clicked();
	//ģ���������
	void on_pb_SaveData_clicked();
	//ģ�������ȡ
	void on_pb_ReadData_clicked();
	//ѡ�񱣴�ģ�͵�·��
	void on_pb_SelectModelPath_clicked();
	//����ģ�͵�ָ��·��
	void on_pb_SaveModelFile_clicked();
	//�Ƿ����ø߼�����
	void on_ckb_AdvancedData_stateChanged(int arg1);
	//ģ��ƥ�������ÿ��Cell����¼�
	void on_tablewidget_Result_Template_cellClicked(int row, int column);
	//ģ��ƥ��������ͷ�ĵ���¼��ۺ���
	void slot_VerticalHeader_TemplateData(int);
	//*****************************************************************************

	//ѵ��ģ��ۺ���
	int slot_CreateTemplate(RunParamsStruct_Template &runParams);
public slots:
	//����ģ��ۺ���
	int slot_FindTemplate();
private:
	Ui::TemplateWindowClass ui;
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
	//ģ��Сͼ
	HObject ho_CropModelImg, m_hResizedImg_Crop;;
	HTuple hv_WindowHandle_ModelImg;
	HTuple m_scaledHeight_Crop, m_scaledWidth_Crop;
	HTuple m_hvScaledRate_Crop;
	HTuple m_imgWidth_Crop, m_imgHeight_Crop;
	//*****************************************************
	HObject RetRegionShow;
	HObject RetContourShow;
	int RetCountShow = 0;
	//ģ��ƥ�����***************************************************************



	QVector<HObject> ho_TemplateRegions;   //ģ��ѵ�����򼯺�
	QVector<HObject> ho_SearchRegions;   //ģ���������򼯺�
	QVector<HObject> ho_ShadowRegions;   //��Ĥ���򼯺�

	HObject TrainRoi_Template;     //ģ��ѵ������
	HObject SearchRoi_Template;     //ģ����������
	HObject ShadowRoi_Template;        //��ģ����
	QString RoiShape = "rectangle1";
	//**************************************************************************
public:
	int processId;	//����ID
	string nodeName;	//ģ������
	int processModbuleID;	//ģ��ID
	string ConfigPath;	//�����ļ���·��
	string XmlPath;	//XML·��
	TemplateDetect* TemplateDetect1;     //ģ����ָ�����1

	RunParamsStruct_Template hv_RunParamsTemplate; //���в���
};
