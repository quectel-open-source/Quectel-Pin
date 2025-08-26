#include "QIDIOCRWindow.h"
#ifdef _WIN32
#pragma comment(lib , "DXGI.lib")
QIDIOCRWindow::QIDIOCRWindow(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	QFile f("./qdarkstyle/theme/darkstyle.qss");

	if (!f.exists()) {
		printf("Unable to set stylesheet, file not found\n");
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}
	setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	ui.tablewidget_Results_QIDI->horizontalHeader()->setSortIndicatorShown(true);		//显示排序图标（默认为上下箭头）
	ui.tablewidget_Results_QIDI->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);	//设置第0列 升序排序
	ui.tablewidget_Results_QIDI->horizontalHeader()->setStyleSheet("QHeaderView::up-arrow { subcontrol-position: center right; padding-right: 12px;"
		"image: url(:/QtToolTest/Resources/向上.png);}"
		"QHeaderView::down-arrow { subcontrol-position: center right; padding-right: 12px;"
		"image: url(:/QtToolTest/Resources/向下.png);}");
	//连接水平表头响应
	connect(ui.tablewidget_Results_QIDI->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortItems(int)));

	//初始化窗口
	//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
	ui.lbl_Window->setMouseTracking(true);
	//生成空图像
	GenEmptyObj(&ho_Image);
	ho_Image.Clear();
	m_hHalconID = NULL;
	m_hLabelID = (Hlong)ui.lbl_Window->winId();
	IniTableData_QIDI();
	//初始化模型及环境
	QIDIinit();
}

QIDIOCRWindow::~QIDIOCRWindow() {
}

QIDIOCRWindow::QIDIOCRWindow(HObject image, int _processID, int modubleID, std::string _modubleName
	, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig) {
	ui.setupUi(this);
	
	setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	ui.tablewidget_Results_QIDI->horizontalHeader()->setSortIndicatorShown(true);		//显示排序图标（默认为上下箭头）
	ui.tablewidget_Results_QIDI->horizontalHeader()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);	//设置第0列 升序排序
	ui.tablewidget_Results_QIDI->horizontalHeader()->setStyleSheet("QHeaderView::up-arrow { subcontrol-position: center right; padding-right: 12px;"
		"image: url(:/QtToolTest/Resources/向上.png);}"
		"QHeaderView::down-arrow { subcontrol-position: center right; padding-right: 12px;"
		"image: url(:/QtToolTest/Resources/向下.png);}");
	//连接水平表头响应
	//connect(ui.tablewidget_Results_QIDI->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortItems(int)));
	//
	//connect(ui.windowMin, &QPushButton::clicked, this, &QIDIOCRWindow::windowMin);
	//connect(ui.windowMax, &QPushButton::clicked, this, &QIDIOCRWindow::windowMax);
	//connect(ui.windowClose, &QCheckBox::clicked, this, &QIDIOCRWindow::windowClose);
	//connect(ui.pBtn_FitImage, &QPushButton::clicked, this, &QIDIOCRWindow::pBtn_FitImage);
	//connect(ui.btn_ClearWindow, &QPushButton::clicked, this, &QIDIOCRWindow::btn_ClearWindow);
	//connect(ui.btn_FindCharacter, &QCheckBox::clicked, this, &QIDIOCRWindow::btn_FindCharacter);
	//connect(ui.pushButton_saveQIDI, &QPushButton::clicked, this, &QIDIOCRWindow::pushButton_saveQIDI);
	//connect(ui.btn_DrawRoi_QIDI, &QCheckBox::clicked, this, &QIDIOCRWindow::btn_DrawRoi_QIDI);
	//connect(ui.btn_ClearRoi_QIDI, &QPushButton::clicked, this, &QIDIOCRWindow::btn_ClearRoi_QIDI);
	//connect(ui.toolButton_ChooseModel, &QCheckBox::clicked, this, &QIDIOCRWindow::toolButton_ChooseModel);


	////初始化窗口
	////设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
	//ui.lbl_Window->setMouseTracking(true);
	////生成空图像
	//GenEmptyObj(&ho_Image);
	//ho_Image.Clear();
	//m_hHalconID = NULL;
	//m_hLabelID = (Hlong)ui.lbl_Window->winId();


	//nodeName = _modubleName;
	//processModbuleID = modubleID;
	//processId = _processID;
	//refreshConfig = _refreshConfig;
	//refreshPort = _refreshPort;
	////窗口初始化
	////椭圆检测
	//QIDIOCRDetect1 = new QIDIOCRDetect(nodeName, processModbuleID, processId);
	//ho_Image = image.Clone();
	//GiveParameterToWindow();
	//showImg(ho_Image, "margin", "green");
	//nodeEditor1 = new nodeEditor(nodeName, processId, processModbuleID, refreshPort);
	//QWidget* tabWidget = new QWidget();
	//ui.tabWidget->addTab(tabWidget, "节点设置");
	//nodeEditor1->readNodeInfoToWidget(tabWidget);

	//runParams.modelPath = "";
	//GenEmptyObj(&runParams.ho_SearchRegion);
	//runParams.ho_SearchRegion.Clear();

	//try {
	//	ErrorCode_Xml errorCode = dataIOC.ReadParams_OCR(runParams, processId, nodeName, processModbuleID);
	//}
	//catch (std::exception e) {
	//	std::cout << e.what() << std::endl;
	//}

	//IniTableData_QIDI();
	//if (runParams.modelPath != "") {
	//	chooseModel(runParams.modelPath.c_str());
	//	//初始化模型及环境

	//}
	//QIDIinit();
	//GenEmptyObj(&ho_ShowObj);
	//qlog = new QLog(processId, processModbuleID, nodeName);
	//qlog->writeLog(logType::INFO, "open window");

	//ui.btn_DrawRoi_QIDI->setEnabled(true);
}

