#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <vector>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <thread>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace HalconCpp;
using namespace cv;

//仿射变换类型结构体
struct TypeStruct_AffineMatrix
{
	HTuple Create_HomMat2D_Translate = "创建平移矩阵";
	HTuple CreateHomMat2D_Rotate = "创建旋转矩阵";
	HTuple CreateHomMat2D_Scale = "创建缩放矩阵";
	HTuple CreateHomMat2D_SlantX = "创建X轴斜切矩阵";
	HTuple CreateHomMat2D_SlantY = "创建Y轴斜切矩阵";
	HTuple CreateHomMat2D_RigidShift_PP = "创建点点刚性变换矩阵";//至少两个点
	HTuple CreateHomMat2D_RigidShift_PA = "创建点角度刚性变换矩阵";
	HTuple CreateHomMat2D_NPoints = "创建N点齐次变换矩阵";//至少三个点
	HTuple Add_Translate_ToHomMat2D = "添加平移矩阵";
	HTuple Add_Rotate_ToHomMat2D = "添加旋转矩阵";
	HTuple Add_Scale_ToHomMat2D = "添加缩放矩阵";
	HTuple Add_SlantX_ToHomMat2D = "添加X轴斜切矩阵";
	HTuple Add_SlantY_ToHomMat2D = "添加Y轴斜切矩阵";
};
// 创建或添加仿射变换运行参数结构体
struct RunParamsStruct_CrateHomMat2D_AffineMatrix
{
	HTuple hv_HomMat2D_InPut;			//待添加新的矩阵类型的矩阵(创建矩阵时用不着他，只有现有矩阵想添加别的向量时有用，例如添加平移向量)

	HTuple hv_Type_HomMat2D;			//矩阵类型

	HTuple OffsetX_Translat;			//平移矩阵X方向平移距离
	HTuple OffsetY_Translat;			//平移矩阵Y方向平移距离

	HTuple CenterX_Rotate;				//旋转矩阵中心X坐标
	HTuple CenterY_Rotate;				//旋转矩阵中心Y坐标
	HTuple Phi_Rotate;				    //旋转矩阵旋转角度

	HTuple Sx_Scale;					//缩放矩阵X方向缩放系数
	HTuple Sy_Scale;					//缩放矩阵Y方向缩放系数
	HTuple Px_Scale;					//缩放矩阵缩放中心点X
	HTuple Py_Scale;					//缩放矩阵缩放中心点Y

	HTuple Theta_Slant;					//斜切角度
	HTuple Px_Slant;					//斜切中心点X
	HTuple Py_Slant;					//斜切中心点Y

	HTuple OriginalX_RigidShift_PP;     //点点刚性变换矩阵原始坐标X数组
	HTuple OriginalY_RigidShift_PP;     //点点刚性变换矩阵原始坐标Y数组
	HTuple DestX_RigidShift_PP;         //点点刚性变换矩阵目标坐标X数组
	HTuple DestY_RigidShift_PP;         //点点刚性变换矩阵目标坐标Y数组

	HTuple OriginalX_RigidShift_PA;     //点角度刚性变换矩阵原始坐标X
	HTuple OriginalY_RigidShift_PA;     //点角度刚性变换矩阵原始坐标Y
	HTuple OriginalA_RigidShift_PA;     //点角度刚性变换矩阵原始角度
	HTuple DestX_RigidShift_PA;         //点角度刚性变换矩阵目标坐标X
	HTuple DestY_RigidShift_PA;         //点角度刚性变换矩阵目标坐标Y
	HTuple DestA_RigidShift_PA;         //点角度刚性变换矩阵目标角度

	HTuple Px_NPoints;					//N点齐次变换矩阵原始坐标X
	HTuple Py_NPoints;					//N点齐次变换矩阵原始坐标Y
	HTuple Qx_NPoints;					//N点齐次变换矩阵目标坐标X
	HTuple Qy_NPoints;					//N点齐次变换矩阵目标坐标X

};

//结果参数结构体
struct ResultParamsStruct_AffineMatrix
{
	HTuple hv_HomMat2D;					 //输出矩阵

	HTuple hv_Row_Affine;			     //矩阵转换后的行坐标
	HTuple hv_Column_Affine;			 //矩阵转换后的列坐标
	HObject ho_Image_Affine;			 //矩阵转换后的图片
	HObject ho_Region_Affine;			 //矩阵转换后的区域
	HObject ho_ContourXld_Affine;	     //矩阵转换后的轮廓
	HTuple hv_RetNum = 0;				 //OK结果个数

};

//仿射变换类型结构体
struct TypeStruct_AffineMatrixDetect
{
	//创建矩阵参数
	HTuple Create_HomMat2D_Translate = "创建平移矩阵";
	HTuple CreateHomMat2D_Rotate = "创建旋转矩阵";
	HTuple CreateHomMat2D_Scale = "创建缩放矩阵";
	HTuple CreateHomMat2D_SlantX = "创建X轴斜切矩阵";
	HTuple CreateHomMat2D_SlantY = "创建Y轴斜切矩阵";
	HTuple CreateHomMat2D_RigidShift_PA = "创建点角度刚性变换矩阵";
	HTuple CreateHomMat2D_ProjectionTrans = "创建投影变换矩阵";
	//应用矩阵参数
	HTuple AffineTrans_Image = "图像变换";
	HTuple AffineTrans_Region = "区域变换";
	HTuple AffineTrans_Xld = "轮廓变换";
	HTuple AffineTrans_Point = "坐标点变换";
	HTuple ProjectionTrans_Image = "图像投影变换";
	HTuple ProjectionTrans_Region = "区域投影变换";
	HTuple ProjectionTrans_Xld = "轮廓投影变换";
	HTuple hv_NULL = "null";
};
// 运行参数结构体
struct RunParamsStruct_AffineMatrixDetect
{
	//**************创建矩阵参数*******************************************
	HTuple hv_Type_CreateHomMat2D = "null";			//创建矩阵类型

	HTuple OffsetRow_Translate = 0;			//平移矩阵Row方向平移距离
	HTuple OffsetColumn_Translate = 0;			//平移矩阵Column方向平移距离

	HTuple CenterRow_Rotate = 0;				//旋转矩阵中心Row坐标
	HTuple CenterColumn_Rotate = 0;				//旋转矩阵中心Column坐标
	HTuple Phi_Rotate = 0;				    //旋转矩阵旋转角度
	HTuple hv_ReverseAngle = 0;				//强制转正(0不启用，1启用)

	HTuple Srow_Scale = 1;					//缩放矩阵Row方向缩放系数
	HTuple Scolumn_Scale = 1;					//缩放矩阵Column方向缩放系数
	HTuple Row_Scale = 0;					//缩放矩阵缩放中心点Row
	HTuple Column_Scale = 0;					//缩放矩阵缩放中心点Column

	HTuple Theta_Slant = 0;					//斜切角度
	HTuple Row_Slant = 0;					//斜切中心点Row
	HTuple Column_Slant = 0;					//斜切中心点Column

	HTuple OriginalRow_RigidShift_PA = 0;     //点角度刚性变换矩阵原始坐标Row
	HTuple OriginalColumn_RigidShift_PA = 0;     //点角度刚性变换矩阵原始坐标Column
	HTuple OriginalA_RigidShift_PA = 0;     //点角度刚性变换矩阵原始角度
	HTuple DestRow_RigidShift_PA = 0;         //点角度刚性变换矩阵目标坐标Row
	HTuple DestColumn_RigidShift_PA = 0;         //点角度刚性变换矩阵目标坐标Column
	HTuple DestA_RigidShift_PA = 0;         //点角度刚性变换矩阵目标角度

	HTuple hv_Row_LeftTop = 0;		//投影变换四边形左上角Row坐标			
	HTuple hv_Column_LeftTop = 0;		//投影变换左上角Column坐标		
	HTuple hv_Row_RightTop = 0;		//投影变换四边形右上角Row坐标			
	HTuple hv_Column_RightTop = 100;		//投影变换右上角Column坐标	
	HTuple hv_Row_RightBottom = 100;	//投影变换四边形右下角Row坐标			
	HTuple hv_Column_RightBottom = 100;	//投影变换右下角Column坐标	
	HTuple hv_Row_LeftBottom = 100;		//投影变换四边形左下角Row坐标			
	HTuple hv_Column_LeftBottom = 0;		//投影变换左下角Column坐标	

	//*********************应用矩阵参数*********************************************
	HTuple hv_Type_ApplyHomMat2D = "null";			//应用矩阵变换类型
	HObject ho_ImgInput;					//输入图像，待转换
	HObject ho_RegionInput;					//输入区域，待转换
	HObject ho_XldInput;					//输入轮廓，待转换
	HTuple hv_RowInput = 0;					//输入点Row坐标，待转换
	HTuple hv_ColumnInput = 0;					//输入点Column坐标，待转换

};

//仿射变换结果结构体
struct ResultParamsStruct_AffineMatrixDetect
{
	//矩阵输出
	HTuple hv_HomMat2D = NULL;
	//仿射变换的输出
	HObject ho_ImgDest;
	HObject ho_RegionDest;
	HObject ho_XldDest;
	HTuple hv_RowDest;
	HTuple hv_ColumnDest;
};
//条形码类型结构体
struct CodeTypeStruct_BarCode
{
	HTuple CODE128 = "Code 128";
	HTuple CODE39 = "Code 39";
	HTuple CODE93 = "Code 93";
	HTuple EAN13 = "EAN-13";
	HTuple EAN8 = "EAN-8";
	HTuple ITF25 = "2/5 Industrial";
	HTuple UPCA = "UPC-A";
	HTuple UPCE = "UPC-E";
	HTuple CODABAR = "Codabar";
	HTuple PHARMACODE = "PharmaCode";
	HTuple AUTO = "auto";
};
//一维码检测运行参数结构体
struct RunParamsStruct_BarCode
{
	HObject ho_SearchRegion;					    //搜索区域
	HTuple hv_CodeType_Run;							//条码类型(默认"auto"，否则选择结构体CodeTypeStruct_BarCode里面的额参数),暂时不给默认值，因为增加条码类型的时候auto会在数组最前面，导致报错
	HTuple hv_CodeNum = 1;						    //识别数量(设置为0，则扫描所有条码)
	HTuple hv_Tolerance = "high";                   //识别容差(默认高级别，默认high，还可以设置"low")
	HTuple hv_HeadChar = "null";					    //特定字符开头
	HTuple hv_EndChar = "null";						    //特定字符结尾
	HTuple hv_ContainChar = "null";						//包含特定字符
	HTuple hv_NotContainChar = "null";					//不包含特定字符
	HTuple hv_CodeLength_Run = 2;               //条码最小长度
	HTuple hv_TimeOut = 50;						//超时时间(单位ms)
	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，区域跟随
	bool isFollow = false;                //是否跟随
};
//条形码识别结果结构体
struct ResultParamsStruct_BarCode
{
	//条码结果参数
	HTuple hv_CodeType_Result;         //条码类型
	HTuple hv_CodeContent;			  //条码内容
	HTuple hv_CodeLength_Result;	  //条码长度
	HObject ho_ContourBarCode;		  //条码轮廓
	HObject ho_RegionBarCode;		  //条码识别结果区域
	HTuple hv_Row;                    //条码中心行坐标
	HTuple hv_Column;                 //条码中心列坐标
	HTuple hv_RetNum = 0;			 //找到的条码个数(默认是0，识别到条码后大于0)
	HTuple hv_QualityGrade;		    //条码质量等级0-4(iso15416)
	
	void clear()
	{
		ho_ContourBarCode.Clear();
		ho_RegionBarCode.Clear();
	};

	~ResultParamsStruct_BarCode()
	{
		clear();
	}
};

//图像二值化类型结构体
struct TypeStruct_BinarizationDetect
{
	HTuple Fixed_Threshold = "固定阈值";
	HTuple Auto_Threshold = "自适应阈值";
	HTuple Dyn_Threshold = "动态阈值";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";
	HTuple hv_Null = "null";
};
//图像二值化运行参数结构体
struct RunParamsStruct_BinarizationDetect
{
	HTuple hv_ThresholdType = "固定阈值";					//图像二值化类型
	HTuple hv_LowThreshold = 50;						//低阈值(固定阈值)
	HTuple hv_HighThreshold = 128;					//高阈值(固定阈值)
	HTuple hv_Sigma = 1;						//平滑阈值
	HTuple hv_CoreWidth = 3;					//动态阈值(滤波核宽度)
	HTuple hv_CoreHeight = 3;					//动态阈值(滤波核高度)
	HTuple hv_DynThresholdContrast = 10;		//动态阈值分割图像对比度
	HTuple hv_DynThresholdPolarity = "dark";	//动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

