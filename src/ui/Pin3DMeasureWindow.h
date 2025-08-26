#pragma once
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include "ui_Pin3DMeasureWindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <QMessageBox>
#include <pcl/point_types.h>
//#include <QtConcurrent>
#include <qdatetime.h>
//#include "DataIO.h"
#include "Pin3DMeasure.h"
#include<vtkGenericOpenGLRenderWindow.h>		
#include<QVTKOpenGLNativeWidget.h>
#include <qdebug.h>
#include <QInputDialog>
#include "datas.h"


typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloudT;


class Pin3DMeasureWindow : public QMainWindow
{
	Q_OBJECT

public:
	Pin3DMeasureWindow(QWidget *parent = nullptr);
	/*Pin3DMeasureWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr image3D, int _processID, int modubleID, std::string _modubleName
		, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig, std::function<std::string()> getConfigPath);*/
	Pin3DMeasureWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr image3D, int _processID, int modubleID, std::string _modubleName
		, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig, std::string getConfigPath);
	~Pin3DMeasureWindow();

private:
	//Exe路径
	std::string FolderPath();
	//返回一个不含strOld
	std::string strReplaceAll(const std::string& strResource, const std::string& strOld, const std::string& strNew);
	void IniPin3DMeasureClassObject();

	void PP_Callback(const pcl::visualization::PointPickingEvent& event, void* args);
	void PPB_Callback(const pcl::visualization::PointPickingEvent& event, void* args);
	void PPPin_Callback(const pcl::visualization::PointPickingEvent& event, void* args);
	boost::mutex cloud_mutex;

	bool has_suffix(const std::string& filename, const std::string& suffix);

	void ShowMultiCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2);
	void SetRunParams_Pin3DMeasure(RunParamsStruct_Pin3DMeasure &runParams);
	void UpdatePin3DMeasureWindow(const RunParamsStruct_Pin3DMeasure &runParams);
	int GetSpinBoxValue(QTableWidget *tableWidget, int row, int column);
	double GetDoubleSpinBoxValue(QTableWidget *tableWidget, int row, int column);
	QString GetComboBoxValue(QTableWidget *tableWidget, int row, int column);

	int CreateSpinBox(int row, int column, int value);
	int CreateDoubleSpinBoxMM(int row, int column, double value);
	int CreateDoubleSpinBoxDegrees(int row, int column, double value);
	int CreateDoubleSpinBox(int row, int column, double value);
	int CreateComboBox(int row, int column, QString value);

private slots:

	void on_pBtn_OpenCloud_clicked();
	void on_pBtn_SaveCloud_clicked();
	int on_pBtn_PickModel_clicked();

	int on_pBtn_AddPinROI_clicked();
	int on_pBtn_DeletePinROI_clicked();
	void on_pBtn_PickCropModulePoint_clicked();

	int on_pBtn_RunCropModule_clicked();
	int on_pBtn_RunMatchCloud_clicked();
	int on_pBtn_RunFourPointFitPlane_clicked();
	int on_pBtn_RunPinCal_clicked();

	int on_pBtn_PickCropPinPoint_clicked();
	int on_pBtn_PinMeasureAlgo_clicked();
	
	void on_pBtn_PickBaseFourPoint_clicked();
	void on_btn_SavePinMeasureData_clicked();
	void on_btn_LoadPinMeasureData_clicked();
	
	void on_pBtn_ClearData_clicked();

	void LocateChoose();

	void on_toolButton_clicked();
	void on_toolButton_2_clicked();
	void on_toolButton_3_clicked();

private:
	Ui::Pin3DMeasureWindowClass ui;

	pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud;
	pcl::PointCloud<pcl::PointXYZ>::Ptr match_cloud;
	pcl::PointCloud<pcl::PointXYZ>::Ptr current_cloud;

	Eigen::Vector4f BasePlaneCoefficient;
	int BasePointPickOrder = 0;

	vtkRenderer* ren = vtkRenderer::New();
	vtkGenericOpenGLRenderWindow * renderWindow = vtkGenericOpenGLRenderWindow::New();
	pcl::visualization::PCLVisualizer::Ptr viewer;

public:
	int processID;	//流程ID
	string nodeName;	//模块名称
	int processModbuleID;	//模块ID

	std::function<void(int processID, int modubleID, std::string modubleName)> refreshConfig;

	string ConfigPath;	//配置文件夹路径
	string XmlPath;	//XML路径

	modubleDatas* modubleDatas1 = nullptr;
	Pin3DMeasure* Pin3DMeasureParams1;
	RunParamsStruct_Pin3DMeasure RunParamsPin3DMeasure;
	pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud;

	void wheelEvent(QWheelEvent* event) override;
	void LoadShowCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
};

struct PinMeasCallbackArgs {
	PointCloudT::Ptr clicked_points_3d;
	pcl::visualization::PCLVisualizer::Ptr viewerPtr;
};