std::string QIDIOCRWindow::WStringToString(const std::wstring& wstr) {
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <dxgi.h>
#else

#endif
#include <dataTranfer.h>

int QIDIOCRWindow::getGPUnumber() {
#ifdef _WIN32
	// Windows平台代码
	IDXGIFactory* pFactory;
	IDXGIAdapter* pAdapter;
	std::vector<IDXGIAdapter*> vAdapters; // 显卡
	int iAdapterNum = 0; // 显卡的数量
	int NVIDIANum = 0; // NVIDIA显卡的数量
	// 创建一个DXGI工厂
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

	if (FAILED(hr))
		return -1;

	// 枚举适配器
	while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
		vAdapters.push_back(pAdapter);
		++iAdapterNum;
	}

	for (size_t i = 0; i < vAdapters.size(); i++) {
		// 获取信息
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
	
	return 0;
#endif
}


void QIDIOCRWindow::resizeEvent(QResizeEvent* event) {
	if (ho_Image.Key() == nullptr) {
		return;
	}
	if (m_hHalconID != NULL) {
		CloseWindow(m_hHalconID);
	}

	int  width = ui.lbl_Window->width();
	int  height = ui.lbl_Window->height();


	OpenWindow(0,
		0,
		(Hlong)width,
		(Hlong)height,
		m_hLabelID,
		"visible",
		"",
		&m_hHalconID);
	showImg(ho_Image, "margin", "green");
}

void QIDIOCRWindow::windowMin() {
	QIDIOCRWindow::showMinimized(); //最小化
}

void QIDIOCRWindow::windowMax() {
	if (QIDIOCRWindow::isMaximized()) {
		QIDIOCRWindow::showNormal();//还原事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		QIDIOCRWindow::showMaximized();//最大化事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void QIDIOCRWindow::windowClose() {
	if (isDrawRoi) {
		QMessageBox::information(this, "提示", "正在绘制中，请先鼠标右键结束后操作!", QMessageBox::Ok);
		return;
	}

	QIDIOCRDetect1->unInit();
	this->close();
}

void QIDIOCRWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void QIDIOCRWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//当窗口最大化或最小化时也不进行触发
		if (QIDIOCRWindow::isMaximized() || QIDIOCRWindow::isMinimized()) {
			return;
		}
		else {
			//当在按钮之类需要鼠标操作的地方不进行触发(防误触)
			if (ui.windowClose->underMouse() || ui.windowMax->underMouse() || ui.windowMin->underMouse()
				) {

			}
			else if (keypoint == 1) {

			}
			else {
				//QIDIOCRWindow::move(QIDIOCRWindow::mapToGlobal(event->pos() - whereismouse));//移动
			}
		}
	}
	event->accept();
}

void QIDIOCRWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

//初始化结果显示表格
void QIDIOCRWindow::IniTableData_QIDI() {
	QTableWidgetItem* hearderItem;
	QStringList hearderText;
	std::vector<std::string> titleList = QIDIOCRDetect1->getModubleResultTitleList();
	for (int index = 0; index < titleList.size(); index++)
		hearderText.push_back(QString::fromStdString(titleList[index]));
	ui.tablewidget_Results_QIDI->setColumnCount(hearderText.count());
	for (int i = 0; i < ui.tablewidget_Results_QIDI->columnCount(); i++) {
		hearderItem = new QTableWidgetItem(hearderText.at(i));
		QFont font = hearderItem->font();
		font.setBold(true);//设置为粗体
		font.setPointSize(11);//设置字体大小
		hearderItem->setTextColor(Qt::red);//字体颜色
		hearderItem->setFont(font);//设置字体
		ui.tablewidget_Results_QIDI->setHorizontalHeaderItem(i, hearderItem);
	}
}

//显示图像
void  QIDIOCRWindow::showImg(const HObject& ho_Img, HTuple hv_FillType, HTuple hv_Color) {
	if (ho_Img.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}

	//判断窗口句柄
	if (m_hHalconID != NULL) {
		//如果有图像，则先清空图像
		DetachBackgroundFromWindow(m_hHalconID);
	}
	else {
		//打开窗口
		OpenWindow(0, 0, ui.lbl_Window->width(), ui.lbl_Window->height(), m_hLabelID, "visible", "", &m_hHalconID);
	}

	//设置图形显示属性
	SetDraw(m_hHalconID, hv_FillType);
	SetColor(m_hHalconID, hv_Color);
	SetLineWidth(m_hHalconID, 1);
	//设置窗口显示变量
	ui.lbl_Window->setHalconWnd(ho_Img, ho_ShowObj, isShowObj, m_hHalconID, ui.lbl_Status);

	//获取图像大小
	GetImageSize(ho_Img, &m_imgWidth, &m_imgHeight);
	//获取缩放系数
	TupleMin2(1.0 * ui.lbl_Window->width() / m_imgWidth, 1.0 * ui.lbl_Window->height() / m_imgHeight, &m_hvScaledRate);
	//缩放图像
	ZoomImageFactor(ho_Img, &m_hResizedImg, m_hvScaledRate, m_hvScaledRate, "constant");
	//获取缩放后的大小
	GetImageSize(m_hResizedImg, &m_scaledWidth, &m_scaledHeight);
	//打开窗口
	if (1.0 * ui.lbl_Window->width() / m_imgWidth < 1.0 * ui.lbl_Window->height() / m_imgHeight) {
		SetWindowExtents(m_hHalconID, ui.lbl_Window->height() / 2.0 - m_scaledHeight / 2.0, 0, ui.lbl_Window->width(), m_scaledHeight);
	}
	else {
		SetWindowExtents(m_hHalconID, 0, ui.lbl_Window->width() / 2.0 - m_scaledWidth / 2.0, m_scaledWidth, ui.lbl_Window->height());

	}
	SetPart(m_hHalconID, 0, 0, m_imgHeight - 1, m_imgWidth - 1);
	//这句显示图像，不会闪烁(把图像设置成窗口的背景图)
	AttachBackgroundToWindow(ho_Img, m_hHalconID);

	//显示图形
	if (isShowObj) {
		SetCheck("give_error");
		try {
			DispObj(ho_ShowObj, m_hHalconID);
		}
		catch (...) {
			return;
		}
		SetCheck("give_error");
	}
}