	HObject ho_SearchRegion;			 //搜索区域
	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	bool isFollow = false;                //是否跟随

};
//图像二值化结果参数结构体
struct ResultParamsStruct_BinarizationDetect
{
	HObject ho_DestImage;  //二值化图像
	HObject ho_DestRegions;     //二值化区域
	HTuple  hv_RetNum = 0;    //二值化区域数量(默认是0，ok大于0)
	//二值化区域形态学结果参数
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
	HTuple hv_Angle;
	HTuple hv_Circularity;
	HTuple hv_Compactness;
	HTuple hv_Convexity;
	HTuple hv_Rectangularity;
	HTuple hv_Height;
	HTuple hv_Width;
	HTuple hv_OuterRadius;
	HTuple hv_InnerRadius;
	HTuple hv_Rect2Len1;
	HTuple hv_Rect2Len2;
	HTuple hv_Rect2Angle;

};

//区域操作参数类型结构体
struct TypeStruct_RegionOperator_MorphologyDetect
{
	HTuple hv_Dilation_Rectangle = "矩形膨胀";
	HTuple hv_Dilation_Circle = "圆形膨胀";
	HTuple hv_Erosion_Rectangle = "矩形腐蚀";
	HTuple hv_Erosion_Circle = "圆形腐蚀";
	HTuple hv_Opening_Rectangle = "矩形开运算";
	HTuple hv_Opening_Circle = "圆形开运算";
	HTuple hv_Closing_Rectangle = "矩形闭运算";
	HTuple hv_Closing_Circle = "圆形闭运算";
	HTuple hv_BottomHat_Rectangle = "底帽运算矩形结构体";
	HTuple hv_BottomHat_Circle = "底帽运算圆形结构体";
	HTuple hv_TopHat_Rectangle = "顶帽运算矩形结构体";
	HTuple hv_TopHat_Circle = "顶帽运算圆形结构体";
	HTuple hv_Fillup = "孔洞全部填充";
	HTuple hv_FillUp_Shape = "孔洞面积填充";
	HTuple hv_Connection = "计算连通域";
	HTuple hv_Union1 = "所有区域合并";
	HTuple hv_Union2 = "两个区域合并";
	HTuple hv_Complement = "补集";
	HTuple hv_Difference = "差集";
	HTuple hv_Intersection = "交集";
	HTuple hv_ConcatObj = "和集";//区域相加得到区域数组
	HTuple hv_Null = "null";
};

//特征筛选参数类型结构体
struct TypeStruct_FeaturesFilter_MorphologyDetect
{
	//HTuple hv_Area_Filter = "area";//面积
	//HTuple hv_Row_Filter = "row";//行坐标（Y）
	//HTuple hv_Column_Filter = "column";//列坐标(X)
	//HTuple hv_Angle_Filter = "orientation";//角度
	//HTuple hv_Circularity_Filter = "circularity";//圆度
	//HTuple hv_Compactness_Filter = "compactness";//紧密度
	//HTuple hv_Convexity_Filter = "convexity";//凸度
	//HTuple hv_Rectangularity_Filter = "rectangularity";//矩形度
	//HTuple hv_Height_Filter = "height";//高度
	//HTuple hv_Width_Filter = "width";//宽度
	//HTuple hv_OuterRadius_Filter = "outer_radius";//最小外接圆半径
	//HTuple hv_InnerRadius_Filter = "inner_radius";//最大内接圆半径
	//HTuple hv_Rect2Len1_Filter = "rect2_len1";//最小外接矩半长
	//HTuple hv_Rect2Len2_Filter = "rect2_len2";//最小外接矩半宽
	//HTuple hv_Rect2Angle_Filter = "rect2_phi";//最小外接矩角度
	HTuple hv_Area_Filter = "面积";//area
	HTuple hv_Column_Filter = "X";//列坐标(X)column
	HTuple hv_Row_Filter = "Y";//行坐标（Y）row
	HTuple hv_Angle_Filter = "角度";//orientation
	HTuple hv_Circularity_Filter = "圆度";//circularity
	HTuple hv_Compactness_Filter = "紧密度";//compactness
	HTuple hv_Convexity_Filter = "凸度";//convexity
	HTuple hv_Rectangularity_Filter = "矩形度";//rectangularity
	HTuple hv_Height_Filter = "高度";//height
	HTuple hv_Width_Filter = "宽度";//width
	HTuple hv_OuterRadius_Filter = "最小外接圆半径";//outer_radius
	HTuple hv_InnerRadius_Filter = "最大内接圆半径";//inner_radius
	HTuple hv_Rect2Len1_Filter = "最小外接矩半长";//rect2_len1
	HTuple hv_Rect2Len2_Filter = "最小外接矩半宽";//rect2_len2
	HTuple hv_Rect2Angle_Filter = "最小外接矩角度";//rect2_phi
};

//形态学运行参数结构体
struct RunParamsStruct_MorphologyDetect
{
	//区域操作参数
	HTuple hv_RegionOperator_Type;				 //数组，区域操作类型(需要使用结构体TypeStruct_RegionOperator_MorphologyDetect里面的参数)
	HTuple hv_Height_Operator;					//数组，矩形结构元素长度
	HTuple hv_Width_Operator;					//数组，矩形结构元素宽度
	HTuple hv_CircleRadius_Operator;			//数组，圆形结构元素半径
	HTuple hv_MinArea_FillUpShape_Operator;				//孔洞填充最小面积
	HTuple hv_MaxArea_FillUpShape_Operator;				//孔洞填充最大面积

	//*******************************************************************************************************************************
	//筛选算法参数
	//区域筛选参数
	HTuple hv_FeaturesFilter_Type;				 //数组，区域筛选条件数组(需要使用结构体TypeStruct_FeaturesFilter_MorphologyDetect里面的参数)
	HTuple hv_And_Or_Filter = "and";			//区域筛选多个条件之间选择"and"或者"or",默认"and"
	HTuple hv_MinValue_Filter;					//数组，区域筛选参数最小值
	HTuple hv_MaxValue_Filter;					//数组，区域筛选参数最大值
};


//形态学结果参数结构体
struct ResultParamsStruct_MorphologyDetect
{
	HObject ho_DestRegions;     //形态学结果区域
	HTuple hv_RetNum = 0;    //结果区域数量
	//区域筛选结果参数
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
	HTuple hv_Angle;
	HTuple hv_Circularity;
	HTuple hv_Compactness;
	HTuple hv_Convexity;
	HTuple hv_Rectangularity;
	HTuple hv_Height;
	HTuple hv_Width;
	HTuple hv_OuterRadius;
	HTuple hv_InnerRadius;
	HTuple hv_Rect2Len1;
	HTuple hv_Rect2Len2;
	HTuple hv_Rect2Angle;
};

//图像二值化类型结构体
struct TypeStruct_Binarization_Blob
{
	HTuple Fixed_Threshold = "固定阈值";
	HTuple Auto_Threshold = "自适应阈值";
	HTuple Dyn_Threshold = "动态阈值";
	HTuple DynPolarity_Light = "light";
	HTuple DynPolarity_Dark = "dark";
	HTuple DynPolarity_Equal = "equal";
	HTuple DynPolarity_NotEqual = "not_equal";
	HTuple hv_Null = "null";
};
//区域操作参数类型结构体
struct TypeStruct_RegionOperator_Blob
{
	HTuple hv_Dilation_Rectangle = "矩形膨胀";
	HTuple hv_Dilation_Circle = "圆形膨胀";
	HTuple hv_Erosion_Rectangle = "矩形腐蚀";
	HTuple hv_Erosion_Circle = "圆形腐蚀";
	HTuple hv_Opening_Rectangle = "矩形开运算";
	HTuple hv_Opening_Circle = "圆形开运算";
	HTuple hv_Closing_Rectangle = "矩形闭运算";
	HTuple hv_Closing_Circle = "圆形闭运算";
	HTuple hv_BottomHat_Rectangle = "底帽运算矩形结构体";
	HTuple hv_BottomHat_Circle = "底帽运算圆形结构体";
	HTuple hv_TopHat_Rectangle = "顶帽运算矩形结构体";
	HTuple hv_TopHat_Circle = "顶帽运算圆形结构体";
	HTuple hv_Fillup = "孔洞全部填充";
	HTuple hv_FillUp_Shape = "孔洞面积填充";
	HTuple hv_Connection = "计算连通域";
	HTuple hv_Union1 = "所有区域合并";
	HTuple hv_Complement = "补集";
	HTuple hv_Null = "null";
};

//特征筛选参数类型结构体
struct TypeStruct_FeaturesFilter_Blob
{
	HTuple hv_Area_Filter = "面积";//area
	HTuple hv_Column_Filter = "X";//列坐标(X)column
	HTuple hv_Row_Filter = "Y";//行坐标（Y）row
	HTuple hv_Angle_Filter = "角度";//orientation
	HTuple hv_Circularity_Filter = "圆度";//circularity
	HTuple hv_Compactness_Filter = "紧密度";//compactness
	HTuple hv_Convexity_Filter = "凸度";//convexity
	HTuple hv_Rectangularity_Filter = "矩形度";//rectangularity
	HTuple hv_Height_Filter = "高度";//height
	HTuple hv_Width_Filter = "宽度";//width
	HTuple hv_OuterRadius_Filter = "最小外接圆半径";//outer_radius
	HTuple hv_Rect2Len1_Filter = "最小外接矩半长";//rect2_len1
	HTuple hv_Rect2Len2_Filter = "最小外接矩半宽";//rect2_len2
	HTuple hv_Rect2Angle_Filter = "最小外接矩角度";//rect2_phi
};
//Blob运行参数结构体
struct RunParamsStruct_Blob
{
	HTuple hv_IsUsSearchRoi = 0;//0 不启用搜索区域，1 启用搜索区域
	//搜索区域，矩形的两个角点
	cv::Point P1_SearchRoi = cv::Point(10, 200);
	cv::Point P2_SearchRoi = cv::Point(10, 200);
	//二值化运行参数
	HTuple hv_ThresholdType = "固定阈值";					//图像二值化类型
	HTuple hv_LowThreshold = 50;						//低阈值(固定阈值)
	HTuple hv_HighThreshold = 128;					//高阈值(固定阈值)
	HTuple hv_Sigma = 1;						//平滑阈值
	HTuple hv_CoreWidth = 3;					//动态阈值(滤波核宽度)
	HTuple hv_CoreHeight = 3;					//动态阈值(滤波核高度)
	HTuple hv_DynThresholdContrast = 10;		//动态阈值分割图像对比度
	HTuple hv_DynThresholdPolarity = "dark";	//动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

	//形态学运行参数
	HTuple hv_RegionOperator_Type;				 //数组，区域操作类型(需要使用结构体TypeStruct_RegionOperator_MorphologyDetect里面的参数)
	HTuple hv_Height_Operator;					//数组，矩形结构元素长度
	HTuple hv_Width_Operator;					//数组，矩形结构元素宽度
	HTuple hv_CircleRadius_Operator;			//数组，圆形结构元素半径
	HTuple hv_MinArea_FillUpShape_Operator;				//孔洞填充最小面积
	HTuple hv_MaxArea_FillUpShape_Operator;				//孔洞填充最大面积
	//区域筛选参数
	HTuple hv_FeaturesFilter_Type;				 //数组，区域筛选条件数组(需要使用结构体TypeStruct_FeaturesFilter_MorphologyDetect里面的参数)
	HTuple hv_And_Or_Filter = "and";			//区域筛选多个条件之间选择"and"或者"or",默认"and"
	HTuple hv_MinValue_Filter;					//数组，区域筛选参数最小值
	HTuple hv_MaxValue_Filter;					//数组，区域筛选参数最大值

	HTuple hv_SetBlobCount = 1;

	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	bool isFollow = false;                //是否跟随
};
//Blob结果参数结构体
struct ResultParamsStruct_Blob
{
	HObject ho_DestImage;  //二值化图像
	HObject ho_DestRegions;     //结果区域
	HTuple hv_RetNum = 0;    //结果区域数量
	//区域筛选结果参数
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
	HTuple hv_Angle;
	HTuple hv_Circularity;
	HTuple hv_Compactness;
	HTuple hv_Convexity;
	HTuple hv_Rectangularity;
	HTuple hv_Height;
	HTuple hv_Width;
	HTuple hv_OuterRadius;
	HTuple hv_Rect2Len1;
	HTuple hv_Rect2Len2;
	HTuple hv_Rect2Angle;

	void clear()
	{
		ho_DestImage.Clear();
		ho_DestRegions.Clear();
	};

	~ResultParamsStruct_Blob()
	{
		clear();
	}
};

