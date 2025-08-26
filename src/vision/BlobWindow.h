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
	//初始化显示窗口
	void InitWindow();
	//显示图片(hv_FillType 可以选择margin或者fill)
	void showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color);
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	void GiveParameterToWindow();
	//**********************************************************************************************
		//初始化Blob结果表格
	void IniTableData_Blob();
	//初始化Blob窗口控件
	void IniBlobTools();
	//运行Blob参数赋值函数
	void SetRunParams_Blob(RunParamsStruct_Blob &runParams);
	//运行参数更新到窗口
	void UpdateBlobDataWindow(const RunParamsStruct_Blob &runParams);
	//Exe路径
	string FolderPath();
	//返回一个不含strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
signals:
	//刷新结果表格信号
	void sig_BlobResults(ResultParamsStruct_Blob ResultParams_Blob, qint64 Ct);
private slots:
	//刷新结果表格槽函数
	void slot_BlobResults(ResultParamsStruct_Blob ResultParams_Blob, qint64 Ct);
	//打开图片
	void on_pBtn_ReadImage_clicked();
	//图像自适应
	void on_pBtn_FitImage_clicked();
	//清空窗口
	void on_btn_ClearWindow_clicked();
	//是否显示图形复选框
	void on_ckb_ShowObj_stateChanged(int arg1);

	//*************************************************************************
		//Blob
	//Blob添加形态学操作类型
	void on_pb_AddOperatorType_Morphology_Blob_clicked();

	//Blob清空形态学操作类型
	void on_pb_ClearOperatorType_Morphology_Blob_clicked();
	//绘制	roi函数
	void DrawRoiFunc(int RoiShape, HObject &OutRegion);
	//绘制ROI
	void on_pb_DrawRoi_Blob_clicked();
	//清空ROI
	void on_pb_ClearRoi_Blob_clicked();
	//运行Blob函数
	void on_pb_Run_clicked();
	//添加筛选特征
	void on_pb_AddFeature_Blob_clicked();

	//清空筛选特征
	void on_pb_ClearFeature_Blob_clicked();
	//保存Blob参数
	void on_pb_SaveData_clicked();
	//读取Blob参数
	void on_pb_LoadData_clicked();
	//Blob二值化类型选择事件
	void on_cmb_Type_Binarization_Blob_currentTextChanged(const QString &arg1);
	//Blob形态学区域操作类型选择事件
	void on_cmb_OperatorType_Morphology_Blob_activated(const QString &arg1);
	//Blob结果表格每个Cell点击事件
	void on_tablewidget_Results_Blob_cellClicked(int row, int column);
	//Blob结果表格表头的点击事件槽函数
	void slot_VerticalHeader_Blob(int);
public slots:
	//Blob检测槽函数
	int slot_BlobDetect();

private:
	Ui::BlobWindowClass ui;
	DataIO dataIOC;//参数存取对象
	QAction * Action1_Morphology;
	QMenu * Menu_Morphology;
	QAction * Action1_Filter;
	QMenu * Menu_Filter;
//刷图变量*****************************************************************
//显示图像的控件id
	HTuple m_hLabelID;            //QLabel控件句柄
	HTuple m_hHalconID;            //Halcon显示窗口句柄
	//原始图像的尺寸
	HTuple m_imgWidth, m_imgHeight;
	//图片路径列表
	HTuple m_imgFiles;

	//需要刷新到窗口的图形
	HObject ho_ShowObj;
	//是否显示图形
	bool isShowObj = true;
	//当前图像
	HObject ho_Image;
	//缩放后的图像
	HObject m_hResizedImg;
	//缩放系数
	HTuple m_hvScaledRate;
	//缩放后图像的大小
	HTuple m_scaledHeight, m_scaledWidth;
	//****************************************************************************

	HObject SearchRoi_Blob;     //图像二值化搜索区域
	RunParamsStruct_Blob hv_RunParamsBlob;//Blob图像二值化运行参数
	
public:
	int processId;	//流程ID
	string nodeName;	//模块名称
	int processModbuleID;	//模块ID
	string ConfigPath;	//配置文件夹路径
	string XmlPath;	//XML路径
			//Blob分析
	BlobDetect* BlobDetect1;
};
