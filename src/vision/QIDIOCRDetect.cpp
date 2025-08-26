#include "QIDIOCRDetect.h"
#include <regex>
#if defined(_WIN32) || defined(_WIN64)
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <dataSingleton.h>
using namespace cv;

QIDIOCRDetect::QIDIOCRDetect() {

}

QIDIOCRDetect::QIDIOCRDetect(std::string _nodeName, int _processModbuleID, int _processID, bool isAlgorithm) :algorithmModuble(_nodeName, _processModbuleID, _processID) {
	GenEmptyObj(&runParams.ho_SearchRegion);
	runParams.ho_SearchRegion.Clear();
	qLog = new QLog(processID, processModbuleID, nodeName);

	//std::string newXMLPath = replaceAll(dataSingleton::getXMLPath(), "/", "\\");
	//std::string exeConfig = getConfigFile();
	//if (newXMLPath != exeConfig && isAlgorithm) {
	//	std::string imageFile = getImageFile(_nodeName + ".jpg");
	//	if (fileExist(imageFile)) {
	//		ReadImage(&Image, imageFile.c_str());
	//		for (int index = 0; index < 5; index++)
	//			run(-1);
	//	}
	//}
}

QIDIOCRDetect::~QIDIOCRDetect() {

}

void QIDIOCRDetect::run(int detectID){
	clock_t t1 = data::Clock();
	qLog->writeLog(logType::INFO, "[detectID:" + std::to_string(detectID) + "]	start run");
	GenEmptyObj(&result_DisplyResion);
	try {
		if (isRefresh) {
			isRefresh = false;
			if (getRefreshConfig(processID, processModbuleID, nodeName) != 0) {
				qLog->writeLog(logType::ERROR1, "[detectID:" + std::to_string(detectID) + "]	error in getRefreshConfig");
				updateNodeData(detectID);
				return;
			}
		}

		setStruct();
		ResultParamsStruct_OCR _resultParams;
		int result = QIDIOCRFun(Image, _resultParams);
		resultParams = _resultParams;
		getStruct();
		qLog->writeLog(logType::INFO, "[detectID:" + std::to_string(detectID) + "]	end run time " + std::to_string(data::Clock() - t1));
	}
#ifdef _WIN32
	catch (const seh_excpetion& error) {
		qLog->writeLog(logType::ERROR1, "SEHerror: [" + to_string(error.code()) + "] " + error.what() + "	Class: " + GET_CLASS_NAME() + "	file:	" + GET_FILE_NAME() + "	func:	" + GET_FUNCTION_NAME() + "	line:	" + to_string(GET_LINE_NUMBER()));
	}
#endif
	catch (...) {
		GenEmptyObj(&DestRegions);
		GenEmptyObj(&ContourCodeOCR);
		GenEmptyObj(&RegionCodeOCR);
		GenEmptyObj(&result_DisplyResion);
		qLog->writeLog(logType::ERROR1, "[detectID:" + std::to_string(detectID) + "]	error in run algorithm");
		qLog->writeLog(logType::INFO, "[detectID:" + std::to_string(detectID) + "]	end run time " + std::to_string(data::Clock() - t1));
	}
	try {
		updateNodeData(detectID);
	}
#ifdef _WIN32
	catch (const seh_excpetion& error) {
		qLog->writeLog(logType::ERROR1, "SEHerror: [" + to_string(error.code()) + "] " + error.what() + "	Class: " + GET_CLASS_NAME() + "	file:	" + GET_FILE_NAME() + "	func:	" + GET_FUNCTION_NAME() + "	line:	" + to_string(GET_LINE_NUMBER()));
	}
#endif
	catch (...) {
		qLog->writeLog(logType::ERROR1, "error in run algorithm");
	}
}

void QIDIOCRDetect::start(bool isStartContinus){

}

void QIDIOCRDetect::end(){

}

void QIDIOCRDetect::setStruct(){

}

