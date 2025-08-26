#include "PinDetect.h"


void PinDetect::run(std::map<std::string, HTuple>& HTupleList, std::map<std::string, HObject>& HObjectList) {
	PinDetectFunc(HObjectList[portNames.Image], runParams, resultParams);

	//HTuple hv_Ret;	//0是OK,非0是NG
	//HTuple hv_RetNum = 0;		//Pin针数量
	//HObject ho_Contour_Base;			     //基准轮廓
	//HObject ho_ModelXLD;//位置修正的模板轮廓
	////基准边结果数据
	//HTuple hv_Row1_BaseLine_Ret;
	//HTuple hv_Column1_BaseLine_Ret;
	//HTuple hv_Row2_BaseLine_Ret;
	//HTuple hv_Column2_BaseLine_Ret;
	////基准圆结数据
	//HTuple hv_Row_BaseCircle_Ret;
	//HTuple hv_Column_BaseCircle_Ret;
	//HTuple hv_Radius_BaseCircle_Ret;

	//HObject ho_Region_AllPin;				 //所有pin针区域
	//HObject ho_Region_OKPin;				//OK的pin针区域
	//HObject ho_Region_NGPin;				//NG的pin针区域
	//HTuple hv_Row_Array;					   //pin针区域中心行坐标
	//HTuple hv_Column_Array;					   //pin针区域中心列坐标
	//HTuple hv_DisX_Pixel_Array;                  //pin针区域中心相对基准的X方向距离,像素
	//HTuple hv_DisY_Pixel_Array;				  //pin针区域中心相对基准的Y方向距离,像素
	//HTuple hv_OffsetX_Pixel_Array;              //pin针区域中心相对基准的X方向偏差值,像素
	//HTuple hv_OffsetY_Pixel_Array;			 //pin针区域中心相对基准的Y方向偏差值,像素
	//HTuple hv_DisX_MM_Array;                  //pin针区域中心相对基准的X方向距离,毫米
	//HTuple hv_DisY_MM_Array;				  //pin针区域中心相对基准的Y方向距离,毫米
	//HTuple hv_OffsetX_MM_Array;              //pin针区域中心相对基准的X方向偏差值,毫米
	//HTuple hv_OffsetY_MM_Array;			 //pin针区域中心相对基准的Y方向偏差值,毫米


	//结果
	HTupleList[portNames.Ret] = resultParams.hv_Ret;
	HTupleList[portNames.RetNum] = resultParams.hv_RetNum;
	HObjectList[portNames.Contour_Base] = resultParams.ho_Contour_Base;
	HObjectList[portNames.ModelXLD] = resultParams.ho_ModelXLD;
	HTupleList[portNames.Row1_BaseLine_Ret] = resultParams.hv_Row1_BaseLine_Ret;
	HTupleList[portNames.Column1_BaseLine_Ret] = resultParams.hv_Column1_BaseLine_Ret;
	HTupleList[portNames.Row2_BaseLine_Ret] = resultParams.hv_Row2_BaseLine_Ret;
	HTupleList[portNames.Column2_BaseLine_Ret] = resultParams.hv_Column2_BaseLine_Ret;
	HTupleList[portNames.Row_BaseCircle_Ret] = resultParams.hv_Row_BaseCircle_Ret;
	HTupleList[portNames.Column_BaseCircle_Ret] = resultParams.hv_Column_BaseCircle_Ret;
	HTupleList[portNames.Radius_BaseCircle_Ret] = resultParams.hv_Radius_BaseCircle_Ret;

	HObjectList[portNames.Region_AllPin] = resultParams.ho_Region_AllPin;
	HObjectList[portNames.Region_OKPin] = resultParams.ho_Region_OKPin;
	HObjectList[portNames.Region_NGPin] = resultParams.ho_Region_NGPin;
	HTupleList[portNames.Row_Array] = resultParams.hv_Row_Array;
	HTupleList[portNames.Column_Array] = resultParams.hv_Column_Array;
	HTupleList[portNames.DisX_Pixel_Array] = resultParams.hv_DisX_Pixel_Array;
	HTupleList[portNames.DisY_Pixel_Array] = resultParams.hv_DisY_Pixel_Array;
	HTupleList[portNames.OffsetX_Pixel_Array] = resultParams.hv_OffsetX_Pixel_Array;
	HTupleList[portNames.OffsetY_Pixel_Array] = resultParams.hv_OffsetY_Pixel_Array;
	HTupleList[portNames.DisX_MM_Array] = resultParams.hv_DisX_MM_Array;
	HTupleList[portNames.DisY_MM_Array] = resultParams.hv_DisY_MM_Array;
	HTupleList[portNames.OffsetX_MM_Array] = resultParams.hv_OffsetX_MM_Array;
	HTupleList[portNames.OffsetY_MM_Array] = resultParams.hv_OffsetY_MM_Array;

}

void PinDetect::ReadParams(std::map<std::string, HTuple>& HTupleList, std::map<std::string, HObject>& HObjectList) {

	//xml文件读取参数
	dllDatas1->ReadParams_PinDetect(runParams, processID, nodeName, processModbuleID);

	HTupleList[portNames.Tolerance_X] = runParams.hv_Tolerance_X;
	HTupleList[portNames.Tolerance_Y] = runParams.hv_Tolerance_Y;
	HTupleList[portNames.MmPixel] = runParams.hv_MmPixel;
	HTupleList[portNames.StandardX_Array] = runParams.hv_StandardX_Array;
	HTupleList[portNames.StandardY_Array] = runParams.hv_StandardY_Array;
	HTupleList[portNames.X_BindingNum] = runParams.hv_X_BindingNum;
	HTupleList[portNames.Y_BindingNum] = runParams.hv_Y_BindingNum;
	HTupleList[portNames.BaseType] = runParams.hv_BaseType;
	HTupleList[portNames.Row1_Base] = runParams.hv_Row1_Base;
	HTupleList[portNames.Column1_Base] = runParams.hv_Column1_Base;
	HTupleList[portNames.Row2_Base] = runParams.hv_Row2_Base;
	HTupleList[portNames.Column2_Base] = runParams.hv_Column2_Base;
	HTupleList[portNames.Radius_Base] = runParams.hv_Radius_Base;
	HTupleList[portNames.MeasureLength1] = runParams.hv_MeasureLength1;
	HTupleList[portNames.MeasureLength2] = runParams.hv_MeasureLength2;
	HTupleList[portNames.MeasureSigma] = runParams.hv_MeasureSigma;
	HTupleList[portNames.MeasureThreshold] = runParams.hv_MeasureThreshold;
	HTupleList[portNames.MeasureTransition] = runParams.hv_MeasureTransition;
	HTupleList[portNames.MeasureSelect] = runParams.hv_MeasureSelect;
	HTupleList[portNames.MeasureNum] = runParams.hv_MeasureNum;
	HTupleList[portNames.MeasureMinScore] = runParams.hv_MeasureMinScore;
	HTupleList[portNames.DistanceThreshold] = runParams.hv_DistanceThreshold;
	HTupleList[portNames.DetectType] = runParams.hv_DetectType;
	HTupleList[portNames.SortType] = runParams.hv_SortType;
	HObjectList[portNames.SearchRegion] = runParams.ho_SearchRegion;
	HTupleList[portNames.ThresholdType] = runParams.hv_ThresholdType;
	HTupleList[portNames.LowThreshold] = runParams.hv_LowThreshold;
	HTupleList[portNames.HighThreshold] = runParams.hv_HighThreshold;
	HTupleList[portNames.Sigma] = runParams.hv_Sigma;
	HTupleList[portNames.CoreWidth] = runParams.hv_CoreWidth;
	HTupleList[portNames.CoreHeight] = runParams.hv_CoreHeight;
	HTupleList[portNames.DynThresholdContrast] = runParams.hv_DynThresholdContrast;
	HTupleList[portNames.DynThresholdPolarity] = runParams.hv_DynThresholdPolarity;
	HTupleList[portNames.AreaFilter_Min] = runParams.hv_AreaFilter_Min;
	HTupleList[portNames.RecLen1Filter_Min] = runParams.hv_RecLen1Filter_Min;
	HTupleList[portNames.RecLen2Filter_Min] = runParams.hv_RecLen2Filter_Min;
	HTupleList[portNames.RowFilter_Min] = runParams.hv_RowFilter_Min;
	HTupleList[portNames.ColumnFilter_Min] = runParams.hv_ColumnFilter_Min;
	HTupleList[portNames.CircularityFilter_Min] = runParams.hv_CircularityFilter_Min;
	HTupleList[portNames.RectangularityFilter_Min] = runParams.hv_RectangularityFilter_Min;
	HTupleList[portNames.WidthFilter_Min] = runParams.hv_WidthFilter_Min;
	HTupleList[portNames.HeightFilter_Min] = runParams.hv_HeightFilter_Min;
	HTupleList[portNames.AreaFilter_Max] = runParams.hv_AreaFilter_Max;
	HTupleList[portNames.RecLen1Filter_Max] = runParams.hv_RecLen1Filter_Max;
	HTupleList[portNames.RecLen2Filter_Max] = runParams.hv_RecLen2Filter_Max;
	HTupleList[portNames.RowFilter_Max] = runParams.hv_RowFilter_Max;
	HTupleList[portNames.ColumnFilter_Max] = runParams.hv_ColumnFilter_Max;
	HTupleList[portNames.CircularityFilter_Max] = runParams.hv_CircularityFilter_Max;
	HTupleList[portNames.RectangularityFilter_Max] = runParams.hv_RectangularityFilter_Max;
	HTupleList[portNames.WidthFilter_Max] = runParams.hv_WidthFilter_Max;
	HTupleList[portNames.HeightFilter_Max] = runParams.hv_HeightFilter_Max;
	HTupleList[portNames.SelectAreaMax] = runParams.hv_SelectAreaMax;
	HTupleList[portNames.OperatorType] = runParams.hv_OperatorType;
	HTupleList[portNames.OperaRec_Width] = runParams.hv_OperaRec_Width;
	HTupleList[portNames.OperaRec_Height] = runParams.hv_OperaRec_Height;
	HTupleList[portNames.OperaCir_Radius] = runParams.hv_OperaCir_Radius;
	HTupleList[portNames.FillUpShape_Min] = runParams.hv_FillUpShape_Min;
	HTupleList[portNames.FillUpShape_Max] = runParams.hv_FillUpShape_Max;
	HTupleList[portNames.MatchMethod] = runParams.hv_MatchMethod;
	HTupleList[portNames.AngleStart] = runParams.hv_AngleStart;
	HTupleList[portNames.AngleExtent] = runParams.hv_AngleExtent;
	HTupleList[portNames.ScaleRMin] = runParams.hv_ScaleRMin;
	HTupleList[portNames.ScaleRMax] = runParams.hv_ScaleRMax;
	HTupleList[portNames.ScaleCMin] = runParams.hv_ScaleCMin;
	HTupleList[portNames.ScaleCMax] = runParams.hv_ScaleCMax;
	HTupleList[portNames.MinScore] = runParams.hv_MinScore;
	HTupleList[portNames.NumMatches] = runParams.hv_NumMatches;
	HObjectList[portNames.TrainRegion] = runParams.ho_TrainRegion;
	HTupleList[portNames.ModelID] = runParams.hv_ModelID;

	HObjectList[portNames.SearchRegion_PositionCorrection] = runParams.ho_SearchRegion_PositionCorrection;
	HObjectList[portNames.TrainRegion_PositionCorrection] = runParams.ho_TrainRegion_PositionCorrection;
	HTupleList[portNames.Row_PositionCorrection] = runParams.hv_Row_PositionCorrection;
	HTupleList[portNames.Column_PositionCorrection] = runParams.hv_Column_PositionCorrection;
	HTupleList[portNames.Angle_PositionCorrection] = runParams.hv_Angle_PositionCorrection;
	HTupleList[portNames.MatchMethod_PositionCorrection] = runParams.hv_MatchMethod_PositionCorrection;
	HTupleList[portNames.AngleStart_PositionCorrection] = runParams.hv_AngleStart_PositionCorrection;
	HTupleList[portNames.AngleExtent_PositionCorrection] = runParams.hv_AngleExtent_PositionCorrection;
	HTupleList[portNames.ScaleRMin_PositionCorrection] = runParams.hv_ScaleRMin_PositionCorrection;
	HTupleList[portNames.ScaleRMax_PositionCorrection] = runParams.hv_ScaleRMax_PositionCorrection;
	HTupleList[portNames.ScaleCMin_PositionCorrection] = runParams.hv_ScaleCMin_PositionCorrection;
	HTupleList[portNames.ScaleCMax_PositionCorrection] = runParams.hv_ScaleCMax_PositionCorrection;
	HTupleList[portNames.MinScore_PositionCorrection] = runParams.hv_MinScore_PositionCorrection;
	HTupleList[portNames.NumMatches_PositionCorrection] = runParams.hv_NumMatches_PositionCorrection;
	HTupleList[portNames.ModelID_PositionCorrection] = runParams.hv_ModelID_PositionCorrection;
	HTupleList[portNames.Check_PositionCorrection] = runParams.hv_Check_PositionCorrection;
	HTupleList[portNames.SortType_PositionCorrection] = runParams.hv_SortType_PositionCorrection;
	HTupleList[portNames.To_Gray] = runParams.hv_ToGray;
	

}

void PinDetect::setPortInfos() {
	setPortInfoInputIconic();
	setPortInfoInputCtrl();
	setPortInfoOutputCtrl();
	setPortInfoOutputIconic();
}

void PinDetect::setPortInfoInputCtrl() {

}

