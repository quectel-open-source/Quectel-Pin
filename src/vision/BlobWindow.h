#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include "ui_BlobWindow.h"
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
#include "BlobDetect.h"
#include "DataIO.h"

class BlobWindow : public QMainWindow
{
	Q_OBJECT

public:
	BlobWindow(QWidget *parent = nullptr);
	BlobWindow(HObject image, int _processID, int modubleID, std::string _modubleName);
	~BlobWindow();
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
		//��ʼ��Blob������
	void IniTableData_Blob();
	//��ʼ��Blob���ڿؼ�
	void IniBlobTools();
	//����Blob������ֵ����
	void SetRunParams_Blob(RunParamsStruct_Blob &runParams);
	//���в������µ�����
	void UpdateBlobDataWindow(const RunParamsStruct_Blob &runParams);
	//Exe·��
	string FolderPath();
	//����һ������strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
signals:
	//ˢ�½������ź�
	void sig_BlobResults(ResultParamsStruct_Blob ResultParams_Blob, qint64 Ct);
private slots:
	//ˢ�½�����ۺ���
	void slot_BlobResults(ResultParamsStruct_Blob ResultParams_Blob, qint64 Ct);
	//��ͼƬ
	void on_pBtn_ReadImage_clicked();
	//ͼ������Ӧ
	void on_pBtn_FitImage_clicked();
	//��մ���
	void on_btn_ClearWindow_clicked();
	//�Ƿ���ʾͼ�θ�ѡ��
	void on_ckb_ShowObj_stateChanged(int arg1);

	//*************************************************************************
		//Blob
	//Blob�����̬ѧ��������
	void on_pb_AddOperatorType_Morphology_Blob_clicked();

	//Blob�����̬ѧ��������
	void on_pb_ClearOperatorType_Morphology_Blob_clicked();
	//����	roi����
	void DrawRoiFunc(int RoiShape, HObject &OutRegion);
	//����ROI
	void on_pb_DrawRoi_Blob_clicked();
	//���ROI
	void on_pb_ClearRoi_Blob_clicked();
	//����Blob����
	void on_pb_Run_clicked();
	//���ɸѡ����
	void on_pb_AddFeature_Blob_clicked();

	//���ɸѡ����
	void on_pb_ClearFeature_Blob_clicked();
	//����Blob����
	void on_pb_SaveData_clicked();
	//��ȡBlob����
	void on_pb_LoadData_clicked();
	//Blob��ֵ������ѡ���¼�
	void on_cmb_Type_Binarization_Blob_currentTextChanged(const QString &arg1);
	//Blob��̬ѧ�����������ѡ���¼�
	void on_cmb_OperatorType_Morphology_Blob_activated(const QString &arg1);
	//Blob������ÿ��Cell����¼�
	void on_tablewidget_Results_Blob_cellClicked(int row, int column);
	//Blob�������ͷ�ĵ���¼��ۺ���
	void slot_VerticalHeader_Blob(int);
public slots:
	//Blob���ۺ���
	int slot_BlobDetect();

private:
	Ui::BlobWindowClass ui;
	DataIO dataIOC;//������ȡ����
	QAction * Action1_Morphology;
	QMenu * Menu_Morphology;
	QAction * Action1_Filter;
	QMenu * Menu_Filter;
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
	//****************************************************************************

	HObject SearchRoi_Blob;     //ͼ���ֵ����������
	RunParamsStruct_Blob hv_RunParamsBlob;//Blobͼ���ֵ�����в���
	
public:
	int processId;	//����ID
	string nodeName;	//ģ������
	int processModbuleID;	//ģ��ID
	string ConfigPath;	//�����ļ���·��
	string XmlPath;	//XML·��
			//Blob����
	BlobDetect* BlobDetect1;
};
