#pragma once
#pragma execution_character_set("utf-8")
#include <string>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include "XmlClass2.h"
//#include <data.h>
#include <QLog.h>
#include <functional>
#include <opencv2/opencv.hpp>
#include <pcl/common/eigen.h>

using namespace HalconCpp;

struct portNames {
	std::string Image = "Image";


	std::string FilePath_Csv = "FilePath_Csv";
	std::string ID_Csv = "ID_Csv";


	std::string maxThreshold = "maxThreshold";
	std::string Sigm = "Sigm";
	std::string Tolerance_X = "Tolerance_X";
	std::string Tolerance_Y = "Tolerance_Y";
	std::string MmPixel = "MmPixel";
	std::string StandardX_Array = "StandardX_Array";
	std::string StandardY_Array = "StandardY_Array";

	std::string X_BindingNum = "X_BindingNum";
	std::string Y_BindingNum = "Y_BindingNum";

	std::string BaseType = "BaseType";
	std::string Row1_Base = "Row1_Base";
	std::string Column1_Base = "Column1_Base";
	std::string Row2_Base = "Row2_Base";
	std::string Column2_Base = "Column2_Base";
	std::string Radius_Base = "Radius_Base";

	std::string MeasureLength1 = "MeasureLength1";
	std::string MeasureLength2 = "MeasureLength2";
	std::string MeasureSigma = "MeasureSigma";
	std::string MeasureThreshold = "MeasureThreshold";
	std::string MeasureTransition = "MeasureTransition";
	std::string MeasureSelect = "MeasureSelect";
	std::string MeasureNum = "MeasureNum";
	std::string MeasureMinScore = "MeasureMinScore";
	std::string DistanceThreshold = "DistanceThreshold";

	std::string DetectType = "DetectType";
	std::string SortType = "SortType";
	std::string SearchRegion = "SearchRegion";
	std::string ThresholdType = "ThresholdType";
	std::string LowThreshold = "LowThreshold";
	std::string HighThreshold = "HighThreshold";
	std::string Sigma = "Sigma";
	std::string CoreWidth = "CoreWidth";
	std::string CoreHeight = "CoreHeight";
	std::string DynThresholdContrast = "DynThresholdContrast";
	std::string DynThresholdPolarity = "DynThresholdPolarity";

	std::string AreaFilter_Min = "AreaFilter_Min";
	std::string RecLen1Filter_Min = "RecLen1Filter_Min";
	std::string RecLen2Filter_Min = "RecLen2Filter_Min";
	std::string RowFilter_Min = "RowFilter_Min";
	std::string ColumnFilter_Min = "ColumnFilter_Min";
	std::string CircularityFilter_Min = "CircularityFilter_Min";
	std::string RectangularityFilter_Min = "RectangularityFilter_Min";
	std::string WidthFilter_Min = "WidthFilter_Min";
	std::string HeightFilter_Min = "HeightFilter_Min";
	std::string AreaFilter_Max = "AreaFilter_Max";
	std::string RecLen1Filter_Max = "RecLen1Filter_Max";
	std::string RecLen2Filter_Max = "RecLen2Filter_Max";
	std::string RowFilter_Max = "RowFilter_Max";
	std::string ColumnFilter_Max = "ColumnFilter_Max";
	std::string CircularityFilter_Max = "CircularityFilter_Max";
	std::string RectangularityFilter_Max = "RectangularityFilter_Max";
	std::string WidthFilter_Max = "WidthFilter_Max";
	std::string HeightFilter_Max = "HeightFilter_Max";
	std::string SelectAreaMax = "SelectAreaMax";

	std::string OperatorType = "OperatorType";
	std::string OperaRec_Width = "OperaRec_Width";
	std::string OperaRec_Height = "OperaRec_Height";
	std::string OperaCir_Radius = "OperaCir_Radius";

	std::string FillUpShape_Min = "FillUpShape_Min";
	std::string FillUpShape_Max = "FillUpShape_Max";

	std::string MatchMethod = "MatchMethod";
	std::string AngleStart = "AngleStart";
	std::string AngleExtent = "AngleExtent";
	std::string ScaleRMin = "ScaleRMin";
	std::string ScaleRMax = "ScaleRMax";
	std::string ScaleCMin = "ScaleCMin";
	std::string ScaleCMax = "ScaleCMax";
	std::string MinScore = "MinScore";
	std::string NumMatches = "NumMatches";
	std::string TrainRegion = "TrainRegion";
	std::string ModelID = "ModelID";