void QIDIOCRWindow::SetRunParams_QIDI() {
	runParams.thresh = ui.doubleSpinBox_detdbthresh->value();
	runParams.unclip_ratio = ui.doubleSpinBox_unclipratio->value();
	runParams.hv_TimeOut = ui.spb_TimeOut_QIDI->value();
	runParams.isdigit = ui.ckb_number_OCR->isChecked();
	runParams.islower = ui.ckb_a_OCR->isChecked();
	runParams.isupper = ui.ckb_A_OCR->isChecked();
	runParams.isSpecial = ui.ckb_Scharacter_OCR->isChecked();
	runParams.ischinese = ui.ckb_china_OCR->isChecked();

	runParams.hv_HeadChar = ui.txt_HeadChar_OCR->text().toStdString().c_str();
	runParams.hv_EndChar = ui.txt_EndChar_OCR->text().toStdString().c_str();
	runParams.hv_MinCodeLength_Run = ui.txt_MinLength_OCR->text().toInt();
	runParams.hv_MaxCodeLength_Run = ui.txt_MaxLength_OCR->text().toInt();
	runParams.hv_ContainChar = ui.txt_ContainChar_OCR->text().toStdString().c_str();
	runParams.hv_NotContainChar = ui.txt_NotContainChar_OCR->text().toStdString().c_str();
	//读取json文件
	QString strPath;
	if (ui.txt_path->text() == "") {
		strPath = "./testocr/model.json";//默认值
		runParams.modelPath = "./testocr/";
	}
	else {
		strPath = ui.txt_path->text();
	}

	QFile readFile(strPath);
	if (!readFile.open(QFile::ReadOnly | QFile::Truncate)) {
		qDebug() << "can't open error!";
		return;
	}
	// 读取文件的全部内容
	QTextStream readStream(&readFile);
	readStream.setCodec("UTF-8");		// 设置读取编码是UTF8
	QString str = readStream.readAll();
	readFile.close();
	/* 修改Json */
   // QJsonParseError类用于在JSON解析期间报告错误。
	QJsonParseError jsonError;
	// 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
	// 如果解析成功，返回QJsonDocument对象，否则返回null
	QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
	if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
		qDebug() << "配置格式错误！" << jsonError.error;
		return;
	}
	// 获取根 { }
	QJsonObject rootObj = doc.object();
	// 修改 [{ }] 中的值
	QJsonValue algorithmValue = rootObj.value("inference");
	if (algorithmValue.type() == QJsonValue::Array) {
		QJsonArray algorithmArray = algorithmValue.toArray();
		// 根据索引获得对应{ }
		QJsonObject obj1 = algorithmArray[0].toObject();
		//修改 { } 中的值
		QJsonValue modelValue = obj1.value(modelType);
		if (modelValue.type() == QJsonValue::Object) {
			QJsonObject modelObject = modelValue.toObject();
			QJsonValue detValue = modelObject.value("det");
			if (detValue.type() == QJsonValue::Object) {
				QJsonObject labelObject = detValue.toObject();
				double qew = ui.doubleSpinBox_detdbthresh->value();
				labelObject["box_thresh"] = ui.doubleSpinBox_detdbthresh->value();
				labelObject["unclip_thresh"] = ui.doubleSpinBox_unclipratio->value();
				modelObject["det"] = labelObject;
			}
			obj1[modelType] = modelObject;
		}
		// 替换覆盖
		algorithmArray.replace(0, obj1);
		rootObj["inference"] = algorithmArray;
	}

	// 将object设置为本文档的主要对象
	doc.setObject(rootObj);
	// 重写打开文件，覆盖原有文件，达到删除文件全部内容的效果
	QFile writeFile(strPath);
	if (!writeFile.open(QFile::WriteOnly | QFile::Truncate)) {
		getmessage("can't open error!");
		return;
	}
	// 将修改后的内容写入文件
	QTextStream wirteStream(&writeFile);
	wirteStream.setCodec("UTF-8");		// 设置读取编码是UTF8
	wirteStream << doc.toJson();		// 写入文件
	writeFile.close();					// 关闭文件
	//quectel_infer.unInit_inference();
	QIDIinit();
}

void QIDIOCRWindow::toolButton_ChooseModel() {
	QString filePath = QFileDialog::getExistingDirectory(this, "请选择模型路径…", "./");
	chooseModel(filePath);
}

void QIDIOCRWindow::chooseModel(QString filePath) {
	if (filePath.size() == 0) {
		getmessage("未选择任何模型路径！");
		return;
	}
	runParams.modelPath = filePath.toStdString();
	QDir d(filePath);
	d.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
	d.setSorting(QDir::Time | QDir::DirsFirst);
	QFileInfoList list = d.entryInfoList();
	GPUmodel_paths.clear();
	CPUmodel_paths.clear();
	for (int i = 0; i < list.size(); i++) {
		if (list.at(i).isFile()) {
			if (list.at(i).suffix() == "json") {
				QString uuiuyyt = filePath + "/" + list.at(i).fileName();
				config_path = uuiuyyt.toStdString();
				Readconfig(uuiuyyt.toStdString());
				ui.txt_path->setText(uuiuyyt);
			}
			if (list.at(i).suffix() == "engine") {
				QString uui = filePath + "/" + list.at(i).fileName();
				GPUmodel_paths.push_back(uui.toStdString());
			}
			if (list.at(i).suffix() == "m") {
				QString uui2 = filePath + "/" + list.at(i).fileName();
				CPUmodel_paths.push_back(uui2.toStdString());
			}
		}
	}
}

int QIDIOCRWindow::Readconfig(std::string configpath1) {
	if (configpath1 == "") {
		getmessage("未加载任何配置文件!");
		return -1;
	}
	ui.txt_path->setText(QString::fromStdString(configpath1));
	QFile file(QString::fromStdString(configpath1));
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qDebug() << "can't open error!";
		return -1;
	}
	// 读取文件的全部内容
	QTextStream stream(&file);
	stream.setCodec("UTF-8");		// 设置读取编码是UTF8
	QString str = stream.readAll();
	file.close();
	// QJsonParseError类用于在JSON解析期间报告错误。
	QJsonParseError jsonError;
	// 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
	// 如果解析成功，返回QJsonDocument对象，否则返回null
	QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
	// 判断是否解析失败
	if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
		getmessage("配置格式错误！");
		return -1;
	}
	QJsonObject rootObj = doc.object();
	// 根据键获取值
	QJsonValue inferenceValue = rootObj.value("inference");
	// 判断类型是否是数组类型
	if (inferenceValue.type() == QJsonValue::Array) {
		// 转换成数组类型
		QJsonArray inferenceArray = inferenceValue.toArray();
		// 遍历数组
		for (int i = 0; i < inferenceArray.count(); i++) {
			// 获取数组的第一个元素，类型是QJsonValue 
			QJsonValue inferenceValueChild = inferenceArray.at(i);
			// 判断是不是对象类型
			if (inferenceValueChild.type() == QJsonValue::Object) {
				// 转换成对象类型
				QJsonObject inferenceObj = inferenceValueChild.toObject();
				// 最后通过value函数就可以获取到值了，解析成功！
				QJsonValue algorithmValue = inferenceObj.value("algorithm");
				QJsonValue frameworkValue = inferenceObj.value("framework");
				//该段代码对模型类别进行区分"detection" ，"segmentation"，"ocr"
				if (algorithmValue.toString() != "OCR") {
					getmessage("非字符识别模型，请重新选择模型文件");
					ui.btn_FindCharacter->setEnabled(false);
					return -1;
				}
				ui.btn_FindCharacter->setEnabled(true);
				modelType = frameworkValue.toString() + "_model";
				QJsonValue modelValue = inferenceObj.value(modelType);
				// 判断是否是object类型
				if (modelValue.type() == QJsonValue::Object) {
					// 转换为QJsonObject类型
					QJsonObject interestObj = modelValue.toObject();
					QJsonValue detValue = interestObj.value("det");
					// 判断是否是Array类型
					if (detValue.type() == QJsonValue::Object) {
						QJsonObject detObj = detValue.toObject();
						// 根据建获取值
						QJsonValue scoreThreshValue = detObj.value("box_thresh");
						ui.doubleSpinBox_detdbthresh->setValue(scoreThreshValue.toDouble());
						QJsonValue iouThreshValue = detObj.value("unclip_thresh");
						ui.doubleSpinBox_unclipratio->setValue(iouThreshValue.toDouble());
					}
				}
			}
		}
	}
	return 0;
}