//运行参数结构体
struct MapImgType_CalibBoardDetect
{
	HTuple hv_Deformity_MapImg = "畸变矫正";
	HTuple hv_Deformity_Projection_MapImg = "畸变加透视矫正";
	HTuple hv_Null = "null";
};
//运行参数结构体
struct RunParamsStruct_CalibBoardDetect
{
	HObject ho_Image;//标定用图，暂时使用一张(可以是一张或者是多张)
	HTuple hv_StartCamPar;//标定需要相机参数
	HTuple hv_CaltabDescrPath;//标定板描述文件路径(Halcon圆点标定板，以".descr"格式的)
};
//结果参数结构体
struct ResultParamsStruct_CalibBoardDetect
{
	HObject ho_CalibContours;// 标定结果路轮廓
	HTuple hv_CamParam;//标定后的相机内参
	HTuple hv_CamPose;//标定后的相机外参
	HTuple hv_Error;//标定误差(优化后投影的均方根误差（RMSE）以误差（以像素为单位）返回)
	HTuple hv_Resolution;//像素当量
	HTuple hv_DistanceCC; //标定后测量的圆心距
	HTuple hv_MapImgType = "null"; //图像矫正类型
	HObject hv_DestImg;//畸变矫正后的图像
};

// 查找直线参数结构体

//抓边运行参数结构体
struct RunParamsStruct_Line
{
	//测量参数
	//直线抓边roi起始点
	HTuple hv_Row1 = 10;
	HTuple hv_Column1 = 10;
	HTuple hv_Row2 = 10;
	HTuple hv_Column2 = 50;

	//抓边模型需要的抓边参数
	HTuple hv_MeasureLength1 = 50;		//卡尺半长
	HTuple hv_MeasureLength2 = 5;		//卡尺半宽
	HTuple hv_MeasureSigma = 1.0;		//平滑值 
	HTuple hv_MeasureThreshold = 20;	//边缘阈值
	HTuple hv_MeasureTransition = "all";	//卡尺极性
	HTuple hv_MeasureSelect = "first";		//边缘选择
	HTuple hv_MeasureNum = 10;		    //卡尺个数
	HTuple hv_InstancesNum = 1;		//抓边数量
	HTuple hv_MeasureMinScore = 0.5;     //最小得分
	HTuple hv_DistanceThreshold = 3.5;   //距离阈值
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//插值算法

	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	bool isFollow = false;                //是否跟随
	HTuple hv_MmPixel = 1;	//像素当量
};
//抓边结果参数结构体
struct ResultParamsStruct_Line
{
	//检测到的直线结果参数
	HTuple hv_LineRowBegin;
	HTuple hv_LineColumnBegin;
	HTuple hv_LineRowEnd;
	HTuple hv_LineColumnEnd;
	//卡尺找到的所有点坐标
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //找到的直线个数(默认是0，找到直线后数值大于0)

	HTuple hv_PointsNum = 0;		//卡尺找到的点的数量
	HObject ho_Contour_Line;      // 直线轮廓
	HObject ho_Contour_Calipers;  //卡尺轮廓
	HObject ho_Contour_CrossPoints;//所有点十字轮廓

	//卡尺所有点距离拟合直线的距离数组
	HTuple hv_DisToLineArray;
	//直线度
	HTuple hv_Straightness = 99999;
};

//抓圆运行参数结构体
struct RunParamsStruct_Circle
{
	//测量参数
	//抓圆roi圆心和半径
	HTuple hv_Row = 50;
	HTuple hv_Column = 50;
	HTuple hv_Radius = 50;

	//抓圆模型需要的参数
	HTuple hv_MeasureLength1 = 50;		//卡尺半长
	HTuple hv_MeasureLength2 = 5;		//卡尺半宽
	HTuple hv_MeasureSigma = 1.0;		//平滑值 
	HTuple hv_MeasureThreshold = 20;	//边缘阈值
	HTuple hv_MeasureTransition = "all";	//卡尺极性
	HTuple hv_MeasureSelect = "first";		//边缘选择
	HTuple hv_MeasureNum = 10;		    //卡尺个数
	HTuple hv_InstancesNum = 1;		//抓边数量
	HTuple hv_MeasureMinScore = 0.5;     //最小得分
	HTuple hv_DistanceThreshold = 3.5;   //距离阈值
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//插值算法

	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	bool isFollow = false;                //是否跟随
	HTuple hv_MmPixel = 1;	//像素当量
		//是否选用矩形框
	HTuple hv_DectType = 0;
};
//抓圆结果参数结构体
struct ResultParamsStruct_Circle
{
	//检测到的圆结果参数
	HTuple hv_CircleRowCenter;
	HTuple hv_CircleColumnCenter;
	HTuple hv_CircleRadius;
	//卡尺找到的所有点坐标
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //找到的圆个数(默认是0，找到圆后数值大于0)

	HTuple hv_PointsNum = 0;		//卡尺找到的点的数量
	HObject ho_Contour_Circle;      // 圆轮廓
	HObject ho_Contour_Calipers;  //卡尺轮廓
	HObject ho_Contour_CrossPoints;//所有点十字轮廓

	//卡尺所有点距离拟合圆的圆心减去半径距离数组
	HTuple hv_DisToCircleArray;
	//圆度
	HTuple hv_Roundness = 99999;

	void clear()
	{
		ho_Contour_Circle.Clear();
		ho_Contour_Calipers.Clear();
		ho_Contour_CrossPoints.Clear();
	};

	~ResultParamsStruct_Circle()
	{
		clear();
	}
};

//运行参数结构体
struct RunParamsStruct_Rectangle
{
	//测量参数
	//矩形检测起始坐标
	HTuple hv_Row = 10;
	HTuple hv_Column = 10;
	HTuple hv_Angle = 0;
	HTuple hv_Length1 = 50;
	HTuple hv_Length2 = 5;

	//模型需要的参数
	HTuple hv_MeasureLength1 = 50;		//卡尺半长
	HTuple hv_MeasureLength2 = 5;		//卡尺半宽
	HTuple hv_MeasureSigma = 1.0;		//平滑值 
	HTuple hv_MeasureThreshold = 20;	//边缘阈值
	HTuple hv_MeasureTransition = "all";	//卡尺极性
	HTuple hv_MeasureSelect = "first";		//边缘选择
	HTuple hv_MeasureNum = 10;		    //卡尺个数
	HTuple hv_InstancesNum = 1;		//抓边数量
	HTuple hv_MeasureMinScore = 0.5;     //最小得分
	HTuple hv_DistanceThreshold = 3.5;   //距离阈值
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//插值算法

	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	HTuple hv_TemplateAngle = 0;	//跟随的模板角度
	bool isFollow = false;                //是否跟随
};
//矩形检测结果参数结构体
struct ResultParamsStruct_Rectangle
{
	//检测到的矩形结果参数
	HTuple hv_RectangleRow;
	HTuple hv_RectangleColumn;
	HTuple hv_RectangleAngle;
	HTuple hv_RectangleLength1;
	HTuple hv_RectangleLength2;
	//卡尺找到的所有点坐标
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //找到的矩形个数(默认是0，找到直线后数值大于0)

	HTuple hv_PointsNum = 0;		//卡尺找到的点的数量
	HObject ho_Contour_Rectangle;      // 矩形轮廓
	HObject ho_Contour_Calipers;  //卡尺轮廓
	HObject ho_Contour_CrossPoints;//所有点十字轮廓
};

//运行参数结构体
struct RunParamsStruct_Ellipse
{
	//测量参数
	//椭圆形检测起始坐标
	HTuple hv_Row = 10;
	HTuple hv_Column = 10;
	HTuple hv_Angle = 0;
	HTuple hv_Radius1 = 50;
	HTuple hv_Radius2 = 25;

	//模型需要的参数
	HTuple hv_MeasureLength1 = 50;		//卡尺半长
	HTuple hv_MeasureLength2 = 5;		//卡尺半宽
	HTuple hv_MeasureSigma = 1.0;		//平滑值 
	HTuple hv_MeasureThreshold = 20;	//边缘阈值
	HTuple hv_MeasureTransition = "all";	//卡尺极性
	HTuple hv_MeasureSelect = "first";		//边缘选择
	HTuple hv_MeasureNum = 10;		    //卡尺个数
	HTuple hv_InstancesNum = 1;		//抓边数量
	HTuple hv_MeasureMinScore = 0.5;     //最小得分
	HTuple hv_DistanceThreshold = 3.5;   //距离阈值
	HTuple hv_MeasureInterpolation = "nearest_neighbor";//插值算法

	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	HTuple hv_TemplateAngle = 0;	//跟随的模板角度
	bool isFollow = false;                //是否跟随
};
//椭圆检测结果参数结构体
struct ResultParamsStruct_Ellipse
{
	//检测到的椭圆形结果参数
	HTuple hv_EllipseRow;
	HTuple hv_EllipseColumn;
	HTuple hv_EllipseAngle;
	HTuple hv_EllipseRadius1;
	HTuple hv_EllipseRadius2;
	HTuple hv_EllipseStartAngle = 0;
	HTuple hv_EllipseEndAngle = 360;
	//卡尺找到的所有点坐标
	HTuple hv_PointsRow;
	HTuple hv_PointsColumn;

	HTuple hv_RetNum = 0;           //找到的椭圆形个数(默认是0，找到椭圆后数值大于0)

	HTuple hv_PointsNum = 0;		//卡尺找到的点的数量
	HObject ho_Contour_Ellipse;      // 椭圆轮廓
	HObject ho_Contour_Calipers;  //卡尺轮廓
	HObject ho_Contour_CrossPoints;//所有点十字轮廓
};

//颜色检测运行参数结构体
struct RunParamsStruct_ColorDetect
{
	HObject ho_SearchRegion;				 //搜索区域
	HTuple hv_ColorHandle = NULL;			//颜色检测句柄
	HObject ho_TrainRegions;		 //待训练的颜色区域
	HTuple hv_MeanValue_R = 128;			//R通道平均灰度值
	HTuple hv_MeanValue_G = 128;			//G通道平均灰度值
	HTuple hv_MeanValue_B = 128;			//B通道平均灰度值
	HTuple hv_MinScore = 0.5;				//最小分数(低于此分数，认为NG)
	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，抓边跟随
	bool isFollow = false;                //是否跟随

};
//颜色检测结果参数结构体
struct ResultParamsStruct_ColorDetect
{
	HTuple hv_Score;			//每个颜色种类各自的分数(例如认为当前图片是红色的置信度是0.65，认为当前图片是蓝色的置信度是0.15)
	HObject ho_DestRegions;     //颜色识别结果区域
	HTuple hv_AreaTotal;		//每个颜色种类各自的总面积(例如找到红色的产品3个，总面积600.例如找到蓝色色的产品1个，总面积100)

};

//颜色检测运行参数结构体
struct RunParamsStruct_ColorDetect_MoreNum
{
	HObject ho_SearchRegion;				 //搜索区域
	HTuple hv_ColorHandle = NULL;			//颜色检测句柄
	vector<HTuple> hv_ColorName;			 //需要识别的颜色数组
	vector<HObject> ho_TrainRegions;		 //待训练的颜色区域
	vector<HTuple> hv_MeanValue_R;			//R通道平均灰度值
	vector<HTuple> hv_MeanValue_G;			//G通道平均灰度值
	vector<HTuple> hv_MeanValue_B;			//B通道平均灰度值

};
//颜色检测结果参数结构体
struct ResultParamsStruct_ColorDetect_MoreNum
{
	vector<HTuple>  hv_ColorName;		//识别到的颜色名称数组
	vector<HObject> ho_DestRegions;     //颜色识别结果区域数组
	vector<HTuple> hv_Score;			//每个颜色种类各自的分数(例如认为当前图片是红色的置信度是0.65，认为当前图片是蓝色的置信度是0.15)
	vector<HTuple> hv_AreaTotal;		//每个颜色种类各自的总面积(例如找到红色的产品3个，总面积600.例如找到蓝色色的产品1个，总面积100)

};

//仿射变换类型结构体
struct TypeStruct_CreateContourDetect
{
	HTuple hv_Null = "null";
	HTuple hv_Polygon = "polygon"; //"多边形所有点坐标生成轮廓";
	HTuple hv_Region = "region"; //"区域生成轮廓"
	HTuple hv_Skeleton = "skeleton"; // "骨架生成轮廓"
};
// 运行参数结构体
struct RunParamsStruct_CreateContourDetect
{
	HTuple hv_TypeCreateContour = "region";		//转换类型
	HTuple hv_Rows_Polygon;	//多边形点行坐标数组
	HTuple hv_Cols_Polygon;	//多边形点列坐标数组
	HObject hv_InputRegion;	//输入区域
	HObject hv_InputSkeleton;//输入骨架
};
//结果参数结构体
struct ResultParamsStruct_CreateContourDetect
{
	HObject hv_DestXLD;					 //输出轮廓

	HTuple hv_Rows_XLD;					 //轮廓点行坐标
	HTuple hv_Columns_XLD;				 //轮廓点列坐标
};

//需要创建ROI的类型结构体
struct TypeStruct_CreateRoiDetect
{
	HTuple hv_Circle = "圆";
	HTuple hv_Ellipse = "椭圆";
	HTuple hv_Rectangle1 = "矩形";
	HTuple hv_Rectangle2 = "可旋转矩形";
	HTuple hv_Line = "直线";
	HTuple hv_NULL = "null";
};
//区域操作参数类型结构体
struct RunParamsStruct_CreateRoiDetect
{
	HTuple hv_TypeRoi = "null";				//ROI类型