	std::string SearchRegion_PositionCorrection = "SearchRegion_PositionCorrection";
	std::string TrainRegion_PositionCorrection = "TrainRegion_PositionCorrection";
	std::string Row_PositionCorrection = "Row_PositionCorrection";
	std::string Column_PositionCorrection = "Column_PositionCorrection";
	std::string Angle_PositionCorrection = "Angle_PositionCorrection";
	std::string MatchMethod_PositionCorrection = "MatchMethod_PositionCorrection";
	std::string AngleStart_PositionCorrection = "AngleStart_PositionCorrection";
	std::string AngleExtent_PositionCorrection = "AngleExtent_PositionCorrection";
	std::string ScaleRMin_PositionCorrection = "ScaleRMin_PositionCorrection";
	std::string ScaleRMax_PositionCorrection = "ScaleRMax_PositionCorrection";
	std::string ScaleCMin_PositionCorrection = "ScaleCMin_PositionCorrection";
	std::string ScaleCMax_PositionCorrection = "ScaleCMax_PositionCorrection";
	std::string MinScore_PositionCorrection = "MinScore_PositionCorrection";
	std::string NumMatches_PositionCorrection = "NumMatches_PositionCorrection";
	std::string ModelID_PositionCorrection = "ModelID_PositionCorrection";
	std::string Check_PositionCorrection = "Check_PositionCorrection";
	std::string SortType_PositionCorrection = "SortType_PositionCorrection";

	std::string Ret = "Ret";
	std::string RetNum = "RetNum";
	std::string Contour_Base = "Contour_Base";
	std::string ModelXLD = "ModelXLD";

	std::string Row1_BaseLine_Ret = "Row1_BaseLine_Ret";
	std::string Column1_BaseLine_Ret = "Column1_BaseLine_Ret";
	std::string Row2_BaseLine_Ret = "Row2_BaseLine_Ret";
	std::string Column2_BaseLine_Ret = "Column2_BaseLine_Ret";

	std::string Row_BaseCircle_Ret = "Row_BaseCircle_Ret";
	std::string Column_BaseCircle_Ret = "Column_BaseCircle_Ret";
	std::string Radius_BaseCircle_Ret = "Radius_BaseCircle_Ret";

	std::string Region_AllPin = "Region_AllPin";
	std::string Region_OKPin = "Region_OKPin";
	std::string Region_NGPin = "Region_NGPin";
	std::string Row_Array = "Row_Array";
	std::string Column_Array = "Column_Array";
	std::string DisX_Pixel_Array = "DisX_Pixel_Array";
	std::string DisY_Pixel_Array = "DisY_Pixel_Array";
	std::string OffsetX_Pixel_Array = "OffsetX_Pixel_Array";
	std::string OffsetY_Pixel_Array = "OffsetY_Pixel_Array";
	std::string DisX_MM_Array = "DisX_MM_Array";
	std::string DisY_MM_Array = "DisY_MM_Array";
	std::string OffsetX_MM_Array = "OffsetX_MM_Array";
	std::string OffsetY_MM_Array = "OffsetY_MM_Array";
	std::string To_Gray = "To_Gray";


	//----------------------------3D 部分--------------------------------
	std::string Image3D = "Image3D";
	std::string PinHeight = "PinHeight";
	std::string PinFlag = "PinFlag";
	std::string Result = "Result";
};

//Pin针检测类型参数结构体
struct TypeParamsStruct_Pin {
	//Pin检测类型参数
	HTuple DetectType_Blob = "Blob";	//blob
	HTuple DetectType_Template = "模板匹配";			//模版匹配
	HTuple BaseType_Line = "直线";		//直线
	HTuple BaseType_Circle = "圆";	//圆
	//pin区域排序类型
	HTuple SortType_Row = "按行排序";//按行排序
	HTuple SortType_Column = "按列排序";//按列排序
	//二值化类型参数
	HTuple Fixed_Threshold = "固定阈值";
	HTuple Auto_Threshold = "自适应阈值";
	HTuple Dyn_Threshold = "动态阈值";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";

	//形态学方式
	HTuple Opening_Rec = "矩形开运算";
	HTuple Closing_Rec = "矩形闭运算";
	HTuple Opening_Cir = "圆形开运算";
	HTuple Closing_Cir = "圆形闭运算";
	HTuple Erosion_Rec = "矩形腐蚀";
	HTuple Dilation_Rec = "矩形膨胀";
	HTuple Erosion_Cir = "圆形腐蚀";
	HTuple Dilation_Cir = "圆形膨胀";