void QIDIOCRWindow::QIDIinit() {

	//if (config_path == "") {
	//	config_path = "./testocr/model.json";//默认值
	//	runParams.modelPath = "./testocr/";
	//	GPUmodel_paths.push_back("./testocr/ocr_det.engine");
	//	GPUmodel_paths.push_back("./testocr/ocr_rec.engine");
	//	CPUmodel_paths.push_back("./testocr/ocr_det.m");
	//	CPUmodel_paths.push_back("./testocr/ocr_rec.m");
	//}
	//else {
	//	config_path = ui.txt_path->text().toStdString();
	//}
	//if (Readconfig(config_path) != 0) {
	//	return;
	//}

	//is.open(config_path.c_str(), ios::app);
	//if (!reader.parse(is, jsonValue)) {
	//	getmessage("模型读取失败！");
	//	return;
	//}
	//else {
	//	inferValue = jsonValue["inference"][0];  // inference字段为数组，取第0位，后续取值均类似写法
	//}
	//is.close();
	//string save_image_root;
	//int init_flag = 0;
	//if (getGPUnumber() == 0) {
	//	//init_flag = quectel_infer.initial_inference(inferValue, CPUmodel_paths, save_image_root);
	//	ui.radioButton_CPU->setChecked(true);
	//	runParams.isCpu = true;
	//	ui.radioButton_GPU->setEnabled(false);
	//}
	//else {
	//	if (ui.radioButton_GPU->isChecked() == true) {
	//		//init_flag = quectel_infer.initial_inference(inferValue, GPUmodel_paths, save_image_root);
	//		runParams.isCpu = false;
	//	}
	//	else {
	//		//init_flag = quectel_infer.initial_inference(inferValue, CPUmodel_paths, save_image_root);
	//		runParams.isCpu = true;
	//	}
	//	ui.radioButton_GPU->setEnabled(true);
	//}

	//if (init_flag < 0) {
	//	getmessage("模型初始化失败！");
	//	return;
	//}
	//else {
	//	getmessage("模型初始化完成！");
	//}
}

//图像自适应窗口
void QIDIOCRWindow::pBtn_FitImage() {
	//if (ho_Image.Key() == nullptr) {
	//	//图像为空，或者图像被clear
	//	QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
	//	return;
	//}
	//showImg(ho_Image, "margin", "blue");
	//if (ho_SearchRegions.size() > 0) {
	//	for (int i = 0; i < ho_SearchRegions.size(); i++) {
	//		SetDraw(m_hHalconID, "margin");
	//		DispObj(ho_SearchRegions[i], m_hHalconID);

	//	}
	//}
	//if (hv_resultRegions.size() > 0) {
	//	for (int i = 0; i < hv_resultRegions.size(); i++) {
	//		SetDraw(m_hHalconID, "fill");
	//		SetRgba(m_hHalconID, 255, 0, 0, 100);
	//		DispObj(hv_resultRegions[i], m_hHalconID);
	//	}
	//}
}

//是否显示图形复选框
void QIDIOCRWindow::on_ckb_ShowObj_stateChanged(int arg1) {
	//if (arg1 == Qt::Checked) {
	//	isShowObj = true;
	//}
	//else {
	//	isShowObj = false;
	//}
	////给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	//GiveParameterToWindow();
}

//清空窗口
void QIDIOCRWindow::btn_ClearWindow() {
	//if (ho_Image.Key() == nullptr) {
	//	//图像为空，或者图像被clear
	//	QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
	//	return;
	//}
	////清空窗口显示的图形变量
	//GenEmptyObj(&ho_ShowObj);
	////显示原图
	//showImg(ho_Image, "margin", "green");
}

