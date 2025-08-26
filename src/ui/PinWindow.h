#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include "ui_PinWindow.h"
#include <QSettings>
#include <QVector>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

#include <qdatetime.h>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include "PinDetect.h"
#include <QLog.h>
#include "datas.h"
#include <rs.h>



class PinWindow : public QMainWindow
{
	Q_OBJECT

public:
	PinWindow(QWidget *parent = nullptr);
	//PinWindow(HObject image, int _processID, int modubleID, std::string _modubleName
	//	, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig, std::function<std::string()> getConfigPath);
	PinWindow(HObject image, int _processID, int modubleID, std::string _modubleName
		, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig, std::string getConfigPath);
	~PinWindow();
	QPoint whereismouse;
	BOOL keypoint = 0;
	void getmessage(QString value);
	std::string getConfigPathTp();
	ResultParamsStruct_Pin getResult()
	{
		return mResultParams;
	};


protected:
	virtual void resizeEvent(QResizeEvent* event);
private:
	//初始化显示窗口
	void InitWindow();
	//显示图片(hv_FillType 可以选择margin或者fill)
	void showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color);
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	void GiveParameterToWindow();
	//设置输入参数
	void RefreshRunparams(const RunParamsStruct_Pin &RunParams);
	//设置输入参数
	void RefreshRunparams_ROI(const RunParamsStruct_Pin &RunParams, int index1);
	void RefreshRunparams_Base(const RunParamsStruct_Pin &RunParams, int index1);
	//设置输出参数
	void SetResultparams(qint64 CT);
	//初始化工艺参数表格
	void IniTableData_Standard();
	//绘制箭头
	void ShowArrow_Line(HTuple windowId, HTuple LineRow1, HTuple LineCol1, HTuple LineRow2, HTuple LineCol2);
	//绘制箭头
	void ShowArrow_Circle(HTuple windowId, HTuple CircleRow, HTuple CircleCol, HTuple CircleRadius);

	//Exe路径
	string FolderPath();
	//返回一个不含strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	QLog* qlog = nullptr; 

private slots:
	void on_toolButton_clicked();
	void on_toolButton_2_clicked();
	void on_toolButton_3_clicked();
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	//打开图片
	void on_pBtn_ReadImage_clicked();
	//图像自适应
	void on_pBtn_FitImage_clicked();
	//清空窗口
	void on_btn_ClearWindow_clicked();
	//是否显示图形复选框
	void on_ckb_ShowObj_stateChanged(int arg1);

	//绘制搜索区域
	void on_pb_DrawRoi_clicked();
	//清除ROI
	void on_pb_ClearRoi_clicked();
	//Blob测试
	void on_pb_BlobTest_clicked();
	//找基准测试
	void on_pb_BaseTest_clicked();
	//Blob结果表格每个Cell点击事件
	void on_tablewidget_Results_Blob_cellClicked(int row, int column);
	//Blob结果表格表头的点击事件槽函数
	void slot_VerticalHeader_Blob(int);
	//Pin结果表格每个Cell点击事件
	void on_tablewidget_ResultData_cellClicked(int row, int column);
	//Pin结果表格表头的点击事件槽函数
	void slot_VerticalHeader_ResultData(int);
	//模板匹配结果表格每个Cell点击事件
	void on_tablewidget_Results_Template_cellClicked(int row, int column);
	//模板匹配结果表格表头的点击事件槽函数
	void slot_VerticalHeader_TemplateData(int);
	//模板匹配结果表格表头的点击事件槽函数
	void slot_VerticalHeader_TemplateData2(int);
	//添加基准
	void on_pb_AddBase_clicked();
	//删除基准
	void on_pb_DeleteBase_clicked();
	//绘制基准
	void on_pb_DrawBase_clicked();
	//添加工艺参数
	void on_pb_AddValue_clicked();
	//清空工艺参数
	void on_pb_ClearValue_clicked();
	//计算Pin数量
	void on_pb_CalculatePinNum_clicked();
	//工艺参数一键导入
	void on_pb_ImportStandardData_clicked();
	//一键设置默认工艺参数
	void on_pb_SetDefaultStandardData_clicked();

	//基准表格删除行槽函数
	void slot_DeleteRow();
	//基准参数设置
	void on_pb_SetBaseData_clicked();
	//添加Blob搜索区域
	void on_pb_AddRoi_clicked();
	//设置当前区域Blob参数
	void on_pb_SetBlobData_clicked();
	//刷新绑定基准的控件
	void on_pb_RefreshBinding_clicked();
	//绑定基准
	void on_pb_Binding_clicked();
	//设置标准参数
	void on_pb_SetStandardData_clicked();
	//Pin针检测运行
	void on_pb_Run_clicked();
	//保存参数
	void on_pb_SaveData_clicked();
	//读取参数
	void on_pb_ReadData_clicked();
	//绘制模板训练区域
	void on_btn_DrawTrainRoi_Template_clicked();
	//绘制掩模区域
	void on_btn_DrawShaddowRoi_Template_clicked();
	//合并模板区域Roi
	void on_btn_UnionRoi_Template_clicked();
	//清除模板区域Roi
	void on_btn_ClearRoi_Template_clicked();
	//训练模板
	void on_btn_Train_Template_clicked();

	//切换ROI
	void on_cmb_SearchRegion_activated(const QString &arg1);
	//切换基准
	void on_cmb_BaseSelect_activated(const QString &arg1);
	//选择保存CSV路径
	void on_pb_CsvPath_clicked();

	//位置修正绘制模板训练区域
	void on_btn_DrawTrainRoi_Template_2_clicked();
	//位置修正绘制掩模区域
	void on_btn_DrawShaddowRoi_Template_2_clicked();
	//位置修正合并模板区域Roi
	void on_btn_UnionRoi_Template_2_clicked();
	//位置修正清除模板区域Roi
	void on_btn_ClearRoi_Template_2_clicked();
	//位置修正训练模板
	void on_btn_Train_Template_2_clicked();
	//是否启用位置修正
	void on_ckb_PositionCorrection_stateChanged(int state);
	//绘制位置修正搜索区域
	void on_pb_SearchROI_PositionCorrection_clicked();
	//设置位置修正参数
	void on_pb_SetData_PositionCorrection_clicked();
	//运行位置修正
	void on_pb_Run_PositionCorrection_clicked();
public slots:
	//找pin槽函数
	int slot_PinDetect();
private:
	Ui::PinWindowClass ui;

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
	//Pin检测
	PinDetect *PinDetect1;
	RunParamsStruct_Pin RunParams_Pin;
	ResultParamsStruct_Pin mResultParams;



	HObject BlobRegionShow;
	HObject BaseContourShow;
	int BlobCountShow = 0;
	QVector<HObject> vecTemplateRegions;
	QVector<HObject> vecShadowRegions;

public:
	DllDatas* dllDatas1 = nullptr;

	int processId;	//流程ID
	string nodeName;	//模块名称
	int processModbuleID;	//模块ID
	string ConfigPath;	//配置文件夹路径
	string XmlPath;	//XML路径

	std::function<void(int processID, int modubleID, std::string modubleName)> refreshConfig;
	void wheelEvent(QWheelEvent* event) override;
signals:
	void showPinDeubg_signal(ResultParamsStruct_Pin result, PointItemConfig config);
};