	//模板匹配类型
	HTuple ShapeModel = "基于形状";
	HTuple NccModel = "基于互相关";

	HTuple hv_Null = "null";
};

//Pin针检测运行参数结构体
struct RunParamsStruct_Pin {
	//保存CSV路径
	string FilePath_Csv = "D:/PinData/";
	int ID_Csv = 0;
	//控制参数
	HTuple hv_Tolerance_X = 20;		//X方向允许的偏差范围
	HTuple hv_Tolerance_Y = 20;		//Y方向允许的偏差范围
	HTuple hv_MmPixel = 0.01;		//像素当量
	HTuple hv_StandardX_Array;	//X方向基准参数(数组)
	HTuple hv_StandardY_Array;	//Y方向基准参数(数组)

	//Pin区域绑定的基准编号
	HTuple hv_X_BindingNum;	//数组
	HTuple hv_Y_BindingNum;	//数组

	//基准参数
	HTuple hv_BaseType;				//数组，基准类型(默认是直线，，否则选择结构体RunParamsStruct_Pin里面参数)
	HTuple hv_Row1_Base;	//数组(直线起点row，或者圆心row)
	HTuple hv_Column1_Base;	//数组(直线起点column，或者圆心column)
	HTuple hv_Row2_Base;	//数组(直线终点row)
	HTuple hv_Column2_Base;	//数组(直线终点column)
	HTuple hv_Radius_Base;	//数组(圆半径)
	//找边找圆模型需要的算法参数
	HTuple hv_MeasureLength1;		//数组,卡尺半长,
	HTuple hv_MeasureLength2;		//数组,卡尺半宽
	HTuple hv_MeasureSigma;		//数组,平滑值 
	HTuple hv_MeasureThreshold;	//数组,边缘阈值
	HTuple hv_MeasureTransition;	//数组,卡尺极性
	HTuple hv_MeasureSelect;		//数组,边缘选择
	HTuple hv_MeasureNum;		    //数组,卡尺个数
	HTuple hv_MeasureMinScore;     //数组,最小得分
	HTuple hv_DistanceThreshold;   //数组,距离阈值

	//二值化参数
	HTuple hv_DetectType;			//数组，检测类型(默认"二值化"，否则选择结构体RunParamsStruct_Pin里面参数)
	HTuple hv_SortType;				//数组,pin区域排序方式
	HObject ho_SearchRegion;		//数组,搜索区域
	HTuple hv_ThresholdType;		//数组,图像二值化类型
	HTuple hv_LowThreshold;				//数组,低阈值(固定阈值)
	HTuple hv_HighThreshold;				//数组,高阈值(固定阈值)
	HTuple hv_Sigma;						//数组,平滑阈值
	HTuple hv_CoreWidth;					//数组,动态阈值(滤波核宽度)
	HTuple hv_CoreHeight;					//数组,动态阈值(滤波核高度)
	HTuple hv_DynThresholdContrast;		//数组,动态阈值分割图像对比度
	HTuple hv_DynThresholdPolarity;	//数组,动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

	//筛选Blob参数
	HTuple hv_BlobCount;//数组,Blob个数
	HTuple hv_AreaFilter_Min;	//数组
	HTuple hv_RecLen1Filter_Min;//数组
	HTuple hv_RecLen2Filter_Min;//数组
	HTuple hv_RowFilter_Min;//数组
	HTuple hv_ColumnFilter_Min;//数组
	HTuple hv_CircularityFilter_Min;//数组
	HTuple hv_RectangularityFilter_Min;//数组
	HTuple hv_WidthFilter_Min;//数组
	HTuple hv_HeightFilter_Min;//数组
	HTuple hv_AreaFilter_Max;//数组
	HTuple hv_RecLen1Filter_Max;//数组
	HTuple hv_RecLen2Filter_Max;//数组
	HTuple hv_RowFilter_Max;//数组
	HTuple hv_ColumnFilter_Max;//数组
	HTuple hv_CircularityFilter_Max;//数组
	HTuple hv_RectangularityFilter_Max;//数组
	HTuple hv_WidthFilter_Max;//数组
	HTuple hv_HeightFilter_Max;//数组
	HTuple hv_SelectAreaMax;	//选择区域内面积最大值的区域
	//形态学类型
	HTuple hv_OperatorType;//数组
	HTuple hv_OperaRec_Width;//数组
	HTuple hv_OperaRec_Height;//数组
	HTuple hv_OperaCir_Radius;//数组
	//孔洞填充面积
	HTuple hv_FillUpShape_Min;//数组
	HTuple hv_FillUpShape_Max;//数组