void QIDIOCRWindow::pushButton_saveQIDI() {

	/*if (runParams.ho_SearchRegion.Key() != nullptr) {
		HTuple width, height, ratio;
		HeightWidthRatio(runParams.ho_SearchRegion, &height, &width, &ratio);
		int height1 = height.I();
		int width1 = width.I();
		if (height1 > 0 && height1 < 48 && width1>0 && width1 < 320) {
			QMessageBox::information(this, "提示", "ROI宽度小于320或者高度小于48，无法进行保存", QMessageBox::Ok);
			return;
		}
	}
	SetRunParams_QIDI();
	dataIOC.WriteParams_OCR(runParams, processId, nodeName, processModbuleID);
	qlog->writeLog(logType::INFO, "save window config");
	refreshConfig(processId, processModbuleID, nodeName);
	int result = QIDIOCRDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
	if (result < 0) {
		getmessage("模型初始化失败！");
		qlog->writeLog(logType::ERROR1, "error in save window config");
		return;
	}
	else {
		getmessage("模型初始化完成！");
	}*/

	//QString filePath = QFileDialog::getOpenFileName(this, tr("选择图片"), ".", tr("Image Files(*.jpg *.jpeg *.bmp *.png)"));
	//std::string str = filePath.toStdString();     // 中文路径也可读取
	//if (str != "") {
	//	try {
	//		ReadImage(&ho_Image, str.c_str());
	//	}
	//	catch (HException& Exception) {
	//		qDebug() << (Exception.ProcName().Text()); //实际的HALCON算子的名字
	//		getmessage(Exception.ErrorMessage().Text() + Exception.ErrorCode()); //错误信息
	//	}

	//	Matimage = HObject2Mat(ho_Image);

	//	if (Matimage.channels() == 1) {
	//		cv::cvtColor(Matimage, Matimage, cv::COLOR_GRAY2RGB);
	//	}
	//	Matimage.copyTo(displyImage);
	//	Matimage.copyTo(oriimage);
	//	GenEmptyObj(&ho_ShowObj);
	//	showImg(ho_Image, "margin", "blue");
	//	if (ho_SearchRegions.size() != 0) {
	//		UnionRoi_Template();
	//	}
	//}
	//QString filePath = QFileDialog::getExistingDirectory(this, "请选择图片路径…", "./");
	//if (filePath.size() == 0)
	//{
	//	getmessage("未选择任何文件！");
	//	return;
	//}
	//QDir d(filePath);
	//d.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
	//d.setSorting(QDir::Time | QDir::DirsFirst);
	//QFileInfoList list = d.entryInfoList();
	//for (int i = 0; i < list.size(); i++)
	//{
	//	if (list.at(i).isFile())
	//	{
	//		if (list.at(i).suffix() == "bmp" | list.at(i).suffix() == "png" | list.at(i).suffix() == "jpg" | list.at(i).suffix() == "jpeg")
	//		{
	//			imagelist.append(filePath + "/" + list.at(i).fileName());
	//		}
	//	}
	//}
	//imageindex = 0;
	//if (imagelist.size() > 0)
	//{
	//	QString filePath = imagelist.at(imageindex).filePath();
	//	std::string str = filePath.toStdString();
	//	if (str != "")
	//	{
	//		ReadImage(&ho_Image, str.c_str());
	//		Matimage = HObject2Mat(ho_Image);
	//		if (Matimage.channels() == 1)
	//		{
	//			cv::cvtColor(Matimage, Matimage, cv::COLOR_GRAY2RGB);
	//		}

	//		GenEmptyObj(&ho_ShowObj);
	//		showImg(ho_Image, "margin", "green");
	//	}
	//}
}

void QIDIOCRWindow::btn_DrawRoi_QIDI() {
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "blue");
	SetLineWidth(m_hHalconID, 2);
	HObject TempRoi;
	HTuple RecRow1, RecCol1, RecRow2, RecCol2;
	if (isDrawRoi) {
		QMessageBox::information(this, "提示", "正在绘制中，请先鼠标右键结束后操作!", QMessageBox::Ok);
		return;
	}
	isDrawRoi = true;
	DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
	isDrawRoi = false;
	GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
	ho_SearchRegions.push_back(TempRoi);
	ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	GiveParameterToWindow();
	DispObj(TempRoi, m_hHalconID);

	UnionRoi_Template();
}

void  QIDIOCRWindow::btn_ClearRoi_QIDI() {
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	oriimage.copyTo(Matimage);
	ho_SearchRegions.clear();
	GenEmptyObj(&SearchRoi_Template);
	SearchRoi_Template.Clear();
	GenEmptyObj(&ho_SearchRegion);
	ho_SearchRegion.Clear();
	GenEmptyObj(&runParams.ho_SearchRegion);
	runParams.ho_SearchRegion.Clear();
	//清空窗口显示的图形变量
	GenEmptyObj(&ho_ShowObj);
	if (SearchRoi_Template.Key() != nullptr) {
		ConcatObj(ho_ShowObj, SearchRoi_Template, &ho_ShowObj);
	}
	oriimage.copyTo(Matimage);
	showImg(ho_Image, "margin", "green");
}

//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
void QIDIOCRWindow::GiveParameterToWindow() {
	if (ho_Image.Key() == nullptr) {
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}

	//判断窗口句柄
	if (m_hHalconID != NULL) {
		//如果有图像，则先清空图像
		//DetachBackgroundFromWindow(m_hHalconID);
	}
	else {
		//打开窗口
		OpenWindow(0, 0, ui.lbl_Window->width(), ui.lbl_Window->height(), m_hLabelID, "visible", "", &m_hHalconID);
	}

	//设置图形显示属性
	SetDraw(m_hHalconID, "margin");
	SetColor(m_hHalconID, "blue");
	SetLineWidth(m_hHalconID, 2);
	//设置窗口显示变量
	ui.lbl_Window->setHalconWnd(ho_Image, ho_ShowObj, true, m_hHalconID, ui.lbl_Status);
}

//合并模板区域Roi
void QIDIOCRWindow::UnionRoi_Template() {
	HObject imagereduce;
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	if (ho_SearchRegions.isEmpty()) {
		QMessageBox::information(this, "提示", "区域ROI为空!", QMessageBox::Ok);
		return;
	}
	else {
		SetColor(m_hHalconID, "blue");
		SetLineWidth(m_hHalconID, 2);
		//区域合并
		if (ho_SearchRegions.count() == 1) {
			SearchRoi_Template = ho_SearchRegions[0];
		}
		else {
			SearchRoi_Template = ho_SearchRegions[0];
			for (int i = 1; i < ho_SearchRegions.count(); i++) {
				Union2(SearchRoi_Template, ho_SearchRegions[i], &SearchRoi_Template);
			}
		}
		//窗口显示图形赋值
		ConcatObj(ho_ShowObj, SearchRoi_Template, &ho_ShowObj);
		//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
		GiveParameterToWindow();
		DispObj(SearchRoi_Template, m_hHalconID);
		getmessage("搜索区域数量为:" + QString::number(ho_SearchRegions.count()));
		Union1(SearchRoi_Template, &ho_SearchRegion);
		runParams.ho_SearchRegion = ho_SearchRegion;
		try {
			//计算补集
			HObject ho_DomainRegion;
			GetDomain(ho_Image, &ho_DomainRegion);
			HObject ho_ComplementRegion;
			Difference(ho_DomainRegion, SearchRoi_Template, &ho_ComplementRegion);
			//计算通道数
			HTuple hv_CountChannels;
			CountChannels(ho_Image, &hv_CountChannels);

			if (hv_CountChannels == 1) {
				//填充0
				HalconCpp::PaintRegion(ho_ComplementRegion, ho_Image, &imagereduce, 0, "fill");
			}
			else if (hv_CountChannels == 3) {
				//分离三通道
				HObject ho_R, ho_G, ho_B;
				Decompose3(ho_Image, &ho_R, &ho_G, &ho_B);
				//填充0
				HObject ho_Reduce1, ho_Reduce2, ho_Reduce3;
				HalconCpp::PaintRegion(ho_ComplementRegion, ho_R, &ho_Reduce1, 0, "fill");
				HalconCpp::PaintRegion(ho_ComplementRegion, ho_G, &ho_Reduce2, 0, "fill");
				HalconCpp::PaintRegion(ho_ComplementRegion, ho_B, &ho_Reduce3, 0, "fill");
				//合并三通道
				HalconCpp::Compose3(ho_Reduce1, ho_Reduce2, ho_Reduce3, &imagereduce);
			}
			else {
				GenEmptyObj(&imagereduce);
				imagereduce.Clear();
				return;
			}
		}
		catch (...) {
			GenEmptyObj(&imagereduce);
			imagereduce.Clear();
			return;
		}
		Matimage = dataTranfer::HObject2Mat(imagereduce);
	}
}

