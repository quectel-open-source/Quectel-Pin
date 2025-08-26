#pragma once
#include <QtWidgets/QMainWindow>
#include <Halcon.h>
#include <halconcpp/HalconCpp.h>
#include "opencv2/opencv.hpp"
#ifdef _WIN32

#include "ui_QIDIOCRWindow.h"
#include <QMessageBox>
#include <QDateTime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<QFileDialog>
#include<QFileInfoList>
#include<QFileInfo>
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <regex>

//#include "Quectel_Infer.h"
#include "putTextCN.h"
#include "QIDIOCRDetect.h"
#include <QPainter>
#include <QLog.h>
#include <json.h>
#include <dataDetect.h>
#include <DataIO.h>


#endif

using namespace std;
using namespace cv;
using namespace HalconCpp;
class QIDIOCRWindow : public QMainWindow {
	Q_OBJECT
#ifdef _WIN32
public:
	QIDIOCRWindow(QWidget* parent = nullptr);
	QIDIOCRWindow(HObject image, int _processID, int modubleID, std::string _modubleName
		, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig);
	~QIDIOCRWindow();
	QString modelType;
	Json::Reader reader;
	Json::Value jsonValue;
	Json::Value inferValue;
	ifstream is;
	//Quectel_Infer quectel_infer = Quectel_Infer();
	QFileInfoList imagelist;
	int imageindex;
	QPoint whereismouse;
	int keypoint = 0;
	QFileInfoList modellist;
	cv::Point point1;
	cv::Point point2;
	cv::Point point3;
	cv::Point point4;
	RunParamsStruct_OCR runParams;
	ResultParamsStruct_OCR ResultQIDI;
	std::vector<HObject> hv_resultRegions; //缺陷区域集合
	std::string config_path;
	std::vector<QString> ho_selectParameter;
	QList<std::vector<QString>> ho_selectParameters;
	int indexParameters;
	bool sortUpDown;
	std::vector<std::string> GPUmodel_paths; //GPU模型地址
	std::vector<std::string> CPUmodel_paths; //CPU模型地址
private:
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
	Mat Matimage;
	Mat displyImage;
	Mat oriimage;
	HObject SearchRoi_Template;     //ROI图片
	QVector<HObject> ho_SearchRegions;   //ROI区域集合;
	HObject ho_SearchRegion;
	//缩放后的图像
	HObject m_hResizedImg;
	//缩放系数
	HTuple m_hvScaledRate;
	//缩放后图像的大小
	HTuple m_scaledHeight, m_scaledWidth;
	QLog* qlog = nullptr; 
private slots:

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);
	void getmessage(QString value);
	void IniTableData_QIDI();
	void SetTableData_QIDI(const ResultParamsStruct_OCR& resultStruct, qint64 Ct);
	void SetRunParams_QIDI();


	void windowMin();
	void windowMax();
	void windowClose();
	//图像自适应
	void pBtn_FitImage();
	//清空窗口
	void btn_ClearWindow();
	//是否显示图形复选框
	void on_ckb_ShowObj_stateChanged(int arg1);
	void btn_FindCharacter();
	void pushButton_saveQIDI();
	void btn_DrawRoi_QIDI();
	void btn_ClearRoi_QIDI();
	void toolButton_ChooseModel();
	void showImg(const HObject& ho_Img, HTuple hv_FillType, HTuple hv_Color);
	void GiveParameterToWindow();
	void UnionRoi_Template();
	void QIDIinit();
	void QIDITest(Mat);
	int Readconfig(std::string);
	void on_tablewidget_Results_QIDI_cellClicked(int row, int column);
	void sortItems(int column);
	int getGPUnumber();
	std::string WStringToString(const std::wstring& wstr);
private:
	Ui::QIDIOCRWindowClass ui;

	int processId = 0;	//流程ID
	string nodeName = "QIDI";	//模块名称
	int processModbuleID = 0;	//模块ID

	std::function<void(int processID, int modubleID, std::string modubleName)> refreshConfig;
	std::string modelTypeStr = "";

	QIDIOCRDetect* QIDIOCRDetect1 = nullptr;

	int WriteTxt(string content);
	void PathSet(const string ConfigPath, const string XmlPath);

	void chooseModel(QString filePath);

	DataIO dataIOC;//参数存取对象

	QImage Mat2QImage(const cv::Mat& mat);

	cv::Mat QImage2Mat(const QImage image);

	bool isDrawRoi = false;
#else
QIDIOCRWindow(QWidget* parent = nullptr) {

}
	QIDIOCRWindow(HObject image, int _processID, int modubleID, std::string _modubleName, std::string configPath
		, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig) {
	}
~QIDIOCRWindow(){}
#endif
};