	//模板匹配参数
	HTuple hv_MatchMethod;      //模板类型:"基于形状"，"基于互相关	
	HTuple hv_AngleStart;            //起始角度
	HTuple hv_AngleExtent;                //角度范围
	HTuple hv_ScaleRMin;                 //最小行缩放
	HTuple hv_ScaleRMax;                 //最大行缩放
	HTuple hv_ScaleCMin;                 //最小列缩放
	HTuple hv_ScaleCMax;				   //最大列缩放
	HTuple hv_MinScore;               //最低分数
	HTuple hv_NumMatches;               //匹配个数，0则自动选择，100则最多匹配100个
	HObject ho_TrainRegion;				//训练区域
	HTuple hv_ModelID;                     //模板句柄

	//位置修正参数
	HObject ho_SearchRegion_PositionCorrection;	//搜索区域
	HObject ho_TrainRegion_PositionCorrection;	//训练区域
	HTuple hv_Row_PositionCorrection = 0;	//位置修正模板row
	HTuple hv_Column_PositionCorrection = 0;//位置修正模板column
	HTuple hv_Angle_PositionCorrection = 0;//位置修正模板angle
	HTuple hv_MatchMethod_PositionCorrection = "基于形状";      //模板类型:"基于形状"，"基于互相关	
	HTuple hv_AngleStart_PositionCorrection = -180;            //起始角度
	HTuple hv_AngleExtent_PositionCorrection = 180;                //终止角度
	HTuple hv_ScaleRMin_PositionCorrection = 1;                 //最小行缩放
	HTuple hv_ScaleRMax_PositionCorrection = 1;                 //最大行缩放
	HTuple hv_ScaleCMin_PositionCorrection = 1;                 //最小列缩放
	HTuple hv_ScaleCMax_PositionCorrection = 1;				   //最大列缩放
	HTuple hv_MinScore_PositionCorrection = 0.5;               //最低分数
	HTuple hv_NumMatches_PositionCorrection = 1;               //匹配个数，0则自动选择，100则最多匹配100个
	HTuple hv_ModelID_PositionCorrection = NULL;                     //模板句柄
	HTuple hv_Check_PositionCorrection = 0;	//是否启用位置修正(0不启用， 1启用)
	HTuple hv_SortType_PositionCorrection;	//排序方式

	HTuple hv_ToGray = 0;	//灰度处理
};

//Pin针检测结果结构体
struct ResultParamsStruct_Pin {
	HTuple hv_Ret = 1;	//1是OK,0是NG
	HTuple hv_RetNum = 0;		//Pin针数量
	HObject ho_Contour_Base;			     //基准轮廓
	HObject ho_ModelXLD;//位置修正的模板轮廓
	//基准边结果数据
	HTuple hv_Row1_BaseLine_Ret;
	HTuple hv_Column1_BaseLine_Ret;
	HTuple hv_Row2_BaseLine_Ret;
	HTuple hv_Column2_BaseLine_Ret;
	//基准圆结数据
	HTuple hv_Row_BaseCircle_Ret;
	HTuple hv_Column_BaseCircle_Ret;
	HTuple hv_Radius_BaseCircle_Ret;

	HObject ho_Region_AllPin;				 //所有pin针区域
	HObject ho_Region_OKPin;				//OK的pin针区域
	HObject ho_Region_NGPin;				//NG的pin针区域
	HTuple hv_Row_Array;					   //pin针区域中心行坐标
	HTuple hv_Column_Array;					   //pin针区域中心列坐标
	HTuple hv_DisX_Pixel_Array;                  //pin针区域中心相对基准的X方向距离,像素
	HTuple hv_DisY_Pixel_Array;				  //pin针区域中心相对基准的Y方向距离,像素
	HTuple hv_OffsetX_Pixel_Array;              //pin针区域中心相对基准的X方向偏差值,像素
	HTuple hv_OffsetY_Pixel_Array;			 //pin针区域中心相对基准的Y方向偏差值,像素
	HTuple hv_DisX_MM_Array;                  //pin针区域中心相对基准的X方向距离,毫米
	HTuple hv_DisY_MM_Array;				  //pin针区域中心相对基准的Y方向距离,毫米
	HTuple hv_OffsetX_MM_Array;              //pin针区域中心相对基准的X方向偏差值,毫米
	HTuple hv_OffsetY_MM_Array;			 //pin针区域中心相对基准的Y方向偏差值,毫米