void QIDIOCRDetect::getStruct(){
	try {
		DestRegions = resultParams.ho_DestRegions;
		ContourCodeOCR = resultParams.ho_ContourCodeOCR;
		RegionCodeOCR = resultParams.ho_RegionCodeOCR;

		OCRContent = resultParams.hv_OCRContent;
		OCRLengthResult = resultParams.hv_OCRLength_Result;
		OCRScoreResult = resultParams.hv_OCRScore_Result;
		Row = resultParams.hv_Row;
		Column = resultParams.hv_Column;
		RetNum = resultParams.hv_RetNum;

		GenEmptyObj(&result_DisplyResion);
		if (RetNum.I() > 0) {
			HTuple countRegion;
			if (resultParams.ho_DestRegions.Key() == nullptr)
				return;
			CountObj(resultParams.ho_DestRegions, &countRegion);
			if (countRegion.I() > 0)
				result_DisplyResion = resultParams.ho_DestRegions.Clone();
		}
	}
#ifdef _WIN32
	catch (const seh_excpetion& error) {
		qLog->writeLog(logType::ERROR1, "SEHerror: [" + to_string(error.code()) + "] " + error.what() + "	Class: " + GET_CLASS_NAME() + "	file:	" + GET_FILE_NAME() + "	func:	" + GET_FUNCTION_NAME() + "	line:	" + to_string(GET_LINE_NUMBER()));
	}
#endif
	catch (...) {
		qLog->writeLog(logType::ERROR1, "error in run algorithm");
	}
}

void QIDIOCRDetect::releaseResource(){

}

bool QIDIOCRDetect::isExistShowRegion() {
	return true;
}

HObject QIDIOCRDetect::getShowRegion() {
	return result_DisplyResion;
}

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#endif
void getFiles(const std::string& path, std::vector<std::string>& folder_list) {
	//查找path文件夹中的所有文件，不搜索子文件夹，同时兼容windows和linux，返回文件名列表
#if defined(_WIN32) || defined(_WIN64)
	_finddata_t file;
	long long lf;
	if ((lf = _findfirst((path + "/*").c_str(), &file)) == -1) {
		return;
	}
	do {
		if (!(file.attrib & _A_SUBDIR)) {
			folder_list.push_back(file.name);
		}
	} while (_findnext(lf, &file) == 0);
	_findclose(lf);
#else
	DIR* dir;
	struct dirent* ptr;
	if ((dir = opendir(path.c_str())) == NULL) {
		return;
	}
	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_type == 8) {
			folder_list.push_back(ptr->d_name);
		}
	}
	closedir(dir);
#endif


}

#if defined(_WIN32) || defined(_WIN64)
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#else

#endif

int QIDIOCRDetect::getGPUnumber() {
#if defined(_WIN32) || defined(_WIN64)
	// Windows-specific code using DXGI
	IDXGIFactory* pFactory;
	IDXGIAdapter* pAdapter;
	std::vector<IDXGIAdapter*> vAdapters;       
	int iAdapterNum = 0;
	int NVIDIANum = 0;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	if (FAILED(hr))
		return -1;

	while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
		vAdapters.push_back(pAdapter);
		++iAdapterNum;
	}

	for (size_t i = 0; i < vAdapters.size(); i++) {
		DXGI_ADAPTER_DESC adapterDesc;
		vAdapters[i]->GetDesc(&adapterDesc);
		std::wstring aa(adapterDesc.Description);
		std::string bb = WStringToString(aa);
		if (bb.substr(0, 6) == "NVIDIA") {
			NVIDIANum++;
		}
	}
	vAdapters.clear();
	return NVIDIANum;

#else
	std::vector<std::string> folder_list;
	std::string path = "/sys/class/drm";
	getFiles(path, folder_list);
	int NVIDIANum = 0;
	for (const auto& folder : folder_list) {
		if (folder.find("card") != std::string::npos) {
			std::string card_path = path + "/" + folder + "/device/vendor";
			std::ifstream vendor_file(card_path);
			if (vendor_file.is_open()) {
				std::string vendor;
				std::getline(vendor_file, vendor);
				if (vendor == "0x10de") {
					NVIDIANum++;
				}
				vendor_file.close();
			}
		}
	}
	return NVIDIANum;
#endif
}