	HTuple hv_Row_Circle = 0;			//圆心row
	HTuple hv_Column_Circle = 0;		//圆心column
	HTuple hv_Radius_Circle = 10;		//圆半径

	HTuple hv_Row_Ellipse = 0;			//椭圆圆心row
	HTuple hv_Column_Ellipse = 0;		//椭圆圆心column
	HTuple hv_Angle_Ellipse = 0;		//椭圆长半轴与水平线角度
	HTuple hv_Radius1_Ellipse = 20;		//椭圆长半轴
	HTuple hv_Radius2_Ellipse = 10;		//椭圆短半轴

	HTuple hv_Row1_Rectangle1 = 0;		//矩形左上角row
	HTuple hv_Column1_Rectangle1 = 0;	//矩形左上角column
	HTuple hv_Row2_Rectangle1 = 20;		//矩形右下角row
	HTuple hv_Column2_Rectangle1 = 10;	//矩形右下角column

	HTuple hv_Row_Rectangle2 = 0;		//旋转矩形中心row
	HTuple hv_Column_Rectangle2 = 0;	//旋转矩形中心column
	HTuple hv_Angle_Rectangle2 = 0;     //旋转矩形纵轴与水平线角度
	HTuple hv_Length1_Rectangle2 = 20;   //旋转矩形半宽
	HTuple hv_Length2_Rectangle2 = 10;   //旋转矩形半高

	HTuple hv_RowBegin_Line = 0;					//直线起点Row
	HTuple hv_ColumnBegin_Line = 0;				//直线起点Col
	HTuple hv_RowEnd_Line = 20;					//直线终点Row
	HTuple hv_ColumnEnd_Line = 10;				//直线终点Col
};

//裁剪类型结构体
struct TypeStruct_CutImgDetect
{
	HTuple hv_CropDomain = "CropDomain";
	HTuple hv_ReduceDomain = "ReduceDomain";
	HTuple hv_NULL = "null";
};
// 运行参数结构体
struct RunParamsStruct_CutImgDetect
{
	HTuple hv_CutImgType = "ReduceDomain";	//裁图类型
};

//拟合类型结构体
struct TypeStruct_FitDetect
{
	HTuple hv_Circle = "圆";
	HTuple hv_Ellipse = "椭圆";
	HTuple hv_Rectangle = "矩形";
	HTuple hv_Line = "直线";
	HTuple hv_NULL = "null";
};
//拟合运行参数结构体
struct RunParamsStruct_FitDetect
{
	HTuple hv_FitType = "null";
	vector<HTuple> hv_Rows;//拟合用行坐标数组
	vector<HTuple> hv_Columns;//拟合用列坐标数组
};
//拟合结果参数结构体
struct ResultParamsStruct_FitDetect
{
	ResultParamsStruct_Line Line_FitResult;
	ResultParamsStruct_Circle Circle_FitResult;
	ResultParamsStruct_Rectangle Rectangle_FitResult;
	ResultParamsStruct_Ellipse Ellipse_FitResult;
};

//傅里叶变换结果图像类型结构体
struct ResultType_FourierTransDetect
{
	HTuple hv_Byte = "byte";
	HTuple hv_Real = "real";
};
//傅里叶变换卷积滤波器类型结构体
struct FilterType_FourierTransDetect
{
	HTuple hv_GaussFilter = "高斯滤波器";
	HTuple hv_MeanFilter = "均值滤波器";
	HTuple hv_Highpass = "高通滤波器";
	HTuple hv_Lowpass = "低通滤波器";
	HTuple hv_SinBandPass = "正弦带通滤波器";
	HTuple hv_BandPass = "带通滤波器";
	HTuple hv_BandFilter = "带阻滤波器";
	HTuple hv_GaussBandFilter = "高斯带通滤波器";
};
//运行参数结构体
struct RunParamsStruct_FourierTransDetect
{
	HTuple hv_ResultType = "real";		 //变换后的结果图类型
	HTuple hv_FilterType = "高斯滤波器";		 //卷积类型
	HTuple hv_Sigma = 1.0;			 //高斯平滑阈值
	HTuple hv_MaskWidth = 3;		 //均值滤波器宽
	HTuple hv_MaskHeight = 3;		 //均值滤波器高	 
	HTuple hv_Frequency = 0.1;		 //高通或低通或者正弦截止频率(范围0-1)
	HTuple hv_BandMinFrequency = 0.1;		 //带通最小频率(范围0-1)
	HTuple hv_BandMaxFrequency = 0.2;		 //带通最大频率(范围0-1)
	HTuple hv_GaussBandMinSigma = 3.0;	//高斯带通最小Sigma
	HTuple hv_GaussBandMaxSigma = 10.0;	//高斯带通最大Sigma
};

// 运行参数结构体
struct RunParamsStruct_ImgClassifyDetect
{
	HTuple hv_ClassMethod = "MLP";	//MLP或者SVM
	vector<HTuple> hv_ImgFilePaths;	//训练图片文件夹数组
	vector<HTuple> hv_ClassNames;	//训练名称数组
	HTuple hv_ClassifyHandle;	//训练句柄
	HTuple hv_ErrorData;	//训练误差
};
// 结果参数结构体
struct ResultParamsStruct_ImgClassifyDetect
{
	HTuple hv_RetName;	//识别结果名称
	HTuple hv_Confidence;	//识别结果置信度
};

//图像变换类型结构体
struct TypeStruct_ImgConvertDetect
{
	HTuple RGB_ToGray = "彩色转灰度图";
	HTuple Img_ToRGB = "彩色转RGB";
	HTuple RGB_ToHSV = "彩色转HSV";
	HTuple RGB_ToYUV = "彩色转YUV";
	HTuple RGB_ToHSI = "彩色转HSI";
};
//图像变换运行参数结构体
struct RunParamsStruct_ImgConvertDetect
{
	HTuple hv_ImgConvertDetectType;				//图像变换类型
};
//图像变换结果参数结构体
struct ResultParamsStruct_ImgConvertDetect
{
	HObject ho_GrayImage;  //灰度图像
	//RGB通道图像
	HObject ho_RgbR;
	HObject ho_RgbG;
	HObject ho_RgbB;
	//HSV通道图像
	HObject ho_HsvH;
	HObject ho_HsvS;
	HObject ho_HsvV;
	//HSI通道图像
	HObject ho_HsiH;
	HObject ho_HsiS;
	HObject ho_HsiI;
	//YUV通道图像
	HObject ho_YuvY;
	HObject ho_YuvU;
	HObject ho_YuvV;
};

//图像增强类型结构体
struct TypeStruct_ImgProcessOneDetect
{
	HTuple Gauss_Filter = "高斯滤波";
	HTuple Bilateral_Filter = "双边滤波";
	HTuple Mean_Image = "均值滤波";
	HTuple Median_Rect = "中值滤波";
	HTuple Shock_Filter = "冲击波滤波";
	HTuple Equ_Histo_Image = "直方图均衡";
	HTuple Emphasize = "对比度增强";
	HTuple Scale_Image = "灰度拉伸";
	HTuple Scale_ImageMax = "灰度拉伸至0-255";
	HTuple Invert_Image = "灰度反转";
	HTuple Log_Image = "对数变换";
	HTuple Exp_Image = "指数变换";
	HTuple GrayErosion_Rect = "灰度腐蚀";
	HTuple GrayDilation_Rect = "灰度膨胀";
	HTuple GrayOpening = "灰度开运算";
	HTuple GrayClosing = "灰度闭运算";
};
//图像增强运行参数结构体
struct RunParamsStruct_ImgProcessOneDetect
{
	HTuple hv_ProcessType;               //数组，图像增强类型
	HTuple hv_CoreWidth;			//数组，滤波核宽度
	HTuple hv_CoreHeight;			//数组，滤波核高度
	HTuple hv_Sigma;				//数组，平滑阈值
	HTuple hv_BilateralFilterThreshold; //数组，双边滤波的阈值(凡是灰度值之差小于此值的像素，都会被平滑)
	HTuple hv_Scale_Image_MultValue;	//数组，灰度拉伸乘数因子
	HTuple hv_Scale_Image_AddValue;	//数组，灰度拉伸加数因子

	HObject ho_SearchRegion;			 //搜索区域
};

//图像增强类型结构体
struct TypeStruct_ImgProcessTwoDetect
{
	HTuple Add_Img = "图像相加";
	HTuple Sub_Img = "图像相减";
	HTuple Mult_Img = "图像相乘";
	HTuple Div_Img = "图像相除";
	HTuple Min_Img = "两图灰度最小";
	HTuple Max_Img = "两图灰度最大";
};
//图像增强运行参数结构体
struct RunParamsStruct_ImgProcessTwoDetect
{
	HTuple hv_ProcessType = "null";       //图像处理类型

	HTuple hv_MultValue = 1;	    //乘数因子
	HTuple hv_AddValue = 0;		   //加数因子
};

// 类型结构体
struct TypeStruct_ImgRotateDetect
{
	HTuple hv_Mirror = "镜像";		//镜像
	HTuple hv_MirrorRow = "row";
	HTuple hv_MirrorColumn = "column";

	HTuple hv_Rotate = "旋转";		//旋转
};
// 运行参数结构体
struct RunParamsStruct_ImgRotateDetect
{
	HTuple hv_Type = "旋转";
	HTuple hv_Angle = 0;			//旋转角度（是角度，不是弧度）
	HTuple hv_MirrorDirection = "row";//镜像方向
};

//类型结构体
struct TypeStruct_ImgSaveDetect
{
	string hv_SaveOk = "SaveOk";
	string hv_SaveNg = "SaveNg";
	string hv_SaveOkAndNg = "SaveOkAndNg";
	string hv_SaveNone = "SaveNone";
	string hv_FileName_Id_Time = "Id_Time";
};
// 运行参数结构体
struct RunParamsStruct_ImgSaveDetect
{
	HTuple IsOk = 0;			//是否OK,0为ok,非0为NG
	string hv_SavePath = "C:\\ImgSave";//存图路径
	string hv_SaveType = "bmp";	//存图类型bmp, jpeg, png, tiff
	string hv_SaveMethod = "SaveOkAndNg";//存图策略(默认OK和NG都保存)
	string hv_FileName = "Id_Time";//文件名称
};

// 运行参数结构体
struct RunParamsStruct_ImgShowDetect
{
	vector<HObject> vec_InputRegion;		//输入区域数组
	vector<HObject> vec_InputXld;		//输入XLD数组
	vector<HTuple> vec_Ret;		//输入结果数组，1是OK，非1是NG
	HTuple hv_Ret = 0;		//1是OK,非1是NG
	HTuple hv_LineWidth = 2;	//绘制区域的线宽
	HTuple hv_Word_X = 10;		//绘制文字的X坐标
	HTuple hv_Word_Y = 10;		//绘制文字的Y坐标
	HTuple hv_Word_Size = 10;	//文字的字号
};
// 结果参数结构体
struct ResultParamsStruct_ImgShowDetect
{
	HObject ho_DestImg;		//输出图片
};

//交点类型结构体
struct TypeStruct_IntersectionDetect
{
	HTuple hv_LineLine = "直线直线交点";
	HTuple hv_SegmentLine = "线段直线交点";
	HTuple hv_SegmentSegment = "线段线段交点";
	HTuple hv_LineCircle = "直线圆交点";
	HTuple hv_SegmentCircle = "线段圆交点";
	HTuple hv_CircleCircle = "圆圆交点";
	HTuple hv_LineEllipse = "直线椭圆交点";
	HTuple hv_SegmentEllipse = "线段椭圆交点";
	HTuple hv_NULL = "null";
};
//交点运行参数结构体
struct RunParamsStruct_IntersectionDetect
{
	HTuple hv_IntersectType = "null";
	ResultParamsStruct_Line LineA;
	ResultParamsStruct_Line LineB;
	ResultParamsStruct_Circle CircleA;
	ResultParamsStruct_Circle CircleB;
	ResultParamsStruct_Ellipse EllipseA;
};
//交点运行结果结构体
struct ResultParamsStruct_IntersectionDetect
{
	//交点数量
	HTuple hv_PointNum = 0;
	//交点坐标
	HTuple hv_Row1_Result;
	HTuple hv_Column1_Result;
	HTuple hv_Row2_Result;
	HTuple hv_Column2_Result;

};

//运行参数结构体
struct RunParamsStruct_Judgement
{
	vector<string> hv_GlobalName;
	string hv_AndOr = "and";
	vector<HTuple> hv_CurrValue;
	vector<double> hv_MinValue;
	vector<double> hv_MaxValue;
	vector<HTuple> hv_LabelStr;
};