void QIDIOCRWindow::QIDITest(Mat image) {
	//ResultQIDI.hv_RetNum = 0;
	//hv_resultRegions.clear();
	//ResultQIDI.hv_Row.Clear();
	//ResultQIDI.hv_Column.Clear();
	//ResultQIDI.hv_OCRContent.Clear();
	//ResultQIDI.hv_OCRLength_Result.Clear();
	//HObject imgTemp;
	//int RetNumTemp = 0;
	//string predictLabel = "";
	//bool isShow = true;
	//std::vector<Results> results;
	//cv::Mat dst_image = image.clone();
	//cv::Mat map_label(image.rows, image.cols, CV_8UC1); //output label maskmap
	//if (image.empty()) {
	//	return;
	//}
	////run inference
	//try {
	//	//quectel_infer.inference(image.data, dst_image.data, predictLabel, image.cols, image.rows, image.channels(), isShow, map_label.data, results);
	//}
	//catch (const std::exception& e) {
	//	std::cout << e.what();
	//}
	//int resultsize = results.size();
	//ResultQIDI.hv_RetNum = resultsize;
	//if (isShow) {
	//	for (int i = 0; i < results.size(); i++) {
	//		//计算字符得分
	//		if (results[i].score < runParams.thresh) {
	//			continue;
	//		}
	//		if (runParams.isdigit == true) {
	//			regex reg1("^[0-9]+$");
	//			if (!regex_match(results[i].label, reg1)) {
	//				continue;
	//			}
	//		}
	//		if (runParams.isupper == true) {
	//			regex reg2("^[A-Z]+$");
	//			if (!regex_match(results[i].label, reg2)) {
	//				continue;
	//			}
	//		}
	//		if (runParams.islower == true) {
	//			regex reg3("^[a-z]+$");
	//			if (!regex_match(results[i].label, reg3)) {
	//				continue;
	//			}
	//		}
	//		if (runParams.isSpecial == true) {

	//			regex reg4("[~!/@#$%^&*()-_=+\\|[{}];:\'\",<.>/?]+");
	//			if (!regex_match(results[i].label, reg4)) {
	//				continue;
	//			}

	//		}
	//		if (runParams.ischinese == true) {
	//			regex reg5("^[\u4e00-\u9fa5]{0,}$");
	//			if (!regex_match(results[i].label.c_str(), reg5)) {
	//				continue;
	//			}
	//		}
	//		//计算字符长度
	//		HTuple CodeLengthTemp;//计算条码长度临时变量
	//		TupleStrlen(results[i].label.c_str(), &CodeLengthTemp);
	//		if (CodeLengthTemp < runParams.hv_MinCodeLength_Run ||
	//			CodeLengthTemp > runParams.hv_MaxCodeLength_Run) {
	//			//长度不满足，退出当前循环
	//			continue;
	//		}
	//		if (runParams.hv_ContainChar != "") {
	//			string yuiy = runParams.hv_ContainChar.S().Text();
	//			//查找字符串是否包含特定字符
	//			regex reg6(".*" + yuiy + ".*");
	//			if (!regex_match(results[i].label, reg6)) {
	//				continue;
	//			}
	//		}
	//		if (runParams.hv_NotContainChar != "") {
	//			//查找字符串是否不包含特定字符
	//			string yuiyt = runParams.hv_NotContainChar.S().Text();
	//			//查找字符串是否包含特定字符
	//			regex reg7("^(?!.*" + yuiyt + ").*$");
	//			if (!regex_match(results[i].label, reg7)) {
	//				continue;
	//			}
	//		}
	//		if (runParams.hv_HeadChar != "") {
	//			//查找字符串是否特定字符开头
	//			string yuiyrt = runParams.hv_HeadChar.S().Text();
	//			//查找字符串是否包含特定字符
	//			regex reg7("^(" + yuiyrt + ").*");
	//			if (!regex_match(results[i].label, reg7)) {
	//				continue;
	//			}
	//		}
	//		if (runParams.hv_EndChar != "") {
	//			//查找字符串是否特定字符结尾
	//			string yuiyrrt = runParams.hv_EndChar.S().Text();
	//			//查找字符串是否包含特定字符
	//			regex reg8(".*" + yuiyrrt + "$");
	//			if (!regex_match(results[i].label, reg8)) {
	//				continue;
	//			}
	//		}

	//		point1 = cv::Point(results[i].pointsVector[0][0], results[i].pointsVector[0][1]);
	//		point2 = cv::Point(results[i].pointsVector[1][0], results[i].pointsVector[1][1]);
	//		point3 = cv::Point(results[i].pointsVector[2][0], results[i].pointsVector[2][1]);
	//		point4 = cv::Point(results[i].pointsVector[3][0], results[i].pointsVector[3][1]);

	//		QImage image1 = Mat2QImage(displyImage);
	//		//图片绘制文字代码
	//		QPixmap pixmap = QPixmap::fromImage(image1);
	//		QPainter p(&pixmap);
	//		QFont font1("微软雅黑", QFont::Bold, true);
	//		font1.setPixelSize(image1.height() / 50);//设置字体大小
	//		font1.setLetterSpacing(QFont::AbsoluteSpacing, 5);//设置字符间距
	//		font1.setPointSize(image1.height() / 50);
	//		p.setFont(font1);//使用字体

	//		p.setPen(QPen(Qt::green, image1.height() / 200));
	//		p.drawLine(QPoint(point1.x, point1.y), QPoint(point2.x, point2.y));
	//		p.drawLine(QPoint(point3.x, point3.y), QPoint(point2.x, point2.y));
	//		p.drawLine(QPoint(point3.x, point3.y), QPoint(point4.x, point4.y));
	//		p.drawLine(QPoint(point4.x, point4.y), QPoint(point1.x, point1.y));

	//		p.setPen(QPen(Qt::red));//设置画笔颜色
	//		p.drawText(results[i].cx - results[i].width / 2, results[i].cy - results[i].height / 2 - 10, QString::fromLocal8Bit(results[i].label.c_str()));
	//		//转Mat
	//		QImage imgTemp2 = pixmap.toImage();
	//		displyImage = QImage2Mat(imgTemp2).clone();

	//		ResultQIDI.hv_Column.Append(results[i].cx);
	//		ResultQIDI.hv_Row.Append(results[i].cy);
	//		ResultQIDI.hv_OCRLength_Result.Append(CodeLengthTemp);
	//		ResultQIDI.hv_OCRContent.Append(results[i].label.c_str());
	//		ResultQIDI.hv_OCRScore_Result.Append(results[i].score);
	//		RetNumTemp += 1;
	//	}

	//}
	//ResultQIDI.hv_RetNum = RetNumTemp;//找到的字符个数
}