std::string QIDIOCRDetect::WStringToString(const std::wstring& wstr) {
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

int QIDIOCRDetect::getRefreshConfig(int processID, int modubleID, std::string modubleName) {
	try {
		qLog->writeLog(logType::INFO, "update algorithm config");
		int ret = 1;
		GenEmptyObj(&runParams.ho_SearchRegion);
		runParams.ho_SearchRegion.Clear();
		ErrorCode_Xml errorCode = dataIOC.ReadParams_OCR(runParams, processID, nodeName, processModbuleID);
		if (runParams.modelPath == "") {
			qLog->writeLog(logType::ERROR1, "error in update algorithm config");
			return-1;
		}
		else {
			chooseModel(runParams.modelPath);
			unInit();
			return initModel();
		}
	}
	catch (...) {
		qLog->writeLog(logType::ERROR1, "error in update algorithm config");
		return -1;
	}
	return 0;
}

std::vector<std::string> QIDIOCRDetect::getModubleResultTitleList() {
	std::vector<std::string> result;
	modubleResultTitleList.clear();
	modubleResultTitleList.push_back("字符内容");
	modubleResultTitleList.push_back("字符分数");
	modubleResultTitleList.push_back("字符长度");
	modubleResultTitleList.push_back("X坐标");
	modubleResultTitleList.push_back("Y坐标");
	result = modubleResultTitleList;
	return result;
}
#ifdef _WIN32
#else
#include <iostream>
#include <string>
#include <vector>
#include <iconv.h>

std::string convertToUTF8(const std::string& local8BitString) {
	iconv_t cd = iconv_open("UTF-8", "LOCAL8BIT");
	if (cd == (iconv_t)-1) {
		std::cerr << "iconv_open failed" << std::endl;
		return "";
	}

	size_t inbytesleft = local8BitString.size();
	size_t outbytesleft = inbytesleft * 2; // 预留足够的空间
	std::vector<char> outbuf(outbytesleft);
	char* inbuf = const_cast<char*>(local8BitString.c_str());
	char* outptr = outbuf.data();

	if (iconv(cd, &inbuf, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
		std::cerr << "iconv failed" << std::endl;
		iconv_close(cd);
		return "";
	}

	iconv_close(cd);
	return std::string(outbuf.data(), outbuf.size() - outbytesleft);
}
#endif
std::vector<std::map<int, std::string>> QIDIOCRDetect::getModubleResultList() {
	std::vector<std::map<int, std::string>> results;
	try {
		int count = resultParams.hv_RetNum.I();
		for (int i = 0; i < count; i++) {
			std::map<int, std::string> result;
			int index = 0;
#ifdef _WIN32
			result[index++] = resultParams.hv_OCRContent[i].S().Text();
#else
			result[index++] = convertToUTF8(resultParams.hv_OCRContent[i].S().Text());
#endif
			result[index++] = formatDobleValue(resultParams.hv_OCRScore_Result[i].D(), 2);
			result[index++] = std::to_string(resultParams.hv_OCRLength_Result[i].I());
			result[index++] = formatDobleValue(resultParams.hv_Column[i].D(), 2);
			result[index++] = formatDobleValue(resultParams.hv_Row[i].D(), 2);
			results.push_back(result);
		}
	}
#ifdef _WIN32
	catch (const seh_excpetion& error) {
		qLog->writeLog(logType::ERROR1, "SEHerror: [" + to_string(error.code()) + "] " + error.what() + "	Class: " + GET_CLASS_NAME() + "	file:	" + GET_FILE_NAME() + "	func:	" + GET_FUNCTION_NAME() + "	line:	" + to_string(GET_LINE_NUMBER()));
	}
#endif
	catch (...) {
		qLog->writeLog(logType::ERROR1, "error in run algorithm");
	}
	return results;
}

void  QIDIOCRDetect::unInit() {
	if (isLoaded) {
#ifdef _WIN32
		//quectel_infer.unInit_inference();
#endif
		isLoaded = false;
	}
}

void QIDIOCRDetect::chooseModel(std::string configPath) {
	std::vector<std::string> files;
	getFiles(configPath, files);
	for (int i = 0; i < files.size(); i++) {
		std::string file = files[i];
		std::string suffix = file.substr(file.find_last_of(".") + 1);
		if (suffix == "json") {
			config_path = configPath+"/"+file;
		}
		else if (suffix == "engine") {
			GPUmodel_paths.push_back(configPath + "/" + file);
		}
		else if (suffix == "m") {
			CPUmodel_paths.push_back(configPath + "/" + file);
		}
	}
}

int QIDIOCRDetect::initModel(){
	int init_flag = 0;
//#ifdef _WIN32
//	Json::Value inferValue;
//	Json::Reader reader;
//	Json::Value jsonValue;
//	ifstream is;
//
//	is.open(config_path.c_str(), ios::app);
//	if (!reader.parse(is, jsonValue)) {
//		//getmessage("模型读取失败！");
//		is.close();
//		return -1;
//	}
//	inferValue = jsonValue["inference"][0];  // inference字段为数组，取第0位，后续取值均类似写法
//
//
//	std::string algorithm = jsonValue["inference"][0]["algorithm"].asString();
//
//	is.close();
//	string save_image_root;
//
//	if (getGPUnumber() == 0) {
//		init_flag = quectel_infer.initial_inference(inferValue, CPUmodel_paths, save_image_root);
//	}
//	else {
//		if (runParams.isCpu == false) {
//			init_flag = quectel_infer.initial_inference(inferValue, GPUmodel_paths, save_image_root);
//		}
//		else {
//			init_flag = quectel_infer.initial_inference(inferValue, CPUmodel_paths, save_image_root);
//		}
//	}
//	isLoaded = true;
//#endif
	return init_flag;
}

int QIDIOCRDetect::QIDIOCRFun(HObject ho_Image, ResultParamsStruct_OCR& ResultParams){
	try {
	//resultDL.clear();
//	ResultParams.hv_RetNum = 0;
//	//hv_resultRegions.clear();
//	OutImage = ho_Image;
//	ResultParams.hv_Row.Clear();
//	ResultParams.hv_Column.Clear();
//	ResultParams.hv_OCRContent.Clear();
//	ResultParams.hv_OCRLength_Result.Clear();
//
//	GenEmptyObj(&ResultParams.ho_DestRegions);
//	if (ho_Image.Key() == nullptr) {
//		return -1;
//	}
//	HObject ho_ImageReduced;
//	GenEmptyObj(&ho_ImageReduced);
//	if (runParams.ho_SearchRegion.Key() == nullptr)
//		ho_ImageReduced = ho_Image.Clone();
//	else {
//		ReduceDomain(ho_Image, runParams.ho_SearchRegion, &ho_ImageReduced);
//		//WriteObject(RunParams.ho_SearchRegion,"E:\\RNDY\\debugPath\\0706\\x64\\Debug\\22");
//	}
//
//	HObject ho_ImageReduced1;
//	GenEmptyObj(&ho_ImageReduced1);
//	CropDomain(ho_ImageReduced, &ho_ImageReduced1);
//	Mat image = HObject2Mat(ho_ImageReduced1);
//	if (image.channels() == 1) {
//		cvtColor(image, image, COLOR_GRAY2BGR);
//	}
//
//
//
//	HObject imgTemp;
//	int RetNumTemp = 0;
//	string predictLabel = "";
//	IsShowInfo _isShowInfo;
//	_isShowInfo.is_getLabel = true;
//	_isShowInfo.is_enablePost = false;
//	_isShowInfo.is_genResultImg = true;
//	_isShowInfo.colors = { {0,0,255}, {100,175,255}, {128,0,128},{255,0,0} };  // 缺陷颜色，bgr格式
//
//	std::vector<Results> results;
//	cv::Mat dst_image = image.clone();
//	cv::Mat map_label(image.rows, image.cols, CV_8UC1); //output label maskmap
//	if (image.empty()) {
//		return -1;
//	}
//	//run inference
//	try {
//#ifdef _WIN32
//		quectel_infer.inference(image.data, dst_image.data, predictLabel, image.cols, image.rows, image.channels(), _isShowInfo, map_label.data, results);
//#endif
//	}
//	catch (const std::exception& e) {
//		qLog->writeLog(logType::ERROR1, "error in algorithm");
//		std::cout << e.what();
//	}
//
//
//
//	int resultsize = results.size();
//	ResultParams.hv_RetNum = resultsize;
//
//	HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
//	if (runParams.ho_SearchRegion.Key() == nullptr) {
//		hv_Row1 = 0;
//		hv_Column1 = 0;
//	}
//	else
//		SmallestRectangle1(runParams.ho_SearchRegion, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
//	int offsetY = (int)hv_Row1.D();
//	int offsetX = (int)hv_Column1.D();
//
//	if (true) {
//		for (int i = 0; i < results.size(); i++) {
//			//计算字符得分
//			if (results[i].score < runParams.thresh) {
//				continue;
//			}
//			if (runParams.isdigit == true) {
//				regex reg1("^[0-9]+$");
//				if (!regex_match(results[i].label, reg1)) {
//					continue;
//				}
//			}
//			if (runParams.isupper == true) {
//				regex reg2("^[A-Z]+$");
//				if (!regex_match(results[i].label, reg2)) {
//					continue;
//				}
//			}
//			if (runParams.islower == true) {
//				regex reg3("^[a-z]+$");
//				if (!regex_match(results[i].label, reg3)) {
//					continue;
//				}
//			}
//			if (runParams.isSpecial == true) {
//
//				regex reg4("[~!/@#$%^&*()-_=+\\|[{}];:\'\",<.>/?]+");
//				if (!regex_match(results[i].label, reg4)) {
//					continue;
//				}
//
//			}
//			if (runParams.ischinese == true) {
//				regex reg5("^[\u4e00-\u9fa5]{0,}$");
//				if (!regex_match(results[i].label.c_str(), reg5)) {
//					continue;
//				}
//			}
//			//计算字符长度
//			HTuple CodeLengthTemp;//计算条码长度临时变量
//			TupleStrlen(results[i].label.c_str(), &CodeLengthTemp);
//			if (CodeLengthTemp < runParams.hv_MinCodeLength_Run ||
//				CodeLengthTemp > runParams.hv_MaxCodeLength_Run) {
//				//长度不满足，退出当前循环
//				continue;
//			}
//			if (runParams.hv_ContainChar != ""&& runParams.hv_ContainChar != "null") {
//				string yuiy = runParams.hv_ContainChar.S().Text();
//				//查找字符串是否包含特定字符
//				regex reg6(".*" + yuiy + ".*");
//				if (!regex_match(results[i].label, reg6)) {
//					continue;
//				}
//			}
//			if (runParams.hv_NotContainChar != ""&& runParams.hv_NotContainChar != "null") {
//				//查找字符串是否不包含特定字符
//				string yuiyt = runParams.hv_NotContainChar.S().Text();
//				//查找字符串是否包含特定字符
//				regex reg7("^(?!.*" + yuiyt + ").*$");
//				if (!regex_match(results[i].label, reg7)) {
//					continue;
//				}
//			}
//			if (runParams.hv_HeadChar != "" && runParams.hv_HeadChar != "null") {
//				//查找字符串是否特定字符开头
//				string yuiyrt = runParams.hv_HeadChar.S().Text();
//				//查找字符串是否包含特定字符
//				regex reg7("^(" + yuiyrt + ").*");
//				if (!regex_match(results[i].label, reg7)) {
//					continue;
//				}
//			}
//			if (runParams.hv_EndChar != "" && runParams.hv_EndChar != "null") {
//				//查找字符串是否特定字符结尾
//				string yuiyrrt = runParams.hv_EndChar.S().Text();
//				//查找字符串是否包含特定字符
//				regex reg8(".*" + yuiyrrt + "$");
//				if (!regex_match(results[i].label, reg8)) {
//					continue;
//				}
//			}
//
//			double  angle = results[i].angle;
//			double cosA = cos(angle / (180.0 / CV_PI));
//			double sinA = sin(angle / (180.0 / CV_PI));
//			//左上，右上，右下，左下
//
//			results[i].cx += offsetX;
//			results[i].cy += offsetY;
//			results[i].pointsVector[0][0] += offsetX;
//			results[i].pointsVector[0][1] += offsetY;
//			results[i].pointsVector[1][0] += offsetX;
//			results[i].pointsVector[1][1] += offsetY;
//			results[i].pointsVector[2][0] += offsetX;
//			results[i].pointsVector[2][1] += offsetY;
//			results[i].pointsVector[3][0] += offsetX;
//			results[i].pointsVector[3][1] += offsetY;
//
//			point1 = cv::Point(results[i].pointsVector[0][0], results[i].pointsVector[0][1]);
//			point2 = cv::Point(results[i].pointsVector[1][0], results[i].pointsVector[1][1]);
//			point3 = cv::Point(results[i].pointsVector[2][0], results[i].pointsVector[2][1]);
//			point4 = cv::Point(results[i].pointsVector[3][0], results[i].pointsVector[3][1]);
//
//
//			//line(displyImage, point1, point2, cv::Scalar(0, 255, 0), 3, 2);
//			//line(displyImage, point3, point2, cv::Scalar(0, 255, 0), 3, 2);
//			//line(displyImage, point3, point4, cv::Scalar(0, 255, 0), 3, 2);
//			//line(displyImage, point4, point1, cv::Scalar(0, 255, 0), 3, 2);
//
//			//resultDL.push_back(results[i]);
//
//			HObject regionLine;
//			GenEmptyObj(&regionLine);
//			GenRegionLine(&regionLine, point1.y, point1.x, point2.y, point2.x);
//			Union2(regionLine, ResultParams.ho_DestRegions, &ResultParams.ho_DestRegions);
//			GenEmptyObj(&regionLine);
//			GenRegionLine(&regionLine, point3.y, point3.x, point2.y, point2.x);
//			Union2(regionLine, ResultParams.ho_DestRegions, &ResultParams.ho_DestRegions);
//			GenEmptyObj(&regionLine);
//			GenRegionLine(&regionLine, point3.y, point3.x, point4.y, point4.x);
//			Union2(regionLine, ResultParams.ho_DestRegions, &ResultParams.ho_DestRegions);
//			GenEmptyObj(&regionLine);
//			GenRegionLine(&regionLine, point4.y, point4.x, point1.y, point1.x);
//			Union2(regionLine, ResultParams.ho_DestRegions, &ResultParams.ho_DestRegions);
//
//			/*putTextZH(displyImage, results[i].label.c_str(), cv::Point(results[i].cx - results[i].width / 2,
//				results[i].cy - results[i].height / 2 - 20), cv::Scalar(0, 0, 255), 25, "微软雅黑", false, false);*/
//
//			ResultParams.hv_Column.Append(results[i].cx);
//			ResultParams.hv_Row.Append(results[i].cy);
//			ResultParams.hv_OCRLength_Result.Append(CodeLengthTemp);
//			ResultParams.hv_OCRContent.Append(results[i].label.c_str());
//			ResultParams.hv_OCRScore_Result.Append(results[i].score);
//			RetNumTemp += 1;
//		}
//
//	}
//
//
//	//MoveRegion(ResultParams.ho_DestRegions, &ResultParams.ho_DestRegions, hv_Row1, hv_Column1);
//	//GenEmptyObj(&ResultParams.ho_DestRegions);
//	ResultParams.hv_RetNum = RetNumTemp;//找到的字符个数
//	this->resultParams = ResultParams;
			}
#ifdef _WIN32
			catch (const seh_excpetion& error) {
				qLog->writeLog(logType::ERROR1, "SEHerror: [" + to_string(error.code()) + "] " + error.what() + "	Class: " + GET_CLASS_NAME() + "	file:	" + GET_FILE_NAME() + "	func:	" + GET_FUNCTION_NAME() + "	line:	" + to_string(GET_LINE_NUMBER()));
			}
#endif
			catch (...) {
				qLog->writeLog(logType::ERROR1, "error in run algorithm");
			}
	return 0;
}

//std::vector<Results> QIDIOCRDetect::getResultOCR(){
//	return resultDL;
//}