//测量类型结构体
struct TypeStruct_MeasurementDetect
{
	HTuple hv_DistanceSegmentLine = "线段直线距离";
	HTuple hv_DistanceSegmentSegment = "线段线段距离";
	HTuple hv_DistanceSegmentCircle = "线段圆距离";
	HTuple hv_DistanceLineCircle = "直线圆距离";
	HTuple hv_DistanceCircleCircle = "圆圆距离";
	HTuple hv_DistancePointPoint = "点点距离";
	HTuple hv_DistancePointLine = "点直线距离";
	HTuple hv_DistancePointSegment = "点线段距离";
	HTuple hv_AngleLineLine = "直线直线角度";
	HTuple hv_AngleLineHorizon = "直线水平线角度";

	HTuple hv_DistanceContourContour = "轮廓轮廓距离";
	HTuple hv_DistanceLineContour = "直线轮廓距离";
	HTuple hv_DistanceSegmentContour = "线段轮廓距离";
	HTuple hv_DistancePointContour = "点轮廓距离";

	HTuple hv_DistanceRegionRegion = "区域区域距离";
	HTuple hv_DistanceLineRegion = "直线区域距离";
	HTuple hv_DistanceSegmentRegion = "线段区域距离";
	HTuple hv_DistancePointRegion = "点区域距离";
	HTuple hv_NULL = "null";
};
//测量运行参数结构体
struct RunParamsStruct_MeasurementDetect
{
	HTuple hv_MeasureType = "null";			//测量类型
	HTuple hv_PixelMm = 1;					//像素当量

	ResultParamsStruct_Line LineA;			//输入直线1
	ResultParamsStruct_Line LineB;			//输入直线2
	ResultParamsStruct_Circle CircleA;		//输入圆1
	ResultParamsStruct_Circle CircleB;		//输入圆2
	HTuple hv_Row_PointA;					//输入点1行坐标
	HTuple hv_Col_PointA;					//输入点1列坐标
	HTuple hv_Row_PointB;					//输入点1行坐标
	HTuple hv_Col_PointB;				    //输入点1列坐标

	HObject hv_Contour1;					//输入轮廓1
	HObject hv_Contour2;					//输入轮廓2
	HObject hv_Region1;					    //输入区域1
	HObject hv_Region2;						//输入区域2
};
//测量运行结果结构体
struct ResultParamsStruct_MeasurementDetect
{
	HTuple hv_DistanceMin;
	HTuple hv_DistanceMax;
	HTuple hv_Angle;
	HObject ho_OutObj;
};

//光度立体卷积类型结构体
struct TypeStruct_PhotoStereo
{
	HTuple GaussCurvature = "高斯曲率";
	HTuple MeanCurvature = "平均曲率";
	HTuple Curl = "向量旋度";
	HTuple Divergence = "向量散度";
};
//运行参数结构体
struct RunParamsStruct_PhotoStereo
{
	HTuple hv_PhotoStereoType = "高斯曲率";		//卷积类型
	HTuple hv_Sigma = 1.0;			//平滑阈值
	//光源入射光线与相机中轴线夹角
	HTuple hv_Slants1 = 45;
	HTuple hv_Slants2 = 45;
	HTuple hv_Slants3 = 45;
	HTuple hv_Slants4 = 45;
	//光束中轴线与产品夹角(俯视图，光从图像右侧打过来，角度就是0°，从上面打过来，角度是90°，从左面打过来，角度是180°，下边打过来是270°)
	HTuple hv_Tilts1 = 0;
	HTuple hv_Tilts2 = 90;
	HTuple hv_Tilts3 = 180;
	HTuple hv_Tilts4 = 270;
};

//253010 视觉模块更新，和之前PIN冲突，改名加个_New
//Pin针检测类型参数结构体
struct TypeParamsStruct_Pin_New
{
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
struct RunParamsStruct_Pin_New
{
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
};
//Pin针检测结果结构体
struct ResultParamsStruct_Pin_New
{
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
};

//Pcb检测类型参数结构体
struct TypeParamsStruct_Pcb
{
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
//Pcb检测运行参数结构体
struct RunParamsStruct_Pcb
{
	//控制参数
	HTuple hv_Tolerance_X = 20;		//X方向允许的偏差范围
	HTuple hv_Tolerance_Y = 20;		//Y方向允许的偏差范围
	HTuple hv_Tolerance_A = 360;	//角度允许的偏差范围
	HTuple hv_Tolerance_Area = 999999999;	//面积允许的偏差范围
	HTuple hv_MmPixel = 0.01;		//像素当量
	HTuple hv_StandardX_Array;	//X方向基准参数(数组)
	HTuple hv_StandardY_Array;	//Y方向基准参数(数组)
	HTuple hv_StandardA_Array;	//角度基准参数(数组)
	HTuple hv_StandardArea_Array;	//面积基准参数(数组)

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
};
//Pcb检测结果结构体
struct ResultParamsStruct_Pcb
{
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

	HObject ho_Region_AllPcb;				 //所有pin针区域
	HObject ho_Region_OKPcb;				//OK的pin针区域
	HObject ho_Region_NGPcb;				//NG的pin针区域
	HTuple hv_Row_Array;					   //pin针区域中心行坐标
	HTuple hv_Column_Array;					   //pin针区域中心列坐标
	HTuple hv_Angle_Array;					   //pin针区域角度
	HTuple hv_Area_Array;					   //pin针区域面积
	HTuple hv_DisX_Pixel_Array;                  //pin针区域中心相对基准的X方向距离,像素
	HTuple hv_DisY_Pixel_Array;				  //pin针区域中心相对基准的Y方向距离,像素
	HTuple hv_OffsetX_Pixel_Array;              //pin针区域中心相对基准的X方向偏差值,像素
	HTuple hv_OffsetY_Pixel_Array;			 //pin针区域中心相对基准的Y方向偏差值,像素
	HTuple hv_DisX_MM_Array;                  //pin针区域中心相对基准的X方向距离,毫米
	HTuple hv_DisY_MM_Array;				  //pin针区域中心相对基准的Y方向距离,毫米
	HTuple hv_OffsetX_MM_Array;              //pin针区域中心相对基准的X方向偏差值,毫米
	HTuple hv_OffsetY_MM_Array;			 //pin针区域中心相对基准的Y方向偏差值,毫米
	HTuple hv_OffsetA_Array;//数组，角度偏差
	HTuple hv_OffsetArea_Array;//数组，面积偏差
};


//极坐标变换运行参数结构体
struct RunParamsStruct_PolarTransDetect
{
	//参数
	HTuple hv_InnerRadius = 50;		//圆环内圆半径
	HTuple hv_OuterRadius = 150;	//圆环外圆半径
	HTuple hv_Row = 100;			//圆心行坐标
	HTuple hv_Column = 100;			//圆心列坐标

	//原始图像的宽高
	HTuple hv_OriginalWidth;
	HTuple hv_OriginalHeight;

};

//运行参数结构体
struct RunParamsStruct_PrintDetection
{
	//搜索区域
	HObject ho_SearchRegion;
	//训练区域
	HObject ho_TrainRegion;
	//变形模板模型句柄
	HTuple hv_Deformable_ID;
	//差异模板模型句柄
	HTuple hv_Variation_ID;

	string toolName;			//直线名称
	string jobName;                      //流程名称

};
//结果参数结构体
struct ResultParamsStruct_PrintDetection
{
	//变形模板匹配轮廓
	HObject ho_Deformed_Contours;
	//缺陷区域
	HObject ho_DirtyRegion;
	//缺陷面积
	HTuple hv_DirtyArea;
	//变形矫正后的图像
	HObject ho_Rectified_Img;
	//OK数量
	HTuple hv_RetNum = 0;

};

//极坐标变换运行参数结构体
struct RunParamsStruct_ProjectionTrans
{
	//参数
	HTuple hv_Row_LeftTop;		//投影变换四边形左上角行坐标			
	HTuple hv_Column_LeftTop;		//投影变换左上角列坐标		
	HTuple hv_Row_RightTop;		//投影变换四边形右上角行坐标			
	HTuple hv_Column_RightTop;	//投影变换右上角列坐标	
	HTuple hv_Row_RightBottom;	//投影变换四边形右下角行坐标			
	HTuple hv_Column_RightBottom;	//投影变换右下角列坐标	
	HTuple hv_Row_LeftBottom;		//投影变换四边形左下角行坐标			
	HTuple hv_Column_LeftBottom;	//投影变换左下角列坐标	

	string toolName;			//工具名称
	string jobName;                      //流程名称
};

//二维码类型结构体
struct CodeTypeStruct_Code2d
{
	HTuple DM = "Data Matrix ECC 200";
	HTuple QR = "QR Code";
	HTuple Aztec = "Aztec Code";
	HTuple GS1Aztec = "GS1 Aztec Code";
	HTuple GS1DM = "GS1 DataMatrix";
	HTuple GS1QR = "GS1 QR Code";
	HTuple MicroQR = "Micro QR Code";
	HTuple PDF417 = "PDF417";
};
//二维码检测运行参数结构体
struct RunParamsStruct_Code2d
{
	HObject ho_SearchRegion;					//搜索区域
	HTuple hv_CodeType_Run = "QR Code";			//条码类型(默认"Data Matrix ECC 200")
	HTuple hv_CodeNum = 1;					//识别数量(设置为0，则扫描所有条码)
	HTuple hv_Tolerance = "high";               //识别容差(默认高级别，默认high，还可以设置"low")
	HTuple hv_HeadChar = "null";					//特定字符开头
	HTuple hv_EndChar = "null";						//特定字符结尾
	HTuple hv_ContainChar = "null";					//包含特定字符
	HTuple hv_NotContainChar = "null";				//不包含特定字符
	HTuple hv_CodeLength_Run = 2;			    //条码最小长度
	HTuple hv_TimeOut = 50;					//超时时间(单位ms)
	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，区域跟随
	bool isFollow = false;                //是否跟随
};
//二维码识别结果结构体
struct ResultParamsStruct_Code2d
{
	//条码结果参数
	HTuple hv_CodeType_Result;         //二维码类型
	HTuple hv_CodeContent;			  //二维码内容
	HTuple hv_CodeLength_Result;	  //二维码长度
	HObject ho_ContourCode2d;		  //二维码轮廓
	HObject ho_RegionCode2d;		  //二维码区域
	HTuple hv_Row;                    //二维码中心行坐标
	HTuple hv_Column;                 //二维码中心列坐标
	HTuple hv_RetNum = 0;			 //找到的二维个数(默认是0，识别到二维码后大于0)
	HTuple hv_QualityGrade;		    //条码质量等级0-4(iso15416)
	~ResultParamsStruct_Code2d()
	{
		ho_ContourCode2d.Clear();
		ho_RegionCode2d.Clear();
	}

};

//以下结构体和 RobotCalibDetect 里的结构体冲突，先屏蔽这边 20250507
////九点标定类型结构体
//struct RunParamsStruct_RobotCalib
//{
//	//机器人标定参数
//	HTuple hv_NPointCamera_X;	  //九点标定像素坐标X
//	HTuple hv_NPointCamera_Y;    //九点标定像素坐标Y
//	HTuple hv_NPointRobot_X;	 //九点标定机器坐标X
//	HTuple hv_NPointRobot_Y;	 //九点标定机器坐标Y
//	HTuple hv_HomMat2D = NULL;	 //九点标定矩阵
//	HTuple hv_CenterCamera_X;	 //旋转中心标定像素坐标X
//	HTuple hv_CenterCamera_Y;    //旋转中心标定像素坐标Y
//	HTuple hv_CenterRobot_X;	//旋转中心标定机器坐标X
//	HTuple hv_CenterRobot_Y;    //旋转中心标定机器坐标Y
//	HTuple hv_CenterRobot_A;    //旋转中心标定机器坐标A
//	HTuple hv_Center_X;			//机器坐标旋转中心X
//	HTuple hv_Center_Y;			//机器坐标旋转中心Y
//	HTuple hv_Center_Radius;	//机器坐标旋转半径
//	HObject ho_CenterCircle;	//旋转中心的圆
//
//
//	//是否计算偏移量还是机器人绝对坐标
//	bool hv_IsAbsCoordinate = false;
//	//九宫格相邻两点之间距离(单位mm)
//	HTuple hv_DistancePP_Robot = 10.0;
//	//旋转中心标定时，每次旋转的角度
//	HTuple hv_AngleRotate_Robot = 10;
//	//旋转中心标定时，机器人绝对坐标
//	HTuple hv_CenterPosition_RobotX = 0.000;
//	HTuple hv_CenterPosition_RobotY = 0.000;
//	HTuple hv_CenterPosition_RobotA = 0.000;
//	//建模位机器人绝对坐标
//	HTuple hv_ModelPosition_RobotX = 0.000;
//	HTuple hv_ModelPosition_RobotY = 0.000;
//	HTuple hv_ModelPosition_RobotA = 0.000;
//	//建模位物料上面Mark点坐标
//	HTuple hv_ModelX = 0.000;	//模板位置X
//	HTuple hv_ModelY = 0.000;	//模板位置Y
//	HTuple hv_ModelA = 0.000;	//模板位置A
//	//待纠偏物料上面Mark点当前坐标
//	HTuple hv_NowX = 0.000;		//当前位置X
//	HTuple hv_NowY = 0.000;		//当前位置Y
//	HTuple hv_NowA = 0.000;		//当前位置A
//	bool hv_IsCamFirst = true;  //先拍后抓或者先抓后拍
//	//补偿角度是否取反(当图像中角度正方向与机器人的旋转角度正反向相反时，需要把补偿角度取反),true是取反，false是不取反
//	bool hv_IsReverseAngle = false;
//	//是否启用旋转中心 true是启用, false不启用
//	bool hv_IsUseCenterCalib = true;
//
//	//计算绝对坐标输入参数
//	HTuple hv_AbsInputX = 0.000;	//输入的像素绝对坐标X
//	HTuple hv_AbsInputY = 0.000;	//输入的像素绝对坐标Y
//	HTuple hv_AbsInputA = 0.000;	//输入的像素绝对坐标A
//
//};
////计算偏移量或者绝对坐标结果结构体
//struct ResultParamsStruct_RobotCalib
//{
//	//偏移量结果参数
//	HTuple hv_OffsetX = -1;	//偏移量X
//	HTuple hv_OffsetY = -1;	//偏移量Y
//	HTuple hv_OffsetA = -1;	//偏移量A
//
//	//绝对坐标结果参数
//	HTuple hv_AbsOutputX = -1;	//机器人绝对坐标X
//	HTuple hv_AbsOutputY = -1;	//机器人绝对坐标Y
//	HTuple hv_AbsOutputA = -1;	//机器人绝对坐标A
//
//};

//仿射变换类型结构体
struct TypeStruct_ShapeTransDetect
{
	HTuple hv_Null = "null";
	HTuple hv_Convex = "convex"; //"凸包面";
	HTuple hv_Ellipse = "ellipse"; //"最小外接椭圆"
	HTuple hv_Inner_Circle = "inner_circle"; // "最大内接圆"
	HTuple hv_Outer_Circle = "outer_circle"; // "最小外接圆"
	HTuple hv_Inner_Rectangle1 = "inner_rectangle1"; // "最大内接矩形"
	HTuple hv_Rectangle1 = "rectangle1"; // "最小外接矩形"
	HTuple hv_Rectangle2 = "rectangle2"; // "最小外接旋转矩形"
};
// 运行参数结构体
struct RunParamsStruct_ShapeTransDetect
{
	HTuple hv_TypeTrans = "convex";		//转换类型
};
//结果参数结构体
struct ResultParamsStruct_ShapeTransDetect
{
	HObject hv_DestRegion;					 //输出区域