//QImage转Mat
cv::Mat QIDIOCRWindow::QImage2Mat(const QImage image) {
	if (image.isNull()) {
		return cv::Mat();
	}

	cv::Mat mat;
	switch (image.format()) {
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
		break;
	case QImage::Format_Grayscale8:
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

//Mat转QImage
QImage QIDIOCRWindow::Mat2QImage(const cv::Mat& mat) {
	if (mat.empty()) return QImage();
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_16UC1) {
		//qDebug() << "CV_16UC1转换";
		cv::Mat mat_8;
		mat.convertTo(mat_8, CV_8UC1, 255.0 / 4095.0);
		// 16位数据（2的16次方65535）归一化到8位数据（2的8次方）
		//mat.convertTo(mat_8, CV_8UC1,1/65535.0*255.0); 
		//mat.convertTo(mat_8, CV_8UC1,1/4096*255.0);

		QImage image(mat_8.cols, mat_8.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++) {
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar* pSrc = mat_8.data;
		for (int row = 0; row < mat_8.rows; row++) {
			uchar* pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat_8.cols);
			pSrc += mat_8.step;
		}
		mat_8.release();
		return image;
	}
	if (mat.type() == CV_8UC1) {
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++) {
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar* pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++) {
			uchar* pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3) {
		// Copy input Mat
		const uchar* pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4) {
		// Copy input Mat
		const uchar* pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else {
		return QImage();
	}
}

void QIDIOCRWindow::btn_FindCharacter() {
	/*if (imageindex >= 0 && imageindex < imagelist.size() - 1)
	{
		QString filePath = imagelist.at(imageindex + 1).filePath();
		std::string str = filePath.toStdString();
		if (str != "")
		{
			ReadImage(&ho_Image, str.c_str());
			Matimage = HObject2Mat(ho_Image);
			if (Matimage.channels() == 1)
			{
				cv::cvtColor(Matimage, Matimage, cv::COLOR_GRAY2RGB);
			}

			GenEmptyObj(&ho_ShowObj);
			showImg(ho_Image, "margin", "green");
		}
		imageindex++;
	}*/


	Matimage = dataTranfer::HObject2Mat(ho_Image);

	if (Matimage.channels() == 1) {
		cv::cvtColor(Matimage, Matimage, cv::COLOR_GRAY2RGB);
	}
	Matimage.copyTo(displyImage);
	Matimage.copyTo(oriimage);
	GenEmptyObj(&ho_ShowObj);
	showImg(ho_Image, "margin", "blue");
	if (ho_SearchRegions.size() != 0) {
		UnionRoi_Template();
	}

	if (Matimage.empty()) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}

	pushButton_saveQIDI();
	QDateTime startTime = QDateTime::currentDateTime();

	//QIDITest(Matimage);
	QIDIOCRDetect1->QIDIOCRFun(ho_Image, ResultQIDI);
	//std::vector<Results> results = QIDIOCRDetect1->getResultOCR();

	////绘制结果
	//for (int i = 0; i < results.size(); i++) {
	//	//计算字符得分
	//	if (results[i].score < runParams.thresh) {
	//		continue;
	//	}
	//	if (runParams.isdigit == true) {
	//		regex reg1("^[0-9]+$");
	//		if (!regex_match(results[i].label, reg1)) {
	//			continue;
	//		}
	//	}
	//	if (runParams.isupper == true) {
	//		regex reg2("^[A-Z]+$");
	//		if (!regex_match(results[i].label, reg2)) {
	//			continue;
	//		}
	//	}
	//	if (runParams.islower == true) {
	//		regex reg3("^[a-z]+$");
	//		if (!regex_match(results[i].label, reg3)) {
	//			continue;
	//		}
	//	}
	//	if (runParams.isSpecial == true) {

	//		regex reg4("[~!/@#$%^&*()-_=+\\|[{}];:\'\",<.>/?]+");
	//		if (!regex_match(results[i].label, reg4)) {
	//			continue;
	//		}

	//	}
	//	if (runParams.ischinese == true) {
	//		regex reg5("^[\u4e00-\u9fa5]{0,}$");
	//		if (!regex_match(results[i].label.c_str(), reg5)) {
	//			continue;
	//		}
	//	}
	//	//计算字符长度
	//	HTuple CodeLengthTemp;//计算条码长度临时变量
	//	TupleStrlen(results[i].label.c_str(), &CodeLengthTemp);
	//	if (CodeLengthTemp < runParams.hv_MinCodeLength_Run ||
	//		CodeLengthTemp > runParams.hv_MaxCodeLength_Run) {
	//		//长度不满足，退出当前循环
	//		continue;
	//	}
	//	if (runParams.hv_ContainChar != "") {
	//		string yuiy = runParams.hv_ContainChar.S().Text();
	//		//查找字符串是否包含特定字符
	//		regex reg6(".*" + yuiy + ".*");
	//		if (!regex_match(results[i].label, reg6)) {
	//			continue;
	//		}
	//	}
	//	if (runParams.hv_NotContainChar != "") {
	//		//查找字符串是否不包含特定字符
	//		string yuiyt = runParams.hv_NotContainChar.S().Text();
	//		//查找字符串是否包含特定字符
	//		regex reg7("^(?!.*" + yuiyt + ").*$");
	//		if (!regex_match(results[i].label, reg7)) {
	//			continue;
	//		}
	//	}
	//	if (runParams.hv_HeadChar != "") {
	//		//查找字符串是否特定字符开头
	//		string yuiyrt = runParams.hv_HeadChar.S().Text();
	//		//查找字符串是否包含特定字符
	//		regex reg7("^(" + yuiyrt + ").*");
	//		if (!regex_match(results[i].label, reg7)) {
	//			continue;
	//		}
	//	}
	//	if (runParams.hv_EndChar != "") {
	//		//查找字符串是否特定字符结尾
	//		string yuiyrrt = runParams.hv_EndChar.S().Text();
	//		//查找字符串是否包含特定字符
	//		regex reg8(".*" + yuiyrrt + "$");
	//		if (!regex_match(results[i].label, reg8)) {
	//			continue;
	//		}
	//	}

	//	point1 = cv::Point(results[i].pointsVector[0][0], results[i].pointsVector[0][1]);
	//	point2 = cv::Point(results[i].pointsVector[1][0], results[i].pointsVector[1][1]);
	//	point3 = cv::Point(results[i].pointsVector[2][0], results[i].pointsVector[2][1]);
	//	point4 = cv::Point(results[i].pointsVector[3][0], results[i].pointsVector[3][1]);

	//	QImage image1 = Mat2QImage(displyImage);
	//	//图片绘制文字代码
	//	QPixmap pixmap = QPixmap::fromImage(image1);
	//	QPainter p(&pixmap);
	//	QFont font1("微软雅黑", QFont::Bold, true);
	//	font1.setPixelSize(image1.height() / 50);//设置字体大小
	//	font1.setLetterSpacing(QFont::AbsoluteSpacing, 5);//设置字符间距
	//	font1.setPointSize(image1.height() / 50);
	//	p.setFont(font1);//使用字体

	//	p.setPen(QPen(Qt::green, image1.height() / 200));
	//	p.drawLine(QPoint(point1.x, point1.y), QPoint(point2.x, point2.y));
	//	p.drawLine(QPoint(point3.x, point3.y), QPoint(point2.x, point2.y));
	//	p.drawLine(QPoint(point3.x, point3.y), QPoint(point4.x, point4.y));
	//	p.drawLine(QPoint(point4.x, point4.y), QPoint(point1.x, point1.y));

	//	p.setPen(QPen(Qt::red));//设置画笔颜色
	//	p.drawText(results[i].cx - results[i].width / 2, results[i].cy - results[i].height / 2 - 10, QString::fromLocal8Bit(results[i].label.c_str()));
	//	//转Mat
	//	QImage imgTemp2 = pixmap.toImage();
	//	displyImage = QImage2Mat(imgTemp2).clone();
	//}


	HObject SHOWIMAGE;
	SHOWIMAGE = dataTranfer::Mat2HObject(displyImage);
	//ho_ShowObj = ResultQIDI.ho_DestRegions;
	showImg(SHOWIMAGE, "margin", "green");
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	//刷新结果
	SetTableData_QIDI(ResultQIDI, intervalTimeMS);

}

//设置结果显示表格数据
void QIDIOCRWindow::SetTableData_QIDI(const ResultParamsStruct_OCR& resultStruct, qint64 Ct) {
	//清空结果表格数据
	ui.tablewidget_Results_QIDI->clearContents();
	//判断是否找到缺陷
	if (resultStruct.hv_RetNum == 0) {
		//没有找到缺陷，退出
		GenEmptyObj(&ho_ShowObj);
		showImg(ho_Image, "margin", "green");
		return;
	}
	//刷新时间
	getmessage("耗时:" + QString::number(Ct) + "ms");

	//刷新所有找到的缺陷信息
	std::vector<std::map<int, std::string>> resultList = QIDIOCRDetect1->getModubleResultList();
	int count = resultList.size();

	ui.tablewidget_Results_QIDI->setRowCount(count);
	for (int i = 0; i < count; i++) {
		QTableWidgetItem* item;
		for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++) {
			item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_Results_QIDI->setItem(i, iter->first, item);
		}
	}
}
void QIDIOCRWindow::on_tablewidget_Results_QIDI_cellClicked(int row, int column) {
	int SelectRow = row;
	if (SelectRow < 0) {
		return;
	}
	//刷新结果区域
	if (hv_resultRegions.size() != 0) {
		//HObject ho_ObjTemp;
		//SelectObj(hv_resultRegions[SelectRow], &ho_ObjTemp, SelectRow + 1);
		////先显示所有区域
		//GenEmptyObj(&ho_ShowObj);
		//ConcatObj(ho_ShowObj, hv_resultRegions[SelectRow], &ho_ShowObj);
		//showImg(resultResion, "fill", "green");//margin
		if (hv_resultRegions.size() > 0) {
			for (int i = 0; i < hv_resultRegions.size(); i++) {
				SetDraw(m_hHalconID, "fill");
				SetColor(m_hHalconID, "red");
				SetRgba(m_hHalconID, 255, 0, 0, 100);
				DispObj(hv_resultRegions[i], m_hHalconID);

			}

		}
		//再显示选择区域
		SetColor(m_hHalconID, "green");
		DispObj(hv_resultRegions[SelectRow], m_hHalconID);
	}
}
void QIDIOCRWindow::sortItems(int column) {
	//if (sortUpDown)		//sortUpDown为自定义的一个bool变量，表示升序还是降序
	//{
	//	ui.tablewidget_Results_QIDI->sortItems(column, Qt::AscendingOrder);
	//	sortUpDown = false;
	//}
	//else
	//{
	//	ui.tablewidget_Results_QIDI->sortItems(column, Qt::DescendingOrder);
	//	sortUpDown = true;
	//}
}


int QIDIOCRWindow::WriteTxt(string content) {
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

#include <iostream>
#include <string>
// 包含跨平台文件操作的头文件
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

void QIDIOCRWindow::PathSet(const std::string ConfigPath, const std::string XmlPath) {
	XmlClass2 xmlC; // xml存取变量

	// 判定目录或文件是否存在的标识符
	int mode = 0;
#ifdef _WIN32
	if (_access(ConfigPath.c_str(), mode)) {
#else
	if (access(ConfigPath.c_str(), F_OK)) {
#endif
		// 在Linux下创建目录需要指定权限模式
#ifdef _WIN32
		_mkdir(ConfigPath.c_str());
#else
		mkdir(ConfigPath.c_str(), 0755); // 使用755权限：rwxr-xr-x
#endif
	}

	// 创建XML(存在就不会创建)
	int Ret = xmlC.CreateXML(XmlPath);
	if (Ret != 0) {
		WriteTxt("打开参数配置xml文件失败");
		// mutex1.unlock();
		return;
	}
	}
#endif