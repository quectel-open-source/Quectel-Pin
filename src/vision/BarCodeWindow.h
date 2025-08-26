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
	//初始化显示窗口
	void InitWindow();
	//显示图片(hv_FillType 可以选择margin或者fill)
	void showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color);
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	void GiveParameterToWindow();
	//**********************************************************************************************
	//运行查找一维码结构体参数赋值函数
	void SetRunParams_BarCode(RunParamsStruct_BarCode &runParams);
	//初始化一维码结果显示表格
	void IniTableData_BarCode();
	//设置一维码结果显示表格数据
	void SetTableData_BarCode(const ResultParamsStruct_BarCode &resultStruct, qint64 Ct);
	//更新一维码检测运行参数到窗口
	void UpdateBarCodeDataWindow(const RunParamsStruct_BarCode &runParams);
	//Exe路径
	string FolderPath();
	//返回一个不含strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	//******************************************************************************************************
private slots:
	//打开图片
	void on_pBtn_ReadImage_clicked();
	//图像自适应
	void on_pBtn_FitImage_clicked();
	//清空窗口
	void on_btn_ClearWindow_clicked();
	//是否显示图形复选框
	void on_ckb_ShowObj_stateChanged(int arg1);
	//***************************************************************************
	//一维码检测槽函数
	void on_pb_Run_clicked();
	//绘制一维码搜索区域
	void on_btn_DrawRoi_BarCode_clicked();
	//保存一维码检测参数
	void on_pb_SaveData_clicked();
	//读取一维码检测参数
	void on_pb_LoadData_clicked();
	//清空ROI
	void on_btn_ClearRoi_BarCode_clicked();
	//**************************************************************************

	//结果表格每个Cell点击事件
	void on_tablewidget_Results_BarCode_cellClicked(int row, int column);
	//结果表格表头的点击事件槽函数
	void slot_VerticalHeader_BarCode(int);
public slots:
	//检测一维码槽函数
	int slot_FindBarCode();
private:
	Ui::BarCodeWindowClass ui;
	DataIO dataIOC;//参数存取对象

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
	//*****************************************************
		//条码检测参数变量

	QVector<HObject> ho_BarCodeRegions;   //一维码搜索区域集合
	HObject SearchRoi_BarCode;     //一维码搜索区域
	RunParamsStruct_BarCode hv_RunParamsBarCode;  //一维码运行参数
	HObject RetRegionShow;
	int RetCountShow = 0;
	//条码检测结果（erik 新加）
	ResultParamsStruct_BarCode mResultParams;
	//****************************************************************************
public:
	int processId;	//流程ID
	string nodeName;	//模块名称
	int processModbuleID;	//模块ID
	string ConfigPath;	//配置文件夹路径
	string XmlPath;	//XML路径
	BarCodeDetect* barCodeParams1;        //一维码检测类指针对象1
};