	std::vector<int> ngIdx; //按顺序存ng的针的顺序索引
	HObject ho_Image; //识别原图

	void clear()
	{
		if (ho_Contour_Base.IsInitialized())
			ho_Contour_Base.Clear();
		if (ho_ModelXLD.IsInitialized())
			ho_ModelXLD.Clear();
		if(ho_Region_AllPin.IsInitialized())
			ho_Region_AllPin.Clear();
		if (ho_Region_OKPin.IsInitialized())
			ho_Region_OKPin.Clear();
		if (ho_Region_NGPin.IsInitialized())
			ho_Region_NGPin.Clear();
		if (ho_Image.IsInitialized())
			ho_Image.Clear();
	};

	~ResultParamsStruct_Pin()
	{
		clear();
	}
};

class DllDatas {
public:
	int WriteParams_PinDetect(const RunParamsStruct_Pin& runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	ErrorCode_Xml ReadParams_PinDetect(RunParamsStruct_Pin& runParams, const int& processId, const string& nodeName, const int& processModbuleID);

	//void setGetConfigPath(std::function<std::string()> getConfigPath) {
	//	this->getConfigPath = getConfigPath;
	//}

	void setGetConfigPath(std::string getConfigPath) {
		this->getConfigPath = getConfigPath;
	}

	void setConfigPath(std::string fileName) {
		this->configFileName = fileName;
	}

	std::string formatDobleValue(double val, int fixed);

	void PathSet(const std::string path, const std::string XmlPath);
	int WriteTxt(std::string content);
	void writeSearchRegion(HObject searchRegion, const int& processId, const string& nodeName, const int& processModbuleID);
	void readSearchRegion(HObject& searchRegion, const int& processId, const string& nodeName, const int& processModbuleID);
	void stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken);

private:
	//std::function<std::string()> getConfigPath;
	std::string getConfigPath;
	std::string configFileName;
};


//-------------------------------------3D 部分-----------------------------------

struct RunParamsStruct_Pin3DMeasure {
	double ROI_CenterX;
	double ROI_CenterY;
	double ROI_CenterZ;
	double ROI_LengthX;
	double ROI_LengthY;
	double ROI_LengthZ;
	double ROI_RX;
	double ROI_RY;
	double ROI_RZ;

	std::string LocateMethod;
	std::string ModelName;
	double DownSamplingVoxel;
	int MatchIterations;
	bool PopWindowShowMatchCloud;

	double BasePlanePoints[4][3];
	bool PopWindowShowBasePointCloud;

	int SORNeighborPoints;
	double SOROutlierThreshold;

	double EuclideanClusterTolerance;
	int EuclideanClusterMinPoints;
	int EuclideanClusterMaxPoints;

	double PinStandard;   //高度标准值
	double PinUpperTol;   //高度上公差
	double PinLowerTol;   //高度下公差
	double PinCorrect;    //高度补偿值	
	double PinNum;
	std::string SortOrder;
	int HeadAddZeroNum;
	int TailAddZeroNum;

	bool PopWindowShowCropPinCloud;
	bool PopWindowShowPinSORCloud;
	bool PopWindowShowPinDownCloud;
	bool PopWindowShowPinClusterCloud;
	bool ShowResult;

	int PinParamsNum;
	std::vector<std::string> PinParamsName;

	std::vector<double> PinROI_CenterX_Vector;
	std::vector<double> PinROI_CenterY_Vector;
	std::vector<double> PinROI_CenterZ_Vector;
	std::vector<double> PinROI_LengthX_Vector;
	std::vector<double> PinROI_LengthY_Vector;
	std::vector<double> PinROI_LengthZ_Vector;
	std::vector<double> PinROI_RX_Vector;
	std::vector<double> PinROI_RY_Vector;
	std::vector<double> PinROI_RZ_Vector;
	std::vector<double> PinDownSamplingVoxel_Vector;
	std::vector<int> SORNeighborPoints_Vector;
	std::vector<double> SOROutlierThreshold_Vector;

	std::vector<double> EuclideanClusterTolerance_Vector;
	std::vector<int> EuclideanClusterMinPoints_Vector;
	std::vector<int> EuclideanClusterMaxPoints_Vector;

