#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include "ui_RobotCalibWindow.h"
#include <QSettings>
#include <QSettings>
#include <QVector>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QtConcurrent>
#include <qdatetime.h>
#include <QListWidget>
#include <QDialog>
#include <QDesktopWidget>
#include <QTableWidgetItem>
#include <QColorDialog>
#include "BaseItem.h"
#include <QGraphicsOpacityEffect>
#include "QGraphicsViews.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include "PaintDetect.h"
#include "Common.h"
#include "RobotCalibDetect.h"


struct CenterCalibStruct
{
	QPointF pointC;//旋转中心坐标
	QList<double> rAList;//计算旋转中心的集合
};
using namespace std;
using namespace HalconCpp;
class RobotCalibWindow : public QWidget
{
	Q_OBJECT

public:
	RobotCalibWindow(QWidget *parent = nullptr);
	~RobotCalibWindow();
	QPoint whereismouse;
	bool keypoint = 0;
private:
	void getmessage(QString value);
	//初始化显示窗口
	void InitWindow();
	//计算Mark点中心
	int MarkCenter(const HObject &ho_Img, double &X, double &Y, HObject &retContour);
	//初始化数据显示表格
	void IniTableData();
	//设置标定参数
	int SetRunParams_Calib(RunParamsStruct_RobotCalib &runParams);
	//设置模板匹配参数
	int SetRunParams_Template(RunParamsStruct_RobotCalib &runParams);
	//更新参数到窗口
	void UpDataWindow(const RunParamsStruct_RobotCalib &runParams);
	//刷新偏移量结果
	void SetOffsetData(const RunParamsStruct_RobotCalib &runParams, const ResultParamsStruct_RobotCalib &resultParams);
	//Exe路径
	string FolderPath();
	//返回一个不含strOld
	string strReplaceAll(const string& strResource, const string& strOld, const string& strNew);
	//按照指定字符分割字符串到vector
	void stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken);
	//线程延迟(不阻塞主线程，QThread::Sleep()会阻塞主线程)
	void Delay_MSec(unsigned int msec); //毫秒

private slots:
	//打开图片
	void on_pb_ReadImage_clicked();
	//添加标定roi
	void on_pb_AddCalibRoi_clicked();
	//清除标定roi
	void on_pb_ClearCalibRoi_clicked();
	//找圆
	void on_pb_FindCircleCalib_clicked();
	//保存参数
	void on_pb_SaveData_clicked();
	//读取参数
	void on_pb_ReadData_clicked();
	//***************************************************************************
	//引导模式
	void on_cmb_locateMode_activated(const QString &arg1);
	//九点标定添加变量
	void on_pb_AddValue_clicked();
	//九点标定清空变量
	void on_pb_ClearValue_clicked();
	//九点标定开始
	void on_pb_Calibrate_clicked();
	//九点标定删除行槽函数
	void slot_DeleteRow();
	//九点标定获取像素坐标槽函数
	void slot_GetPixel();
	//旋转中心添加变量
	void on_pb_AddValue_RotateCenter_clicked();
	//旋转中心清空变量
	void on_pb_ClearValue_RotateCenter_clicked();
	//旋转中心标定开始
	void on_pb_Calibrate_RotateCenter_clicked();
	//旋转中心删除行槽函数
	void slot_DeleteRow_RotateCenter();
	//旋转中心获取机器坐标槽函数
	void slot_GetRobot_RotateCenter();
	//计算机器人抓取偏移量
	void on_pb_GetOffset_clicked();
	//计算机器人绝对坐标
	void on_pb_TransPoint_clicked();
	//计算偏移量或者机器人坐标函数
	int slot_CalculateLocate(RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams);
	//*********************************************
	//是否启用高级参数
	void on_ckb_AdvancedData_stateChanged(int arg1);
	//绘制训练区域
	void on_pb_DrawTrainRoi_clicked();
	//绘制掩膜区域
	void on_pb_DrawShaddowRoi_clicked();
	//清除训练区域
	void on_pb_ClearTrainRoi_clicked();
	//切换搜索区域
	void on_cmb_SearchRoi_activated(const QString &arg1);
	//切换查看图片
	void on_cmb_ImgSelect_activated(const QString &arg1);
	//训练模板
	void on_pb_Train_Template_clicked();
	//查找模板
	void on_pb_FindTemplate_clicked();
	//训练模板槽函数
	int slot_CreateTemplate(RunParamsStruct_RobotCalib &runParams);
	//查找模板槽函数
	int slot_FindTemplate();
	//计算机器人九点坐标
	void on_pb_CalculateRobot_clicked();
private:
	Ui::RobotCalibWindowClass ui;
	//刷图变量*****************************************************************
	cv::Mat MatImage;
	HObject ho_Image;
	HObject ho_CropModelImg;
	QGraphicsViews *view;
	QVector<RectangleItem *> vec_RoiSearch_Calib;
	QVector<RectangleItem *> vec_RoiTrain_Template;
	QVector<RectangleItem *> vec_RoiSearch_Template;
	QVector<RectangleItem *> vec_RoiShaddow_Template;
	HObject TrainRoi_Template;     //模板训练区域
	HObject SearchRoi_Template;     //模板搜索区域
	HObject ShadowRoi_Template;		//掩膜区域
	QImage Q_InputImg;
	QImage Q_OutputImg;
	QColor color;
	PaintDetect *PaintDetect2;
	Common *Common1;

	QList<QPointF> rPList;//机器人坐标
	CenterCalibStruct CenterPStruct;//旋转中心结构体

	RobotCalibDetect *RobotCalibDetect1;
	RunParamsStruct_RobotCalib RunParams_RobotCalib;
	//标定标志位：0 关闭标定模式，1 机器人标定，2 旋转中心标定
	int isStartCalib = 0;
	//标定点位(1-9)
	int RobotCalibNo = 0;

public:
	int processId = 0;	//流程ID
	string nodeName = "Robot";	//模块名称
	int processModbuleID = 0;	//模块ID
	string ConfigPath;	//配置文件夹路径
	string XmlPath;	//XML路径

};
