#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <fstream>
#include <pcl/filters/crop_box.h>
#include <pcl/common/common.h>
#include <pcl/filters/extract_indices.h>
#include <qdebug.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/features/moment_of_inertia_estimation.h>
#include <pcl/registration/icp.h> 
#include <pcl/filters/voxel_grid.h>
#include "lvm_sdk.h"

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>

#include <pcl/filters/convolution_3d.h>
#include <pcl/common/gaussian.h>
#include <opencv2/opencv.hpp>
#include "datas.h"
#include "dllAPI.h"


//#include <pcl/search/kdtree.h>//kdtree头文件
//#include <pcl/features/normal_3d.h>//法线特征估计
//#include <pcl/segmentation/region_growing.h>//区域生长



class Pin3DMeasure
{
public:
	Pin3DMeasure();
	Pin3DMeasure(std::string _nodeName, int _processModbuleID, int _processID);
	~Pin3DMeasure();

	//void setGetConfigPath(std::function<std::string()> getConfigPath);
	void setGetConfigPath(std::string getConfigPath);

	portNames portNames;
	std::map<std::string, portRegisterInfo> inputCtrlInfos;
	std::map<std::string, portRegisterInfo> outputCtrlInfos;
	std::map<std::string, portRegisterInfo> inputIconicInfos;
	std::map<std::string, portRegisterInfo> outputIconicInfos;

	groupLists groupLists;
	std::string groupName = groupLists.visualRecognitionLayoutV;
	modubleType modubleType = modubleType::Halcon;
	std::string modelCaption = "pin针3D";

	void setPortInfos();
	void setPortInfoInputCtrl();
	void setPortInfoOutputCtrl();
	void setPortInfoInputIconic();
	void setPortInfoOutputIconic();

	void run(std::map<std::string, HTuple>& HTupleList, pcl::PointCloud<pcl::PointXYZ>::Ptr& image3D);
	void ReadParams(std::map<std::string, HTuple>& HTupleList, std::map<std::string, HObject>& HObjectList);

	int getRefreshConfig(int processID, int modubleID, std::string modubleName);
	std::vector<std::string> getModubleResultTitleList();
	std::vector<std::map<int, std::string>> getModubleResultList();


private:
	std::vector<std::string> modubleResultTitleList;
public:

	int processID;
	int processModbuleID;
	std::string nodeName;

	string configPath;
	string XMLPath;
	RunParamsStruct_Pin3DMeasure mRunParams;
	ResultParamsStruct_Pin3DMeasure mResultParams;

	modubleDatas* modubleDatas1 = nullptr;

	void GaussianFilter(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud_after_gaussian_filter);

	void CloudShowPopWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);

	void MultiCloudShowPopWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2);

	void MultiCloudShowCropPopWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2,
		RunParamsStruct_Pin3DMeasure &runparams, Eigen::Matrix3f &inverseRotationMatrix);

	void Create3DRoi(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cropped_inverse,
		RunParamsStruct_Pin3DMeasure &runparams, Eigen::Matrix3f &inverseRotationMatrix);

	void MatchingICP(pcl::PointCloud<pcl::PointXYZ>::Ptr model_cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_trans,
		RunParamsStruct_Pin3DMeasure &runparams, Eigen::Matrix4f &transformMatrixInverse);

	int CloudVoxelDownsampling(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
		RunParamsStruct_Pin3DMeasure &runparams,
		pcl::PointCloud<pcl::PointXYZ>::Ptr CloudSampling);

	void FitPlaneLeastSquares(std::vector<cv::Point3f> ptsvec, Eigen::Vector4f &coefficient);

	void CalMinCircumscribedCubeCenterPose(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
		Eigen::Vector3f &mass_center);

	void CalRegionPose(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_region, RunParamsStruct_Pin3DMeasure &runparams,
		Eigen::Vector3f &mass_center);

	void StatisticalOutlierRemoval(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered, RunParamsStruct_Pin3DMeasure &runparams);

	void ClusterCalDis(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, RunParamsStruct_Pin3DMeasure &runparams,
		Eigen::VectorXf BasePlaneCoefficient, std::vector<cv::Point3d> &PinCenter,
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_show);

	void CalDistance3DPointToPlane(Eigen::VectorXf FitPlaneRANSAC_coefficient,
		Eigen::VectorXf MinCircumscribedCubeCenterPose, double &distance3DPointToPlane);

	static bool AscCompareX(cv::Point3d pt1, cv::Point3d pt2);
	static bool AscCompareY(cv::Point3d pt1, cv::Point3d pt2);

	static bool DescCompareX(cv::Point3d pt1, cv::Point3d pt2);
	static bool DescCompareY(cv::Point3d pt1, cv::Point3d pt2);

	int CropModuleCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud,
		RunParamsStruct_Pin3DMeasure runparams);

	int MatchTransCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud,
		RunParamsStruct_Pin3DMeasure &runparams, 
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_crop_moduleROI_trans);

	int FourPointFitPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_crop_module,
		RunParamsStruct_Pin3DMeasure &runparams, Eigen::Vector4f &BasePlaneCoefficient);

	int CropPinCalCoord(pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr pin_cloud,
		RunParamsStruct_Pin3DMeasure &runparams, Eigen::Vector4f BasePlaneCoefficient,
		ResultParamsStruct_Pin3DMeasure &resultparams, int OrderNum);

	int PinMeasureAlgo(pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud,
		RunParamsStruct_Pin3DMeasure &runparams, ResultParamsStruct_Pin3DMeasure &resultparams);

private:
	//写日志函数
	int WriteTxt(std::string content);

};