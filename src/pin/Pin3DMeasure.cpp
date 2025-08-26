#include "Pin3DMeasure.h"
#include "qc_log.h"

#pragma region 需要修改的代码

//点云显示
bool has_suffix(const std::string& filename, const std::string& suffix) {
	std::size_t index = filename.find(suffix, filename.size() - suffix.size());
	return (index != std::string::npos);
}

void Pin3DMeasure::run(std::map<std::string, HTuple>& HTupleList, pcl::PointCloud<pcl::PointXYZ>::Ptr& image3D) {


	PinMeasureAlgo(image3D, mRunParams, mResultParams);

	//结果
	HTupleList[portNames.PinFlag] = HTuple();
	for (int index = 0; index < mResultParams.PinFlag.size(); index++) {
		TupleConcat(HTupleList[portNames.PinFlag], HTuple(mResultParams.PinFlag[index]), &HTupleList[portNames.PinFlag]);
	}
	//根据PinFlag判断结果
	HTupleList[portNames.Result] = 1;
	for (int index = 0; index < mResultParams.PinFlag.size(); index++) {
		if (!mResultParams.PinFlag[index]) {
			HTupleList[portNames.Result] = 0;
			break;
		}
	}

	HTupleList[portNames.PinHeight] = HTuple();
	for (int index = 0; index < mResultParams.PinHeight.size(); index++) {
		TupleConcat(HTupleList[portNames.PinHeight], HTuple(mResultParams.PinHeight[index]), &HTupleList[portNames.PinHeight]);
	}
}

void Pin3DMeasure::ReadParams(std::map<std::string, HTuple>& HTupleList, std::map<std::string, HObject>& HObjectList) {

	//xml文件读取参数
	modubleDatas1->ReadParams_Pin3DMeasure(mRunParams, processID, nodeName, processModbuleID);
	//HObjectList[portNames.SearchRegion] = mRunParams.ho_SearchRegion;
	//HTupleList[portNames.ColorValue1_Min] = mRunParams.hv_ColorValue1_Min;
	//HTupleList[portNames.ColorValue1_Max] = mRunParams.hv_ColorValue1_Max;
	//HTupleList[portNames.ColorValue2_Min] = mRunParams.hv_ColorValue2_Min;
	//HTupleList[portNames.ColorValue2_Max] = mRunParams.hv_ColorValue2_Max;
	//HTupleList[portNames.ColorValue3_Min] = mRunParams.hv_ColorValue3_Min;
	//HTupleList[portNames.ColorValue3_Max] = mRunParams.hv_ColorValue3_Max;
	//HTupleList[portNames.ColorArea_Min] = mRunParams.hv_ColorArea_Min;
	//HTupleList[portNames.ColorArea_Max] = mRunParams.hv_ColorArea_Max;
	//HTupleList[portNames.Channel_ColorExtract] = mRunParams.hv_Channel_ColorExtract;
	//HObjectList[portNames.SearchRegion_PositionCorrection] = mRunParams.ho_SearchRegion_PositionCorrection;
	//HObjectList[portNames.TrainRegion_PositionCorrection] = mRunParams.ho_TrainRegion_PositionCorrection;
	//HTupleList[portNames.Row_PositionCorrection] = mRunParams.hv_Row_PositionCorrection;
	//HTupleList[portNames.Column_PositionCorrection] = mRunParams.hv_Column_PositionCorrection;
	//HTupleList[portNames.Angle_PositionCorrection] = mRunParams.hv_Angle_PositionCorrection;
	//HTupleList[portNames.MatchMethod_PositionCorrection] = mRunParams.hv_MatchMethod_PositionCorrection;
	//HTupleList[portNames.AngleStart_PositionCorrection] = mRunParams.hv_AngleStart_PositionCorrection;
	//HTupleList[portNames.AngleExtent_PositionCorrection] = mRunParams.hv_AngleExtent_PositionCorrection;
	//HTupleList[portNames.ScaleRMin_PositionCorrection] = mRunParams.hv_ScaleRMin_PositionCorrection;
	//HTupleList[portNames.ScaleRMax_PositionCorrection] = mRunParams.hv_ScaleRMax_PositionCorrection;
	//HTupleList[portNames.ScaleCMin_PositionCorrection] = mRunParams.hv_ScaleCMin_PositionCorrection;
	//HTupleList[portNames.ScaleCMax_PositionCorrection] = mRunParams.hv_ScaleCMax_PositionCorrection;
	//HTupleList[portNames.MinScore_PositionCorrection] = mRunParams.hv_MinScore_PositionCorrection;
	//HTupleList[portNames.NumMatches_PositionCorrection] = mRunParams.hv_NumMatches_PositionCorrection;
	//HTupleList[portNames.ModelID_PositionCorrection] = mRunParams.hv_ModelID_PositionCorrection;
	//HTupleList[portNames.Check_PositionCorrection] = mRunParams.hv_Check_PositionCorrection;
	//HTupleList[portNames.Channel_PositionCorrection] = mRunParams.hv_Channel_PositionCorrection;
}

void Pin3DMeasure::setPortInfos() {
	setPortInfoInputIconic();
	setPortInfoInputCtrl();
	setPortInfoOutputCtrl();
	setPortInfoOutputIconic();
}

void Pin3DMeasure::setPortInfoInputCtrl() {

}