	std::vector<double> PinStandard_Vector;
	std::vector<double> PinUpperTol_Vector;
	std::vector<double> PinLowerTol_Vector;
	std::vector<double> PinCorrect_Vector;
	std::vector<double> PinNum_Vector;
	std::vector<std::string> SortOrder_Vector;
	std::vector<int> HeadAddZeroNum_Vector;
	std::vector<int> TailAddZeroNum_Vector;
	void clear()
	{
		// 基础类型变量重置
		ROI_CenterX = 0.0;
		ROI_CenterY = 0.0;
		ROI_CenterZ = 0.0;
		ROI_LengthX = 0.0;
		ROI_LengthY = 0.0;
		ROI_LengthZ = 0.0;
		ROI_RX = 0.0;
		ROI_RY = 0.0;
		ROI_RZ = 0.0;

		// 字符串类型清空
		LocateMethod.clear();
		ModelName.clear();
		DownSamplingVoxel = 0.0;
		MatchIterations = 0;
		PopWindowShowMatchCloud = false;

		// 4x3数组清零
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 3; ++j) {
				BasePlanePoints[i][j] = 0.0;
			}
		}
		PopWindowShowBasePointCloud = false;

		SORNeighborPoints = 0;
		SOROutlierThreshold = 0.0;

		EuclideanClusterTolerance = 0.0;
		EuclideanClusterMinPoints = 0;
		EuclideanClusterMaxPoints = 0;

		PinStandard = 0.0;
		PinUpperTol = 0.0;
		PinLowerTol = 0.0;
		PinCorrect = 0.0;
		PinNum = 0.0;
		SortOrder.clear();
		HeadAddZeroNum = 0;
		TailAddZeroNum = 0;

		PopWindowShowCropPinCloud = false;
		PopWindowShowPinSORCloud = false;
		PopWindowShowPinDownCloud = false;
		PopWindowShowPinClusterCloud = false;
		ShowResult = false;

		// 向量类成员清空
		PinParamsNum = 0;
		PinParamsName.clear();

		PinROI_CenterX_Vector.clear();
		PinROI_CenterY_Vector.clear();
		PinROI_CenterZ_Vector.clear();
		PinROI_LengthX_Vector.clear();
		PinROI_LengthY_Vector.clear();
		PinROI_LengthZ_Vector.clear();
		PinROI_RX_Vector.clear();
		PinROI_RY_Vector.clear();
		PinROI_RZ_Vector.clear();
		PinDownSamplingVoxel_Vector.clear();
		SORNeighborPoints_Vector.clear();
		SOROutlierThreshold_Vector.clear();

		EuclideanClusterTolerance_Vector.clear();
		EuclideanClusterMinPoints_Vector.clear();
		EuclideanClusterMaxPoints_Vector.clear();

		PinStandard_Vector.clear();
		PinUpperTol_Vector.clear();
		PinLowerTol_Vector.clear();
		PinCorrect_Vector.clear();
		PinNum_Vector.clear();
		SortOrder_Vector.clear();
		HeadAddZeroNum_Vector.clear();
		TailAddZeroNum_Vector.clear();
	}
};

struct ResultParamsStruct_Pin3DMeasure {
	std::vector<double> PinHeight;
	std::vector<bool> PinFlag;
	bool isOK = true;
	cv::Mat depthImg;
	~ResultParamsStruct_Pin3DMeasure()
	{
		depthImg.release();

	}
};


class modubleDatas {
public:

	int WriteParams_Pin3DMeasure(const RunParamsStruct_Pin3DMeasure& runparams,
		const int& processId, const string& nodeName, const int& processModbuleID);

	ErrorCode_Xml ReadParams_Pin3DMeasure(RunParamsStruct_Pin3DMeasure& runparams,
		const int& processId, const string& nodeName, const int& processModbuleID);

	//void setGetConfigPath(std::function<std::string()> getConfigPath) {
	//	this->getConfigPath = getConfigPath;
	//}

	void setGetConfigPath(std::string getConfigPath) {
		this->getConfigPath = getConfigPath;
	}

	void setConfigPath(std::string fileName) {
		this->configFileName = fileName;
	}

	std::string formatDobleValue(double val, int fixed);

private:
	//std::function<std::string()> getConfigPath;
	std::string getConfigPath;
	std::string configFileName;
	void PathSet(const std::string path, const std::string XmlPath);
	int WriteTxt(std::string content);
	void writeSearchRegion(HObject searchRegion, const int& processId, const string& nodeName, const int& processModbuleID);
	void readSearchRegion(HObject& searchRegion, const int& processId, const string& nodeName, const int& processModbuleID);
	void stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken);

};