	HTuple hv_Row1_Rectangle1;			      //矩形的左上角点行坐标
	HTuple hv_Column1_Rectangle1;			  //矩形的左上角点列坐标
	HTuple hv_Row2_Rectangle1;			      //矩形的右下角点行坐标
	HTuple hv_Column2_Rectangle1;			  //矩形的右下角点列坐标

	HTuple hv_Row_Circle;					 //圆形的中心点行坐标
	HTuple hv_Column_Circle;				 //圆形的中心点列坐标
	HTuple hv_Radius_Circle;				 //圆形的半径

	HTuple hv_Row_Rectangle2;			      //旋转矩形的中心点行坐标
	HTuple hv_Column_Rectangle2;			  //旋转矩形的中心点列坐标
	HTuple hv_Length1_Rectangle2;			  //旋转矩形的半长
	HTuple hv_Length2_Rectangle2;			  //旋转矩形的半宽
	HTuple hv_Phi_Rectangle2;			      //旋转矩形的角度

	HTuple hv_Row_Ellipse;					 //椭圆的中心点行坐标
	HTuple hv_Column_Ellipse;				 //椭圆的中心点列坐标
	HTuple hv_Radius1_Ellipse;				 //椭圆的长半轴
	HTuple hv_Radius2_Ellipse;				 //椭圆的短半轴
	HTuple hv_Phi_Ellipse;					 //椭圆的角度

	HTuple hv_Row_Convex;					 //凸包的中心点行坐标
	HTuple hv_Column_Convex;				 //凸包的中心点列坐标
};

//模板匹配类型结构体
struct ModelTypeStruct_Template
{
	//模板类型:"none"无缩放,"scaled"同步缩放,"aniso"异步缩放,"ncc"基于互相关
	HTuple None = "none";
	HTuple Scaled = "scaled";
	HTuple Aniso = "aniso";
	HTuple Ncc = "ncc";
	//模板参数类型
	HTuple Auto = "auto";
};
// 模板匹配参数结构体
struct RunParamsStruct_Template
{
	HTuple hv_MatchMethod = "scaled";      //模板类型
	HTuple hv_NumLevels = "auto";              //金字塔级别
	HTuple hv_AngleStart = -30;                //起始角度
	HTuple hv_AngleExtent = 30;                //角度范围
	HTuple hv_AngleStep = "auto";              //角度步长
	HTuple hv_ScaleRMin = 0.9;                 //最小行缩放(ScaleModel模式的时候hv_ScaleRMin_Train代表X和Y方向最小缩放系数)
	HTuple hv_ScaleRMax = 1.1;                 //最大行缩放(ScaleModel模式的时候hv_ScaleRMax_Train代表X和Y方向最大缩放系数)
	HTuple hv_ScaleRStep = "auto";             //行方向缩放步长
	HTuple hv_ScaleCMin = 0.9;                 //最小列缩放
	HTuple hv_ScaleCMax = 1.1;				   //最大列缩放
	HTuple hv_ScaleCStep = "auto";             //列方向缩放步长
	HTuple hv_Optimization = "auto";           //优化算法
	HTuple hv_Metric = "use_polarity";         //极性/度量
	HTuple hv_Contrast = "auto";               //对比度
	HTuple hv_MinContrast = "auto";            //最小对比度

	HTuple hv_Angle_Original = 0;			 //创建模板时，原始模板角度
	HTuple hv_CenterRow_Original = 0;		//创建模板时，原始模板Y
	HTuple hv_CenterCol_Original = 0;		//创建模板时，原始模板X

	HTuple hv_MinScore = 0.5;               //最低分数
	HTuple hv_NumMatches = 1;               //匹配个数，0则自动选择，100则最多匹配100个
	HTuple hv_MaxOverlap = 0.5;             //要找到模型实例的最大重叠
	HTuple hv_SubPixel = "least_squares";   //亚像素精度
	HTuple hv_Greediness = 0.9;             //贪婪系数

	HTuple hv_ModelID;                     //模板句柄
	HObject ho_TrainRegion;				   //模板训练区域
	HObject ho_CropModelImg;			   //模板训练小图
	HObject ho_SearchRegion;			  //搜索区域
	HTuple hv_TimeOut = 1000;			//超时时间(单位ms)
};

// 查找模板结果结构体
struct ResultParamsStruct_Template
{
	//建模时的坐标和角度
	HTuple hv_Angle_Original = 0;			 //创建模板时，原始模板角度
	HTuple hv_CenterRow_Original = 0;		//创建模板时，原始模板Y
	HTuple hv_CenterCol_Original = 0;		//创建模板时，原始模板X
	//运行时找到的模板结果
	HTuple hv_Row;                    //中心点（X）
	HTuple hv_Column;                 //中心点（Y）
	HTuple hv_Angle;                  //角度
	HTuple hv_ScaleR;                 //行缩放(ScaleModel模式的时候hv_ScaleR代表X和Y方向缩放系数)
	HTuple hv_ScaleC;                 //列缩放
	HTuple hv_Score;                  //匹配分数
	HTuple hv_RetNum = 0;             //找到的模板个数(默认0，找到模板后会大于0)

	HObject ho_Contour_Template;      // 模板轮廓
	HObject ho_Region_Template;      // 模板轮廓
	//仿射变换矩阵(用于抓边跟随)
	HTuple hv_ROIHomMat2D;

	void clear()
	{
		ho_Contour_Template.Clear();
		ho_Region_Template.Clear();
	};

	~ResultParamsStruct_Template()
	{
		clear();
	}

};



//差分算法类型结构体
struct ModelTypeStruct_ImgDifference
{
	//模板类型:"none"无缩放,"scaled"同步缩放,"aniso"异步缩放,"ncc"基于互相关
	HTuple None = "none";
	HTuple Scaled = "scaled";
	HTuple Aniso = "aniso";
	HTuple Ncc = "ncc";
	//模板参数类型
	HTuple Auto = "auto";
};
// 差分算法参数结构体
struct RunParamsStruct_ImgDifference
{
	//模板匹配参数
	HTuple hv_IsUsSearchRoi = 0;//0 不启用搜索区域，1启用搜索区域
	HTuple hv_MatchMethod = "scaled";      //模板类型
	HTuple hv_NumLevels = "auto";              //金字塔级别
	HTuple hv_AngleStart = -90;                //起始角度
	HTuple hv_AngleExtent = 90;                //角度范围
	HTuple hv_AngleStep = "auto";              //角度步长
	HTuple hv_ScaleRMin = 0.9;                 //最小行缩放(ScaleModel模式的时候hv_ScaleRMin_Train代表X和Y方向最小缩放系数)
	HTuple hv_ScaleRMax = 1.1;                 //最大行缩放(ScaleModel模式的时候hv_ScaleRMax_Train代表X和Y方向最大缩放系数)
	HTuple hv_ScaleRStep = "auto";             //行方向缩放步长
	HTuple hv_ScaleCMin = 0.9;                 //最小列缩放
	HTuple hv_ScaleCMax = 1.1;				   //最大列缩放
	HTuple hv_ScaleCStep = "auto";             //列方向缩放步长
	HTuple hv_Optimization = "auto";           //优化算法
	HTuple hv_Metric = "use_polarity";         //极性/度量
	HTuple hv_Contrast = "auto";               //对比度
	HTuple hv_MinContrast = "auto";            //最小对比度

	HTuple hv_Angle_Original = 0;			 //创建模板时，原始模板角度
	HTuple hv_CenterRow_Original = 0;		//创建模板时，原始模板Y
	HTuple hv_CenterCol_Original = 0;		//创建模板时，原始模板X

	HTuple hv_MinScore = 0.5;               //最低分数
	HTuple hv_NumMatches = 1;               //匹配个数，0则自动选择，100则最多匹配100个
	HTuple hv_MaxOverlap = 0.5;             //要找到模型实例的最大重叠
	HTuple hv_SubPixel = "none";   //亚像素精度
	HTuple hv_Greediness = 0.5;             //贪婪系数

	HTuple hv_ModelID = NULL;                     //模板句柄
	HObject ho_TrainRegion;				   //模板训练区域
	HObject ho_CropModelImg;			   //模板训练小图
	HObject ho_ModelImg;			       //模板训练大图
	HObject ho_SearchRegion;			  //搜索区域
	HTuple hv_TimeOut = 1000;			//超时时间(单位ms)
	//缺陷过滤参数
	HTuple hv_IsUseGauss = 0;//是否启用高斯滤波，1 使用，0 不使用
	HTuple hv_Sigma = 3;
	HTuple hv_Mult = 1;
	HTuple hv_Add = 0;
	HTuple hv_FilterGrayMin = 0;
	HTuple hv_FilterGrayMax = 128;
	HTuple hv_FilterAreaMin = 100;
	HTuple hv_FilterAreaMax = 999999999;
	HTuple hv_FilterWidthMin = 1;
	HTuple hv_FilterWidthMax = 999999999;
	HTuple hv_FilterHeightMin = 1;
	HTuple hv_FilterHeightMax = 999999999;
};

// 差分算法结果结构体
struct ResultParamsStruct_ImgDifference
{
	//运行时找到的模板结果
	HTuple hv_Row;                    //中心点（X）
	HTuple hv_Column;                 //中心点（Y）
	HTuple hv_Angle;                  //角度
	HTuple hv_Score;                  //匹配分数
	HTuple hv_RetNum = 0;             //找到的模板个数(默认0，找到模板后会大于0)

	HObject ho_Contour_Template;      // 模板轮廓
	HObject ho_Region_Template;      // 模板轮廓
	HObject ho_ErrorRegion;	//缺陷区域
	HTuple hv_ErrorNum = 0; //缺陷数量
};


//运行参数结构体
struct RunParamsStruct_CharacterCheckDetect
{
	HTuple hv_InputValue1;//输入值1
	HTuple hv_InputValue2;//输入值2
};
//运行参数结构体
struct ResultParamsStruct_CharacterCheckDetect
{
	HTuple hv_Ret;//输入值1
};

//字符检测运行参数结构体
struct RunParamsStruct_CharacterDetect
{
	//搜索区域
	HObject ho_SearchRoi;
	HObject ho_ModelImg;
	vector<HObject> ModelCutImg;
	vector<string> ModelCutImgPath;
	vector<HObject> TrainRegions;
	vector<HObject> VariationModelROIs;
	vector<HTuple> Ori_RowRefs;
	vector<HTuple> Ori_ColRefs;
	vector<HTuple> ShapeModel_ID;
	vector<HTuple> Ori_Widths_Rec;
	vector<HTuple> Ori_Heights_Rec;
	vector<HTuple> Ori_Row1s_Rec;
	vector<HTuple> Ori_Col1s_Rec;
	vector<HTuple> Ori_Row2s_Rec;
	vector<HTuple> Ori_Col2s_Rec;
	vector<HTuple> VarModel_ID;
	HObject ModelRegions;