void Pin3DMeasure::setPortInfoOutputCtrl() {
	portRegisterInfo portInfo;
	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = true;
	portInfo.chineseName = "高度结果";
	outputCtrlInfos[portNames.PinFlag] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = true;
	portInfo.chineseName = "高度值";
	outputCtrlInfos[portNames.PinHeight] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = true;
	portInfo.chineseName = "结果";
	outputCtrlInfos[portNames.Result] = portInfo;
}

void Pin3DMeasure::setPortInfoInputIconic() {
	portRegisterInfo portInfo;
	portInfo.portType = ePortType::Image3D;
	portInfo.isNecessity = true;
	portInfo.isSelect = true;
	portInfo.chineseName = "3D图像";
	inputIconicInfos[portNames.Image3D] = portInfo;
}

void Pin3DMeasure::setPortInfoOutputIconic() {
}
#pragma endregion

int Pin3DMeasure::getRefreshConfig(int processID, int modubleID, std::string modubleName) {
	return 0;
}

std::vector<std::string> Pin3DMeasure::getModubleResultTitleList() {
	std::vector<std::string> result;
	result.clear();
	result.push_back("高度值");
	result.push_back("高度结果");
	return result;
}

std::vector<std::map<int, std::string>> Pin3DMeasure::getModubleResultList() {
	std::vector<std::map<int, std::string>> results;
	int count = mResultParams.PinHeight.size();
	if (count > 0) {
		for (int i = 0; i < count; i++) {
			std::map<int, std::string> result;
			int index = 0;
			result[index++] = modubleDatas1->formatDobleValue(mResultParams.PinHeight[i], 2);
			result[index++] = std::to_string(mResultParams.PinFlag[i]);

			results.push_back(result);
		}
	}
	return results;
}

Pin3DMeasure::Pin3DMeasure()
{
	setPortInfos();
}

Pin3DMeasure::Pin3DMeasure(std::string _nodeName, int _processModbuleID, int _processID) {
	processID = _processID;
	processModbuleID = _processModbuleID;
	nodeName = _nodeName;

	setPortInfos();

	modubleDatas1 = new modubleDatas();
}

void Pin3DMeasure::setGetConfigPath(std::string getConfigPath) {
	modubleDatas1->setGetConfigPath(getConfigPath);
}

Pin3DMeasure::~Pin3DMeasure()
{

}

//高斯滤波
void Pin3DMeasure::GaussianFilter(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud_after_gaussian_filter)
{
	pcl::filters::GaussianKernel<pcl::PointXYZ, pcl::PointXYZ>::Ptr kernel(new pcl::filters::GaussianKernel<pcl::PointXYZ, pcl::PointXYZ>);
	(*kernel).setSigma(4);
	(*kernel).setThresholdRelativeToSigma(4);
	//(*kernel).setThreshold(0.05);
	pcl::search::KdTree<pcl::PointXYZ>::Ptr kdtree(new pcl::search::KdTree<pcl::PointXYZ>);
	(*kdtree).setInputCloud(cloud);
	pcl::filters::Convolution3D <pcl::PointXYZ, pcl::PointXYZ, pcl::filters::GaussianKernel<pcl::PointXYZ, pcl::PointXYZ> > convolution;
	convolution.setKernel(*kernel);
	convolution.setInputCloud(cloud);
	convolution.setSearchMethod(kdtree);
	convolution.setRadiusSearch(0.3);
	//convolution.setNumberOfThreads(5);
	convolution.convolve(*cloud_after_gaussian_filter);
}

//点云显示
void Pin3DMeasure::CloudShowPopWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ>(cloud, "sample cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
	viewer->addCoordinateSystem(5.0);
	viewer->initCameraParameters();
	std::string str = "Number of point clouds: " + std::to_string(cloud->points.size());
	viewer->setShowFPS(false);
	viewer->addText(str, 0, 0, 20, 1.0, 1.0, 1.0, "sre");
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	viewer->removeAllPointClouds();
}

void Pin3DMeasure::MultiCloudShowPopWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2)
{
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ>(cloud1, "sample cloud1");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud1");
	viewer->addCoordinateSystem(5.0);
	//viewer->initCameraParameters();
	//std::string str = "Number of point clouds: " + std::to_string(cloud1->points.size());
	viewer->setShowFPS(false);
	//viewer->addText(str, 0, 0, 20, 1.0, 1.0, 1.0, "sre");
	viewer->addPointCloud<pcl::PointXYZ>(cloud2, "sample cloud2");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1, 0, 0, "sample cloud2");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud2");
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	viewer->removeAllPointClouds();
}

void Pin3DMeasure::MultiCloudShowCropPopWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2,
	RunParamsStruct_Pin3DMeasure &runparams, Eigen::Matrix3f &inverseRotationMatrix)
{
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ>(cloud1, "sample cloud1");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud1");
	viewer->addCoordinateSystem(5.0);
	viewer->setShowFPS(false);
	Eigen::Quaternionf rotation(inverseRotationMatrix);
	std::string cube;
	Eigen::Vector3f center(runparams.ROI_CenterX, runparams.ROI_CenterY, runparams.ROI_CenterZ);
	viewer->addCube(center, rotation, runparams.ROI_LengthX, runparams.ROI_LengthY, runparams.ROI_LengthZ, cube);
	//设置矩形框只有骨架
	viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, cube);
	viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 0, 1, cube);
	viewer->addPointCloud<pcl::PointXYZ>(cloud2, "sample cloud2");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1, 0, 0, "sample cloud2");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud2");
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	viewer->removeAllPointClouds();
}