void PinDetect::setPortInfoOutputCtrl() {

	//HTuple hv_Ret;	//0是OK,非0是NG
	//HTuple hv_RetNum = 0;		//Pin针数量

	////基准边结果数据
	//HTuple hv_Row1_BaseLine_Ret;
	//HTuple hv_Column1_BaseLine_Ret;
	//HTuple hv_Row2_BaseLine_Ret;
	//HTuple hv_Column2_BaseLine_Ret;
	////基准圆结数据
	//HTuple hv_Row_BaseCircle_Ret;
	//HTuple hv_Column_BaseCircle_Ret;
	//HTuple hv_Radius_BaseCircle_Ret;


	//HTuple hv_Row_Array;					   //pin针区域中心行坐标
	//HTuple hv_Column_Array;					   //pin针区域中心列坐标
	//HTuple hv_DisX_Pixel_Array;                  //pin针区域中心相对基准的X方向距离,像素
	//HTuple hv_DisY_Pixel_Array;				  //pin针区域中心相对基准的Y方向距离,像素
	//HTuple hv_OffsetX_Pixel_Array;              //pin针区域中心相对基准的X方向偏差值,像素
	//HTuple hv_OffsetY_Pixel_Array;			 //pin针区域中心相对基准的Y方向偏差值,像素
	//HTuple hv_DisX_MM_Array;                  //pin针区域中心相对基准的X方向距离,毫米
	//HTuple hv_DisY_MM_Array;				  //pin针区域中心相对基准的Y方向距离,毫米
	//HTuple hv_OffsetX_MM_Array;              //pin针区域中心相对基准的X方向偏差值,毫米
	//HTuple hv_OffsetY_MM_Array;			 //pin针区域中心相对基准的Y方向偏差值,毫米



	portRegisterInfo portInfo;
	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = true;
	portInfo.isSelect = true;
	portInfo.chineseName = "结果";
	outputCtrlInfos[portNames.Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = true;
	portInfo.isSelect = true;
	portInfo.chineseName = "结果数量";
	outputCtrlInfos[portNames.RetNum] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准边Y1";
	outputCtrlInfos[portNames.Row1_BaseLine_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准边X1";
	outputCtrlInfos[portNames.Column1_BaseLine_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准边Y2";
	outputCtrlInfos[portNames.Row2_BaseLine_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准边X2";

	outputCtrlInfos[portNames.Column2_BaseLine_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准圆Y";
	outputCtrlInfos[portNames.Row_BaseCircle_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准圆X";
	outputCtrlInfos[portNames.Column_BaseCircle_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准圆半径";
	outputCtrlInfos[portNames.Radius_BaseCircle_Ret] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针区域中心行坐标";
	outputCtrlInfos[portNames.Row_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针区域中心列坐标";
	outputCtrlInfos[portNames.Column_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针X方向像素";
	outputCtrlInfos[portNames.DisX_Pixel_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针Y方向像素";

	outputCtrlInfos[portNames.DisY_Pixel_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针X方向偏差值";
	outputCtrlInfos[portNames.OffsetX_Pixel_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针Y方向偏差值";
	outputCtrlInfos[portNames.OffsetY_Pixel_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针X方向距离";
	outputCtrlInfos[portNames.DisX_MM_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针Y方向距离";
	outputCtrlInfos[portNames.DisY_MM_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针X方向偏差值";
	outputCtrlInfos[portNames.OffsetX_MM_Array] = portInfo;

	portInfo.portType = ePortType::HTuple;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "Pin针Y方向偏差值";
	outputCtrlInfos[portNames.OffsetY_MM_Array] = portInfo;
}

void PinDetect::setPortInfoInputIconic() {
	portRegisterInfo portInfo;
	portInfo.portType = ePortType::HImage;
	portInfo.isNecessity = true;
	portInfo.isSelect = true;
	portInfo.chineseName = "图像";
	inputIconicInfos[portNames.Image] = portInfo;
}

void PinDetect::setPortInfoOutputIconic() {

	//HObject ho_Region_AllPin;				 //所有pin针区域
	//HObject ho_Region_OKPin;				//OK的pin针区域
	//HObject ho_Region_NGPin;				//NG的pin针区域
	//HObject ho_Contour_Base;			     //基准轮廓
	//HObject ho_ModelXLD;//位置修正的模板轮廓

	portRegisterInfo portInfo;
	portInfo.portType = ePortType::HRegion;
	portInfo.isNecessity = false;
	portInfo.isSelect = true;
	portInfo.chineseName = "所有pin针区域";

	outputIconicInfos[portNames.Region_AllPin] = portInfo;

	portInfo.portType = ePortType::HRegion;
	portInfo.isNecessity = false;
	portInfo.isSelect = true;
	portInfo.chineseName = "OK的pin针区域";
	outputIconicInfos[portNames.Region_OKPin] = portInfo;

	portInfo.portType = ePortType::HRegion;
	portInfo.isNecessity = false;
	portInfo.isSelect = true;
	portInfo.chineseName = "NG的pin针区域";
	outputIconicInfos[portNames.Region_NGPin] = portInfo;

	portInfo.portType = ePortType::HXLD;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "基准轮廓";
	outputIconicInfos[portNames.Contour_Base] = portInfo;

	portInfo.portType = ePortType::HXLD;
	portInfo.isNecessity = false;
	portInfo.isSelect = false;
	portInfo.chineseName = "位置修正的模板轮廓";
	outputIconicInfos[portNames.ModelXLD] = portInfo;
}

PinDetect::PinDetect() {
	setPortInfos();
}

PinDetect::PinDetect(std::string _nodeName, int _processModbuleID, int _processID) {
	processID = _processID;
	processModbuleID = _processModbuleID;
	nodeName = _nodeName;

	qLog = new QLog(processID, processModbuleID, nodeName);
	setPortInfos();
	dllDatas1 = new DllDatas();
}

void PinDetect::setGetConfigPath(std::string getConfigPath) {
	dllDatas1->setGetConfigPath(getConfigPath);
}

PinDetect::~PinDetect() 
{
	////释放通用类指针资源
	//delete m_Common;
}

int PinDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName) {
	try {
		qLog->writeLog(logType::INFO, "update algorithm config");
		int ret = 1;
		//XML读取参数
		ErrorCode_Xml errorCode = dllDatas1->ReadParams_PinDetect(runParams, processID, modubleName, modubleID);
		if (errorCode != Ok_Xml) {
			return 1;
		}
	}

	catch (...) {
		qLog->writeLog(logType::ERROR1, "error in update algorithm config");
		return -1;
	}
	return 0;
}


std::vector<std::string> PinDetect::getModubleResultTitleList() {
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("X坐标");
	modubleResultTitleList.push_back("Y坐标");
	modubleResultTitleList.push_back("距基准X-像素");
	modubleResultTitleList.push_back("距基准Y-像素");
	modubleResultTitleList.push_back("偏差X-像素");
	modubleResultTitleList.push_back("偏差Y-像素");
	modubleResultTitleList.push_back("距基准X-毫米");
	modubleResultTitleList.push_back("距基准Y-毫米");
	modubleResultTitleList.push_back("偏差X-毫米");
	modubleResultTitleList.push_back("偏差Y-毫米");

	result = modubleResultTitleList;

	return result;
}
std::vector<std::map<int, std::string>> PinDetect::getModubleResultList() {
	std::vector<std::map<int, std::string>> results;
	int count = resultParams.hv_RetNum.I();
	if (count > 0) {
		for (int i = 0; i < count; i++) {
			std::map<int, std::string> result;
			int index = 0;
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_Column_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_Row_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_DisX_Pixel_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_DisY_Pixel_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_OffsetX_Pixel_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_OffsetY_Pixel_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_DisX_MM_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_DisY_MM_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_OffsetX_MM_Array[i].D(), 2);
			result[index++] = dllDatas1->formatDobleValue(resultParams.hv_OffsetY_MM_Array[i].D(), 2);

			results.push_back(result);
		}
	}
	return results;
}

//Pin针检测
int PinDetect::PinDetectFunc(const HObject& ho_Image, const RunParamsStruct_Pin& runParams, ResultParamsStruct_Pin& resultParams) {
	try {
		//清空结果参数
		GenEmptyObj(&resultParams.ho_Region_AllPin);
		GenEmptyObj(&resultParams.ho_Region_OKPin);
		GenEmptyObj(&resultParams.ho_Region_NGPin);
		GenEmptyObj(&resultParams.ho_Contour_Base);
		GenEmptyObj(&resultParams.ho_ModelXLD);

		resultParams.hv_RetNum = 0;
		resultParams.hv_Row1_BaseLine_Ret.Clear();
		resultParams.hv_Column1_BaseLine_Ret.Clear();
		resultParams.hv_Row2_BaseLine_Ret.Clear();
		resultParams.hv_Column2_BaseLine_Ret.Clear();

		resultParams.hv_Row_BaseCircle_Ret.Clear();
		resultParams.hv_Column_BaseCircle_Ret.Clear();
		resultParams.hv_Radius_BaseCircle_Ret.Clear();

		resultParams.hv_Row_Array.Clear();
		resultParams.hv_Column_Array.Clear();
		resultParams.hv_DisX_Pixel_Array.Clear();
		resultParams.hv_DisY_Pixel_Array.Clear();
		resultParams.hv_OffsetX_Pixel_Array.Clear();
		resultParams.hv_OffsetY_Pixel_Array.Clear();
		resultParams.hv_DisX_MM_Array.Clear();
		resultParams.hv_DisY_MM_Array.Clear();
		resultParams.hv_OffsetX_MM_Array.Clear();
		resultParams.hv_OffsetY_MM_Array.Clear();
		resultParams.hv_Ret = 0;

		//mutex1.lock();
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			//mutex1.unlock();
			resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
			//mResultParams = resultParams;
			return 1;
		}
		TypeParamsStruct_Pin mType;
		HObject GrayImg;
		Rgb1ToGray(ho_Image, &GrayImg);
		int ngNum = 0;

		HTuple hv_ROIHomMat2D = NULL;//仿射变换ROI跟随
		//是否启用位置修正
		if (runParams.hv_Check_PositionCorrection == 1)
		{
			//找模板开始
			HObject ho_Region_Ret, ho_XLD_Ret;
			HTuple ho_Row_Ret, hv_Column_Ret, hv_Angle_Ret, hv_Score_Ret;
			int ret = FindTemplate(GrayImg, 0, runParams, ho_XLD_Ret, ho_Region_Ret, ho_Row_Ret, hv_Column_Ret, hv_Angle_Ret, hv_Score_Ret);
			if (ho_Row_Ret.TupleLength() == 1)
			{
				VectorAngleToRigid(runParams.hv_Row_PositionCorrection, runParams.hv_Column_PositionCorrection,
					runParams.hv_Angle_PositionCorrection.TupleRad(),
					ho_Row_Ret, hv_Column_Ret, hv_Angle_Ret.TupleRad(),
					&hv_ROIHomMat2D);
				ConcatObj(resultParams.ho_ModelXLD, ho_XLD_Ret, &resultParams.ho_ModelXLD);
			}
		}

		//找基准
		int baseNum = runParams.hv_BaseType.TupleLength().I();
		if (baseNum <= 0)
		{
			//基准参数为空
			if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
			{
				AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
			}
			else
			{
				resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
			}
			//mResultParams = resultParams;
			return 1;
		}
		for (int i = 0; i < baseNum; i++)
		{
			if (runParams.hv_BaseType[i] == mType.BaseType_Line)
			{
				//找直线开始
				RunParamsStruct_Pin lineStruct;
				lineStruct.hv_Row1_Base = runParams.hv_Row1_Base[i];
				lineStruct.hv_Column1_Base = runParams.hv_Column1_Base[i];
				lineStruct.hv_Row2_Base = runParams.hv_Row2_Base[i];
				lineStruct.hv_Column2_Base = runParams.hv_Column2_Base[i];
				//是否进行仿射变换，抓边跟随
				if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
				{
					//利用传入的矩阵进行仿射变换
					AffineTransPixel(hv_ROIHomMat2D, lineStruct.hv_Row1_Base, lineStruct.hv_Column1_Base, &lineStruct.hv_Row1_Base, &lineStruct.hv_Column1_Base);
					AffineTransPixel(hv_ROIHomMat2D, lineStruct.hv_Row2_Base, lineStruct.hv_Column2_Base, &lineStruct.hv_Row2_Base, &lineStruct.hv_Column2_Base);
				}

				lineStruct.hv_MeasureLength1 = runParams.hv_MeasureLength1[i];
				lineStruct.hv_MeasureLength2 = runParams.hv_MeasureLength2[i];
				lineStruct.hv_MeasureSigma = runParams.hv_MeasureSigma[i];
				lineStruct.hv_MeasureThreshold = runParams.hv_MeasureThreshold[i];
				lineStruct.hv_MeasureTransition = runParams.hv_MeasureTransition[i];
				lineStruct.hv_MeasureSelect = runParams.hv_MeasureSelect[i];
				lineStruct.hv_MeasureNum = runParams.hv_MeasureNum[i];
				lineStruct.hv_MeasureMinScore = runParams.hv_MeasureMinScore[i];
				lineStruct.hv_DistanceThreshold = runParams.hv_DistanceThreshold[i];

				HTuple row1Line, col1Line, row2Line, col2Line;
				HObject xldLine;
				int ret = FindLine(GrayImg, lineStruct, row1Line, col1Line, row2Line, col2Line, xldLine);
				if (ret != 0)
				{
					//找边失败
					if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
					{
						AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
					}
					else
					{
						resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
					}
					resultParams.hv_Row1_BaseLine_Ret.Append(-1);
					resultParams.hv_Column1_BaseLine_Ret.Append(-1);
					resultParams.hv_Row2_BaseLine_Ret.Append(-1);
					resultParams.hv_Column2_BaseLine_Ret.Append(-1);
				}
				else
				{
					resultParams.hv_Row1_BaseLine_Ret.Append(row1Line);
					resultParams.hv_Column1_BaseLine_Ret.Append(col1Line);
					resultParams.hv_Row2_BaseLine_Ret.Append(row2Line);
					resultParams.hv_Column2_BaseLine_Ret.Append(col2Line);
					ConcatObj(resultParams.ho_Contour_Base, xldLine, &resultParams.ho_Contour_Base);
				}
			}
			else if (runParams.hv_BaseType[i] == mType.BaseType_Circle)
			{
				//找圆开始
				RunParamsStruct_Pin circleStruct;
				circleStruct.hv_Row1_Base = runParams.hv_Row1_Base[i];
				circleStruct.hv_Column1_Base = runParams.hv_Column1_Base[i];
				circleStruct.hv_Radius_Base = runParams.hv_Radius_Base[i];
				//是否进行仿射变换，抓圆跟随
				if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
				{
					//利用传入的矩阵进行仿射变换
					AffineTransPixel(hv_ROIHomMat2D, circleStruct.hv_Row1_Base, circleStruct.hv_Column1_Base, &circleStruct.hv_Row1_Base, &circleStruct.hv_Column1_Base);
				}

				circleStruct.hv_MeasureLength1 = runParams.hv_MeasureLength1[i];
				circleStruct.hv_MeasureLength2 = runParams.hv_MeasureLength2[i];
				circleStruct.hv_MeasureSigma = runParams.hv_MeasureSigma[i];
				circleStruct.hv_MeasureThreshold = runParams.hv_MeasureThreshold[i];
				circleStruct.hv_MeasureTransition = runParams.hv_MeasureTransition[i];
				circleStruct.hv_MeasureSelect = runParams.hv_MeasureSelect[i];
				circleStruct.hv_MeasureNum = runParams.hv_MeasureNum[i];
				circleStruct.hv_MeasureMinScore = runParams.hv_MeasureMinScore[i];
				circleStruct.hv_DistanceThreshold = runParams.hv_DistanceThreshold[i];

				HTuple rowCircle, colCircle, radiusCircle;
				HObject xldCircle;
				int ret = FindCircle(GrayImg, circleStruct, rowCircle, colCircle, radiusCircle, xldCircle);
				if (ret != 0)
				{
					//找圆失败
					if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
					{
						AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
					}
					else
					{
						resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
					}
					resultParams.hv_Row_BaseCircle_Ret.Append(-1);
					resultParams.hv_Column_BaseCircle_Ret.Append(-1);
					resultParams.hv_Radius_BaseCircle_Ret.Append(-1);
				}
				else
				{
					resultParams.hv_Row_BaseCircle_Ret.Append(rowCircle);
					resultParams.hv_Column_BaseCircle_Ret.Append(colCircle);
					resultParams.hv_Radius_BaseCircle_Ret.Append(radiusCircle);
					ConcatObj(resultParams.ho_Contour_Base, xldCircle, &resultParams.ho_Contour_Base);
				}
			}
		}


		//Blob找Pin区域
		HTuple countR;
		CountObj(runParams.ho_SearchRegion, &countR);
		int regionNum = countR.I();
		if (regionNum <= 0)
		{
			//Pin搜索区域为空
			//mResultParams = resultParams;
			return 1;
		}
		for (int i = 0; i < regionNum; i++)
		{
			if (runParams.hv_DetectType[i] == mType.DetectType_Blob)
			{
				//临时参数
				RunParamsStruct_Pin blobStruct;
				SelectObj(runParams.ho_SearchRegion, &blobStruct.ho_SearchRegion, i + 1);
				if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
				{
					AffineTransRegion(blobStruct.ho_SearchRegion, &blobStruct.ho_SearchRegion, hv_ROIHomMat2D, "nearest_neighbor");
				}
				//搜索区域轮廓添加到结果轮廓输出
				HObject roiContour;
				GenContourRegionXld(blobStruct.ho_SearchRegion, &roiContour, "border");
				ConcatObj(resultParams.ho_Contour_Base, roiContour, &resultParams.ho_Contour_Base);

				blobStruct.hv_DetectType = runParams.hv_DetectType[i];
				blobStruct.hv_SortType = runParams.hv_SortType[i];

				//二值化参数
				blobStruct.hv_ThresholdType = runParams.hv_ThresholdType[i];		//数组,图像二值化类型
				blobStruct.hv_LowThreshold = runParams.hv_LowThreshold[i];				//数组,低阈值(固定阈值)
				blobStruct.hv_HighThreshold = runParams.hv_HighThreshold[i];				//数组,高阈值(固定阈值)
				blobStruct.hv_Sigma = runParams.hv_Sigma[i];						//数组,平滑阈值
				blobStruct.hv_CoreWidth = runParams.hv_CoreWidth[i];					//数组,动态阈值(滤波核宽度)
				blobStruct.hv_CoreHeight = runParams.hv_CoreHeight[i];					//数组,动态阈值(滤波核高度)
				blobStruct.hv_DynThresholdContrast = runParams.hv_DynThresholdContrast[i];		//数组,动态阈值分割图像对比度
				blobStruct.hv_DynThresholdPolarity = runParams.hv_DynThresholdPolarity[i];	//数组,动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

				//筛选Blob参数
				blobStruct.hv_AreaFilter_Min = runParams.hv_AreaFilter_Min[i];	//数组
				blobStruct.hv_RecLen1Filter_Min = runParams.hv_RecLen1Filter_Min[i];//数组
				blobStruct.hv_RecLen2Filter_Min = runParams.hv_RecLen2Filter_Min[i];//数组
				blobStruct.hv_RowFilter_Min = runParams.hv_RowFilter_Min[i];//数组
				blobStruct.hv_ColumnFilter_Min = runParams.hv_ColumnFilter_Min[i];//数组
				blobStruct.hv_CircularityFilter_Min = runParams.hv_CircularityFilter_Min[i];//数组
				blobStruct.hv_RectangularityFilter_Min = runParams.hv_RectangularityFilter_Min[i];//数组
				blobStruct.hv_WidthFilter_Min = runParams.hv_WidthFilter_Min[i];//数组
				blobStruct.hv_HeightFilter_Min = runParams.hv_HeightFilter_Min[i];//数组
				blobStruct.hv_AreaFilter_Max = runParams.hv_AreaFilter_Max[i];//数组
				auto area = blobStruct.hv_AreaFilter_Max.D();
				blobStruct.hv_RecLen1Filter_Max = runParams.hv_RecLen1Filter_Max[i];//数组
				blobStruct.hv_RecLen2Filter_Max = runParams.hv_RecLen2Filter_Max[i];//数组
				blobStruct.hv_RowFilter_Max = runParams.hv_RowFilter_Max[i];//数组
				blobStruct.hv_ColumnFilter_Max = runParams.hv_ColumnFilter_Max[i];//数组
				blobStruct.hv_CircularityFilter_Max = runParams.hv_CircularityFilter_Max[i];//数组
				blobStruct.hv_RectangularityFilter_Max = runParams.hv_RectangularityFilter_Max[i];//数组
				blobStruct.hv_WidthFilter_Max = runParams.hv_WidthFilter_Max[i];//数组
				blobStruct.hv_HeightFilter_Max = runParams.hv_HeightFilter_Max[i];//数组
				//孔洞填充面积
				blobStruct.hv_FillUpShape_Min = runParams.hv_FillUpShape_Min[i];//数组
				blobStruct.hv_FillUpShape_Max = runParams.hv_FillUpShape_Max[i];//数组
				//形态学类型
				blobStruct.hv_OperatorType = runParams.hv_OperatorType[i];//数组
				blobStruct.hv_OperaRec_Width = runParams.hv_OperaRec_Width[i];//数组
				blobStruct.hv_OperaRec_Height = runParams.hv_OperaRec_Height[i];//数组
				blobStruct.hv_OperaCir_Radius = runParams.hv_OperaCir_Radius[i];//数组
				blobStruct.hv_SelectAreaMax = runParams.hv_SelectAreaMax[i];
				blobStruct.hv_BlobCount = runParams.hv_BlobCount[i];
				//Blob分析开始
				HObject ho_Region_PinRet;
				HTuple ho_Row_PinRet, hv_Column_PinRet;
				HTuple hv_Area_BlobRet, hv_Row_BlobRet, hv_Column_BlobRet, hv_RecLen1_BlobRet, hv_RecLen2_BlobRet, hv_Circularity_BlobRet, hv_Rectangularity_BlobRet,
					hv_Width_BlobRet, hv_Height_BlobRet;
				int ret = PinBlob(GrayImg, blobStruct, ho_Region_PinRet, ho_Row_PinRet, hv_Column_PinRet,
					hv_Area_BlobRet, hv_Row_BlobRet, hv_Column_BlobRet, hv_RecLen1_BlobRet, hv_RecLen2_BlobRet, hv_Circularity_BlobRet, hv_Rectangularity_BlobRet,
					hv_Width_BlobRet, hv_Height_BlobRet);
				if (ret != 0)
				{
					//没有找到pin就先不要画区域了
					//ConcatObj(resultParams.ho_Region_NGPin, blobStruct.ho_SearchRegion, &resultParams.ho_Region_NGPin);
					//全部给-1
					HTuple valueT;
					TupleGenConst(blobStruct.hv_BlobCount, -1, &valueT);
					ho_Row_PinRet = valueT;
					hv_Column_PinRet = valueT;
					resultParams.hv_Row_Array.Append(ho_Row_PinRet);
					resultParams.hv_Column_Array.Append(hv_Column_PinRet);
				}
				else
				{
					auto blob1 = blobStruct.hv_BlobCount.D();
					auto blob2 = ho_Row_PinRet.TupleLength().D();
					if (blobStruct.hv_BlobCount == ho_Row_PinRet.TupleLength())
					{
						//Pin区域的结果输出
						ConcatObj(resultParams.ho_Region_AllPin, ho_Region_PinRet, &resultParams.ho_Region_AllPin);
						resultParams.hv_Row_Array.Append(ho_Row_PinRet);
						resultParams.hv_Column_Array.Append(hv_Column_PinRet);
					}
					else
					{
						//漏检
						if (blobStruct.hv_BlobCount > ho_Row_PinRet.TupleLength())
						{
							ConcatObj(resultParams.ho_Region_AllPin, ho_Region_PinRet, &resultParams.ho_Region_AllPin);
							resultParams.hv_Row_Array.Append(ho_Row_PinRet);
							resultParams.hv_Column_Array.Append(hv_Column_PinRet);
							//计算漏检数量
							int missCount = blobStruct.hv_BlobCount - ho_Row_PinRet.TupleLength();
							HTuple valueTemp;
							TupleGenConst(missCount, -1, &valueTemp);
							ho_Row_PinRet.Append(valueTemp);
							hv_Column_PinRet.Append(valueTemp);
							resultParams.hv_Row_Array.Append(valueTemp);
							resultParams.hv_Column_Array.Append(valueTemp);
							//把漏检的区域赋值出来(把搜索区域当做找到的Pin区域赋值出来)
							for (int q2 = 0; q2 < missCount; q2++)
							{
								ConcatObj(resultParams.ho_Region_AllPin, blobStruct.ho_SearchRegion, &resultParams.ho_Region_AllPin);
							}
						}
						else//过检
						{
							//计算多检数量(多检数量应该剔除，否则数据会混乱)
							int AddCount = ho_Row_PinRet.TupleLength() - blobStruct.hv_BlobCount;
							HTuple rowTemp, colTemp;
							for (int q1 = 0; q1 < blobStruct.hv_BlobCount; q1++)
							{
								HObject addRegion;
								SelectObj(ho_Region_PinRet, &addRegion, q1 + 1);
								ConcatObj(resultParams.ho_Region_AllPin, addRegion, &resultParams.ho_Region_AllPin);
								resultParams.hv_Row_Array.Append(ho_Row_PinRet[q1]);
								resultParams.hv_Column_Array.Append(hv_Column_PinRet[q1]);
								rowTemp.Append(ho_Row_PinRet[q1]);
								colTemp.Append(hv_Column_PinRet[q1]);
							}
							ho_Row_PinRet.Clear();
							hv_Column_PinRet.Clear();
							ho_Row_PinRet = rowTemp;
							hv_Column_PinRet = colTemp;
						}
					}
				}
				//计算Pin区域中心到基准边的距离
				HTuple pinCount;
				TupleLength(ho_Row_PinRet, &pinCount);
				if (runParams.hv_BaseType[HTuple(runParams.hv_X_BindingNum[i])] == mType.BaseType_Line &&
					runParams.hv_BaseType[HTuple(runParams.hv_Y_BindingNum[i])] == mType.BaseType_Line)
				{
					//如果基准是直线，计算所有pin中心点到直线的距离
					//分别计算所有点到水平和竖直基准边的距离
					for (int j = 0; j < pinCount; j++)
					{
						if (resultParams.hv_Row1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 || resultParams.hv_Column1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 ||
							resultParams.hv_Row2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 || resultParams.hv_Column2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1)
						{
							//没有找到基准
							resultParams.hv_DisX_Pixel_Array.Append(99999);
							resultParams.hv_DisY_Pixel_Array.Append(99999);
							resultParams.hv_DisX_MM_Array.Append(99999);
							resultParams.hv_DisY_MM_Array.Append(99999);
						}
						else if (ho_Row_PinRet[j] == -1 || hv_Column_PinRet[j] == -1)
						{
							//没有找到针
							resultParams.hv_DisX_Pixel_Array.Append(-99999);
							resultParams.hv_DisY_Pixel_Array.Append(-99999);
							resultParams.hv_DisX_MM_Array.Append(-99999);
							resultParams.hv_DisY_MM_Array.Append(-99999);
						}
						else
						{
							HTuple hv_Distance_X;
							//找点到直线的距离
							DistancePl(ho_Row_PinRet[j], hv_Column_PinRet[j],
								resultParams.hv_Row1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])], resultParams.hv_Column1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])],
								resultParams.hv_Row2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])], resultParams.hv_Column2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])],
								&hv_Distance_X);
							HTuple hv_Distance_Y;
							//找点到直线的距离
							DistancePl(ho_Row_PinRet[j], hv_Column_PinRet[j],
								resultParams.hv_Row1_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])], resultParams.hv_Column1_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])],
								resultParams.hv_Row2_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])], resultParams.hv_Column2_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])],
								&hv_Distance_Y);
							resultParams.hv_DisX_Pixel_Array.Append(hv_Distance_X);
							resultParams.hv_DisY_Pixel_Array.Append(hv_Distance_Y);
							resultParams.hv_DisX_MM_Array.Append(hv_Distance_X * runParams.hv_MmPixel);
							resultParams.hv_DisY_MM_Array.Append(hv_Distance_Y * runParams.hv_MmPixel);
						}
					}
				}
				else if (runParams.hv_BaseType[HTuple(runParams.hv_X_BindingNum[i])] == mType.BaseType_Circle &&
					runParams.hv_BaseType[HTuple(runParams.hv_Y_BindingNum[i])] == mType.BaseType_Circle)
				{
					//如果基准是圆，计算圆心到pin中心距离
					for (int j = 0; j < pinCount; j++)
					{
						if (resultParams.hv_Row_BaseCircle_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 || resultParams.hv_Column_BaseCircle_Ret[HTuple(runParams.hv_Y_BindingNum[i])] == -1 ||
							resultParams.hv_Radius_BaseCircle_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1)
						{
							//没有找到基准
							resultParams.hv_DisX_Pixel_Array.Append(99999);
							resultParams.hv_DisY_Pixel_Array.Append(99999);
							resultParams.hv_DisX_MM_Array.Append(99999);
							resultParams.hv_DisY_MM_Array.Append(99999);
						}
						else if (ho_Row_PinRet[j] == -1 || hv_Column_PinRet[j] == -1)
						{
							//没有找到针
							resultParams.hv_DisX_Pixel_Array.Append(-99999);
							resultParams.hv_DisY_Pixel_Array.Append(-99999);
							resultParams.hv_DisX_MM_Array.Append(-99999);
							resultParams.hv_DisY_MM_Array.Append(-99999);
						}
						else
						{
							HTuple hv_Distance_X, hv_Distance_Y;
							hv_Distance_Y = ho_Row_PinRet[j] - resultParams.hv_Row_BaseCircle_Ret[HTuple(runParams.hv_X_BindingNum[i])];
							hv_Distance_X = hv_Column_PinRet[j] - resultParams.hv_Column_BaseCircle_Ret[HTuple(runParams.hv_Y_BindingNum[i])];
							resultParams.hv_DisX_Pixel_Array.Append(hv_Distance_X);
							resultParams.hv_DisY_Pixel_Array.Append(hv_Distance_Y);
							resultParams.hv_DisX_MM_Array.Append(hv_Distance_X * runParams.hv_MmPixel);
							resultParams.hv_DisY_MM_Array.Append(hv_Distance_Y * runParams.hv_MmPixel);
						}
					}
				}
				else
				{
					//基准不存在
					if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
					{
						AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
					}
					else
					{
						resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
					}
					//mResultParams = resultParams;
					return 1;
				}
			}
			else if (runParams.hv_DetectType[i] == mType.DetectType_Template)
			{
				//临时参数
				RunParamsStruct_Pin modelStruct;
				SelectObj(runParams.ho_SearchRegion, &modelStruct.ho_SearchRegion, i + 1);
				if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
				{
					AffineTransRegion(modelStruct.ho_SearchRegion, &modelStruct.ho_SearchRegion, hv_ROIHomMat2D, "nearest_neighbor");
				}
				//搜索区域轮廓添加到结果轮廓输出
				HObject roiContour;
				GenContourRegionXld(modelStruct.ho_SearchRegion, &roiContour, "border");
				ConcatObj(resultParams.ho_Contour_Base, roiContour, &resultParams.ho_Contour_Base);

				modelStruct.hv_DetectType = runParams.hv_DetectType[i];
				modelStruct.hv_SortType = runParams.hv_SortType[i];
				//模板匹配参数
				modelStruct.hv_MatchMethod = runParams.hv_MatchMethod[i];
				modelStruct.hv_AngleStart = runParams.hv_AngleStart[i];            //起始角度
				modelStruct.hv_AngleExtent = runParams.hv_AngleExtent[i];                //角度范围
				modelStruct.hv_ScaleRMin = runParams.hv_ScaleRMin[i];                 //最小行缩放
				modelStruct.hv_ScaleRMax = runParams.hv_ScaleRMax[i];                 //最大行缩放
				modelStruct.hv_ScaleCMin = runParams.hv_ScaleCMin[i];                 //最小列缩放
				modelStruct.hv_ScaleCMax = runParams.hv_ScaleCMax[i];				   //最大列缩放
				modelStruct.hv_MinScore = runParams.hv_MinScore[i];             //最低分数
				modelStruct.hv_NumMatches = runParams.hv_NumMatches[i];               //匹配个数，0则自动选择，100则最多匹配100个
				modelStruct.hv_ModelID = runParams.hv_ModelID[i];				//模型句柄

				//找模板开始
				HObject ho_Region_PinRet, ho_XLD_Ret;
				HTuple ho_Row_PinRet, hv_Column_PinRet, hv_Angle_PinRet, hv_Score_PinRet;
				int ret = FindTemplate(GrayImg, 1, modelStruct, ho_XLD_Ret, ho_Region_PinRet, ho_Row_PinRet, hv_Column_PinRet, hv_Angle_PinRet, hv_Score_PinRet);
				if (ret != 0)
				{
					//没有找到pin
					//没有找到pin就先不要画区域了
					//ConcatObj(resultParams.ho_Region_NGPin, modelStruct.ho_SearchRegion, &resultParams.ho_Region_NGPin);
					HTuple valueT;
					TupleGenConst(modelStruct.hv_NumMatches, -1, &valueT);
					ho_Row_PinRet = valueT;
					hv_Column_PinRet = valueT;
					hv_Angle_PinRet = valueT;
					resultParams.hv_Row_Array.Append(ho_Row_PinRet);
					resultParams.hv_Column_Array.Append(hv_Column_PinRet);
				}
				else
				{
					if (modelStruct.hv_NumMatches == ho_Row_PinRet.TupleLength())
					{
						//Pin区域的结果输出
						ConcatObj(resultParams.ho_Region_AllPin, ho_Region_PinRet, &resultParams.ho_Region_AllPin);
						resultParams.hv_Row_Array.Append(ho_Row_PinRet);
						resultParams.hv_Column_Array.Append(hv_Column_PinRet);
					}
					else
					{
						//漏检
						if (modelStruct.hv_NumMatches > ho_Row_PinRet.TupleLength())
						{
							//先把结果输出
							ConcatObj(resultParams.ho_Region_AllPin, ho_Region_PinRet, &resultParams.ho_Region_AllPin);
							resultParams.hv_Row_Array.Append(ho_Row_PinRet);
							resultParams.hv_Column_Array.Append(hv_Column_PinRet);
							//计算漏检数量
							HTuple missCount = modelStruct.hv_NumMatches - ho_Row_PinRet.TupleLength();
							if (missCount > 0)
							{
								HTuple valueTemp;
								TupleGenConst(missCount, -1, &valueTemp);
								ho_Row_PinRet.Append(valueTemp);
								hv_Column_PinRet.Append(valueTemp);
								hv_Angle_PinRet.Append(valueTemp);
								resultParams.hv_Row_Array.Append(valueTemp);
								resultParams.hv_Column_Array.Append(valueTemp);
								//把漏检的搜索区域赋值出来(把搜索区域当做Pin区域输出到所有区域)
								for (int q2 = 0; q2 < missCount; q2++)
								{
									ConcatObj(resultParams.ho_Region_AllPin, modelStruct.ho_SearchRegion, &resultParams.ho_Region_AllPin);
								}
							}
						}
						else//过检
						{
							//计算多检数量(多检数量应该剔除，否则数据会混乱)
							int AddCount = ho_Row_PinRet.TupleLength() - modelStruct.hv_NumMatches;
							HTuple rowTemp, colTemp;
							for (int q1 = 0; q1 < modelStruct.hv_NumMatches; q1++)
							{
								HObject addRegion;
								SelectObj(ho_Region_PinRet, &addRegion, q1 + 1);
								ConcatObj(resultParams.ho_Region_AllPin, addRegion, &resultParams.ho_Region_AllPin);
								resultParams.hv_Row_Array.Append(ho_Row_PinRet[q1]);
								resultParams.hv_Column_Array.Append(hv_Column_PinRet[q1]);
								rowTemp.Append(ho_Row_PinRet[q1]);
								colTemp.Append(hv_Column_PinRet[q1]);
							}
							ho_Row_PinRet.Clear();
							hv_Column_PinRet.Clear();
							ho_Row_PinRet = rowTemp;
							hv_Column_PinRet = colTemp;
						}
					}
				}
				//计算Pin区域中心到基准边的距离
				HTuple pinCount;
				TupleLength(ho_Row_PinRet, &pinCount);
				if (runParams.hv_BaseType[HTuple(runParams.hv_X_BindingNum[i])] == mType.BaseType_Line &&
					runParams.hv_BaseType[HTuple(runParams.hv_Y_BindingNum[i])] == mType.BaseType_Line)
				{
					//如果基准是直线，计算所有pin中心点到直线的距离
					//分别计算所有点到水平和竖直基准边的距离
					for (int j = 0; j < pinCount; j++)
					{
						if (resultParams.hv_Row1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 || resultParams.hv_Column1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 ||
							resultParams.hv_Row2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 || resultParams.hv_Column2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1)
						{
							//没有找到基准
							resultParams.hv_DisX_Pixel_Array.Append(99999);
							resultParams.hv_DisY_Pixel_Array.Append(99999);
							resultParams.hv_DisX_MM_Array.Append(99999);
							resultParams.hv_DisY_MM_Array.Append(99999);
						}
						else if (ho_Row_PinRet[j] == -1 || hv_Column_PinRet[j] == -1)
						{
							//没有找到针
							resultParams.hv_DisX_Pixel_Array.Append(-99999);
							resultParams.hv_DisY_Pixel_Array.Append(-99999);
							resultParams.hv_DisX_MM_Array.Append(-99999);
							resultParams.hv_DisY_MM_Array.Append(-99999);
						}
						else
						{
							HTuple hv_Distance_X;
							//找点到直线的距离
							DistancePl(ho_Row_PinRet[j], hv_Column_PinRet[j],
								resultParams.hv_Row1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])], resultParams.hv_Column1_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])],
								resultParams.hv_Row2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])], resultParams.hv_Column2_BaseLine_Ret[HTuple(runParams.hv_X_BindingNum[i])],
								&hv_Distance_X);
							HTuple hv_Distance_Y;
							//找点到直线的距离
							DistancePl(ho_Row_PinRet[j], hv_Column_PinRet[j],
								resultParams.hv_Row1_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])], resultParams.hv_Column1_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])],
								resultParams.hv_Row2_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])], resultParams.hv_Column2_BaseLine_Ret[HTuple(runParams.hv_Y_BindingNum[i])],
								&hv_Distance_Y);
							resultParams.hv_DisX_Pixel_Array.Append(hv_Distance_X);
							resultParams.hv_DisY_Pixel_Array.Append(hv_Distance_Y);
							resultParams.hv_DisX_MM_Array.Append(hv_Distance_X * runParams.hv_MmPixel);
							resultParams.hv_DisY_MM_Array.Append(hv_Distance_Y * runParams.hv_MmPixel);
						}
					}
				}
				else if (runParams.hv_BaseType[HTuple(runParams.hv_X_BindingNum[i])] == mType.BaseType_Circle &&
					runParams.hv_BaseType[HTuple(runParams.hv_Y_BindingNum[i])] == mType.BaseType_Circle)
				{
					//如果基准是圆，计算圆心到pin中心距离
					for (int j = 0; j < pinCount; j++)
					{
						if (resultParams.hv_Row_BaseCircle_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1 || resultParams.hv_Column_BaseCircle_Ret[HTuple(runParams.hv_Y_BindingNum[i])] == -1 ||
							resultParams.hv_Radius_BaseCircle_Ret[HTuple(runParams.hv_X_BindingNum[i])] == -1)
						{
							//没有找到基准
							resultParams.hv_DisX_Pixel_Array.Append(99999);
							resultParams.hv_DisY_Pixel_Array.Append(99999);
							resultParams.hv_DisX_MM_Array.Append(99999);
							resultParams.hv_DisY_MM_Array.Append(99999);
						}
						else if (ho_Row_PinRet[j] == -1 || hv_Column_PinRet[j] == -1)
						{
							//没有找到针
							resultParams.hv_DisX_Pixel_Array.Append(-99999);
							resultParams.hv_DisY_Pixel_Array.Append(-99999);
							resultParams.hv_DisX_MM_Array.Append(-99999);
							resultParams.hv_DisY_MM_Array.Append(-99999);
						}
						else
						{
							HTuple hv_Distance_X, hv_Distance_Y;
							hv_Distance_Y = ho_Row_PinRet[j] - resultParams.hv_Row_BaseCircle_Ret[HTuple(runParams.hv_X_BindingNum[i])];
							hv_Distance_X = hv_Column_PinRet[j] - resultParams.hv_Column_BaseCircle_Ret[HTuple(runParams.hv_Y_BindingNum[i])];
							resultParams.hv_DisX_Pixel_Array.Append(hv_Distance_X);
							resultParams.hv_DisY_Pixel_Array.Append(hv_Distance_Y);
							resultParams.hv_DisX_MM_Array.Append(hv_Distance_X * runParams.hv_MmPixel);
							resultParams.hv_DisY_MM_Array.Append(hv_Distance_Y * runParams.hv_MmPixel);
						}
					}
				}
				else
				{
					//基准不存在
					if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
					{
						AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
					}
					else
					{
						resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
					}
					//mResultParams = resultParams;
					return 1;
				}
			}
			else
			{
				//Pin检测类型不存在
				if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
				{
					AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
				}
				else
				{
					resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
				}
				//mResultParams = resultParams;
				return 1;
			}
		}

		//计算每一个Pin区域到基准边距离，与客户设置的基准值的偏差(使用i < pinCount,如果Pin区域漏检，会导致数据错乱，暂时先这么写)
		HTuple CountPin1;
		TupleLength(resultParams.hv_DisX_Pixel_Array, &CountPin1);
		auto pin1 = CountPin1.D();
		HTuple CountPin2;
		TupleLength(runParams.hv_StandardX_Array, &CountPin2);
		auto pin2 = CountPin2.D();
		if (CountPin1 != CountPin2)
		{
			//客户的基准数量与找到的pin数量不一致
			if (runParams.hv_Check_PositionCorrection == 1 && hv_ROIHomMat2D != NULL)
			{
				AffineTransRegion(runParams.ho_SearchRegion, &resultParams.ho_Region_NGPin, hv_ROIHomMat2D, "nearest_neighbor");
			}
			else
			{
				resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
			}
			//mResultParams = resultParams;
			return 1;
		}
		for (int i = 0; i < CountPin1; i++)
		{
			//先判断有没有找到针，如果没有找到直接NG.如果找到针，则判断是否需要基准数据，如果不需要基准，直接给0.否则计算到基准距离
			if (resultParams.hv_DisX_Pixel_Array[i] == -99999 && resultParams.hv_DisY_Pixel_Array[i] == -99999 &&
				resultParams.hv_DisX_MM_Array[i] == -99999 && resultParams.hv_DisY_MM_Array[i] == -99999)
			{
				//没有找到针
				resultParams.hv_OffsetX_MM_Array.Append(99999);
				resultParams.hv_OffsetY_MM_Array.Append(99999);
				resultParams.hv_OffsetX_Pixel_Array.Append(99999);
				resultParams.hv_OffsetY_Pixel_Array.Append(99999);
				//偏差超限，NG
				HObject ho_ObjectSelected6;
				SelectObj(resultParams.ho_Region_AllPin, &ho_ObjectSelected6, i + 1);
				ConcatObj(resultParams.ho_Region_NGPin, ho_ObjectSelected6, &resultParams.ho_Region_NGPin);
				resultParams.ngIdx.push_back(i + 1);
				ngNum += 1;
			}
			else
			{
				if (resultParams.hv_DisX_Pixel_Array[i] == 0 && resultParams.hv_DisY_Pixel_Array[i] == 0 &&
					resultParams.hv_DisX_MM_Array[i] == 0 && resultParams.hv_DisY_MM_Array[i] == 0)
				{
					//无基准，不需要计算偏差
					resultParams.hv_OffsetX_MM_Array.Append(0);
					resultParams.hv_OffsetY_MM_Array.Append(0);
					resultParams.hv_OffsetX_Pixel_Array.Append(0);
					resultParams.hv_OffsetY_Pixel_Array.Append(0);
					//ok
					HObject ho_ObjectSelected6;
					SelectObj(resultParams.ho_Region_AllPin, &ho_ObjectSelected6, i + 1);
					ConcatObj(resultParams.ho_Region_OKPin, ho_ObjectSelected6, &resultParams.ho_Region_OKPin);
				}
				else if (resultParams.hv_DisX_Pixel_Array[i] == 99999 && resultParams.hv_DisY_Pixel_Array[i] == 99999 &&
					resultParams.hv_DisX_MM_Array[i] == 99999 && resultParams.hv_DisY_MM_Array[i] == 99999)
				{
					//查找基准失败，赋值99999					
					resultParams.hv_OffsetX_MM_Array.Append(99999);
					resultParams.hv_OffsetY_MM_Array.Append(99999);
					resultParams.hv_OffsetX_Pixel_Array.Append(99999);
					resultParams.hv_OffsetY_Pixel_Array.Append(99999);
					//偏差超限，NG
					HObject ho_ObjectSelected6;
					SelectObj(resultParams.ho_Region_AllPin, &ho_ObjectSelected6, i + 1);
					ConcatObj(resultParams.ho_Region_NGPin, ho_ObjectSelected6, &resultParams.ho_Region_NGPin);
					resultParams.ngIdx.push_back(i + 1);
					ngNum += 1;
				}
				else
				{
					//计算偏差
					HTuple hv_xTemp, hv_yTemp;
					hv_xTemp = resultParams.hv_DisX_MM_Array[i] - runParams.hv_StandardX_Array[i];
					hv_yTemp = resultParams.hv_DisY_MM_Array[i] - runParams.hv_StandardY_Array[i];
					//判断绝对值是否超限
					HObject ho_ObjectSelected6;
					SelectObj(resultParams.ho_Region_AllPin, &ho_ObjectSelected6, i + 1);
					HTuple hv_Abs, hv_Abs1;
					TupleAbs(hv_xTemp, &hv_Abs);
					TupleAbs(hv_yTemp, &hv_Abs1);
					resultParams.hv_OffsetX_MM_Array.Append(((hv_Abs * 1000).TupleRound()).D() / 1000);
					resultParams.hv_OffsetY_MM_Array.Append(((hv_Abs1 * 1000).TupleRound()).D() / 1000);
					//resultParams.hv_OffsetY_MM_Array.Append(hv_Abs1);
					resultParams.hv_OffsetX_Pixel_Array.Append((((hv_Abs / runParams.hv_MmPixel) * 1000).TupleRound()) / 1000);
					resultParams.hv_OffsetY_Pixel_Array.Append((((hv_Abs1 / runParams.hv_MmPixel) * 1000).TupleRound()) / 1000);
					if (hv_Abs > runParams.hv_Tolerance_X || hv_Abs1 > runParams.hv_Tolerance_Y)
					{
						ngNum += 1;
						//偏差超限，NG
						ConcatObj(resultParams.ho_Region_NGPin, ho_ObjectSelected6, &resultParams.ho_Region_NGPin);
						resultParams.ngIdx.push_back(i + 1);
					}
					else
					{
						//ok
						ConcatObj(resultParams.ho_Region_OKPin, ho_ObjectSelected6, &resultParams.ho_Region_OKPin);
					}
				}
			}
		}
		resultParams.hv_RetNum = resultParams.hv_OffsetX_MM_Array.TupleLength();
		if (ngNum > 0)
		{
			resultParams.hv_Ret = 0;
		}
		else
		{
			resultParams.hv_Ret = 1;
		}

		//暂时先屏蔽，现在的导出不做在这 250211 erik
		//WriteCsv(runParams, resultParams);
		//mResultParams = resultParams;

		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
		//清空结果参数
		GenEmptyObj(&resultParams.ho_Region_AllPin);
		GenEmptyObj(&resultParams.ho_Region_OKPin);
		GenEmptyObj(&resultParams.ho_Region_NGPin);
		GenEmptyObj(&resultParams.ho_Contour_Base);

		resultParams.hv_RetNum = 0;
		resultParams.hv_Row1_BaseLine_Ret.Clear();
		resultParams.hv_Column1_BaseLine_Ret.Clear();
		resultParams.hv_Row2_BaseLine_Ret.Clear();
		resultParams.hv_Column2_BaseLine_Ret.Clear();

		resultParams.hv_Row_BaseCircle_Ret.Clear();
		resultParams.hv_Column_BaseCircle_Ret.Clear();
		resultParams.hv_Radius_BaseCircle_Ret.Clear();

		resultParams.hv_Row_Array.Clear();
		resultParams.hv_Column_Array.Clear();
		resultParams.hv_DisX_Pixel_Array.Clear();
		resultParams.hv_DisY_Pixel_Array.Clear();
		resultParams.hv_OffsetX_Pixel_Array.Clear();
		resultParams.hv_OffsetY_Pixel_Array.Clear();
		resultParams.hv_DisX_MM_Array.Clear();
		resultParams.hv_DisY_MM_Array.Clear();
		resultParams.hv_OffsetX_MM_Array.Clear();
		resultParams.hv_OffsetY_MM_Array.Clear();
		resultParams.hv_Ret = 0;

		resultParams.ho_Region_NGPin = runParams.ho_SearchRegion;
		//mResultParams = resultParams;
		return 1;
	}
}