	//形状模板参数
	int hv_NumLevels = 1;
	double hv_AngleStart = -10;
	double hv_AngleExtent = 20;
	double hv_ScaleRMin = 1;
	double hv_ScaleRMax = 1;
	double hv_ScaleCMin = 1;
	double hv_ScaleCMax = 1;
	string hv_Metric = "use_polarity";
	int hv_Contrast = 30;
	int hv_ContrastMin = 10;
	int hv_SizeMin = 10;
	double hv_ScoreMin = 0.5;
	int hv_NumMatches = 1;
	double hv_MaxOverlap = 0.9;
	string hv_SubPixel = "least_squares";
	double hv_Greediness = 0.5;
	//差异模型参数
	string hv_ErrorType = "light";//"light","dark","light_dark"
	int hv_LightDirtContrast = 30;
	double hv_LightVarRadio = 2;
	int hv_DarkDirtContrast = 30;
	double hv_DarkVarRadio = 2;
	double hv_MinArea = 10;
	double hv_MaxArea = 10000000;
	double hv_EdgeTolerance = 0;
	string OkImgFilePath = "";
	//模型数量
	int hv_ModelNum = 0;
	//图像处理参数
	string hv_FilterType = "gauss_filter";//类型包括"gauss_filter"，"bilateral_filter","mean_filter","median_filter"
	double hv_Sigma = 3;
	int hv_BilateralContrast = 20;
	int hv_MaskWidth = 3;
	int MaskHeight = 3;
	int hv_IsUseFilter = 0;
	//灰度值归一化参数
	int hv_IsUseNormalize = 0;
	vector<HTuple> hv_ForwardGrayValue;
	vector<HTuple> hv_BackGroundGrayValue;
};
//字符检测结果参数结构体
struct ResultParamsStruct_CharacterDetect
{
	HTuple b_IsOk = 1;//结果(1 是OK,0 是NG)
	HObject ShapeModelContours;	//模板轮廓
	HObject ErrorRegions;		//NG区域
	HTuple ErrorNum;					//NG数量
	HTuple hv_Area;
	HTuple hv_Row;
	HTuple hv_Column;
};

//灰度值检测运行参数结构体
struct RunParamsStruct_GrayValueDetect
{
	HTuple hv_HomMat2D = NULL;            //仿射变换矩阵，ROI跟随
	bool isFollow = false;                //是否跟随
	//搜索区域，可以是数组
	HObject ho_SearchRoi;
	//OK灰度阈值
	HTuple hv_MeanGrayValue_Standard;
	//灰度阈值上下限
	HTuple hv_GrayValueOffset_Min;
	HTuple hv_GrayValueOffset_Max;
};
//灰度值检测结果参数结构体
struct ResultParamsStruct_GrayValueDetect
{
	//搜索区域，可以是数组
	HObject ho_AllRegion;
	HObject ho_OKRegion;
	HObject ho_NGRegion;
	//计算的灰度均值，可以是数组
	HTuple hv_MeanGrayValue_Ret;
	//结果，0是ok，非0是ng
	HTuple hv_Ret;
	HTuple hv_RetArray;//结果数组，与搜索区域一一对应
};

//图像拆分运行参数结构体
struct RunParamsStruct_ImgSplitDetect
{
	//设定参数
	HTuple hv_CutRows_Ori = 5;//原始行
	HTuple hv_CutCols_Ori = 5;//原始列
	HTuple hv_OverlapRate = 0.2;//重叠率
	//计算参数
	HTuple hv_CutRows_Dest = 5;//结果行
	HTuple hv_CutCols_Dest = 5;//结果列
	HTuple hv_WidthImg = 4000;//大图的图像宽
	HTuple hv_HeightImg = 3000;//大图的图像高
	HTuple hv_CutWidth = 100;//小图宽
	HTuple hv_CutHeight = 100;//小图高
	//输入参数
	HObject ho_Image;//待拆分图
	HObject Vec_Image;//待拼接图像数组
	//判断参数
	HTuple hv_IsSplit = 0;//0是图像拆分，1是图像拼接
	//裁图数量
	HTuple hv_CutNum = 0;

};
//图像拆分结果参数结构体
struct ResultParamsStruct_ImgSplitDetect
{
	//拼图结果参数
	HObject ho_Img_Dest;//拼接好的大图
	//拆分结果参数
	HObject Vec_Img_Dest;//拆分好的图像数组
	//拆分小图矩形坐标
	HTuple hv_CutRow1_Array = 0;
	HTuple hv_CutColumn1_Array = 0;
	HTuple hv_CutRow2_Array = 0;
	HTuple hv_CutColumn2_Array = 0;
};

//运行参数结构体
struct RunParamsStruct_JointCalibrationDetect
{
	//标定输入参数
	HTuple hv_StartCamPar;//标定需要相机参数
	HTuple hv_CaltabDescrPath;//标定板描述文件路径(Halcon新式标定板，以".cpd"格式的)
	HTuple hv_CaltabThickness = 0.001;//标定板厚度(单位m,米)
	//标定结果
	HObject ho_CalibContours;// 标定结果路轮廓
	HTuple hv_CamParam;//标定后的相机内参
	HTuple hv_CamPose;//标定后的相机外参
	HTuple hv_Error;//标定误差(优化后投影的均方根误差（RMSE）以误差（以像素为单位）返回)
	HTuple hv_Resolution;//像素当量
	//输入待转换到标定板坐标系的像素坐标
	HTuple hv_InputX1 = 0.000;		//待转换的像素坐标X1
	HTuple hv_InputY1 = 0.000;		//待转换的像素坐标Y1

	HTuple hv_InputX2 = 0.000;		//待转换的像素坐标X2
	HTuple hv_InputY2 = 0.000;		//待转换的像素坐标Y2

	HTuple hv_InputX3 = 0.000;		//待转换的像素坐标X3
	HTuple hv_InputY3 = 0.000;		//待转换的像素坐标Y3

	HTuple hv_InputX4 = 0.000;		//待转换的像素坐标X4
	HTuple hv_InputY4 = 0.000;		//待转换的像素坐标Y4

	HTuple hv_InputX5 = 0.000;		//待转换的像素坐标X5
	HTuple hv_InputY5 = 0.000;		//待转换的像素坐标Y5

	HTuple hv_InputX6 = 0.000;		//待转换的像素坐标X6
	HTuple hv_InputY6 = 0.000;		//待转换的像素坐标Y6

	HTuple hv_InputX7 = 0.000;		//待转换的像素坐标X7
	HTuple hv_InputY7 = 0.000;		//待转换的像素坐标Y7

	HTuple hv_InputX8 = 0.000;		//待转换的像素坐标X8
	HTuple hv_InputY8 = 0.000;		//待转换的像素坐标Y8

	HTuple hv_InputX9 = 0.000;		//待转换的像素坐标X9
	HTuple hv_InputY9 = 0.000;		//待转换的像素坐标Y9

	HTuple hv_InputX10 = 0.000;		//待转换的像素坐标X10
	HTuple hv_InputY10 = 0.000;		//待转换的像素坐标Y10
};
//结果参数结构体
struct ResultParamsStruct_JointCalibrationDetect
{
	HTuple hv_OutputX1 = 0.000;	//转换后的坐标X1
	HTuple hv_OutputY1 = 0.000;	//转换后的坐标Y1

	HTuple hv_OutputX2 = 0.000;	//转换后的坐标X2
	HTuple hv_OutputY2 = 0.000;	//转换后的坐标Y2

	HTuple hv_OutputX3 = 0.000;	//转换后的坐标X3
	HTuple hv_OutputY3 = 0.000;	//转换后的坐标Y3

	HTuple hv_OutputX4 = 0.000;	//转换后的坐标X4
	HTuple hv_OutputY4 = 0.000;	//转换后的坐标Y4

	HTuple hv_OutputX5 = 0.000;	//转换后的坐标X5
	HTuple hv_OutputY5 = 0.000;	//转换后的坐标Y5

	HTuple hv_OutputX6 = 0.000;	//转换后的坐标X6
	HTuple hv_OutputY6 = 0.000;	//转换后的坐标Y6

	HTuple hv_OutputX7 = 0.000;	//转换后的坐标X7
	HTuple hv_OutputY7 = 0.000;	//转换后的坐标Y7

	HTuple hv_OutputX8 = 0.000;	//转换后的坐标X8
	HTuple hv_OutputY8 = 0.000;	//转换后的坐标Y8

	HTuple hv_OutputX9 = 0.000;	//转换后的坐标X9
	HTuple hv_OutputY9 = 0.000;	//转换后的坐标Y9

	HTuple hv_OutputX10 = 0.000;	//转换后的坐标X10
	HTuple hv_OutputY10 = 0.000;	//转换后的坐标Y10

};

struct TypeParamsStruct_MachineLearning
{
	//模板匹配类型
	HTuple ShapeModel = "基于形状";
	HTuple NccModel = "基于互相关";
	//排序类型
	HTuple SortType_Row = "按行排序";//按行排序
	HTuple SortType_Column = "按列排序";//按列排序
	//ROI类型
	HTuple DetectROIType_Draw = "手绘ROI";	//手绘ROI
	HTuple DetectROIType_Blob = "Blob自动ROI";	//Blob自动ROI
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

	HTuple hv_Null = "null";
};
// 运行参数结构体
struct RunParamsStruct_MachineLearning
{
	//WeightTolerance指定每次迭代更改权重的阈值。这里，对两次迭代之间的权重变化的绝对值求和。
	//因此，该值取决于权重的数量以及权重的大小，而权重的大小又取决于训练数据的缩放。通常，应使用0.00001到1之间的值。
	//ErrorTolerance指定每次迭代的错误值更改的阈值。该值取决于训练样本的数量以及MLP的输出变量的数量。同样在这里，通常应使用0.00001和1之间的值。
	//如果权重变化小于WeightTolerance并且误差值的变化小于ErrorTolerance，则终止优化。在任何情况下，优化最多在MaxIterations迭代后终止。
	//NumHidden过小会导致损失函数达不到训练要求，NumHidden过大则会导致过拟合，泛性变差，对于未见过的样本推理差。

	//资源参数
	HTuple hv_ImgFilePathArray;	//训练图片文件夹数组
	vector<HObject> ho_DetectROIArray;	//所有的小图检测roi
	HTuple hv_ClassNameArray;	//训练名称数组
	HTuple hv_ClassIDArray;	//训练ID数组
	HTuple hv_NGID = -1;//指定NG的ID值(-1代表不指定)
	HTuple hv_IsUnionDetectROI = 0;//合并所有的检测区域(0 不合并，1 合并),例如色选机，目的是应对整图检测所有的虾脚，剔除NG。并没有指定ROI位置检测
	HTuple hv_IsAddNG = 0;//0是不增加尾部NG种类，1是增加尾部NG种类(增加尾部NG种类后，不允许再增加检测项。尾部NG种类不进行ROI设置，只是判断NG的id用)
	HTuple hv_IsZoomSize = 0;//0是不缩放，1是缩放
	HTuple hv_Zoom_Width = 640;//缩放宽
	HTuple hv_Zoom_Height = 640;//缩放高
	//MLP参数
	HTuple hv_Iterations = 200;//迭代次数
	HTuple hv_NumHidden = 15;//隐藏层数量
	HTuple hv_WeightTolerance = 1;//权重
	HTuple hv_ErrorTolerance = 0.0001;//最小误差
	HTuple hv_Rejection = "no_rejection_class";//设置拒绝项
	HTuple hv_ClassifyHandle = NULL;	//训练句柄
	HTuple hv_ErrorData = 1;	//训练误差
	HTuple hv_ErrorLog;//训练误差收敛函数

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

	//ROI参数
	HTuple ho_DetectROIType = "手绘ROI";	//检测roi的类型
	//二值化参数
	HObject ho_SearchRegion;	//搜索区域非数组
	HTuple hv_SortType;				//数组,pin区域排序方式
	HTuple hv_ThresholdType;		//数组,图像二值化类型
	HTuple hv_LowThreshold;				//数组,低阈值(固定阈值)
	HTuple hv_HighThreshold;				//数组,高阈值(固定阈值)
	HTuple hv_Sigma;						//数组,平滑阈值
	HTuple hv_CoreWidth;					//数组,动态阈值(滤波核宽度)
	HTuple hv_CoreHeight;					//数组,动态阈值(滤波核高度)
	HTuple hv_DynThresholdContrast;		//数组,动态阈值分割图像对比度
	HTuple hv_DynThresholdPolarity;	//数组,动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