//裁剪出立方体点云
void Pin3DMeasure::Create3DRoi(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cropped_inverse,
	RunParamsStruct_Pin3DMeasure &runparams, Eigen::Matrix3f &inverseRotationMatrix)
{
	if (runparams.ROI_RZ == 0 && runparams.ROI_RY == 0 && runparams.ROI_RX == 0)
	{
		pcl::CropBox<pcl::PointXYZ> cropboxs;
		cropboxs.setInputCloud(cloud);
		cropboxs.setMin(Eigen::Vector4f(runparams.ROI_CenterX - runparams.ROI_LengthX / 2,
			runparams.ROI_CenterY - runparams.ROI_LengthY / 2,
			runparams.ROI_CenterZ - runparams.ROI_LengthZ / 2, 1)); //设置最小点
		cropboxs.setMax(Eigen::Vector4f(runparams.ROI_CenterX + runparams.ROI_LengthX / 2,
			runparams.ROI_CenterY + runparams.ROI_LengthY / 2,
			runparams.ROI_CenterZ + runparams.ROI_LengthZ / 2, 1));//设置最大点
		cropboxs.filter(*cloud_cropped_inverse);
	}
	else
	{
		Eigen::Affine3f translate = Eigen::Affine3f::Identity();
		translate.translation() << -runparams.ROI_CenterX, -runparams.ROI_CenterY, -runparams.ROI_CenterZ;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_translation(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::transformPointCloud(*cloud, *cloud_translation, translate);
		Eigen::Vector3f t(0, 0, 0);
		Eigen::Matrix3f rotationMatrix;
		Eigen::Matrix3f rotationMatrix_x;
		Eigen::Matrix3f rotationMatrix_y;
		Eigen::Matrix3f rotationMatrix_z;
		double value_rz = (3.1415926 / 180)*runparams.ROI_RZ;
		double value_ry = (3.1415926 / 180)*runparams.ROI_RY;
		double value_rx = (3.1415926 / 180)*runparams.ROI_RX;
		rotationMatrix_x << 1, 0, 0,
			0, cos(value_rx), -sin(value_rx),
			0, sin(value_rx), cos(value_rx);
		rotationMatrix_y << cos(value_ry), 0, sin(value_ry),
			0, 1, 0,
			-sin(value_ry), 0, cos(value_ry);
		rotationMatrix_z << cos(value_rz), -sin(value_rz), 0,
			sin(value_rz), cos(value_rz), 0,
			0, 0, 1;
		rotationMatrix = rotationMatrix_z * rotationMatrix_y * rotationMatrix_x;
		Eigen::Matrix4f transformMatrixInverse;
		transformMatrixInverse.setIdentity();
		transformMatrixInverse.block<3, 3>(0, 0) = rotationMatrix;
		transformMatrixInverse.topRightCorner(3, 1) = t;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_trans(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::transformPointCloud(*cloud_translation, *cloud_trans, transformMatrixInverse);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cropped(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::CropBox<pcl::PointXYZ> cropbox;
		cropbox.setInputCloud(cloud_trans);
		cropbox.setMin(Eigen::Vector4f(-runparams.ROI_LengthX / 2,
			-runparams.ROI_LengthY / 2, -runparams.ROI_LengthZ / 2, 1)); //设置最小点
		cropbox.setMax(Eigen::Vector4f(runparams.ROI_LengthX / 2,
			runparams.ROI_LengthY / 2, runparams.ROI_LengthZ / 2, 1));//设置最大点
		cropbox.filter(*cloud_cropped);
		pcl::invert3x3Matrix(rotationMatrix, inverseRotationMatrix);
		Eigen::Matrix4f transformMatrixInverse1;
		transformMatrixInverse1.setIdentity();
		transformMatrixInverse1.block<3, 3>(0, 0) = inverseRotationMatrix;
		transformMatrixInverse1.topRightCorner(3, 1) = -t;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cropped_inverse_trans(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::transformPointCloud(*cloud_cropped, *cloud_cropped_inverse_trans, transformMatrixInverse1);
		Eigen::Affine3f translate_crop = Eigen::Affine3f::Identity();
		translate_crop.translation() << runparams.ROI_CenterX, runparams.ROI_CenterY, runparams.ROI_CenterZ;
		pcl::transformPointCloud(*cloud_cropped_inverse_trans, *cloud_cropped_inverse, translate_crop);
	}
}

//多点最小二乘法拟合平面
void Pin3DMeasure::FitPlaneLeastSquares(std::vector<cv::Point3f> ptsvec, Eigen::Vector4f &coefficient)
{
	int n = ptsvec.size();
	cv::Mat input_pts(n, 3, CV_32FC1);
	for (int i = 0; i < n; i++)
	{
		input_pts.at<float>(i, 0) = ptsvec[i].x;
		input_pts.at<float>(i, 1) = ptsvec[i].y;
		input_pts.at<float>(i, 2) = ptsvec[i].z;
	}
	double A11 = 0, A12 = 0, A13 = 0;
	double A21 = 0, A22 = 0, A23 = 0;
	double A31 = 0, A32 = 0, A33 = 0;
	double B1 = 0, B2 = 0, B3 = 0;
	for (int i = 0; i < n; i++)
	{
		A11 += double(input_pts.at<float>(i, 0)) * input_pts.at<float>(i, 0);
		A12 += double(input_pts.at<float>(i, 0)) * input_pts.at<float>(i, 1);
		A13 += input_pts.at<float>(i, 0);
		A21 += double(input_pts.at<float>(i, 0)) * input_pts.at<float>(i, 1);
		A22 += double(input_pts.at<float>(i, 1)) * input_pts.at<float>(i, 1);
		A23 += input_pts.at<float>(i, 1);
		A31 += input_pts.at<float>(i, 0);
		A32 += input_pts.at<float>(i, 1);
		B1 += double(input_pts.at<float>(i, 0)) * input_pts.at<float>(i, 2);
		B2 += double(input_pts.at<float>(i, 1)) * input_pts.at<float>(i, 2);
		B3 += input_pts.at<float>(i, 2);
	}
	A33 = n;
	cv::Mat A(3, 3, CV_32FC1);
	A.at<float>(0, 0) = A11; A.at<float>(0, 1) = A12; A.at<float>(0, 2) = A13;
	A.at<float>(1, 0) = A21; A.at<float>(1, 1) = A22; A.at<float>(1, 2) = A23;
	A.at<float>(2, 0) = A31; A.at<float>(2, 1) = A32; A.at<float>(2, 2) = A33;
	cv::Mat B(3, 1, CV_32FC1);
	B.at<float>(0, 0) = B1; B.at<float>(1, 0) = B2; B.at<float>(2, 0) = B3;
	cv::Mat X;
	X = A.inv() * B;//X为3*1解向量，分别对应平面方程z=ax+by+c中的abc
	cv::Vec4f v;
	v[0] = X.at<float>(0, 0);
	v[1] = X.at<float>(1, 0);
	v[2] = X.at<float>(2, 0);
	double magSq = double(v[0]) * v[0] + double(v[1]) * v[1] + 1;
	if (magSq > 0.0f)
	{
		double oneOverMag = 1.0f / sqrt(magSq);
		v[0] *= oneOverMag;
		v[1] *= oneOverMag;
		v[2] *= oneOverMag;
	}
	coefficient[0] = v[0];
	coefficient[1] = v[1];
	coefficient[2] = -1 / sqrt(magSq);
	coefficient[3] = v[2];
}

void Pin3DMeasure::CalMinCircumscribedCubeCenterPose(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
	Eigen::Vector3f &mass_center)
{
	pcl::MomentOfInertiaEstimation <pcl::PointXYZ> feature_extractor;
	feature_extractor.setInputCloud(cloud);
	feature_extractor.compute();
	std::vector <float> moment_of_inertia;
	std::vector <float> eccentricity;
	pcl::PointXYZ min_point_OBB;
	pcl::PointXYZ max_point_OBB;
	pcl::PointXYZ position_OBB;
	Eigen::Matrix3f rotational_matrix_OBB;
	Eigen::Vector3f major_vector, middle_vector, minor_vector;
	feature_extractor.getMomentOfInertia(moment_of_inertia);
	feature_extractor.getEccentricity(eccentricity);
	feature_extractor.getOBB(min_point_OBB, max_point_OBB, position_OBB, rotational_matrix_OBB);
	feature_extractor.getEigenVectors(major_vector, middle_vector, minor_vector);
	feature_extractor.getMassCenter(mass_center);
}


//取区域算最小包围盒中心坐标
void Pin3DMeasure::CalRegionPose(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_region, RunParamsStruct_Pin3DMeasure &runparams,
	Eigen::Vector3f &mass_center)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_region_ori(new pcl::PointCloud<pcl::PointXYZ>);

	Eigen::Matrix3f inverseRotationMatrix = Eigen::Matrix3f::Identity();

	Create3DRoi(cloud, cloud_region_ori, runparams, inverseRotationMatrix);

	pcl::SampleConsensusModelPlane<pcl::PointXYZ>::Ptr model_plane(new pcl::SampleConsensusModelPlane<pcl::PointXYZ>(cloud_region_ori));
	pcl::RandomSampleConsensus<pcl::PointXYZ> ransac(model_plane);
	ransac.setDistanceThreshold(0.005);	//设置距离阈值，与平面距离小于0.005的点作为内点  
	ransac.computeModel();				//执行模型估计
	std::vector<int> inliers;				//存储内点索引的容器
	ransac.getInliers(inliers);			//提取内点索引
	pcl::copyPointCloud<pcl::PointXYZ>(*cloud_region_ori, inliers, *cloud_region);
	CalMinCircumscribedCubeCenterPose(cloud_region, mass_center);
}


void Pin3DMeasure::ClusterCalDis(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, RunParamsStruct_Pin3DMeasure &runparams,
	Eigen::VectorXf BasePlaneCoefficient, std::vector<cv::Point3d> &PinCenter,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_show)
{
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud);
	std::vector<pcl::PointIndices> cluster_indices;
	pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
	ec.setClusterTolerance(runparams.EuclideanClusterTolerance);
	ec.setMinClusterSize(int(runparams.EuclideanClusterMinPoints));
	ec.setMaxClusterSize(int(runparams.EuclideanClusterMaxPoints));
	ec.setSearchMethod(tree);
	ec.setInputCloud(cloud);
	ec.extract(cluster_indices);

	//boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer);
	//viewer->setBackgroundColor(0, 0, 0);

	int i = 0;
	for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin(); it != cluster_indices.end(); ++it)
	{
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster(new pcl::PointCloud<pcl::PointXYZ>);
		for (std::vector<int>::const_iterator pit = it->indices.begin(); pit != it->indices.end(); pit++)
			cloud_cluster->points.push_back(cloud->points[*pit]);
		cloud_cluster->width = cloud_cluster->points.size();
		cloud_cluster->height = 1;

		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster_gaussian(new pcl::PointCloud<pcl::PointXYZ>);
		GaussianFilter(cloud_cluster, cloud_cluster_gaussian);

		pcl::PointXYZ min;
		pcl::PointXYZ max;
		pcl::getMinMax3D(*cloud_cluster_gaussian, min, max);
		pcl::PointIndices::Ptr indices(new pcl::PointIndices);
		for (size_t i = 0; i < cloud_cluster_gaussian->points.size(); ++i)
		{
			if (cloud_cluster_gaussian->points[i].z >= (max.z - 0.15) && cloud_cluster_gaussian->points[i].z <= max.z)
			{
				indices->indices.push_back(i);
			}
		}
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster_cut(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::ExtractIndices<pcl::PointXYZ> extract;
		extract.setInputCloud(cloud_cluster_gaussian);
		extract.setIndices(indices);
		extract.setNegative(false);
		extract.filter(*cloud_cluster_cut);

		Eigen::Vector3f mass_center;
		CalMinCircumscribedCubeCenterPose(cloud_cluster_cut, mass_center);

		std::vector<double> height_value;
		for (size_t i = 0; i < cloud_cluster_cut->points.size(); ++i)
		{
			height_value.push_back(cloud_cluster_cut->points[i].z);
		}
		double sum = std::accumulate(height_value.begin(), height_value.end(), 0.0);
		double mean = sum / double(height_value.size());
		mass_center[2] = mean;

		double distance3DPointToPlane;
		CalDistance3DPointToPlane(BasePlaneCoefficient, mass_center, distance3DPointToPlane);
		PinCenter.push_back(cv::Point3d(mass_center[0], mass_center[1], distance3DPointToPlane));

		//std::string s = std::to_string(i);
		//pcl::visualization::PointCloudColorHandlerRandom<pcl::PointXYZ> rgb(cloud_cluster_mls);
		//viewer->addPointCloud<pcl::PointXYZ>(cloud_cluster_mls, rgb, s);

		i++;
		*cloud_show += *cloud_cluster_cut;
	}
	//viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
	//viewer->addCoordinateSystem(5.0);
	//viewer->initCameraParameters();
	//viewer->setShowFPS(false);
	//while (!viewer->wasStopped()) {
	//	viewer->spinOnce(100);
	//	boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	//}
}


/*
void PinMeasure::ClusterCalDis(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, RunParamsStruct_PinMeasure &runparams,
	Eigen::VectorXf BasePlaneCoefficient, std::vector<cv::Point3d> &PinCenter,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_show)
{
	int KN_normal = 10;
	float SmoothnessThreshold = 20;
	float CurvatureThreshold = 0.1;
	//法向估计
	pcl::search::Search<pcl::PointXYZ>::Ptr tree = boost::shared_ptr<pcl::search::Search<pcl::PointXYZ> >(new pcl::search::KdTree<pcl::PointXYZ>);
	pcl::PointCloud <pcl::Normal>::Ptr normals(new pcl::PointCloud <pcl::Normal>);
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> normal_estimator;
	normal_estimator.setSearchMethod(tree);
	normal_estimator.setInputCloud(cloud);
	normal_estimator.setKSearch(KN_normal);
	normal_estimator.compute(*normals);

	pcl::RegionGrowing<pcl::PointXYZ, pcl::Normal> reg;
	reg.setMinClusterSize(runparams.RegionGrowMinPoints);
	reg.setMaxClusterSize(runparams.RegionGrowMaxPoints);
	reg.setSearchMethod(tree);
	reg.setNumberOfNeighbours(runparams.RegionGrowNumNeighbours);
	reg.setInputCloud(cloud);
	reg.setInputNormals(normals);
	reg.setSmoothnessThreshold(SmoothnessThreshold / 180.0 * M_PI);
	reg.setCurvatureThreshold(CurvatureThreshold);
	std::vector <pcl::PointIndices> clusters;
	reg.extract(clusters);

	int i = 0;
	for (std::vector<pcl::PointIndices>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
	{
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster(new pcl::PointCloud<pcl::PointXYZ>);
		for (std::vector<int>::const_iterator pit = it->indices.begin(); pit != it->indices.end(); pit++)
			cloud_cluster->points.push_back(cloud->points[*pit]);
		cloud_cluster->width = cloud_cluster->points.size();
		cloud_cluster->height = 1;
		pcl::PointXYZ min;
		pcl::PointXYZ max;
		pcl::getMinMax3D(*cloud_cluster, min, max);
		Eigen::Vector3f mass_center;
		CalMinCircumscribedCubeCenterPose(cloud_cluster, mass_center);
		mass_center[2] = max.z;
		double distance3DPointToPlane;
		CalDistance3DPointToPlane(BasePlaneCoefficient, mass_center, distance3DPointToPlane);
		PinCenter.push_back(cv::Point3d(mass_center[0], mass_center[1], distance3DPointToPlane));
		i++;
		*cloud_show += *cloud_cluster;
	}
}
*/

void Pin3DMeasure::CalDistance3DPointToPlane(Eigen::VectorXf FitPlaneRANSAC_coefficient,
	Eigen::VectorXf MinCircumscribedCubeCenterPose, double &distance3DPointToPlane)
{
	distance3DPointToPlane =
		abs(FitPlaneRANSAC_coefficient[0] * MinCircumscribedCubeCenterPose[0]
			+ FitPlaneRANSAC_coefficient[1] * MinCircumscribedCubeCenterPose[1]
			+ FitPlaneRANSAC_coefficient[2] * MinCircumscribedCubeCenterPose[2]
			+ FitPlaneRANSAC_coefficient[3])
		/ sqrt(FitPlaneRANSAC_coefficient[0] * FitPlaneRANSAC_coefficient[0]
			+ FitPlaneRANSAC_coefficient[1] * FitPlaneRANSAC_coefficient[1]
			+ FitPlaneRANSAC_coefficient[2] * FitPlaneRANSAC_coefficient[2]);
}

//对点的坐标按x从小到大排序，若x值相同则按y从小到大排序
bool Pin3DMeasure::AscCompareX(cv::Point3d pt1, cv::Point3d pt2)
{
	return pt1.x < pt2.x;
}

//对点的坐标按y从小到大排序，若y值相同则按x从小到大排序
bool Pin3DMeasure::AscCompareY(cv::Point3d pt1, cv::Point3d pt2)
{
	return pt1.y < pt2.y;
}

//对点的坐标按x从大到小排序
bool Pin3DMeasure::DescCompareX(cv::Point3d pt1, cv::Point3d pt2)
{
	return pt1.x > pt2.x;
}

//对点的坐标按y从大到小排序
bool Pin3DMeasure::DescCompareY(cv::Point3d pt1, cv::Point3d pt2)
{
	return pt1.y > pt2.y;
}

//离群点滤波
void Pin3DMeasure::StatisticalOutlierRemoval(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered, RunParamsStruct_Pin3DMeasure &runparams)
{
	pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;      //创建滤波器对象
	sor.setInputCloud(cloud);                               //设置待滤波的点云
	sor.setMeanK(runparams.SORNeighborPoints);              //设置查询点临近点数
	sor.setStddevMulThresh(runparams.SOROutlierThreshold);  //设置离群点的阀值
	sor.filter(*cloud_filtered);
}

//ICP精匹配
void Pin3DMeasure::MatchingICP(pcl::PointCloud<pcl::PointXYZ>::Ptr model_cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_trans,
	RunParamsStruct_Pin3DMeasure &runparams, Eigen::Matrix4f &transformMatrixInverse)
{
	pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
	icp.setInputSource(model_cloud);
	icp.setInputTarget(cloud);
	icp.setMaximumIterations(runparams.MatchIterations);

	pcl::PointCloud<pcl::PointXYZ>::Ptr Final(new pcl::PointCloud<pcl::PointXYZ>);
	icp.align(*Final);
	Eigen::Matrix4f transformation = icp.getFinalTransformation();
	Eigen::Vector3f t(0, 0, 0);
	t = transformation.topRightCorner(3, 1);
	t = -t;
	Eigen::Matrix3f rotationMatrix;
	rotationMatrix = transformation.block<3, 3>(0, 0);
	Eigen::Matrix3f inverseRotationMatrix;
	pcl::invert3x3Matrix(rotationMatrix, inverseRotationMatrix);
	transformMatrixInverse.setIdentity();
	transformMatrixInverse.block<3, 3>(0, 0) = inverseRotationMatrix;
	transformMatrixInverse.topRightCorner(3, 1) = t;
	pcl::transformPointCloud(*cloud, *cloud_trans, transformMatrixInverse);
}

//点云体素降采样，基于体素网格
int Pin3DMeasure::CloudVoxelDownsampling(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud,
	RunParamsStruct_Pin3DMeasure &runparams,
	pcl::PointCloud<pcl::PointXYZ>::Ptr CloudSampling)
{
	pcl::VoxelGrid<pcl::PointXYZ> VoxelDownSampling;
	VoxelDownSampling.setInputCloud(cloud);
	VoxelDownSampling.setLeafSize(runparams.DownSamplingVoxel,
		runparams.DownSamplingVoxel, runparams.DownSamplingVoxel);
	VoxelDownSampling.filter(*CloudSampling);
	return 0;
}


int Pin3DMeasure::CropModuleCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud,
	RunParamsStruct_Pin3DMeasure runparams)
{
	Eigen::Matrix3f inverseRotationMatrix = Eigen::Matrix3f::Identity();
	Create3DRoi(ori_cloud, module_cloud, runparams, inverseRotationMatrix);
	return 0;
}


int Pin3DMeasure::MatchTransCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud,
	RunParamsStruct_Pin3DMeasure &runparams, 
	pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud_trans)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud_d(new pcl::PointCloud<pcl::PointXYZ>);
	CloudVoxelDownsampling(module_cloud, runparams, module_cloud_d);

	//加载模板点云并降采样
	pcl::PointCloud<pcl::PointXYZ>::Ptr model_cloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr model_cloud_d(new pcl::PointCloud<pcl::PointXYZ>);
	if (pcl::io::loadPLYFile<pcl::PointXYZ>(runparams.ModelName, *model_cloud) == -1)
	{
		WriteTxt("模板点云加载失败");
	}
	CloudVoxelDownsampling(model_cloud, runparams, model_cloud_d);

	//ICP匹配
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_crop_moduleROI_d_trans(new pcl::PointCloud<pcl::PointXYZ>);
	Eigen::Matrix4f transformMatrixInverse;
	MatchingICP(model_cloud_d, module_cloud_d, cloud_crop_moduleROI_d_trans,
		runparams, transformMatrixInverse);
	pcl::transformPointCloud(*module_cloud, *module_cloud_trans, transformMatrixInverse);

	if (runparams.PopWindowShowMatchCloud == 1)
	{
		MultiCloudShowPopWindow(module_cloud_trans, model_cloud);
	}
	return 0;
}