//Pin找Blob函数
int PinDetect::PinBlob(const HObject &ho_Image, const RunParamsStruct_Pin &runParams, HObject &ho_Region_Pin, HTuple &hv_Row_Pin, HTuple &hv_Column_Pin,
	HTuple &hv_Area_Blob, HTuple &hv_Row_Blob, HTuple &hv_Column_Blob, HTuple &hv_RecLen1_Blob, HTuple &hv_RecLen2_Blob, HTuple &hv_Circularity_Blob, HTuple &hv_Rectangularity_Blob
	, HTuple &hv_Width_Blob, HTuple &hv_Height_Blob)
{
	try
	{
		//清空结果参数
		GenEmptyObj(&ho_Region_Pin);
		hv_Row_Pin.Clear();
		hv_Column_Pin.Clear();
		hv_Area_Blob.Clear();
		hv_Row_Blob.Clear();
		hv_Column_Blob.Clear();
		hv_RecLen1_Blob.Clear();
		hv_RecLen2_Blob.Clear();
		hv_Circularity_Blob.Clear();
		hv_Rectangularity_Blob.Clear();
		hv_Width_Blob.Clear();
		hv_Height_Blob.Clear();

		//mutex1.lock();
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			//mutex1.unlock();
			return 1;
		}

		TypeParamsStruct_Pin mType;
		HObject GrayImg;
		Rgb1ToGray(ho_Image, &GrayImg);
		//Blob分析开始
		HObject mho_Image;
		if (runParams.ho_SearchRegion.Key() == nullptr)
		{
			//搜索区域为空，或者被clear(搜索区域为空，默认全图搜索)
			mho_Image = GrayImg;
		}
		else
		{
			ReduceDomain(GrayImg, runParams.ho_SearchRegion, &mho_Image);
		}
		HObject ho_Region;
		HTuple hv_DestArea, hv_DestRow, hv_DestColumn;
		if (runParams.hv_ThresholdType == mType.Auto_Threshold)
		{
			//固定阈值
			AutoThreshold(mho_Image, &ho_Region, runParams.hv_Sigma);
			AreaCenter(ho_Region, &hv_DestArea, &hv_DestRow, &hv_DestColumn);
		}
		else if (runParams.hv_ThresholdType == mType.Dyn_Threshold)
		{
			//自适应阈值
			HObject ho_MeanImage;
			MeanImage(mho_Image, &ho_MeanImage, runParams.hv_CoreWidth, runParams.hv_CoreHeight);
			DynThreshold(mho_Image, ho_MeanImage, &ho_Region, runParams.hv_DynThresholdContrast.TupleInt(), runParams.hv_DynThresholdPolarity);
			AreaCenter(ho_Region, &hv_DestArea, &hv_DestRow, &hv_DestColumn);
		}
		else if (runParams.hv_ThresholdType == mType.Fixed_Threshold)
		{
			//动态阈值
			Threshold(mho_Image, &ho_Region, runParams.hv_LowThreshold.TupleInt(), runParams.hv_HighThreshold.TupleInt());
			AreaCenter(ho_Region, &hv_DestArea, &hv_DestRow, &hv_DestColumn);
		}
		//判断是否找到Blob
		if (hv_DestArea > 0)
		{
			//孔洞填充
			HObject ho_FillUpRegion;
			FillUpShape(ho_Region, &ho_FillUpRegion, "area", runParams.hv_FillUpShape_Min, runParams.hv_FillUpShape_Max);
			//形态学 hv_OperatorType为选择的形态学类型
			HObject ho_OperaRegion;
			if (runParams.hv_OperatorType == mType.Opening_Rec)
			{
				OpeningRectangle1(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaRec_Width, runParams.hv_OperaRec_Height);
			}
			else if (runParams.hv_OperatorType == mType.Closing_Rec)
			{
				ClosingRectangle1(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaRec_Width, runParams.hv_OperaRec_Height);
			}
			else if (runParams.hv_OperatorType == mType.Opening_Cir)
			{
				OpeningCircle(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaCir_Radius);
			}
			else if (runParams.hv_OperatorType == mType.Closing_Cir)
			{
				ClosingCircle(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaCir_Radius);
			}
			else if (runParams.hv_OperatorType == mType.Erosion_Rec)
			{
				ErosionRectangle1(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaRec_Width, runParams.hv_OperaRec_Height);
			}
			else if (runParams.hv_OperatorType == mType.Dilation_Rec)
			{
				DilationRectangle1(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaRec_Width, runParams.hv_OperaRec_Height);
			}
			else if (runParams.hv_OperatorType == mType.Erosion_Cir)
			{
				ErosionCircle(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaCir_Radius);
			}
			else if (runParams.hv_OperatorType == mType.Dilation_Cir)
			{
				DilationCircle(ho_FillUpRegion, &ho_OperaRegion, runParams.hv_OperaCir_Radius);
			}
			else
			{
				ho_OperaRegion = ho_FillUpRegion;
			}
			//区域筛选
			if (runParams.hv_SelectAreaMax == 1)
			{
				HObject ho_Connections;
				Connection(ho_OperaRegion, &ho_Connections);
				HObject ho_SecRegions;
				SelectShapeStd(ho_Connections, &ho_SecRegions, "max_area", 70);
				HTuple area, r, c;
				AreaCenter(ho_SecRegions, &area, &r, &c);
				if (area > 0)
				{
					//区域排序
					HObject ho_SortedRegions1;
					if (runParams.hv_SortType == mType.SortType_Row)
					{
						//按行排序
						SortRegion(ho_SecRegions, &ho_SortedRegions1, "first_point", "true", "row");
					}
					else if (runParams.hv_SortType == mType.SortType_Column)
					{
						//按列排序
						SortRegion(ho_SecRegions, &ho_SortedRegions1, "first_point", "true", "column");
					}
					else
					{
						ho_SortedRegions1 = ho_SecRegions;
					}

					//输出结果区域
					ho_Region_Pin = ho_SortedRegions1;
					//获取Pin数据
					HTuple hv_Phi_R2, hv_Length1_R2, hv_Length2_R2;
					SmallestRectangle2(ho_Region_Pin, &hv_Row_Pin, &hv_Column_Pin,
						&hv_Phi_R2, &hv_Length1_R2, &hv_Length2_R2);
					//获取Blob筛选数据
					RegionFeatures(ho_Region_Pin, "area", &hv_Area_Blob);
					RegionFeatures(ho_Region_Pin, "row", &hv_Row_Blob);
					RegionFeatures(ho_Region_Pin, "column", &hv_Column_Blob);
					RegionFeatures(ho_Region_Pin, "rect2_len1", &hv_RecLen1_Blob);
					RegionFeatures(ho_Region_Pin, "rect2_len2", &hv_RecLen2_Blob);
					RegionFeatures(ho_Region_Pin, "circularity", &hv_Circularity_Blob);
					RegionFeatures(ho_Region_Pin, "rectangularity", &hv_Rectangularity_Blob);
					RegionFeatures(ho_Region_Pin, "width", &hv_Width_Blob);
					RegionFeatures(ho_Region_Pin, "height", &hv_Height_Blob);
				}
				else
				{
					//没有找到Pin区域
					return 1;
				}
			}
			else
			{
				HObject ho_Connections;
				Connection(ho_OperaRegion, &ho_Connections);
				HObject ho_SecRegions;
				SelectShape(ho_Connections, &ho_SecRegions,
					(HTuple("area").Append("row").Append("column").Append("rect2_len1").Append("rect2_len2").Append("circularity").Append("rectangularity").Append("width").Append("height")), "and",
					(HTuple(runParams.hv_AreaFilter_Min).Append(runParams.hv_RowFilter_Min).Append(runParams.hv_ColumnFilter_Min).Append(runParams.hv_RecLen1Filter_Min).Append(runParams.hv_RecLen2Filter_Min).Append(runParams.hv_CircularityFilter_Min).Append(runParams.hv_RectangularityFilter_Min).Append(runParams.hv_WidthFilter_Min).Append(runParams.hv_HeightFilter_Min)),
					(HTuple(runParams.hv_AreaFilter_Max).Append(runParams.hv_RowFilter_Max).Append(runParams.hv_ColumnFilter_Max).Append(runParams.hv_RecLen1Filter_Max).Append(runParams.hv_RecLen2Filter_Max).Append(runParams.hv_CircularityFilter_Max).Append(runParams.hv_RectangularityFilter_Max).Append(runParams.hv_WidthFilter_Max).Append(runParams.hv_HeightFilter_Max)));
				HTuple objNumber;
				CountObj(ho_SecRegions, &objNumber);
				if (objNumber > 0)
				{
					//区域排序
					HObject ho_SortedRegions1;
					if (runParams.hv_SortType == mType.SortType_Row)
					{
						//按行排序
						SortRegion(ho_SecRegions, &ho_SortedRegions1, "first_point", "true", "row");
					}
					else if (runParams.hv_SortType == mType.SortType_Column)
					{
						//按列排序
						SortRegion(ho_SecRegions, &ho_SortedRegions1, "first_point", "true", "column");
					}
					else
					{
						ho_SortedRegions1 = ho_SecRegions;
					}

					//输出结果区域
					ho_Region_Pin = ho_SortedRegions1;
					//获取Pin数据
					HTuple hv_Phi_R2, hv_Length1_R2, hv_Length2_R2;
					SmallestRectangle2(ho_Region_Pin, &hv_Row_Pin, &hv_Column_Pin,
						&hv_Phi_R2, &hv_Length1_R2, &hv_Length2_R2);
					//获取Blob筛选数据
					RegionFeatures(ho_Region_Pin, "area", &hv_Area_Blob);
					RegionFeatures(ho_Region_Pin, "row", &hv_Row_Blob);
					RegionFeatures(ho_Region_Pin, "column", &hv_Column_Blob);
					RegionFeatures(ho_Region_Pin, "rect2_len1", &hv_RecLen1_Blob);
					RegionFeatures(ho_Region_Pin, "rect2_len2", &hv_RecLen2_Blob);
					RegionFeatures(ho_Region_Pin, "circularity", &hv_Circularity_Blob);
					RegionFeatures(ho_Region_Pin, "rectangularity", &hv_Rectangularity_Blob);
					RegionFeatures(ho_Region_Pin, "width", &hv_Width_Blob);
					RegionFeatures(ho_Region_Pin, "height", &hv_Height_Blob);
				}
				else
				{
					//没有找到Pin区域
					return 1;
				}
			}
		}
		else
		{
			//没有找到Pin区域
			return 1;
		}
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
		//清空结果参数
		GenEmptyObj(&ho_Region_Pin);
		hv_Row_Pin.Clear();
		hv_Column_Pin.Clear();
		hv_Area_Blob.Clear();
		hv_Row_Blob.Clear();
		hv_Column_Blob.Clear();
		hv_RecLen1_Blob.Clear();
		hv_RecLen2_Blob.Clear();
		hv_Circularity_Blob.Clear();
		hv_Rectangularity_Blob.Clear();
		hv_Width_Blob.Clear();
		hv_Height_Blob.Clear();
		return 1;
	}
}
//抓边函数
int PinDetect::FindLine(const HObject &ho_Image, const RunParamsStruct_Pin &runParams,
	HTuple &hv_Row1_Ret, HTuple &hv_Column1_Ret, HTuple &hv_Row2_Ret, HTuple &hv_Column2_Ret, HObject &ho_ContourLine_Ret)
{
	try
	{
		//清空结果
		GenEmptyObj(&ho_ContourLine_Ret);
		hv_Row1_Ret.Clear();
		hv_Column1_Ret.Clear();
		hv_Row2_Ret.Clear();
		hv_Column2_Ret.Clear();

		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			return 1;
		}

		HObject GrayImg;
		Rgb1ToGray(ho_Image, &GrayImg);
		HTuple hv_Index;            //模型id(例如同一个模型一次抓两条边，hv_Index就是[0,1])
		HTuple hv_MetrologyHandle;      //测量句柄
		//创建测量句柄
		CreateMetrologyModel(&hv_MetrologyHandle);
		//获取模型尺寸
		HTuple hv_Width, hv_Height;
		GetImageSize(GrayImg, &hv_Width, &hv_Height);
		//添加测量对象
		SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
		AddMetrologyObjectLineMeasure(hv_MetrologyHandle, runParams.hv_Row1_Base, runParams.hv_Column1_Base, runParams.hv_Row2_Base, runParams.hv_Column2_Base,
			runParams.hv_MeasureLength1, runParams.hv_MeasureLength2, runParams.hv_MeasureSigma, runParams.hv_MeasureThreshold.TupleInt(), "measure_transition", runParams.hv_MeasureTransition, &hv_Index);
		//设置测量对象的参数
		//模型卡尺长，卡尺宽，平滑系数，灰度阈值，极性，在添加模型时已经设置，可以不用再次设置
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_length1",
		//    hv_MeasureLength1);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_length2",
		//   hv_MeasureLength2);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_sigma",
		//    hv_MeasureSigma);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_threshold", hv_MeasureThreshold);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_transition", hv_MeasureTransition);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_select", runParams.hv_MeasureSelect);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_measures", runParams.hv_MeasureNum.TupleInt());
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_instances", 1);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "min_score", runParams.hv_MeasureMinScore);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "distance_threshold", runParams.hv_DistanceThreshold);

		//********************************************
		//执行抓边模型
		ApplyMetrologyModel(GrayImg, hv_MetrologyHandle);
		//获取抓边结果
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "row_begin", &hv_Row1_Ret);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "column_begin", &hv_Column1_Ret);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "row_end", &hv_Row2_Ret);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "column_end", &hv_Column2_Ret);

		//判断是否找到边
		HTuple hv_Length;
		TupleLength(hv_Row1_Ret, &hv_Length);
		if (hv_Length != 1)
		{
			//清空抓边模型
			ClearMetrologyModel(hv_MetrologyHandle);
			//mutex1.unlock();
			return 1;
		}
		//获取直线轮廓
		GetMetrologyObjectResultContour(&ho_ContourLine_Ret, hv_MetrologyHandle, 0, "all", 1.5);
		//清空抓边模型
		ClearMetrologyModel(hv_MetrologyHandle);
		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
				//清空结果
		GenEmptyObj(&ho_ContourLine_Ret);
		hv_Row1_Ret.Clear();
		hv_Column1_Ret.Clear();
		hv_Row2_Ret.Clear();
		hv_Column2_Ret.Clear();
		return -1;
	}
}
//抓圆函数
int PinDetect::FindCircle(const HObject &ho_Image, const RunParamsStruct_Pin &runParams,
	HTuple &hv_Row_Ret, HTuple &hv_Column_Ret, HTuple &hv_Radius_Ret, HObject &ho_ContourCircle_Ret)
{
	try
	{
		//清空结果
		hv_Row_Ret.Clear();
		hv_Column_Ret.Clear();
		hv_Radius_Ret.Clear();
		GenEmptyObj(&ho_ContourCircle_Ret);

		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			return 1;
		}
		HObject GrayImg;
		Rgb1ToGray(ho_Image, &GrayImg);
		HTuple hv_Index;            //模型id(例如同一个模型一次抓两个圆，hv_Index就是[0,1])
		HTuple hv_MetrologyHandle;        //测量句柄
		//创建测量句柄
		CreateMetrologyModel(&hv_MetrologyHandle);
		//获取模型尺寸
		HTuple hv_Width, hv_Height;
		GetImageSize(GrayImg, &hv_Width, &hv_Height);
		//添加测量对象
		SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
		AddMetrologyObjectCircleMeasure(hv_MetrologyHandle, runParams.hv_Row1_Base, runParams.hv_Column1_Base, runParams.hv_Radius_Base,
			runParams.hv_MeasureLength1, runParams.hv_MeasureLength2, runParams.hv_MeasureSigma, runParams.hv_MeasureThreshold.TupleInt(), "measure_transition", runParams.hv_MeasureTransition, &hv_Index);
		//设置测量对象的参数
		//模型卡尺长，卡尺宽，平滑系数，灰度阈值，极性，在添加模型时已经设置，可以不用再次设置
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_length1",
		//    hv_MeasureLength1);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_length2",
		//   hv_MeasureLength2);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_sigma",
		//    hv_MeasureSigma);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_threshold", hv_MeasureThreshold);
		//HOperatorSet.SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_transition", hv_MeasureTransition);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_select", runParams.hv_MeasureSelect);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_measures", runParams.hv_MeasureNum.TupleInt());
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_instances", 1);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "min_score", runParams.hv_MeasureMinScore);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "distance_threshold", runParams.hv_DistanceThreshold);

		//********************************************
		//执行抓圆模型
		ApplyMetrologyModel(GrayImg, hv_MetrologyHandle);
		//获取抓圆结果
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "row", &hv_Row_Ret);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "column", &hv_Column_Ret);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "radius", &hv_Radius_Ret);

		//判断是否找到圆
		HTuple hv_Length;
		TupleLength(hv_Row_Ret, &hv_Length);
		if (hv_Length != 1)
		{
			//清空抓圆模型
			ClearMetrologyModel(hv_MetrologyHandle);
			//mutex1.unlock();
			return 1;
		}
		//获取圆轮廓
		GetMetrologyObjectResultContour(&ho_ContourCircle_Ret, hv_MetrologyHandle, 0, "all", 1.5);
		//清空抓圆模型
		ClearMetrologyModel(hv_MetrologyHandle);

		//mutex1.unlock();
		return 0;
	}
	catch (...)
	{
		//mutex1.unlock();
				//清空结果
		hv_Row_Ret.Clear();
		hv_Column_Ret.Clear();
		hv_Radius_Ret.Clear();
		GenEmptyObj(&ho_ContourCircle_Ret);
		return -1;
	}
}
//查找模板
int PinDetect::FindTemplate(const HObject &ho_Image, int Type, const RunParamsStruct_Pin &RunParams, HObject &ho_XLD_Template, HObject &ho_Region_Pin, HTuple &hv_Row_Pin, HTuple &hv_Column_Pin,
	HTuple &hv_Angle_Pin, HTuple &hv_Score_Pin)
{
	if (Type == 0)
	{
		//mutex1.lock();
		try
		{
			//初始化结果
			GenEmptyObj(&ho_Region_Pin);
			GenEmptyObj(&ho_XLD_Template);
			hv_Row_Pin.Clear();
			hv_Column_Pin.Clear();
			hv_Angle_Pin.Clear();
			hv_Score_Pin.Clear();

			if (ho_Image.Key() == nullptr)
			{
				//图像为空，或者图像被clear
				return 1;
			}
			if (RunParams.hv_ModelID_PositionCorrection == NULL)
			{
				//模板句柄为空
				return 1;
			}
			HObject GrayImg;
			Rgb1ToGray(ho_Image, &GrayImg);

			//下面时是找模板代码
			TypeParamsStruct_Pin m_type;
			HTuple Homat2d_Roi;
			HTuple Homat2d_Window;
			HObject mho_Image;
			if (RunParams.ho_SearchRegion_PositionCorrection.Key() == nullptr)
			{
				//搜索区域为空，或者被clear(搜索区域为空，默认全图搜索)
				mho_Image = GrayImg;
			}
			else
			{
				ReduceDomain(GrayImg, RunParams.ho_SearchRegion_PositionCorrection, &mho_Image);
			}

			//设置超时时间
			if (RunParams.hv_MatchMethod_PositionCorrection == m_type.NccModel)
			{
				//设置查询超时时间1000ms
				SetNccModelParam(RunParams.hv_ModelID_PositionCorrection, "timeout", 1000);
			}
			else
			{
				//设置查询超时时间1000ms
				SetShapeModelParam(RunParams.hv_ModelID_PositionCorrection, "timeout", 1000);
				//设置允许部分位于图像之外
				SetShapeModelParam(RunParams.hv_ModelID_PositionCorrection, "border_shape_models", "true");
			}

			if (RunParams.hv_MatchMethod_PositionCorrection == m_type.ShapeModel)
			{
				//形状匹配
				//设置亚像素和允许轮廓变形系数(以像素为单位，范围是0-32个像素),为了获得有意义的分值并避免错误的匹配，我们建议始终将变形量与最小二乘调整相结合
				HTuple SubPixelTemp;
				SubPixelTemp = (HTuple("least_squares").Append("max_deformation 2"));

				//获取模板参数
				HTuple hv_Row_Ret, hv_Column_Ret, hv_Angle_Ret, hv_ScaleR_Ret, hv_ScaleC_Ret, hv_Score_Ret;
				HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, ScaleMinTemp, ScaleMaxTemp, ScaleStepTemp, MetricTemp, MinContrastTemp;
				GetShapeModelParams(RunParams.hv_ModelID_PositionCorrection, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &ScaleMinTemp,
					&ScaleMaxTemp, &ScaleStepTemp, &MetricTemp, &MinContrastTemp);
				FindAnisoShapeModel(mho_Image, RunParams.hv_ModelID_PositionCorrection, RunParams.hv_AngleStart_PositionCorrection.TupleRad(), 2 * (RunParams.hv_AngleExtent_PositionCorrection.TupleRad()).TupleAbs(),
					RunParams.hv_ScaleRMin_PositionCorrection, RunParams.hv_ScaleRMax_PositionCorrection, RunParams.hv_ScaleCMin_PositionCorrection, RunParams.hv_ScaleCMax_PositionCorrection, RunParams.hv_MinScore_PositionCorrection,
					RunParams.hv_NumMatches_PositionCorrection.TupleInt(), 0.5, SubPixelTemp, (HTuple(NumLevelsTemp).Append(1)), 0.9,
					&hv_Row_Ret, &hv_Column_Ret, &hv_Angle_Ret, &hv_ScaleR_Ret, &hv_ScaleC_Ret, &hv_Score_Ret);

				HObject modelRegion_Ret;
				GenEmptyObj(&modelRegion_Ret);
				HTuple _length = 0;
				TupleLength(hv_Row_Ret, &_length);
				if (_length == 0)
				{
					return 1;
				}
				else
				{
					HObject xldSort;
					GenEmptyObj(&xldSort);
					for (int i = 0; i < _length; i++)
					{
						//弧度转角度
						HTuple tempRad_Angle = hv_Angle_Ret[i];
						hv_Angle_Ret[i] = tempRad_Angle.TupleDeg();
						//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
						Homat2d_Window.Clear();
						VectorAngleToRigid(0, 0, 0, hv_Row_Ret[i], hv_Column_Ret[i], tempRad_Angle,
							&Homat2d_Window);
						//获取模板轮廓
						HObject ho_ContourRet;
						GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID_PositionCorrection, 1);
						//仿射变换
						AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
						ConcatObj(xldSort, ho_ContourRet, &xldSort);
						//获取模板区域
						HTuple countXld;
						CountObj(ho_ContourRet, &countXld);
						HObject region1;
						GenEmptyObj(&region1);
						for (int j = 0; j < countXld; j++)
						{
							HObject xldSelect;
							SelectObj(ho_ContourRet, &xldSelect, j + 1);
							HTuple rowXld, colXld;
							GetContourXld(xldSelect, &rowXld, &colXld);
							HObject region2;
							GenRegionPoints(&region2, rowXld, colXld);
							ConcatObj(region1, region2, &region1);
						}
						HObject regionUnion;
						Union1(region1, &regionUnion);
						ConcatObj(modelRegion_Ret, regionUnion, &modelRegion_Ret);
					}
					//区域排序
					HObject ho_SortedRegions1;
					GenEmptyObj(&ho_SortedRegions1);
					HTuple indexR, RowNew, ColumnNew, AngleNew, ScoreNew;
					if (RunParams.hv_SortType_PositionCorrection == m_type.SortType_Row)
					{
						//按行排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "row");
						TupleSortIndex(hv_Row_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else if (RunParams.hv_SortType_PositionCorrection == m_type.SortType_Column)
					{
						//按列排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "column");
						TupleSortIndex(hv_Column_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else
					{
						ho_SortedRegions1 = modelRegion_Ret;
						RowNew = hv_Row_Ret;
						ColumnNew = hv_Column_Ret;
						AngleNew = hv_Angle_Ret;
						ScoreNew = hv_Score_Ret;
					}
					//结果导出
					ho_XLD_Template = xldSort;
					ho_Region_Pin = ho_SortedRegions1;
					hv_Row_Pin = RowNew;
					hv_Column_Pin = ColumnNew;
					hv_Angle_Pin = AngleNew;
					hv_Score_Pin = ScoreNew;
				}
			}
			else if (RunParams.hv_MatchMethod_PositionCorrection == m_type.NccModel)
			{
				//互相关匹配
				HTuple hv_Row_Ret, hv_Column_Ret, hv_Angle_Ret, hv_ScaleR_Ret, hv_ScaleC_Ret, hv_Score_Ret;
				HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, MetricTemp;
				GetNccModelParams(RunParams.hv_ModelID_PositionCorrection, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &MetricTemp);
				FindNccModel(mho_Image, RunParams.hv_ModelID_PositionCorrection, RunParams.hv_AngleStart_PositionCorrection.TupleRad(), 2 * (RunParams.hv_AngleExtent_PositionCorrection.TupleRad()).TupleAbs(),
					RunParams.hv_MinScore_PositionCorrection, RunParams.hv_NumMatches_PositionCorrection.TupleInt(), 0.5, "true", (HTuple(NumLevelsTemp).Append(1)),
					&hv_Row_Ret, &hv_Column_Ret, &hv_Angle_Ret, &hv_Score_Ret);

				HObject modelRegion_Ret;
				GenEmptyObj(&modelRegion_Ret);
				HTuple _length = 0;
				TupleLength(hv_Row_Ret, &_length);
				if (_length == 0)
				{
					return 1;
				}
				else
				{
					HObject xldSort;
					GenEmptyObj(&xldSort);
					for (int i = 0; i < _length; i++)
					{
						//弧度转角度
						HTuple tempRad_Angle = hv_Angle_Ret[i];
						hv_Angle_Ret[i] = tempRad_Angle.TupleDeg();

						//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
						Homat2d_Window.Clear();
						VectorAngleToRigid(0, 0, 0, hv_Row_Ret[i], hv_Column_Ret[i], tempRad_Angle,
							&Homat2d_Window);
						//获取模板轮廓
						HObject ho_RegionRet;
						GetNccModelRegion(&ho_RegionRet, RunParams.hv_ModelID_PositionCorrection); //获取模板轮廓
						//获取模板区域
						HObject regionAffine;
						AffineTransRegion(ho_RegionRet, &regionAffine, Homat2d_Window, "nearest_neighbor");
						ConcatObj(modelRegion_Ret, regionAffine, &modelRegion_Ret);
						HObject xldTemp;
						GenContourRegionXld(regionAffine, &xldTemp, "border");
						ConcatObj(xldSort, xldTemp, &xldSort);
					}
					//区域排序
					HObject ho_SortedRegions1;
					GenEmptyObj(&ho_SortedRegions1);
					HTuple indexR, RowNew, ColumnNew, AngleNew, ScoreNew;
					if (RunParams.hv_SortType_PositionCorrection == m_type.SortType_Row)
					{
						//按行排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "row");
						TupleSortIndex(hv_Row_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else if (RunParams.hv_SortType_PositionCorrection == m_type.SortType_Column)
					{
						//按列排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "column");
						TupleSortIndex(hv_Column_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else
					{
						ho_SortedRegions1 = modelRegion_Ret;
						RowNew = hv_Row_Ret;
						ColumnNew = hv_Column_Ret;
						AngleNew = hv_Angle_Ret;
						ScoreNew = hv_Score_Ret;
					}
					//结果导出
					ho_XLD_Template = xldSort;
					ho_Region_Pin = ho_SortedRegions1;
					hv_Row_Pin = RowNew;
					hv_Column_Pin = ColumnNew;
					hv_Angle_Pin = AngleNew;
					hv_Score_Pin = ScoreNew;
				}
			}
			else
			{
				return 1;
			}
			return 0;
		}
		catch (...)
		{
			//初始化结果
			GenEmptyObj(&ho_XLD_Template);
			GenEmptyObj(&ho_Region_Pin);
			hv_Row_Pin.Clear();
			hv_Column_Pin.Clear();
			hv_Angle_Pin.Clear();
			hv_Score_Pin.Clear();
			return -1;
		}
	}
	else
	{
		//mutex1.lock();
		try
		{
			//初始化结果
			GenEmptyObj(&ho_XLD_Template);
			GenEmptyObj(&ho_Region_Pin);
			hv_Row_Pin.Clear();
			hv_Column_Pin.Clear();
			hv_Angle_Pin.Clear();
			hv_Score_Pin.Clear();

			if (ho_Image.Key() == nullptr)
			{
				//图像为空，或者图像被clear
				return 1;
			}
			if (RunParams.hv_ModelID == NULL)
			{
				//模板句柄为空
				return 1;
			}
			HObject GrayImg;
			Rgb1ToGray(ho_Image, &GrayImg);

			//下面时是找模板代码
			TypeParamsStruct_Pin m_type;
			HTuple Homat2d_Roi;
			HTuple Homat2d_Window;
			HObject mho_Image;
			if (RunParams.ho_SearchRegion.Key() == nullptr)
			{
				//搜索区域为空，或者被clear(搜索区域为空，默认全图搜索)
				mho_Image = GrayImg;
			}
			else
			{
				ReduceDomain(GrayImg, RunParams.ho_SearchRegion, &mho_Image);
			}

			//设置超时时间
			if (RunParams.hv_MatchMethod == m_type.NccModel)
			{
				//设置查询超时时间1000ms
				SetNccModelParam(RunParams.hv_ModelID, "timeout", 1000);
			}
			else
			{
				//设置查询超时时间1000ms
				SetShapeModelParam(RunParams.hv_ModelID, "timeout", 1000);
				//设置允许部分位于图像之外
				SetShapeModelParam(RunParams.hv_ModelID, "border_shape_models", "true");
			}

			if (RunParams.hv_MatchMethod == m_type.ShapeModel)
			{
				//设置亚像素和允许轮廓变形系数(以像素为单位，范围是0-32个像素),为了获得有意义的分值并避免错误的匹配，我们建议始终将变形量与最小二乘调整相结合
				HTuple SubPixelTemp;
				SubPixelTemp = (HTuple("least_squares").Append("max_deformation 2"));

				//获取模板参数
				HTuple hv_Row_Ret, hv_Column_Ret, hv_Angle_Ret, hv_ScaleR_Ret, hv_ScaleC_Ret, hv_Score_Ret;
				HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, ScaleMinTemp, ScaleMaxTemp, ScaleStepTemp, MetricTemp, MinContrastTemp;
				GetShapeModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &ScaleMinTemp,
					&ScaleMaxTemp, &ScaleStepTemp, &MetricTemp, &MinContrastTemp);
				FindAnisoShapeModel(mho_Image, RunParams.hv_ModelID, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
					RunParams.hv_ScaleRMin, RunParams.hv_ScaleRMax, RunParams.hv_ScaleCMin, RunParams.hv_ScaleCMax, RunParams.hv_MinScore,
					RunParams.hv_NumMatches.TupleInt(), 0.5, SubPixelTemp, (HTuple(NumLevelsTemp).Append(1)), 0.9,
					&hv_Row_Ret, &hv_Column_Ret, &hv_Angle_Ret, &hv_ScaleR_Ret, &hv_ScaleC_Ret, &hv_Score_Ret);

				HObject modelRegion_Ret;
				GenEmptyObj(&modelRegion_Ret);
				HTuple _length = 0;
				TupleLength(hv_Row_Ret, &_length);
				if (_length == 0)
				{
					return 1;
				}
				else
				{
					for (int i = 0; i < _length; i++)
					{
						//弧度转角度
						HTuple tempRad_Angle = hv_Angle_Ret[i];
						hv_Angle_Ret[i] = tempRad_Angle.TupleDeg();
						//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
						Homat2d_Window.Clear();
						VectorAngleToRigid(0, 0, 0, hv_Row_Ret[i], hv_Column_Ret[i], tempRad_Angle,
							&Homat2d_Window);
						//获取模板轮廓
						HObject ho_ContourRet;
						GetShapeModelContours(&ho_ContourRet, RunParams.hv_ModelID, 1);
						//仿射变换
						AffineTransContourXld(ho_ContourRet, &ho_ContourRet, Homat2d_Window);
						//获取模板区域
						HTuple countXld;
						CountObj(ho_ContourRet, &countXld);
						HObject region1;
						GenEmptyObj(&region1);
						for (int j = 0; j < countXld; j++)
						{
							HObject xldSelect;
							SelectObj(ho_ContourRet, &xldSelect, j + 1);
							HTuple rowXld, colXld;
							GetContourXld(xldSelect, &rowXld, &colXld);
							HObject region2;
							GenRegionPoints(&region2, rowXld, colXld);
							ConcatObj(region1, region2, &region1);
						}
						HObject regionUnion;
						Union1(region1, &regionUnion);
						ConcatObj(modelRegion_Ret, regionUnion, &modelRegion_Ret);
					}
					//区域排序
					HObject ho_SortedRegions1;
					GenEmptyObj(&ho_SortedRegions1);
					HTuple indexR, RowNew, ColumnNew, AngleNew, ScoreNew;
					if (RunParams.hv_SortType == m_type.SortType_Row)
					{
						//按行排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "row");
						TupleSortIndex(hv_Row_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else if (RunParams.hv_SortType == m_type.SortType_Column)
					{
						//按列排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "column");
						TupleSortIndex(hv_Column_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else
					{
						ho_SortedRegions1 = modelRegion_Ret;
						RowNew = hv_Row_Ret;
						ColumnNew = hv_Column_Ret;
						AngleNew = hv_Angle_Ret;
						ScoreNew = hv_Score_Ret;
					}
					//结果导出
					ho_Region_Pin = ho_SortedRegions1;
					hv_Row_Pin = RowNew;
					hv_Column_Pin = ColumnNew;
					hv_Angle_Pin = AngleNew;
					hv_Score_Pin = ScoreNew;
				}
			}
			else if (RunParams.hv_MatchMethod == m_type.NccModel)
			{
				HTuple hv_Row_Ret, hv_Column_Ret, hv_Angle_Ret, hv_ScaleR_Ret, hv_ScaleC_Ret, hv_Score_Ret;
				HTuple NumLevelsTemp, AngleStartTemp, AngleExtentTemp, AngleStepTemp, MetricTemp;
				GetNccModelParams(RunParams.hv_ModelID, &NumLevelsTemp, &AngleStartTemp, &AngleExtentTemp, &AngleStepTemp, &MetricTemp);
				FindNccModel(mho_Image, RunParams.hv_ModelID, RunParams.hv_AngleStart.TupleRad(), 2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
					RunParams.hv_MinScore, RunParams.hv_NumMatches.TupleInt(), 0.5, "true", (HTuple(NumLevelsTemp).Append(1)),
					&hv_Row_Ret, &hv_Column_Ret, &hv_Angle_Ret, &hv_Score_Ret);

				HObject modelRegion_Ret;
				GenEmptyObj(&modelRegion_Ret);
				HTuple _length = 0;
				TupleLength(hv_Row_Ret, &_length);
				if (_length == 0)
				{
					return 1;
				}
				else
				{
					for (int i = 0; i < _length; i++)
					{
						//弧度转角度
						HTuple tempRad_Angle = hv_Angle_Ret[i];
						hv_Angle_Ret[i] = tempRad_Angle.TupleDeg();

						//生成ROI仿射变换矩阵(用于显示模板轮廓)，Halcon默认模板图形显示在左上角(0,0,0),利用仿射变换把他显示到图像中间
						Homat2d_Window.Clear();
						VectorAngleToRigid(0, 0, 0, hv_Row_Ret[i], hv_Column_Ret[i], tempRad_Angle,
							&Homat2d_Window);
						//获取模板轮廓
						HObject ho_RegionRet;
						GetNccModelRegion(&ho_RegionRet, RunParams.hv_ModelID); //获取模板轮廓
						//获取模板区域
						HObject regionAffine;
						AffineTransRegion(ho_RegionRet, &regionAffine, Homat2d_Window, "nearest_neighbor");
						ConcatObj(modelRegion_Ret, regionAffine, &modelRegion_Ret);
					}
					//区域排序
					HObject ho_SortedRegions1;
					GenEmptyObj(&ho_SortedRegions1);
					HTuple indexR, RowNew, ColumnNew, AngleNew, ScoreNew;
					if (RunParams.hv_SortType == m_type.SortType_Row)
					{
						//按行排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "row");
						TupleSortIndex(hv_Row_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else if (RunParams.hv_SortType == m_type.SortType_Column)
					{
						//按列排序
						SortRegion(modelRegion_Ret, &ho_SortedRegions1, "first_point", "true", "column");
						TupleSortIndex(hv_Column_Ret, &indexR);
						for (int i = 0; i < hv_Row_Ret.TupleLength(); i++)
						{
							RowNew.Append(hv_Row_Ret[HTuple(indexR[i])]);
							ColumnNew.Append(hv_Column_Ret[HTuple(indexR[i])]);
							AngleNew.Append(hv_Angle_Ret[HTuple(indexR[i])]);
							ScoreNew.Append(hv_Score_Ret[HTuple(indexR[i])]);
						}
					}
					else
					{
						ho_SortedRegions1 = modelRegion_Ret;
						RowNew = hv_Row_Ret;
						ColumnNew = hv_Column_Ret;
						AngleNew = hv_Angle_Ret;
						ScoreNew = hv_Score_Ret;
					}
					//结果导出
					ho_Region_Pin = ho_SortedRegions1;
					hv_Row_Pin = RowNew;
					hv_Column_Pin = ColumnNew;
					hv_Angle_Pin = AngleNew;
					hv_Score_Pin = ScoreNew;
				}
			}
			else
			{
				return 1;
			}
			return 0;
		}
		catch (...)
		{
			//初始化结果
			GenEmptyObj(&ho_XLD_Template);
			GenEmptyObj(&ho_Region_Pin);
			hv_Row_Pin.Clear();
			hv_Column_Pin.Clear();
			hv_Angle_Pin.Clear();
			hv_Score_Pin.Clear();
			return -1;
		}
	}
}


//创建模板
int PinDetect::CreateTemplate(const HObject& ho_Image, RunParamsStruct_Pin& RunParams, int Type) {
	try {
		if (Type == 0) {
			// ================== 模式0：位置校正模板 ==================
			HObject GrayImg;
			// 将RGB图像转为灰度图（Rgb1ToGray适用于三通道RGB图像）
			Rgb1ToGray(ho_Image, &GrayImg);

			TypeParamsStruct_Pin m_type; // 类型参数结构体，用于对比匹配方法

			if (RunParams.hv_MatchMethod_PositionCorrection == m_type.ShapeModel) {
				// ---------- 创建各向异性形状模板（Shape-Based Matching） ----------
				try {
					HObject ho_TemplateImage;
					// 裁剪图像：仅保留训练区域内的部分
					ReduceDomain(GrayImg, RunParams.ho_TrainRegion_PositionCorrection, &ho_TemplateImage);

					// 创建形状模板（支持旋转和缩放）
					CreateAnisoShapeModel(
						ho_TemplateImage,       // 输入图像（已裁剪）
						"auto",                // 金字塔级别自动选择
						RunParams.hv_AngleStart_PositionCorrection.TupleRad(),          // 起始角度（转为弧度）
						2 * (RunParams.hv_AngleExtent_PositionCorrection.TupleRad()).TupleAbs(), // 角度范围（Halcon要求总范围，例如输入30°则覆盖-30°~30°）
						"auto",                // 优化参数自动选择
						RunParams.hv_ScaleRMin_PositionCorrection, // 行方向最小缩放比例（如0.9）
						RunParams.hv_ScaleRMax_PositionCorrection, // 行方向最大缩放比例（如1.1）
						"auto",                // 列方向缩放参数自动选择（若为各向同性，此行可能无效）
						RunParams.hv_ScaleCMin_PositionCorrection, // 列方向最小缩放比例
						RunParams.hv_ScaleCMax_PositionCorrection, // 列方向最大缩放比例
						"auto",                // 对比度阈值自动选择
						"auto",                // 最小对比度自动选择
						"use_polarity",        // 使用极性（明暗对比需一致）
						"auto",                // 模型生成参数自动选择
						"auto",                // 模板原点自动设置（一般为图像中心）
						&RunParams.hv_ModelID_PositionCorrection // 输出模板ID
					);
					return 0; // 成功
				}
				catch (...) {
					WriteTxt("创建模板算法错误!"); // 记录错误日志
					return -1; // 返回错误码
				}
			}
			else if (RunParams.hv_MatchMethod_PositionCorrection == m_type.NccModel) {
				// ---------- 创建NCC模板（Normalized Cross Correlation） ----------
				try {
					HObject ho_TemplateImage;
					ReduceDomain(GrayImg, RunParams.ho_TrainRegion_PositionCorrection, &ho_TemplateImage);

					// 创建NCC模板（适用于光照变化较大的场景）
					CreateNccModel(
						ho_TemplateImage,      // 输入图像（已裁剪）
						"auto",               // 金字塔级别自动选择
						RunParams.hv_AngleStart_PositionCorrection.TupleRad(),         // 起始角度（弧度）
						2 * (RunParams.hv_AngleExtent_PositionCorrection.TupleRad()).TupleAbs(), // 角度范围（同上）
						"auto",               // 优化参数自动选择
						"use_polarity",        // 使用极性
						&RunParams.hv_ModelID_PositionCorrection // 输出模板ID
					);
					return 0;
				}
				catch (...) {
					WriteTxt("创建模板算法错误!");
					return -1;
				}
			}
			else {
				WriteTxt("创建模板，没有正确选择模板类型!"); // 参数错误
				return 1;
			}
		}
		else {
			// ================== 模式非0：常规模板 ==================
			// 流程与模式0类似，但使用不同的参数字段（无_PositionCorrection后缀）
			HObject GrayImg;
			Rgb1ToGray(ho_Image, &GrayImg);
			TypeParamsStruct_Pin m_type;

			if (RunParams.hv_MatchMethod == m_type.ShapeModel) {
				try {
					HObject ho_TemplateImage;
					ReduceDomain(GrayImg, RunParams.ho_TrainRegion, &ho_TemplateImage);
					CreateAnisoShapeModel(
						ho_TemplateImage,
						"auto",
						RunParams.hv_AngleStart.TupleRad(),
						2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
						"auto",
						RunParams.hv_ScaleRMin,
						RunParams.hv_ScaleRMax,
						"auto",
						RunParams.hv_ScaleCMin,
						RunParams.hv_ScaleCMax,
						"auto",
						"auto",
						"use_polarity",
						"auto",
						"auto",
						&RunParams.hv_ModelID
					);
					return 0;
				}
				catch (...) {
					WriteTxt("创建模板算法错误!");
					return -1;
				}
			}
			else if (RunParams.hv_MatchMethod == m_type.NccModel) {
				try {
					HObject ho_TemplateImage;
					ReduceDomain(GrayImg, RunParams.ho_TrainRegion, &ho_TemplateImage);
					CreateNccModel(
						ho_TemplateImage,
						"auto",
						RunParams.hv_AngleStart.TupleRad(),
						2 * (RunParams.hv_AngleExtent.TupleRad()).TupleAbs(),
						"auto",
						"use_polarity",
						&RunParams.hv_ModelID
					);
					return 0;
				}
				catch (...) {
					WriteTxt("创建模板算法错误!");
					return -1;
				}
			}
			else {
				WriteTxt("创建模板，没有正确选择模板类型!");
				return 1;
			}
		}
	}
	catch (...) { // 捕获所有未处理的异常
		return -1; // 返回通用错误码
	}
}

//写日志函数
int PinDetect::WriteTxt(string content)
{
	if (content.empty())
	{
		return -1;
	}
	const char *p = content.c_str();
	ofstream in;
	in.open("C:\\AriLog.txt", ios::app); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
	in << p << "\r";
	in.close();//关闭文件
	return 0;
}
//写入csv
int PinDetect::WriteCsv(const RunParamsStruct_Pin &runParams, const ResultParamsStruct_Pin &resultParams)
{
	try
	{
		// 示例数据  
		std::vector<std::vector<std::string>> DataSave = {
			{"X" , "Y" , "DistanceX_Standard-Pixel" , "DistanceY_Standard-Pixel" , "OffsetX-Pixel" , "OffsetY-Pixel" ,
			"DistanceX_Standard-MM" , "DistanceY_Standard-MM" , "OffsetX-MM" , "OffsetY-MM"}
		};

		for (int i = 0; i < resultParams.hv_Row_Array.TupleLength().I(); i++)
		{
			std::vector<std::string> dataTemp = {
				to_string(resultParams.hv_Column_Array[i].D()),to_string(resultParams.hv_Row_Array[i].D()),
				to_string(resultParams.hv_DisX_Pixel_Array[i].D()),to_string(resultParams.hv_DisY_Pixel_Array[i].D()),
				to_string(resultParams.hv_OffsetX_Pixel_Array[i].D()),to_string(resultParams.hv_OffsetY_Pixel_Array[i].D()),
				to_string(resultParams.hv_DisX_MM_Array[i].D()),to_string(resultParams.hv_DisY_MM_Array[i].D()),
				to_string(resultParams.hv_OffsetX_MM_Array[i].D()),to_string(resultParams.hv_OffsetY_MM_Array[i].D()),
			};

			DataSave.push_back(dataTemp);
		}

		// 判定目录或文件是否存在的标识符
		int mode = 0;
		if (_access(runParams.FilePath_Csv.c_str(), mode))
		{
			//system("mkdir head");
			_mkdir(runParams.FilePath_Csv.c_str());
		}
		// 打开文件进行写入，如果文件已存在则覆盖  
		string path = runParams.FilePath_Csv + to_string(runParams.ID_Csv) + ".csv";
		std::ofstream csvFile(path);

		// 检查文件是否成功打开  
		if (!csvFile.is_open())
		{
			std::cerr << "无法打开文件以进行写入" << std::endl;
			return 1;
		}

		// 遍历数据并写入CSV文件  
		for (const auto& row : DataSave)
		{
			// 使用ostringstream来构建行，并在字段之间添加逗号  
			std::ostringstream rowStream;
			for (size_t i = 0; i < row.size(); ++i)
			{
				rowStream << row[i];
				// 如果不是最后一个字段，则添加逗号  
				if (i < row.size() - 1)
				{
					rowStream << ",";
				}
			}
			// 将行写入文件，并添加换行符  
			csvFile << rowStream.str() << std::endl;
		}

		// 关闭文件  
		csvFile.close();

		std::cout << "CSV文件已成功写入" << std::endl;

		return 0;
	}
	catch (...)
	{
		return 1;
	}
}
//获取系统时间
vector<string> PinDetect::get_current_time()
{
	/*能精确到毫秒*/
	/*
	#include <windows.h>
	#include <iostream>
	*/
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms

	char tmpbuff[16];
	/*年*/
	sprintf_s(tmpbuff, "%d", st.wYear);
	std::string year = tmpbuff;
	/*月*/
	sprintf_s(tmpbuff, "%d", st.wMonth);
	std::string month = tmpbuff;
	/*日*/
	sprintf_s(tmpbuff, "%d", st.wDay);
	std::string day = tmpbuff;
	/*时*/
	sprintf_s(tmpbuff, "%d", st.wHour);
	std::string hour = tmpbuff;
	/*分*/
	sprintf_s(tmpbuff, "%d", st.wMinute);
	std::string minute = tmpbuff;
	/*秒*/
	sprintf_s(tmpbuff, "%d", st.wSecond);
	std::string second = tmpbuff;
	/*毫秒*/
	sprintf_s(tmpbuff, "%d", st.wMilliseconds);
	std::string millisecond = tmpbuff;

	std::vector<string>current_time = {};

	current_time.push_back(year + month + day);
	current_time.push_back(hour + minute + second);
	current_time.push_back(year + month + day + hour + minute + second);
	current_time.push_back(year + "_" + month + "_" + day + "_" + hour + "_" + minute + "_" + second + "_" + millisecond);

	return current_time;
}