	//筛选Blob参数
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
	HTuple hv_SelectAreaMax;	//选择区域内面积最大值的区域(0启用，1不启用)
	//形态学类型
	HTuple hv_OperatorType;//数组
	HTuple hv_OperaRec_Width;//数组
	HTuple hv_OperaRec_Height;//数组
	HTuple hv_OperaCir_Radius;//数组
	//孔洞填充面积
	HTuple hv_FillUpShape_Min;//数组
	HTuple hv_FillUpShape_Max;//数组
};
// 结果参数结构体
struct ResultParamsStruct_MachineLearning
{
	HTuple hv_Ret = 1;           //结果(1 OK, 0 NG)
	HTuple hv_RetNameArray;	//识别结果名称
	HTuple hv_RetIDArray;	//识别ID
	HTuple hv_IsOKArray;	//是否OK(OK, NG)
	HTuple hv_ConfidenceArray;	//识别结果置信度
	HObject ho_AllROIArray;//识别区域
	HObject ho_OKROIArray;//OK区域
	HObject ho_NGROIArray;//NG区域
	HObject ho_ModelXLD;//位置修正的模板轮廓

};

// 运行参数结构体
struct RunParamsStruct_MultCamCalib
{
	HTuple hv_NPointCamera_X;		 //九点标定像素坐标X
	HTuple hv_NPointCamera_Y;		 //九点标定像素坐标Y
	HTuple hv_NPointBoard_X;		 //九点标定标定板坐标X
	HTuple hv_NPointBoard_Y;		 //九点标定标定板坐标Y
	HTuple hv_HomMat2D = NULL;		 //九点标定矩阵

	HTuple hv_InputX1 = 0.000;		//待转换的像素坐标X1
	HTuple hv_InputY1 = 0.000;		//待转换的像素坐标Y1

	HTuple hv_InputX2 = 0.000;		//待转换的像素坐标X2
	HTuple hv_InputY2 = 0.000;		//待转换的像素坐标Y2

	HTuple hv_InputX3 = 0.000;		//待转换的像素坐标X3
	HTuple hv_InputY3 = 0.000;		//待转换的像素坐标Y3

	HTuple hv_InputX4 = 0.000;		//待转换的像素坐标X4
	HTuple hv_InputY4 = 0.000;		//待转换的像素坐标Y4
};

//结果结构体
struct ResultParamsStruct_MultCamCalib
{
	HTuple hv_OutputX1 = 0.000;	//转换后的坐标X1
	HTuple hv_OutputY1 = 0.000;	//转换后的坐标Y1

	HTuple hv_OutputX2 = 0.000;	//转换后的坐标X2
	HTuple hv_OutputY2 = 0.000;	//转换后的坐标Y2

	HTuple hv_OutputX3 = 0.000;	//转换后的坐标X3
	HTuple hv_OutputY3 = 0.000;	//转换后的坐标Y3

	HTuple hv_OutputX4 = 0.000;	//转换后的坐标X4
	HTuple hv_OutputY4 = 0.000;	//转换后的坐标Y4
};

//ColorSortDetect检测类型参数结构体
struct TypeParamsStruct_ColorSort
{
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

	//筛选类型
	HTuple hv_Area_Filter = "area";//area
	HTuple hv_Column_Filter = "column";//列坐标(X)column
	HTuple hv_Row_Filter = "row";//行坐标（Y）row
	HTuple hv_Circularity_Filter = "circularity";//circularity
	HTuple hv_Compactness_Filter = "compactness";//compactness
	HTuple hv_Convexity_Filter = "convexity";//convexity
	HTuple hv_Rectangularity_Filter = "rectangularity";//rectangularity
	HTuple hv_Height_Filter = "height";//height
	HTuple hv_Width_Filter = "width";//width
	HTuple hv_OuterRadius_Filter = "outer_radius";//outer_radius
	HTuple hv_InnerRadius_Filter = "inner_radius";//inner_radius
	HTuple hv_Rect2Len1_Filter = "rect2_len1";//rect2_len1
	HTuple hv_Rect2Len2_Filter = "rect2_len2";//rect2_len2
	HTuple hv_Rect2Angle_Filter = "rect2_phi";//rect2_phi
	HTuple hv_Ra_Filter = "ra";//ra
	HTuple hv_Rb_Filter = "rb";//rb
	HTuple hv_Phi_Filter = "phi";//phi

	HTuple hv_Null = "null";
};
//ColorSortDetect检测运行参数结构体
struct RunParamsStruct_ColorSort
{
	HObject ho_SearchRegion;		//数组,搜索区域
	//二值化参数
	HTuple hv_Channel_Product = "b";//使用哪个通道进行产品提取(r,g,b)
	HTuple hv_ThresholdType = "固定阈值";		//数组,图像二值化类型
	HTuple hv_LowThreshold = 50;				//数组,低阈值(固定阈值)
	HTuple hv_HighThreshold = 128;				//数组,高阈值(固定阈值)
	HTuple hv_Sigma = 1;						//数组,平滑阈值
	HTuple hv_CoreWidth = 5;					//数组,动态阈值(滤波核宽度)
	HTuple hv_CoreHeight = 5;					//数组,动态阈值(滤波核高度)
	HTuple hv_DynThresholdContrast = 15;		//数组,动态阈值分割图像对比度
	HTuple hv_DynThresholdPolarity = "dark";	//数组,动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

	//形态学类型
	HTuple hv_OperatorType = "null";//数组
	HTuple hv_OperaRec_Width = 5;//数组
	HTuple hv_OperaRec_Height = 5;//数组
	HTuple hv_OperaCir_Radius = 5;//数组
	//孔洞填充面积
	HTuple hv_FillUpShape_Min = 1;//数组
	HTuple hv_FillUpShape_Max = 1000;//数组
	//区域筛选参数
	HTuple hv_FeaturesFilter_Type;				 //数组，区域筛选条件数组(需要使用结构体TypeStruct_FeaturesFilter_MorphologyDetect里面的参数)
	HTuple hv_And_Or_Filter = "and";			//区域筛选多个条件之间选择"and"或者"or",默认"and"
	HTuple hv_MinValue_Filter;					//数组，区域筛选参数最小值
	HTuple hv_MaxValue_Filter;					//数组，区域筛选参数最大值

	HTuple hv_UseBlob = 0;//1启用Blob提取产品，0不启用
	//颜色分类
	HTuple hv_ColorValue1_Min = 50;//通道1颜色阈值min
	HTuple hv_ColorValue1_Max = 128;//通道1颜色阈值max
	HTuple hv_ColorValue2_Min = 50;//通道2颜色阈值min
	HTuple hv_ColorValue2_Max = 128;//通道2颜色阈值max
	HTuple hv_ColorValue3_Min = 50;//通道3颜色阈值min
	HTuple hv_ColorValue3_Max = 128;//通道3颜色阈值max
	HTuple hv_ColorArea_Min = 1;//颜色筛选面积min
	HTuple hv_ColorArea_Max = 999999999;//颜色筛选面积max
	HTuple hv_Channel_ColorSort = "hsv";//使用哪个通道进行色选(rgb,hsv,hsi)
	HTuple hv_UseUnionSortRegion = 0;	//启用提取产品后，是否合并产品区域内所有色选区域(1启用,0不启用)

};
//ColorSortDetect检测结果结构体
struct ResultParamsStruct_ColorSort
{
	HTuple hv_AreaRetArray;//面积数组
	HTuple hv_RowRetArray;	//分类好的所有颜色区域最小外接矩中心坐标row
	HTuple hv_ColumnRetArray;	//分类好的所有颜色区域最小外接矩中心坐标column
	HObject ho_RegionRetArray;//分类好的所有颜色区域
};

//ColorExtractDetect检测类型参数结构体
struct TypeParamsStruct_ColorExtract
{
	//模板匹配类型
	HTuple ShapeModel = "基于形状";
	HTuple NccModel = "基于互相关";

	HTuple hv_Null = "null";
};
//ColorExtractDetect检测运行参数结构体
struct RunParamsStruct_ColorExtract
{
	HObject ho_SearchRegion;		//数组,搜索区域

	//颜色抽取参数
	HTuple hv_ColorValue1_Min = 50;//通道1颜色阈值min
	HTuple hv_ColorValue1_Max = 128;//通道1颜色阈值max
	HTuple hv_ColorValue2_Min = 50;//通道2颜色阈值min
	HTuple hv_ColorValue2_Max = 128;//通道2颜色阈值max
	HTuple hv_ColorValue3_Min = 50;//通道3颜色阈值min
	HTuple hv_ColorValue3_Max = 128;//通道3颜色阈值max
	HTuple hv_ColorArea_Min = 1;//颜色筛选面积min
	HTuple hv_ColorArea_Max = 999999999;//颜色筛选面积max
	HTuple hv_Channel_ColorExtract = "hsv";//使用哪个通道进行色选(rgb,hsv,hsi)

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
	HTuple hv_Channel_PositionCorrection = "gray";//使用哪个通道做模板("r","g","b","gray")
};
//ColorExtractDetect检测结果结构体
struct ResultParamsStruct_ColorExtract
{
	HObject ho_ModelXLD;//位置修正的模板轮廓
	HObject ho_AffineTransSearchRegion;//仿射变换后的搜索区域

	HTuple hv_AreaRetArray;//面积数组
	HTuple hv_RowRetArray;	//分类好的所有颜色区域最小外接矩中心坐标row
	HTuple hv_ColumnRetArray;	//分类好的所有颜色区域最小外接矩中心坐标column
	HObject ho_RegionExtract; //抽取到的所有颜色区域

	HTuple hv_Ret = 0;//0是NG，1是OK
};

struct RunParamsStruct_OCRDetect
{
	HTuple hv_Train_Type = 0;//0 免训练模式，1训练模式
	HObject ho_SearchRegion;//识别区域
	//免训练模式参数
	HTuple hv_OCR_Type = "MLP";//识别类型，MLP或者CNN
	HTuple hv_OCR_ModelName = "Document_0-9A-Z_Rej.omc";//模型名称
	HTuple hv_IsDotPrint = 0; //是否为点阵打印字体,0不是,1是
	HTuple hv_NotDetectPunctuation = 1;//屏蔽标点符号,0不屏蔽，1屏蔽
	HTuple hv_NotDetectSeparators = 1;//屏蔽分隔符,0不屏蔽，1屏蔽
	HTuple hv_UseCorrectAngle = 0;//启用角度矫正
	HTuple hv_Height_WordLine = 0;//角度矫正文本行高度
	//训练模式参数
	HTuple hv_BinarizationType = 0;//0是固定阈值，1是自动阈值
	HTuple hv_FixThresholdMin = 0;
	HTuple hv_FixThresholdMax = 75;
	HTuple hv_PolarityChar = 0;//0是黑字符白背景，1是白字符黑背景
	HTuple hv_WidthChar_Min = 50;//字符最小宽度
	HTuple hv_WidthChar_Max = 50;//字符最大宽度
	HTuple hv_HeightChar_Min = 50;//字符最小高度
	HTuple hv_HeightChar_Max = 50;//字符最大高度
	HTuple hv_Distance_Char = 10;//字符之间的最大间距
	HTuple hv_BlockWidth_Min = 10;//笔画最小宽度
	HTuple hv_BlockWidth_Max = 10;//笔画最大宽度
	HTuple hv_BlockHeight_Min = 10;//笔画最小高度
	HTuple hv_BlockHeight_Max = 10;//笔画最大高度
	HTuple hv_BlockArea_Min = 10;//笔画最小面积
	HTuple hv_BlockArea_Max = 1000;//笔画最大面积
	HTuple hv_Distance_Block = 10;//笔画之间最大间距
	HTuple hv_WidthType = 0;//字符宽度类型(0可变,1固定宽高,2动态宽高)
	HTuple hv_FixWidthType_Width = 50;//固定宽高模式，字符的宽
	HTuple hv_FixWidthType_Height = 50;//固定宽高模式，字符的高
	HTuple hv_DynamicWidthType_Width = 50;//动态宽度模式。字符宽
	HTuple hv_DynamicWidthType_Scale = 20;//动态宽度模式。最终分割位置与初始分割位置的最大偏差为距离*百分比*0.01。
	//训练的字符库集合
	HTuple hv_OcrTrainfile = NULL;
};
struct ResultParamsStruct_OCRDetect
{
	//融合输出
	HTuple hv_Confidence;//识别分数
	HTuple hv_OCR_Content;//识别的内容
	HTuple hv_OCR_Row;//识别的位置行坐标
	HTuple hv_OCR_Column;//识别的位置列坐标
	HObject ho_OCR_Region;//识别的区域
	//绘制后的结果图
	Mat ho_DestImg;
};