int Pin3DMeasure::FourPointFitPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_crop_module,
	RunParamsStruct_Pin3DMeasure &runparams, Eigen::Vector4f &BasePlaneCoefficient)
{
	//按取四个点来拟合基准平面
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_point_region(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_point_region_show(new pcl::PointCloud<pcl::PointXYZ>);

	std::vector<cv::Point3f> BasePoints;

	for (int i = 0; i < 4; i++)
	{
		Eigen::Vector3f mass_center;
		runparams.ROI_CenterX = runparams.BasePlanePoints[i][0];
		runparams.ROI_CenterY = runparams.BasePlanePoints[i][1];
		runparams.ROI_CenterZ = runparams.BasePlanePoints[i][2];
		runparams.ROI_LengthX = 1;
		runparams.ROI_LengthY = 1;
		runparams.ROI_LengthZ = 1;
		runparams.ROI_RX = 0;
		runparams.ROI_RY = 0;
		runparams.ROI_RZ = 0;
		CalRegionPose(cloud_crop_module, cloud_point_region, runparams, mass_center);
		BasePoints.push_back(cv::Point3f(mass_center[0], mass_center[1], mass_center[2]));
		*cloud_point_region_show += *cloud_point_region;
	}
	if (runparams.PopWindowShowBasePointCloud == 1)
	{
		MultiCloudShowPopWindow(cloud_crop_module, cloud_point_region_show);
	}
	//最小二乘法拟合平面
	FitPlaneLeastSquares(BasePoints, BasePlaneCoefficient);
	return 0;
}


int Pin3DMeasure::CropPinCalCoord(pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr pin_cloud,
	RunParamsStruct_Pin3DMeasure &runparams, Eigen::Vector4f BasePlaneCoefficient,
	ResultParamsStruct_Pin3DMeasure &resultparams, int OrderNum)
{
	runparams.ROI_CenterX = runparams.PinROI_CenterX_Vector[OrderNum];
	runparams.ROI_CenterY = runparams.PinROI_CenterY_Vector[OrderNum];
	runparams.ROI_CenterZ = runparams.PinROI_CenterZ_Vector[OrderNum];
	runparams.ROI_LengthX = runparams.PinROI_LengthX_Vector[OrderNum];
	runparams.ROI_LengthY = runparams.PinROI_LengthY_Vector[OrderNum];
	runparams.ROI_LengthZ = runparams.PinROI_LengthZ_Vector[OrderNum];
	runparams.ROI_RX = runparams.PinROI_RX_Vector[OrderNum];
	runparams.ROI_RY = runparams.PinROI_RY_Vector[OrderNum];
	runparams.ROI_RZ = runparams.PinROI_RZ_Vector[OrderNum];
	runparams.DownSamplingVoxel = runparams.PinDownSamplingVoxel_Vector[OrderNum];
	runparams.SORNeighborPoints = runparams.SORNeighborPoints_Vector[OrderNum];
	runparams.SOROutlierThreshold = runparams.SOROutlierThreshold_Vector[OrderNum];
	runparams.EuclideanClusterTolerance = runparams.EuclideanClusterTolerance_Vector[OrderNum];
	runparams.EuclideanClusterMinPoints = runparams.EuclideanClusterMinPoints_Vector[OrderNum];
	runparams.EuclideanClusterMaxPoints = runparams.EuclideanClusterMaxPoints_Vector[OrderNum];
	runparams.PinStandard = runparams.PinStandard_Vector[OrderNum];
	runparams.PinUpperTol = runparams.PinUpperTol_Vector[OrderNum];
	runparams.PinLowerTol = runparams.PinLowerTol_Vector[OrderNum];
	runparams.PinCorrect = runparams.PinCorrect_Vector[OrderNum];
	runparams.PinNum = runparams.PinNum_Vector[OrderNum];
	runparams.SortOrder = runparams.SortOrder_Vector[OrderNum];
	runparams.HeadAddZeroNum = runparams.HeadAddZeroNum_Vector[OrderNum];
	runparams.TailAddZeroNum = runparams.TailAddZeroNum_Vector[OrderNum];

	//截取PIN针的ROI区域
	Eigen::Matrix3f inverseRotationMatrix = Eigen::Matrix3f::Identity();

	Create3DRoi(module_cloud, pin_cloud, runparams, inverseRotationMatrix);

	if (runparams.PopWindowShowCropPinCloud == 1)
	{
		CloudShowPopWindow(pin_cloud);
	}

	pcl::PointCloud<pcl::PointXYZ>::Ptr pin_cloud_SOR(new pcl::PointCloud<pcl::PointXYZ>);
	StatisticalOutlierRemoval(pin_cloud, pin_cloud_SOR, runparams);
	if (runparams.PopWindowShowPinSORCloud == 1)
	{
		CloudShowPopWindow(pin_cloud_SOR);
	}

	pcl::PointCloud<pcl::PointXYZ>::Ptr pin_cloud_SOR_d(new pcl::PointCloud<pcl::PointXYZ>);
	CloudVoxelDownsampling(pin_cloud_SOR, runparams, pin_cloud_SOR_d);
	if (runparams.PopWindowShowPinDownCloud == 1)
	{
		CloudShowPopWindow(pin_cloud_SOR_d);
	}
	
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPinShow(new pcl::PointCloud<pcl::PointXYZ>);
	std::vector<cv::Point3d> PinCenter;
	ClusterCalDis(pin_cloud_SOR_d,
		runparams, BasePlaneCoefficient, PinCenter, cloudPinShow);
	if (runparams.PopWindowShowPinClusterCloud == 1)
	{
		CloudShowPopWindow(cloudPinShow);
	}

	//针数量判断
	if (PinCenter.size() != runparams.PinNum)
	{
		for (int i = 0; i < runparams.PinNum; i++) {
			resultparams.PinHeight.push_back(-2);
			resultparams.PinFlag.push_back(bool(false));//返回结果为-2，代表数量不匹配，少针或者多针
		}
		return 0;
	}
	//排序
	if (runparams.SortOrder == "沿X升序")
	{
		std::sort(PinCenter.begin(), PinCenter.end(), AscCompareX);
	}
	else if (runparams.SortOrder == "沿X降序")
	{
		std::sort(PinCenter.begin(), PinCenter.end(), DescCompareX);
	}
	else if (runparams.SortOrder == "沿Y升序")
	{
		std::sort(PinCenter.begin(), PinCenter.end(), AscCompareY);
	}
	else
	{
		std::sort(PinCenter.begin(), PinCenter.end(), DescCompareY);
	}

	for (int i = 0; i < runparams.PinNum; i++)
	{
		resultparams.PinHeight.push_back(PinCenter[i].z + runparams.PinCorrect);
		if (PinCenter[i].z + runparams.PinCorrect >= (runparams.PinStandard + runparams.PinUpperTol)
			|| PinCenter[i].z + runparams.PinCorrect <= (runparams.PinStandard + runparams.PinLowerTol))
		{
			resultparams.PinFlag.push_back(bool(false));
		}
		else
		{
			resultparams.PinFlag.push_back(bool(true));
		}
	}
	for (int i = 0; i < runparams.HeadAddZeroNum; i++)
	{
		//resultparams.PinHeight.insert(resultparams.PinHeight.begin(), 0);
		//resultparams.PinFlag.insert(resultparams.PinFlag.begin(), bool(false));

		resultparams.PinHeight.insert(resultparams.PinHeight.end() - runparams.PinNum, 0);
		resultparams.PinFlag.insert(resultparams.PinFlag.end() - runparams.PinNum, bool(false));
	}
	for (int i = 0; i < runparams.TailAddZeroNum; i++)
	{
		resultparams.PinHeight.push_back(0);
		resultparams.PinFlag.push_back(bool(false));
	}
	return 0;
}

int Pin3DMeasure::PinMeasureAlgo(pcl::PointCloud<pcl::PointXYZ>::Ptr ori_cloud, 
	RunParamsStruct_Pin3DMeasure &runparams, ResultParamsStruct_Pin3DMeasure &resultparams)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud(new pcl::PointCloud<pcl::PointXYZ>);
	CropModuleCloud(ori_cloud, module_cloud, runparams);
	pcl::PointCloud<pcl::PointXYZ>::Ptr module_cloud_trans(new pcl::PointCloud<pcl::PointXYZ>);
	if (runparams.LocateMethod == "ICP匹配定位")
	{
		runparams.PopWindowShowMatchCloud = 0;
		MatchTransCloud(module_cloud, runparams, module_cloud_trans);
	}
	else
	{
		module_cloud_trans = module_cloud;
	}
	Eigen::Vector4f BasePlaneCoefficient;
	runparams.PopWindowShowBasePointCloud = 0;
	FourPointFitPlane(module_cloud_trans, runparams, BasePlaneCoefficient);

	runparams.PopWindowShowCropPinCloud = 0;	
	runparams.PopWindowShowPinDownCloud = 0;	
	runparams.PopWindowShowPinSORCloud = 0;	
	runparams.PopWindowShowPinClusterCloud = 0;

	for (int i = 0; i < runparams.PinParamsNum; i++)
	{
		pcl::PointCloud<pcl::PointXYZ>::Ptr pin_cloud(new pcl::PointCloud<pcl::PointXYZ>);
		CropPinCalCoord(module_cloud_trans, pin_cloud, runparams, BasePlaneCoefficient, resultparams, i);
	}

	//for (int i = 0; i < resultparams.PinFlag.size(); i++)
	//{
	//	//输出结果
	//	if (resultparams.PinFlag[i] == 0)
	//	{
	//		resultparams.isOK = false;
	//		break;
	//	}
	//}

	return 0;
}


int Pin3DMeasure::WriteTxt(std::string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char *p = content.c_str();
	std::ofstream in;
	in.open("D:\\VisionTools\\x64\\Release\\Log\\AriLog.txt", std::ios::app); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
	in << p << "\r";
	in.close();//关闭文件
	return 0;
}