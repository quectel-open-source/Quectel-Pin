#pragma once
#include "algorithmModuble.h"
#include <QLog.h>
#pragma comment(lib , "DXGI.lib")
class EXPORT QIDIOCRDetect :public algorithmModuble {
public:
	QIDIOCRDetect();
	QIDIOCRDetect(std::string _nodeName, int _processModbuleID, int _processID, bool isAlgorithm = false);
	~QIDIOCRDetect();

	void run(int detectID) override;
	void start(bool isStartContinus) override;
	void end() override;
	void setStruct() override;
	void getStruct() override;
	void releaseResource() override;
	bool isExistShowRegion() override;
	HObject getShowRegion() override;

	int getRefreshConfig(int processID, int modubleID, std::string modubleName) override;
	std::vector<std::string> getModubleResultTitleList() override;
	std::vector<std::map<int, std::string>> getModubleResultList() override;

	void chooseModel(std::string configPath);
	int initModel();
	int QIDIOCRFun(HObject ho_Image, ResultParamsStruct_OCR& ResultParams);
#ifdef _WIN32
	//Quectel_Infer quectel_infer = Quectel_Infer();
#endif

	DataIO dataIOC;//参数存取对象
	QLog* qLog = nullptr;
	RunParamsStruct_OCR runParams;
	ResultParamsStruct_OCR resultParams;
	cv::Point point1;
	cv::Point point2;
	cv::Point point3;
	cv::Point point4;

	//std::vector<Results> getResultOCR();

	std::vector<std::string> GPUmodel_paths; //GPU模型地址
	std::vector<std::string> CPUmodel_paths; //CPU模型地址

	std::string config_path = "";
	int getGPUnumber();

	std::string WStringToString(const std::wstring& wstr);
	void  unInit();

	bool isLoaded = false;
};

