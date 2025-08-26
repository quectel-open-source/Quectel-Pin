
#include "datas.h"
//#include <data.h>
#include<iomanip>

//static QLog* qLog = new QLog("datas");
//保存Pin针检测参数(避免使用科学计数法)
int DllDatas::WriteParams_PinDetect(const RunParamsStruct_Pin& runParams, const int& processId, const string& nodeName, const int& processModbuleID) {
	//mutex1.lock();
	string ConfigPath = getConfigPath;
	//string XmlPath = getConfigPath + "/config.xml";
	string XmlPath = configFileName;
	try {
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		//写入数组(数组按照逗号分割存储)
		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("X方向基准参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", to_string(runParams.hv_StandardX_Array.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_StandardX_Array[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_StandardX_Array[i];
				}
				else {
					typeTemp += runParams.hv_StandardX_Array[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", typeTemp.S().Text());
		}

		typeCount = runParams.hv_StandardY_Array.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("Y方向基准参数为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", to_string(runParams.hv_StandardY_Array.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_StandardY_Array[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_StandardY_Array[i];
				}
				else {
					typeTemp += runParams.hv_StandardY_Array[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", typeTemp.S().Text());
		}

		typeCount = runParams.hv_X_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("Pin区域绑定的X基准编号为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", to_string(runParams.hv_X_BindingNum.I()));
		}
		else {
			HTuple typeTemp = runParams.hv_X_BindingNum[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_X_BindingNum[i];
				}
				else {
					typeTemp += runParams.hv_X_BindingNum[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Y_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("Pin区域绑定的Y基准编号为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", to_string(runParams.hv_Y_BindingNum.I()));
		}
		else {
			HTuple typeTemp = runParams.hv_Y_BindingNum[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Y_BindingNum[i];
				}
				else {
					typeTemp += runParams.hv_Y_BindingNum[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", typeTemp.S().Text());
		}

		typeCount = runParams.hv_BaseType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("基准类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", runParams.hv_BaseType.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_BaseType[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_BaseType[i];
				}
				else {
					typeTemp += runParams.hv_BaseType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Row1_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("直线起点row，或者圆心row为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", to_string(runParams.hv_Row1_Base.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_Row1_Base[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Row1_Base[i];
				}
				else {
					typeTemp += runParams.hv_Row1_Base[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Column1_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("直线起点column，或者圆心column为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", to_string(runParams.hv_Column1_Base.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_Column1_Base[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Column1_Base[i];
				}
				else {
					typeTemp += runParams.hv_Column1_Base[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Row2_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("直线终点row为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", to_string(runParams.hv_Row2_Base.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_Row2_Base[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Row2_Base[i];
				}
				else {
					typeTemp += runParams.hv_Row2_Base[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Column2_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("直线终点column为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", to_string(runParams.hv_Column2_Base.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_Column2_Base[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Column2_Base[i];
				}
				else {
					typeTemp += runParams.hv_Column2_Base[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Radius_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("圆半径为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", to_string(runParams.hv_Radius_Base.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_Radius_Base[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Radius_Base[i];
				}
				else {
					typeTemp += runParams.hv_Radius_Base[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureLength1.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("卡尺半长为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", to_string(runParams.hv_MeasureLength1.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureLength1[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureLength1[i];
				}
				else {
					typeTemp += runParams.hv_MeasureLength1[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureLength2.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("卡尺半宽为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", to_string(runParams.hv_MeasureLength2.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureLength2[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureLength2[i];
				}
				else {
					typeTemp += runParams.hv_MeasureLength2[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureSigma.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("平滑值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", to_string(runParams.hv_MeasureSigma.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureSigma[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureSigma[i];
				}
				else {
					typeTemp += runParams.hv_MeasureSigma[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("边缘阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", to_string(runParams.hv_MeasureThreshold.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureThreshold[i];
				}
				else {
					typeTemp += runParams.hv_MeasureThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureTransition.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("卡尺极性为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", runParams.hv_MeasureTransition.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureTransition[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureTransition[i];
				}
				else {
					typeTemp += runParams.hv_MeasureTransition[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureSelect.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("边缘选择为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", runParams.hv_MeasureSelect.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureSelect[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureSelect[i];
				}
				else {
					typeTemp += runParams.hv_MeasureSelect[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureNum.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("卡尺个数为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", to_string(runParams.hv_MeasureNum.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureNum[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureNum[i];
				}
				else {
					typeTemp += runParams.hv_MeasureNum[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MeasureMinScore.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("最小得分为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", to_string(runParams.hv_MeasureMinScore.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MeasureMinScore[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MeasureMinScore[i];
				}
				else {
					typeTemp += runParams.hv_MeasureMinScore[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DistanceThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("距离阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", to_string(runParams.hv_DistanceThreshold.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_DistanceThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_DistanceThreshold[i];
				}
				else {
					typeTemp += runParams.hv_DistanceThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DetectType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("检测类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", runParams.hv_DetectType.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_DetectType[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_DetectType[i];
				}
				else {
					typeTemp += runParams.hv_DetectType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("排序方式为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", runParams.hv_SortType.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_SortType[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_SortType[i];
				}
				else {
					typeTemp += runParams.hv_SortType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("二值化类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", runParams.hv_ThresholdType.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_ThresholdType[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_ThresholdType[i];
				}
				else {
					typeTemp += runParams.hv_ThresholdType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("低阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", to_string(runParams.hv_LowThreshold.TupleInt().I()));
		}
		else {
			HTuple typeTemp = runParams.hv_LowThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_LowThreshold[i];
				}
				else {
					typeTemp += runParams.hv_LowThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("高阈值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", to_string(runParams.hv_HighThreshold.TupleInt().I()));
		}
		else {
			HTuple typeTemp = runParams.hv_HighThreshold[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_HighThreshold[i];
				}
				else {
					typeTemp += runParams.hv_HighThreshold[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", typeTemp.S().Text());
		}

		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("sigma为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", to_string(runParams.hv_Sigma.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_Sigma[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_Sigma[i];
				}
				else {
					typeTemp += runParams.hv_Sigma[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("滤波核宽度为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", to_string(runParams.hv_CoreWidth.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_CoreWidth[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_CoreWidth[i];
				}
				else {
					typeTemp += runParams.hv_CoreWidth[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("滤波核高度为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", to_string(runParams.hv_CoreHeight.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_CoreHeight[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_CoreHeight[i];
				}
				else {
					typeTemp += runParams.hv_CoreHeight[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("动态阈值分割图像对比度为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", to_string(runParams.hv_DynThresholdContrast.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_DynThresholdContrast[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_DynThresholdContrast[i];
				}
				else {
					typeTemp += runParams.hv_DynThresholdContrast[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", typeTemp.S().Text());
		}

		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("动态阈值极性选择为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", runParams.hv_DynThresholdPolarity.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_DynThresholdPolarity[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_DynThresholdPolarity[i];
				}
				else {
					typeTemp += runParams.hv_DynThresholdPolarity[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选面积Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", to_string(runParams.hv_AreaFilter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_AreaFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_AreaFilter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_AreaFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选半长Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", to_string(runParams.hv_RecLen1Filter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen1Filter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen1Filter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen1Filter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选半宽Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", to_string(runParams.hv_RecLen2Filter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen2Filter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen2Filter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen2Filter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选行坐标Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", to_string(runParams.hv_RowFilter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RowFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RowFilter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RowFilter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RowFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选列坐标Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", to_string(runParams.hv_ColumnFilter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_ColumnFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_ColumnFilter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_ColumnFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_CircularityFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", to_string(runParams.hv_CircularityFilter_Min.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_CircularityFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_CircularityFilter_Min[i];
				}
				else {
					typeTemp += runParams.hv_CircularityFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_RectangularityFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", to_string(runParams.hv_RectangularityFilter_Min.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_RectangularityFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_RectangularityFilter_Min[i];
				}
				else {
					typeTemp += runParams.hv_RectangularityFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_WidthFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", to_string(runParams.hv_WidthFilter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_WidthFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_WidthFilter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_WidthFilter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_WidthFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_HeightFilter_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", to_string(runParams.hv_HeightFilter_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_HeightFilter_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_HeightFilter_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_HeightFilter_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_HeightFilter_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选面积Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", to_string(runParams.hv_AreaFilter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_AreaFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_AreaFilter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_AreaFilter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_AreaFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选半长Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", to_string(runParams.hv_RecLen1Filter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen1Filter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen1Filter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen1Filter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen1Filter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选半宽Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", to_string(runParams.hv_RecLen2Filter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RecLen2Filter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RecLen2Filter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RecLen2Filter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RecLen2Filter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选行坐标Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", to_string(runParams.hv_RowFilter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_RowFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_RowFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_RowFilter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_RowFilter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_RowFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选列坐标Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", to_string(runParams.hv_ColumnFilter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_ColumnFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_ColumnFilter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_ColumnFilter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_ColumnFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_CircularityFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", to_string(runParams.hv_CircularityFilter_Max.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_CircularityFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_CircularityFilter_Max[i];
				}
				else {
					typeTemp += runParams.hv_CircularityFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_RectangularityFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", to_string(runParams.hv_RectangularityFilter_Max.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_RectangularityFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_RectangularityFilter_Max[i];
				}
				else {
					typeTemp += runParams.hv_RectangularityFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_WidthFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", to_string(runParams.hv_WidthFilter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_WidthFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_WidthFilter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_WidthFilter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_WidthFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("筛选hv_HeightFilter_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", to_string(runParams.hv_HeightFilter_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_HeightFilter_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_HeightFilter_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_HeightFilter_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_HeightFilter_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("hv_FillUpShape_Min为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", to_string(runParams.hv_FillUpShape_Min.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Min[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Min[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Min[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Min[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", typeTemp.S().Text());
		}

		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("hv_FillUpShape_Max为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max.TupleReal().D();
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", ss.str());
			//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", to_string(runParams.hv_FillUpShape_Max.D()));
		}
		else {
			//保留11位有效位数(不是小数位)
			std::stringstream ss;
			ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[0].D();
			HTuple typeTemp = HTuple(ss.str().c_str()) + ",";
			//HTuple typeTemp = runParams.hv_FillUpShape_Max[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				std::stringstream ss;
				ss << std::setprecision(11) << runParams.hv_FillUpShape_Max[i].D();
				if (i == typeCount - 1) {
					typeTemp += HTuple(ss.str().c_str());
					//typeTemp += runParams.hv_FillUpShape_Max[i];
				}
				else {
					typeTemp += HTuple(ss.str().c_str()) + ",";
					//typeTemp += runParams.hv_FillUpShape_Max[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("形态学类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", runParams.hv_OperatorType.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_OperatorType[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_OperatorType[i];
				}
				else {
					typeTemp += runParams.hv_OperatorType[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("形态学宽为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", to_string(runParams.hv_OperaRec_Width.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_OperaRec_Width[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_OperaRec_Width[i];
				}
				else {
					typeTemp += runParams.hv_OperaRec_Width[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("形态学高为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", to_string(runParams.hv_OperaRec_Height.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_OperaRec_Height[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_OperaRec_Height[i];
				}
				else {
					typeTemp += runParams.hv_OperaRec_Height[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", typeTemp.S().Text());
		}

		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("形态学半径为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", to_string(runParams.hv_OperaCir_Radius.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_OperaCir_Radius[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_OperaCir_Radius[i];
				}
				else {
					typeTemp += runParams.hv_OperaCir_Radius[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", typeTemp.S().Text());
		}

		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("hv_SelectAreaMax为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", to_string(runParams.hv_SelectAreaMax.I()));
		}
		else {
			HTuple typeTemp = runParams.hv_SelectAreaMax[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_SelectAreaMax[i];
				}
				else {
					typeTemp += runParams.hv_SelectAreaMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", typeTemp.S().Text());
		}

		typeCount = runParams.hv_BlobCount.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("hv_BlobCount为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", to_string(runParams.hv_BlobCount.TupleInt().I()));
		}
		else {
			HTuple typeTemp = runParams.hv_BlobCount[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_BlobCount[i];
				}
				else {
					typeTemp += runParams.hv_BlobCount[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", typeTemp.S().Text());
		}

		//灰度处理
		typeCount = runParams.hv_ToGray.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("hv_ToGray为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ToGray", to_string(runParams.hv_ToGray.TupleInt().I()));
		}
		else {
			HTuple typeTemp = runParams.hv_ToGray[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_ToGray[i];
				}
				else {
					typeTemp += runParams.hv_ToGray[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ToGray", typeTemp.S().Text());
		}

		//保存其余参数
		//允许的偏差范围
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", to_string(runParams.hv_Tolerance_X.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", to_string(runParams.hv_Tolerance_Y.D()));
		//像素当量
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", to_string(runParams.hv_MmPixel.D()));
		////CSV路径
		//xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "FilePath_Csv", runParams.FilePath_Csv);

		//保存模板匹配参数
		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板匹配类型为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", runParams.hv_MatchMethod.S().Text());
		}
		else {
			HTuple typeTemp = runParams.hv_MatchMethod[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MatchMethod[i];
				}
				else {
					typeTemp += runParams.hv_MatchMethod[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AngleStart.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板起始角度为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", to_string(runParams.hv_AngleStart.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_AngleStart[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_AngleStart[i];
				}
				else {
					typeTemp += runParams.hv_AngleStart[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", typeTemp.S().Text());
		}

		typeCount = runParams.hv_AngleExtent.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板终止角度为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", to_string(runParams.hv_AngleExtent.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_AngleExtent[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_AngleExtent[i];
				}
				else {
					typeTemp += runParams.hv_AngleExtent[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleRMin.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板行缩放最小值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", to_string(runParams.hv_ScaleRMin.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_ScaleRMin[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_ScaleRMin[i];
				}
				else {
					typeTemp += runParams.hv_ScaleRMin[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleRMax.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板行缩放最大值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", to_string(runParams.hv_ScaleRMax.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_ScaleRMax[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_ScaleRMax[i];
				}
				else {
					typeTemp += runParams.hv_ScaleRMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleCMin.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板列缩放最小值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", to_string(runParams.hv_ScaleCMin.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_ScaleCMin[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_ScaleCMin[i];
				}
				else {
					typeTemp += runParams.hv_ScaleCMin[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", typeTemp.S().Text());
		}

		typeCount = runParams.hv_ScaleCMax.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板列缩放最大值为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", to_string(runParams.hv_ScaleCMax.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_ScaleCMax[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_ScaleCMax[i];
				}
				else {
					typeTemp += runParams.hv_ScaleCMax[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", typeTemp.S().Text());
		}

		typeCount = runParams.hv_MinScore.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板最小分数为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", to_string(runParams.hv_MinScore.D()));
		}
		else {
			HTuple typeTemp = runParams.hv_MinScore[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_MinScore[i];
				}
				else {
					typeTemp += runParams.hv_MinScore[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", typeTemp.S().Text());
		}

		typeCount = runParams.hv_NumMatches.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("模板匹配个数为空，保存失败");
			return 1;
		}
		if (typeCount == 1) {
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", to_string(runParams.hv_NumMatches.I()));
		}
		else {
			HTuple typeTemp = runParams.hv_NumMatches[0] + ",";
			for (int i = 1; i < typeCount; i++) {
				if (i == typeCount - 1) {
					typeTemp += runParams.hv_NumMatches[i];
				}
				else {
					typeTemp += runParams.hv_NumMatches[i] + ",";
				}
			}
			xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", typeTemp.S().Text());
		}

		//保存模板匹配模型
		if (runParams.hv_ModelID.TupleLength() > 0) {
			TypeParamsStruct_Pin mType;
			for (int i = 0; i < runParams.hv_ModelID.TupleLength(); i++) {
				if (runParams.hv_DetectType[i] == mType.DetectType_Template) {
					if (runParams.hv_MatchMethod[i] == mType.NccModel) {
						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.ncm";
						WriteNccModel(runParams.hv_ModelID[i], modelIDPath);
					}
					else {
						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.shm";
						WriteShapeModel(runParams.hv_ModelID[i], modelIDPath);
					}
				}
			}
		}
		else {
			WriteTxt("模板匹配模型ID为空");
		}

		//保存运行区域
		if (runParams.ho_SearchRegion.Key() == nullptr) {
			//图像为空，或者图像被clear
			WriteTxt("搜索区域为空，保存失败");
		}
		else {
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
			WriteRegion(runParams.ho_SearchRegion, PathRunRegion);
		}

		//保存训练区域
		if (runParams.ho_TrainRegion.Key() == nullptr) {
			//图像为空，或者图像被clear
			WriteTxt("训练区域为空，保存失败");
		}
		else {
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
			WriteRegion(runParams.ho_TrainRegion, PathTrainRegion);
		}


		//保存位置修正模板匹配参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", runParams.hv_MatchMethod_PositionCorrection.S().Text());
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", to_string(runParams.hv_AngleStart_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", to_string(runParams.hv_AngleExtent_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", to_string(runParams.hv_ScaleRMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", to_string(runParams.hv_ScaleRMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", to_string(runParams.hv_ScaleCMin_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", to_string(runParams.hv_ScaleCMax_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", to_string(runParams.hv_MinScore_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", to_string(runParams.hv_NumMatches_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", to_string(runParams.hv_Check_PositionCorrection.TupleInt().I()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", to_string(runParams.hv_Row_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", to_string(runParams.hv_Column_PositionCorrection.D()));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", to_string(runParams.hv_Angle_PositionCorrection.D()));
		//保存位置修正模板匹配模型
		try {
			TypeParamsStruct_Pin mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel) {
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				WriteNccModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}
			else {
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				WriteShapeModel(runParams.hv_ModelID_PositionCorrection, modelIDPath);
			}
		}
		catch (...) {
			WriteTxt("位置修正模板匹配模型ID为空,保存失败");
		}
		//保存位置修正的运行区域
		if (runParams.ho_SearchRegion_PositionCorrection.Key() == nullptr) {
			//图像为空，或者图像被clear
			WriteTxt("位置修正搜索区域为空，保存失败");
		}
		else {
			HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		//保存位置修正的训练区域
		if (runParams.ho_TrainRegion_PositionCorrection.Key() == nullptr) {
			//图像为空，或者图像被clear
			WriteTxt("位置修正训练区域为空，保存失败");
		}
		else {
			HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
			WriteRegion(runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}


		WriteTxt("Pin针检测参数保存成功!");
		//mutex1.unlock();
		return 0;
	}
	catch (...) {
		WriteTxt("XML保存代码崩溃，Pin针检测参数保存失败!");
		//mutex1.unlock();
		return -1;
	}
}

//读取Pin针检测参数
ErrorCode_Xml DllDatas::ReadParams_PinDetect(RunParamsStruct_Pin& runParams, const int& processId, const string& nodeName, const int& processModbuleID) {
	//mutex1.lock();
	string ConfigPath = getConfigPath;
	//string XmlPath = getConfigPath + "/config.xml";
	string XmlPath = configFileName;
	try {
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		//读取数组(数组按照逗号分割存储)
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardX_Array", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取Pin检测参数hv_StandardX_Array失败");
			return errorCode;
		}
		vector<string> typeArray;
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StandardX_Array.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StandardX_Array.Append(tempDoubleValue);
		}
		int typeCount = runParams.hv_StandardX_Array.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_StandardX_Array失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_StandardY_Array", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读参数hv_StandardY_Array失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_StandardY_Array.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_StandardY_Array.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_StandardY_Array.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_StandardY_Array失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_X_BindingNum", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML参数hv_X_BindingNum失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_X_BindingNum.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_X_BindingNum.Append(tempIntValue);
		}
		typeCount = runParams.hv_X_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("取参数hv_X_BindingNum失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Y_BindingNum", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Y_BindingNum失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Y_BindingNum.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_Y_BindingNum.Append(tempIntValue);
		}
		typeCount = runParams.hv_Y_BindingNum.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Y_BindingNum失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BaseType", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_BaseType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_BaseType.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_BaseType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_BaseType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_BaseType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row1_Base", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Row1_Base失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Row1_Base.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Row1_Base.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Row1_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Row1_Base失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column1_Base", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Column1_Base失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Column1_Base.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Column1_Base.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Column1_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Column1_Base失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row2_Base", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Row2_Base失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Row2_Base.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Row2_Base.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Row2_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Row2_Base失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column2_Base", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Column2_Base失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Column2_Base.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Column2_Base.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Column2_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Column2_Base失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Radius_Base", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Radius_Base失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Radius_Base.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Radius_Base.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Radius_Base.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Radius_Base失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength1", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureLength1失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureLength1.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MeasureLength1.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MeasureLength1.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureLength1失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureLength2", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureLength2失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureLength2.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MeasureLength2.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MeasureLength2.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureLength2失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSigma", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureSigma失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureSigma.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MeasureSigma.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MeasureSigma.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureSigma失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureThreshold", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MeasureThreshold.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MeasureThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureTransition", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureTransition失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureTransition.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_MeasureTransition.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_MeasureTransition.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureTransition失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureSelect", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureSelect失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureSelect.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_MeasureSelect.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_MeasureSelect.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureSelect失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureNum", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureNum失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureNum.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MeasureNum.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MeasureNum.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureNum失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MeasureMinScore", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MeasureMinScore失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MeasureMinScore.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MeasureMinScore.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MeasureMinScore.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MeasureMinScore失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DistanceThreshold", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_DistanceThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DistanceThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_DistanceThreshold.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_DistanceThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_DistanceThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DetectType", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_DetectType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DetectType.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_DetectType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_DetectType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_DetectType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SortType", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_SortType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_SortType.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_SortType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_SortType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_SortType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ThresholdType", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ThresholdType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ThresholdType.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_ThresholdType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_ThresholdType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ThresholdType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_LowThreshold", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_LowThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_LowThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_LowThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_LowThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_LowThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HighThreshold", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_HighThreshold失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HighThreshold.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_HighThreshold.Append(tempIntValue);
		}
		typeCount = runParams.hv_HighThreshold.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_HighThreshold失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Sigma", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Sigma失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_Sigma.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_Sigma.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_Sigma.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_Sigma失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreWidth", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_CoreWidth失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreWidth.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreWidth.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreWidth.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_CoreWidth失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CoreHeight", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_CoreHeight失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CoreHeight.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CoreHeight.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CoreHeight.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_CoreHeight失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdContrast", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_DynThresholdContrast失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdContrast.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_DynThresholdContrast.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_DynThresholdContrast.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_DynThresholdContrast失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_DynThresholdPolarity", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_DynThresholdPolarity失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_DynThresholdPolarity.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_DynThresholdPolarity.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_DynThresholdPolarity.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_DynThresholdPolarity失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_AreaFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AreaFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AreaFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AreaFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_AreaFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RecLen1Filter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen1Filter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen1Filter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen1Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RecLen1Filter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RecLen2Filter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen2Filter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen2Filter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen2Filter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RecLen2Filter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RowFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RowFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RowFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RowFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RowFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ColumnFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColumnFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColumnFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColumnFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ColumnFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_CircularityFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CircularityFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CircularityFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CircularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_CircularityFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RectangularityFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RectangularityFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RectangularityFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RectangularityFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RectangularityFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_WidthFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_WidthFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_WidthFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_WidthFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_WidthFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_HeightFilter_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HeightFilter_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_HeightFilter_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_HeightFilter_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_HeightFilter_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AreaFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_AreaFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AreaFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AreaFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AreaFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_AreaFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen1Filter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RecLen1Filter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen1Filter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen1Filter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen1Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RecLen1Filter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RecLen2Filter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RecLen2Filter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RecLen2Filter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RecLen2Filter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RecLen2Filter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RecLen2Filter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RowFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RowFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RowFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RowFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RowFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RowFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ColumnFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ColumnFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ColumnFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ColumnFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ColumnFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ColumnFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_CircularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_CircularityFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_CircularityFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_CircularityFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_CircularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_CircularityFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_RectangularityFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_RectangularityFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_RectangularityFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_RectangularityFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_RectangularityFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_RectangularityFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_WidthFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_WidthFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_WidthFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_WidthFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_WidthFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_WidthFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_HeightFilter_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_HeightFilter_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_HeightFilter_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_HeightFilter_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_HeightFilter_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_HeightFilter_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Min", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_FillUpShape_Min失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Min.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Min.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Min.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_FillUpShape_Min失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_FillUpShape_Max", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_FillUpShape_Max失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_FillUpShape_Max.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_FillUpShape_Max.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_FillUpShape_Max.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_FillUpShape_Max失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperatorType", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_OperatorType失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperatorType.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_OperatorType.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_OperatorType.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_OperatorType失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Width", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_OperaRec_Width失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Width.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Width.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Width.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_OperaRec_Width失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaRec_Height", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_OperaRec_Height失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaRec_Height.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaRec_Height.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaRec_Height.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_OperaRec_Height失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_OperaCir_Radius", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_OperaCir_Radius失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_OperaCir_Radius.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_OperaCir_Radius.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_OperaCir_Radius.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_OperaCir_Radius失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_SelectAreaMax", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_SelectAreaMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_SelectAreaMax.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_SelectAreaMax.Append(tempIntValue);
		}
		typeCount = runParams.hv_SelectAreaMax.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_SelectAreaMax失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_BlobCount", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_BlobCount失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_BlobCount.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_BlobCount.Append(tempIntValue);
		}
		typeCount = runParams.hv_BlobCount.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_BlobCount失败");
			return nullKeyName_Xml;
		}

		//读取其余参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_X", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取Pin测参数hv_Tolerance_X失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_X = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Tolerance_Y", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取Pin测参数hv_Tolerance_Y失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_Tolerance_Y = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MmPixel", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取Pin测参数hv_MmPixel失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runParams.hv_MmPixel = tempDoubleValue;

		//errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "FilePath_Csv", valueXml);
		//if (errorCode != Ok_Xml) {
		//	WriteTxt("XML读取Pin测参数FilePath_Csv失败");
		//	return errorCode;
		//}
		//runParams.FilePath_Csv = valueXml;


		//读取模板匹配参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MatchMethod失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MatchMethod.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			runParams.hv_MatchMethod.Append(HTuple(typeArray[i].c_str()));
		}
		typeCount = runParams.hv_MatchMethod.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MatchMethod失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_AngleStart失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AngleStart.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AngleStart.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AngleStart.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_AngleStart失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_AngleExtent失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_AngleExtent.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_AngleExtent.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_AngleExtent.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_AngleExtent失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleRMin失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleRMin.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleRMin.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleRMin.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ScaleRMin失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleRMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleRMax.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleRMax.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleRMax.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ScaleRMax失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleCMin失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleCMin.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleCMin.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleCMin.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ScaleCMin失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleCMax失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_ScaleCMax.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_ScaleCMax.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_ScaleCMax.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_ScaleCMax失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MinScore失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_MinScore.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToDouble(typeArray[i], tempDoubleValue);
			runParams.hv_MinScore.Append(tempDoubleValue);
		}
		typeCount = runParams.hv_MinScore.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_MinScore失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_NumMatches失败");
			return errorCode;
		}
		typeArray.clear();
		stringToken(valueXml, ",", typeArray);
		runParams.hv_NumMatches.Clear();
		for (int i = 0; i < typeArray.size(); i++) {
			xmlC.StrToInt(typeArray[i], tempIntValue);
			runParams.hv_NumMatches.Append(tempIntValue);
		}
		typeCount = runParams.hv_NumMatches.TupleLength().TupleInt().I();
		if (typeCount == 0) {
			WriteTxt("读取参数hv_NumMatches失败");
			return nullKeyName_Xml;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ToGray", valueXml);
		if (errorCode == Ok_Xml) {
			typeArray.clear();
			stringToken(valueXml, ",", typeArray);
			runParams.hv_ToGray.Clear();
			for (int i = 0; i < typeArray.size(); i++) {
				xmlC.StrToDouble(typeArray[i], tempDoubleValue);
				runParams.hv_ToGray.Append(tempDoubleValue);
			}
			typeCount = runParams.hv_ToGray.TupleLength().TupleInt().I();
			//if (typeCount == 0) {
			//	WriteTxt("读取参数hv_ToGray失败");
			//	return nullKeyName_Xml;
			//}
		}
		else
		{
			//不报错了
			runParams.hv_ToGray = 0;
		}

		//运行区域加载
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0) {
			GenEmptyObj(&runParams.ho_SearchRegion);
			ReadRegion(&runParams.ho_SearchRegion, PathRunRegion);
		}
		else {
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
			WriteTxt("XML读取参数ho_SearchRegion失败");
		}

		//训练区域加载
		HTuple PathTrainRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0) {
			GenEmptyObj(&runParams.ho_TrainRegion);
			ReadRegion(&runParams.ho_TrainRegion, PathTrainRegion);
		}
		else {
			GenEmptyObj(&runParams.ho_TrainRegion);
			runParams.ho_TrainRegion.Clear();
			WriteTxt("XML读取参数ho_TrainRegion失败");
		}

		//读取模板匹配模型
		runParams.hv_ModelID.Clear();
		HTuple countRegion1;
		if (!runParams.ho_SearchRegion.IsInitialized() || !runParams.ho_TrainRegion.IsInitialized())
		{
			//没有这个区域，报日志
			WriteTxt("XML读取参数_ModelId失败");
		}
		else
		{
			//读取成功再读模板
			CountObj(runParams.ho_SearchRegion, &countRegion1);
			TypeParamsStruct_Pin mType;
			for (int i = 0; i < countRegion1; i++) {
				if (runParams.hv_DetectType[i] == mType.DetectType_Template) {
					if (runParams.hv_MatchMethod[i] == mType.NccModel) {
						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.ncm";
						HTuple ModelID;
						ReadNccModel(modelIDPath, &ModelID);
						runParams.hv_ModelID.Append(ModelID);
					}
					else if (runParams.hv_MatchMethod[i] == mType.ShapeModel) {
						HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
							+ "_ROI_" + HTuple(to_string(i).c_str()) + "_ModelId.shm";
						HTuple ModelID;
						ReadShapeModel(modelIDPath, &ModelID);
						runParams.hv_ModelID.Append(ModelID);
					}
					else {
						//计算没有此类型，也需要添加空句柄，保证数组长度一致
						HTuple emptyID = NULL;
						runParams.hv_ModelID.Append(emptyID);
					}
				}
				else {
					//计算没有此类型，也需要添加空句柄，保证数组长度一致
					HTuple emptyID = NULL;
					runParams.hv_ModelID.Append(emptyID);
				}
			}
		}




		//读取位置修正模板匹配参数
		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MatchMethod_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MatchMethod_PositionCorrection失败");
		}
		else {
			runParams.hv_MatchMethod_PositionCorrection.Clear();
			runParams.hv_MatchMethod_PositionCorrection = HTuple(valueXml.c_str());
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleStart_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_AngleStart_PositionCorrection失败");
		}
		else {
			runParams.hv_AngleStart_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleStart_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_AngleExtent_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_AngleExtent_PositionCorrection失败");
		}
		else {
			runParams.hv_AngleExtent_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_AngleExtent_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleRMin_PositionCorrection失败");
		}
		else {
			runParams.hv_ScaleRMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleRMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleRMax_PositionCorrection失败");
		}
		else {
			runParams.hv_ScaleRMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleRMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMin_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleCMin_PositionCorrection失败");
		}
		else {
			runParams.hv_ScaleCMin_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMin_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_ScaleCMax_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_ScaleCMax_PositionCorrection失败");
		}
		else {
			runParams.hv_ScaleCMax_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_ScaleCMax_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_MinScore_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_MinScore_PositionCorrection失败");
		}
		else {
			runParams.hv_MinScore_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_MinScore_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_NumMatches_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_NumMatches_PositionCorrection失败");
		}
		else {
			runParams.hv_NumMatches_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_NumMatches_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Check_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Check_PositionCorrection失败");
		}
		else {
			runParams.hv_Check_PositionCorrection.Clear();
			xmlC.StrToInt(valueXml, tempIntValue);
			runParams.hv_Check_PositionCorrection = tempIntValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Row_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Row_PositionCorrection失败");
		}
		else {
			runParams.hv_Row_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Row_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Column_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Column_PositionCorrection失败");
		}
		else {
			runParams.hv_Column_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Column_PositionCorrection = tempDoubleValue;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "hv_Angle_PositionCorrection", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取参数hv_Angle_PositionCorrection失败");
		}
		else {
			runParams.hv_Angle_PositionCorrection.Clear();
			xmlC.StrToDouble(valueXml, tempDoubleValue);
			runParams.hv_Angle_PositionCorrection = tempDoubleValue;
		}

		//位置修正搜索区域加载
		PathRunRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion_PositionCorrection.hobj";
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0) {
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			ReadRegion(&runParams.ho_SearchRegion_PositionCorrection, PathRunRegion);
		}
		else {
			GenEmptyObj(&runParams.ho_SearchRegion_PositionCorrection);
			runParams.ho_SearchRegion_PositionCorrection.Clear();
		}

		//位置修正训练区域加载
		PathTrainRegion = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_TrainRegion_PositionCorrection.hobj";
		FileExists(PathTrainRegion, &isExist);
		if (isExist > 0) {
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			ReadRegion(&runParams.ho_TrainRegion_PositionCorrection, PathTrainRegion);
		}
		else {
			GenEmptyObj(&runParams.ho_TrainRegion_PositionCorrection);
			runParams.ho_TrainRegion_PositionCorrection.Clear();
		}
		//读取位置修正，模板匹配模型
		try {
			runParams.hv_ModelID_PositionCorrection.Clear();
			TypeParamsStruct_Pin mType;
			if (runParams.hv_MatchMethod_PositionCorrection == mType.NccModel) {
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.ncm";
				HTuple ModelID;
				FileExists(modelIDPath, &isExist);
				if (isExist > 0) {
					ReadNccModel(modelIDPath, &ModelID);
					runParams.hv_ModelID_PositionCorrection = ModelID;
				}
			}
			else if (runParams.hv_MatchMethod_PositionCorrection == mType.ShapeModel) {
				HTuple modelIDPath = HTuple(ConfigPath.c_str()) + "/" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str())
					+ "_ModelId_PositionCorrection.shm";
				HTuple ModelID;
				FileExists(modelIDPath, &isExist);
				if (isExist > 0) {
					ReadShapeModel(modelIDPath, &ModelID);
					runParams.hv_ModelID_PositionCorrection = ModelID;
				}
			}
			else {
				//计算没有此类型，也需要添加空句柄，保证数组长度一致
				runParams.hv_ModelID_PositionCorrection = NULL;
			}
		}
		catch (...) {
			WriteTxt("PCB检测参数hv_ModelID_PositionCorrection读取失败!");
		}


		WriteTxt("Pin检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;

	}
	catch (...) {
		WriteTxt("XML读取代码崩溃，Pin检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}


#include <iomanip>
std::string DllDatas::formatDobleValue(double val, int fixed) {
	double val2 = val;
	if (val2 < 0) {
		val2 = -val2;
	}
	int intNum = 0;
	int temp = val2;
	while (temp) {
		temp /= 10;
		intNum++;
	}
	int intNum2 = 0;
	if (intNum == 0 && val2 > 0) {
		while ((int)val2 <= 0) {
			val2 *= 10;
			intNum2++;
		}
		if (intNum2 > 0)
			intNum2--;
	}

	std::ostringstream oss;
	oss << std::setprecision(fixed + intNum - intNum2) << val;
	return oss.str();
}

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif
//路径设置
void DllDatas::PathSet(const string path, const string XmlPath) {
	XmlClass2 xmlC;//xml存取变量

	// 判定目录或文件是否存在的标识符
	int mode = 0;
#if defined(_WIN32) || defined(_WIN64)
	if (_access(path.c_str(), mode)) {
		_mkdir(path.c_str());
	}
#else
	if (access(path.c_str(), mode)) {
		mkdir(path.c_str(), 0755);
	}
#endif

	//创建XML(存在就不会创建)
	int Ret = xmlC.CreateXML(XmlPath);
	if (Ret != 0) {
		WriteTxt("打开参数配置xml文件失败");
		//mutex1.unlock();
		return;
	}
}

//写日志函数
int DllDatas::WriteTxt(string content) {
	if (content.empty()) {
		return -1;
	}
	const char* p = content.c_str();
	ofstream in;
	in.open("D:\\AriLog.txt", ios::app); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
	in << p << "\r";
	in.close();//关闭文件
	return 0;
}

void DllDatas::writeSearchRegion(HObject searchRegion, const int& processId, const string& nodeName, const int& processModbuleID) {
	std::string ConfigPath = getConfigPath;
	//保存运行区域
	if (searchRegion.Key() == nullptr) {
		//图像为空，或者图像被clear
		WriteTxt(nodeName + "区域为空，保存失败");
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "\\" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0) {
			remove(PathRunRegion.S().Text());
		}
	}
	else {
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "\\" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		WriteRegion(searchRegion, PathRunRegion);
	}
}

void DllDatas::readSearchRegion(HObject& searchRegion, const int& processId, const string& nodeName, const int& processModbuleID) {
	std::string ConfigPath = getConfigPath;
	//运行区域加载
	HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "\\" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
	HTuple isExist;
	FileExists(PathRunRegion, &isExist);
	if (isExist > 0) {
		GenEmptyObj(&searchRegion);
		ReadRegion(&searchRegion, PathRunRegion);
	}
	else {
		GenEmptyObj(&searchRegion);
		searchRegion.Clear();
	}
}

//按照指定字符分割字符串到vector
void DllDatas::stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken) {
	string sCopy = sToBeToken;
	int iPosEnd = 0;
	while (true) {
		iPosEnd = sCopy.find(sSeperator);
		if (iPosEnd == -1) {
			vToken.push_back(sCopy);
			break;
		}
		vToken.push_back(sCopy.substr(0, iPosEnd));
		sCopy = sCopy.substr(iPosEnd + 1);
	}
}



//---------------------------- 3D 部分 --------------------------------------
int modubleDatas::WriteParams_Pin3DMeasure(const RunParamsStruct_Pin3DMeasure& runparams,
	const int& processId, const string& nodeName, const int& processModbuleID) {
	string ConfigPath = getConfigPath;
	string XmlPath = configFileName;
	try {
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);

		xmlC.DeleteSection_AllKeys(XmlPath, processId, nodeName, processModbuleID);

		//保存参数
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_CenterX", to_string(runparams.ROI_CenterX));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_CenterY", to_string(runparams.ROI_CenterY));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_CenterZ", to_string(runparams.ROI_CenterZ));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_LengthX", to_string(runparams.ROI_LengthX));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_LengthY", to_string(runparams.ROI_LengthY));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_LengthZ", to_string(runparams.ROI_LengthZ));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_RX", to_string(runparams.ROI_RX));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_RY", to_string(runparams.ROI_RY));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ROI_RZ", to_string(runparams.ROI_RZ));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "LocateMethod", runparams.LocateMethod);
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ModelName", string(runparams.ModelName));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "DownSamplingVoxel", to_string(runparams.DownSamplingVoxel));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "MatchIterations", to_string(runparams.MatchIterations));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowMatchCloud", to_string(runparams.PopWindowShowMatchCloud));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint1CenterX", to_string(runparams.BasePlanePoints[0][0]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint1CenterY", to_string(runparams.BasePlanePoints[0][1]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint1CenterZ", to_string(runparams.BasePlanePoints[0][2]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint2CenterX", to_string(runparams.BasePlanePoints[1][0]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint2CenterY", to_string(runparams.BasePlanePoints[1][1]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint2CenterZ", to_string(runparams.BasePlanePoints[1][2]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint3CenterX", to_string(runparams.BasePlanePoints[2][0]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint3CenterY", to_string(runparams.BasePlanePoints[2][1]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint3CenterZ", to_string(runparams.BasePlanePoints[2][2]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint4CenterX", to_string(runparams.BasePlanePoints[3][0]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint4CenterY", to_string(runparams.BasePlanePoints[3][1]));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "BasePoint4CenterZ", to_string(runparams.BasePlanePoints[3][2]));

		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowBasePointCloud", to_string(runparams.PopWindowShowBasePointCloud));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowCropPinCloud", to_string(runparams.PopWindowShowCropPinCloud));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowPinSORCloud", to_string(runparams.PopWindowShowPinSORCloud));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowPinDownCloud", to_string(runparams.PopWindowShowPinDownCloud));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowPinClusterCloud", to_string(runparams.PopWindowShowPinClusterCloud));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "ShowResult", to_string(runparams.ShowResult));
		xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinParamsNum", to_string(runparams.PinParamsNum));

		if (runparams.PinParamsNum == 0) {
			return 0;
		}
		else {
			for (int a = 0; a < runparams.PinParamsNum; a++) {
				std::string i = std::to_string(a);
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinParamsName" + i, string(runparams.PinParamsName[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_CenterX_Vector" + i, to_string(runparams.PinROI_CenterX_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_CenterY_Vector" + i, to_string(runparams.PinROI_CenterY_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_CenterZ_Vector" + i, to_string(runparams.PinROI_CenterZ_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_LengthX_Vector" + i, to_string(runparams.PinROI_LengthX_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_LengthY_Vector" + i, to_string(runparams.PinROI_LengthY_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_LengthZ_Vector" + i, to_string(runparams.PinROI_LengthZ_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_RX_Vector" + i, to_string(runparams.PinROI_RX_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_RY_Vector" + i, to_string(runparams.PinROI_RY_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinROI_RZ_Vector" + i, to_string(runparams.PinROI_RZ_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinDownSamplingVoxel_Vector" + i, to_string(runparams.PinDownSamplingVoxel_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "SORNeighborPoints_Vector" + i, to_string(runparams.SORNeighborPoints_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "SOROutlierThreshold_Vector" + i, to_string(runparams.SOROutlierThreshold_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "EuclideanClusterTolerance_Vector" + i, to_string(runparams.EuclideanClusterTolerance_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "EuclideanClusterMinPoints_Vector" + i, to_string(runparams.EuclideanClusterMinPoints_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "EuclideanClusterMaxPoints_Vector" + i, to_string(runparams.EuclideanClusterMaxPoints_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinStandard_Vector" + i, to_string(runparams.PinStandard_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinUpperTol_Vector" + i, to_string(runparams.PinUpperTol_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinLowerTol_Vector" + i, to_string(runparams.PinLowerTol_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinCorrect_Vector" + i, to_string(runparams.PinCorrect_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "PinNum_Vector" + i, to_string(runparams.PinNum_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "SortOrder_Vector" + i, string(runparams.SortOrder_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "HeadAddZeroNum_Vector" + i, to_string(runparams.HeadAddZeroNum_Vector[a]));
				xmlC.InsertXMLNode(XmlPath, processId, nodeName, processModbuleID, "TailAddZeroNum_Vector" + i, to_string(runparams.TailAddZeroNum_Vector[a]));
			}
		}

		WriteTxt("PIN针检测参数保存成功!");
		return 0;
	}
	catch (...) {
		WriteTxt("XML保存代码崩溃，PIN针检测参数保存失败!");
		return -1;
	}
}

ErrorCode_Xml modubleDatas::ReadParams_Pin3DMeasure(RunParamsStruct_Pin3DMeasure& runparams,
	const int& processId, const string& nodeName, const int& processModbuleID) {
	string ConfigPath = getConfigPath;
	string XmlPath = configFileName;
	try {
		XmlClass2 xmlC;//xml存取变量
		//路径判断
		PathSet(ConfigPath, XmlPath);
		string valueXml;
		ErrorCode_Xml errorCode;
		int tempIntValue;
		double tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_CenterX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_CenterX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_CenterX = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_CenterY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_CenterY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_CenterY = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_CenterZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_CenterZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_CenterZ = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_LengthX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_LengthX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_LengthX = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_LengthY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_LengthY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_LengthY = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_LengthZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_LengthZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_LengthZ = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_RX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_RX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_RX = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_RY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_RY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_RY = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ROI_RZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数ROI_RZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.ROI_RZ = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "LocateMethod", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数LocateMethod失败");
			return errorCode;
		}
		runparams.LocateMethod = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ModelName", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数ModelName失败");
			return errorCode;
		}
		runparams.ModelName = valueXml;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "DownSamplingVoxel", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数DownSamplingVoxel失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.DownSamplingVoxel = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "MatchIterations", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数MatchIterations失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runparams.MatchIterations = tempIntValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowMatchCloud", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数PopWindowShowMatchCloud失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.PopWindowShowMatchCloud = true;
		}
		else {
			runparams.PopWindowShowMatchCloud = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint1CenterX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint1CenterX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[0][0] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint1CenterY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint1CenterY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[0][1] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint1CenterZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint1CenterZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[0][2] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint2CenterX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint2CenterX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[1][0] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint2CenterY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint2CenterY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[1][1] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint2CenterZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint2CenterZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[1][2] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint3CenterX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint3CenterX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[2][0] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint3CenterY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint3CenterY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[2][1] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint3CenterZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint3CenterZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[2][2] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint4CenterX", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint4CenterX失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[3][0] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint4CenterY", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint4CenterY失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[3][1] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "BasePoint4CenterZ", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数BasePoint4CenterZ失败");
			return errorCode;
		}
		xmlC.StrToDouble(valueXml, tempDoubleValue);
		runparams.BasePlanePoints[3][2] = tempDoubleValue;

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowBasePointCloud", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数PopWindowShowBasePointCloud失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.PopWindowShowBasePointCloud = true;
		}
		else {
			runparams.PopWindowShowBasePointCloud = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowCropPinCloud", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数PopWindowShowCropPinCloud失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.PopWindowShowCropPinCloud = true;
		}
		else {
			runparams.PopWindowShowCropPinCloud = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowPinSORCloud", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数PopWindowShowPinSORCloud失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.PopWindowShowPinSORCloud = true;
		}
		else {
			runparams.PopWindowShowPinSORCloud = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowPinDownCloud", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数PopWindowShowPinDownCloud失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.PopWindowShowPinDownCloud = true;
		}
		else {
			runparams.PopWindowShowPinDownCloud = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PopWindowShowPinClusterCloud", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数PopWindowShowPinClusterCloud失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.PopWindowShowPinClusterCloud = true;
		}
		else {
			runparams.PopWindowShowPinClusterCloud = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "ShowResult", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取轴承测高参数ShowResult失败");
			return errorCode;
		}
		if (valueXml == "1") {
			runparams.ShowResult = true;
		}
		else {
			runparams.ShowResult = false;
		}

		errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinParamsNum", valueXml);
		if (errorCode != Ok_Xml) {
			WriteTxt("XML读取PIN针测量参数PinParamsNum失败");
			return errorCode;
		}
		xmlC.StrToInt(valueXml, tempIntValue);
		runparams.PinParamsNum = tempIntValue;


		if (runparams.PinParamsNum == 0) {
			WriteTxt("PIN针检测参数中没有PIN参数!");
		}
		else {
			for (int a = 0; a < runparams.PinParamsNum; a++) {
				std::string i = std::to_string(a);
				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinParamsName" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取轴承测高参数PinParamsName" + i + "失败");
					return errorCode;
				}
				runparams.PinParamsName.push_back(valueXml);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_CenterX_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_CenterX_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_CenterX_Vector.push_back(tempDoubleValue);




				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_CenterY_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_CenterY_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_CenterY_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_CenterZ_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_CenterZ_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_CenterZ_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_LengthX_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_LengthX_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_LengthX_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_LengthY_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_LengthY_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_LengthY_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_LengthZ_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_LengthZ_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_LengthZ_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_RX_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_RX_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_RX_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_RY_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_RY_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_RY_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinROI_RZ_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinROI_RZ_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinROI_RZ_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinDownSamplingVoxel_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinDownSamplingVoxel_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinDownSamplingVoxel_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "SORNeighborPoints_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数SORNeighborPoints_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToInt(valueXml, tempIntValue);
				runparams.SORNeighborPoints_Vector.push_back(tempIntValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "SOROutlierThreshold_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数SOROutlierThreshold_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.SOROutlierThreshold_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "EuclideanClusterTolerance_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数EuclideanClusterTolerance_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.EuclideanClusterTolerance_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "EuclideanClusterMinPoints_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数EuclideanClusterMinPoints_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToInt(valueXml, tempIntValue);
				runparams.EuclideanClusterMinPoints_Vector.push_back(tempIntValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "EuclideanClusterMaxPoints_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数EuclideanClusterMaxPoints_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToInt(valueXml, tempIntValue);
				runparams.EuclideanClusterMaxPoints_Vector.push_back(tempIntValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinStandard_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinStandard_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinStandard_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinUpperTol_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinUpperTol_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinUpperTol_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinLowerTol_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinLowerTol_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinLowerTol_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinCorrect_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinCorrect_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToDouble(valueXml, tempDoubleValue);
				runparams.PinCorrect_Vector.push_back(tempDoubleValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "PinNum_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数PinNum_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToInt(valueXml, tempIntValue);
				runparams.PinNum_Vector.push_back(tempIntValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "SortOrder_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取轴承测高参数SortOrder_Vector" + i + "失败");
					return errorCode;
				}
				runparams.SortOrder_Vector.push_back(valueXml);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "HeadAddZeroNum_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数HeadAddZeroNum_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToInt(valueXml, tempIntValue);
				runparams.HeadAddZeroNum_Vector.push_back(tempIntValue);

				errorCode = xmlC.GetXMLValue(XmlPath, processId, nodeName, processModbuleID, "TailAddZeroNum_Vector" + i, valueXml);
				if (errorCode != Ok_Xml) {
					WriteTxt("XML读取PIN针测量参数TailAddZeroNum_Vector" + i + "失败");
					return errorCode;
				}
				xmlC.StrToInt(valueXml, tempIntValue);
				runparams.TailAddZeroNum_Vector.push_back(tempIntValue);
			}
		}


		WriteTxt("PIN针检测参数读取成功!");
		//mutex1.unlock();
		return Ok_Xml;
	}
	catch (...) {
		WriteTxt("XML读取代码崩溃，PIN针检测参数读取失败!");
		//mutex1.unlock();
		return nullUnKnow_Xml;
	}
}

#include <iomanip>
std::string modubleDatas::formatDobleValue(double val, int fixed) {
	double val2 = val;
	if (val2 < 0) {
		val2 = -val2;
	}
	int intNum = 0;
	int temp = val2;
	while (temp) {
		temp /= 10;
		intNum++;
	}
	int intNum2 = 0;
	if (intNum == 0 && val2 > 0) {
		while ((int)val2 <= 0) {
			val2 *= 10;
			intNum2++;
		}
		if (intNum2 > 0)
			intNum2--;
	}

	std::ostringstream oss;
	oss << std::setprecision(fixed + intNum - intNum2) << val;
	return oss.str();
}


//路径设置
void modubleDatas::PathSet(const string path, const string XmlPath) {
	XmlClass2 xmlC;//xml存取变量

	// 判定目录或文件是否存在的标识符
	int mode = 0;
#if defined(_WIN32) || defined(_WIN64)
	if (_access(path.c_str(), mode)) {
		_mkdir(path.c_str());
	}
#else
	if (access(path.c_str(), mode)) {
		mkdir(path.c_str(), 0755);
	}
#endif

	//创建XML(存在就不会创建)
	int Ret = xmlC.CreateXML(XmlPath);
	if (Ret != 0) {
		WriteTxt("打开参数配置xml文件失败");
		//mutex1.unlock();
		return;
	}
}

//写日志函数
int modubleDatas::WriteTxt(string content) {
	if (content.empty()) {
		return -1;
	}
	const char* p = content.c_str();
	ofstream in;
	in.open("D:\\AriLog.txt", ios::app); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
	in << p << "\r";
	in.close();//关闭文件
	return 0;
}

void modubleDatas::writeSearchRegion(HObject searchRegion, const int& processId, const string& nodeName, const int& processModbuleID) {
	std::string ConfigPath = getConfigPath;
	//保存运行区域
	if (searchRegion.Key() == nullptr) {
		//图像为空，或者图像被clear
		WriteTxt(nodeName + "区域为空，保存失败");
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "\\" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		HTuple isExist;
		FileExists(PathRunRegion, &isExist);
		if (isExist > 0) {
			remove(PathRunRegion.S().Text());
		}
	}
	else {
		HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "\\" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
		WriteRegion(searchRegion, PathRunRegion);
	}
}

void modubleDatas::readSearchRegion(HObject& searchRegion, const int& processId, const string& nodeName, const int& processModbuleID) {
	std::string ConfigPath = getConfigPath;
	//运行区域加载
	HTuple PathRunRegion = HTuple(ConfigPath.c_str()) + "\\" + HTuple(to_string(processId).c_str()) + "_" + HTuple(nodeName.c_str()) + HTuple(to_string(processModbuleID).c_str()) + "_RunRegion.hobj";
	HTuple isExist;
	FileExists(PathRunRegion, &isExist);
	if (isExist > 0) {
		GenEmptyObj(&searchRegion);
		ReadRegion(&searchRegion, PathRunRegion);
	}
	else {
		GenEmptyObj(&searchRegion);
		searchRegion.Clear();
	}
}

//按照指定字符分割字符串到vector
void modubleDatas::stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken) {
	string sCopy = sToBeToken;
	int iPosEnd = 0;
	while (true) {
		iPosEnd = sCopy.find(sSeperator);
		if (iPosEnd == -1) {
			vToken.push_back(sCopy);
			break;
		}
		vToken.push_back(sCopy.substr(0, iPosEnd));
		sCopy = sCopy.substr(iPosEnd + 1);
	}
}

