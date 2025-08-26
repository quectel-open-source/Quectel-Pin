#pragma execution_character_set("utf-8")
#include "PinWindow.h"

PinWindow::PinWindow(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	//this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	//窗口初始化
	InitWindow();
	ui.frame->hide();


	PinDetect1 = new PinDetect();
	//processId = 1;
	//processModbuleID = 2;
	//PinDetect1 = new PinDetect(nodeName, processModbuleID, processId);
	dllDatas1 = new DllDatas();
	IniTableData_Standard();

	GenEmptyObj(&RunParams_Pin.ho_SearchRegion);
	RunParams_Pin.ho_SearchRegion.Clear();
	GenEmptyObj(&BlobRegionShow);
	BlobRegionShow.Clear();
	////开机自动读图
	//QString Filepath = QCoreApplication::applicationDirPath() + "/03.png";
	//ReadImage(&ho_Image, HTuple(Filepath.toStdString().c_str()));

	//配置文件和XML路径
	ConfigPath = FolderPath() + "/config/";
	XmlPath = FolderPath() + "/config/config.xml";

	ui.ckb_PositionCorrection->setCheckState(Qt::Unchecked);
	ui.pb_SearchROI_PositionCorrection->setEnabled(false);
	//ui.pb_SetData_PositionCorrection->setEnabled(false);
	ui.pb_Run_PositionCorrection->setEnabled(false);
}

PinWindow::PinWindow(HObject image, int _processID, int modubleID, std::string _modubleName
	, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig, std::string getConfigPath) {
	//ui的初始化先屏蔽
	//ui.setupUi(this);

	//setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processId = _processID;

	//ui.ckb_PositionCorrection->setCheckState(Qt::Unchecked);
	//ui.pb_SearchROI_PositionCorrection->setEnabled(false);
	////ui.pb_SetData_PositionCorrection->setEnabled(false);
	//ui.pb_Run_PositionCorrection->setEnabled(false);
	//ui.frame->hide();

	refreshConfig = _refreshConfig;

	////窗口初始化
	//InitWindow();
	PinDetect1 = new PinDetect(nodeName, processModbuleID, processId);
	dllDatas1 = new DllDatas();
	dllDatas1->setGetConfigPath(getConfigPath);
	//ui的初始化先屏蔽
	//IniTableData_Standard();

	//GenEmptyObj(&RunParams_Pin.ho_SearchRegion);
	//RunParams_Pin.ho_SearchRegion.Clear();
	//GenEmptyObj(&BlobRegionShow);
	//BlobRegionShow.Clear();


	if (image.Key() != nullptr)
		ho_Image = image;

	//暂时不需要
	//showImg(ho_Image, "margin", "green");
	//qlog = new QLog(processId, processModbuleID, nodeName);
	//qlog->setPriority(logType::INFO);
	//qlog->writeLog(logType::INFO, "open window");
}

PinWindow::~PinWindow() {
	if(PinDetect1)
		delete PinDetect1;
	if (qlog)
		delete qlog;
}
void PinWindow::on_toolButton_clicked() {
	PinWindow::showMinimized(); //最小化
}

void PinWindow::on_toolButton_2_clicked() {
	if (PinWindow::isMaximized()) {
		PinWindow::showNormal();//还原事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		PinWindow::showMaximized();//最大化事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void PinWindow::on_toolButton_3_clicked() {
	this->close();
}

void PinWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void PinWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//当窗口最大化或最小化时也不进行触发
		if (PinWindow::isMaximized() || PinWindow::isMinimized()) {
			return;
		}
		else {
			//当在按钮之类需要鼠标操作的地方不进行触发(防误触)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() ||
				ui.lbl_Window->underMouse() || ui.cmb_SearchRegion->underMouse() || ui.cmb_ShapeSelect->underMouse()
				|| ui.cmb_PinDetectType->underMouse() || ui.cmb_PinSortType->underMouse()
				|| ui.cmb_Type_Binarization->underMouse() || ui.cmb_OperatorType->underMouse()
				|| ui.pb_SearchROI_PositionCorrection->underMouse() || ui.cmb_SortType->underMouse() || ui.cmb_ShapeSelect_PositionCorrection->underMouse()
				|| ui.cmb_Polarity->underMouse() || ui.cmb_MatchMethod_Train->underMouse() || ui.cmb_MatchMethod_Train_2->underMouse()
				|| ui.cmb_BaseSelect->underMouse() || ui.cmb_BaseType->underMouse()
				|| ui.cmb_MeasureTransition->underMouse() || ui.cmb_MeasureSelect->underMouse()
				|| ui.cmb_PinRegionBinding->underMouse() || ui.cmb_BaseXBinding->underMouse()
				|| ui.cmb_BaseYBinding->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//PinWindow::move(PinWindow::mapToGlobal(event->pos() - whereismouse));//移动
			}
		}
	}
}
void PinWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

void PinWindow::resizeEvent(QResizeEvent* event) {
	try {
		// 在这里处理窗口大小改变事件
	//设置halcon的文件路径为utf8，解决中文乱码
		SetSystem("filename_encoding", "utf8");
		//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
		ui.lbl_Window->setMouseTracking(true);
		m_hHalconID = NULL;
		m_hLabelID = (Hlong)ui.lbl_Window->winId();
		on_pBtn_FitImage_clicked();
	}
	catch (...) {

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

	if (ho_Image.Key() != nullptr)
	{
		showImg(ho_Image, "margin", "green");
	}
}
//初始化显示窗口
void PinWindow::InitWindow() {
	try {
		//设置halcon的文件路径为utf8，解决中文乱码
		SetSystem("filename_encoding", "utf8");
		//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
		ui.lbl_Window->setMouseTracking(true);
		//生成空图像
		GenEmptyObj(&ho_Image);
		ho_Image.Clear();
		GenEmptyObj(&ho_ShowObj);
		GenEmptyObj(&BlobRegionShow);
		BlobRegionShow.Clear();
		GenEmptyObj(&BaseContourShow);
		BaseContourShow.Clear();
		vecTemplateRegions.clear();
		vecShadowRegions.clear();

		m_hHalconID = NULL;
		m_hLabelID = (Hlong)ui.lbl_Window->winId();
	}
	catch (...) {

	}

}

//显示图像
void PinWindow::showImg(const HObject& ho_Img, HTuple hv_FillType, HTuple hv_Color) {
	try {
		if (ho_Img.Key() == nullptr) {
			//图像为空，或者图像被clear
			//QMessageBox::information(this, "提示"), "图像是空，请先加载图像!"), QMessageBox::Ok);
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetSystem("flush_graphic", "false");
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
							//AttachBackgroundToWindow(ho_Img, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(ho_Img, m_hHalconID);

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
	catch (...) {
		QMessageBox::information(this, "提示", "窗口图像显示刷新失败!", QMessageBox::Ok);
		return;
	}

}

//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
void PinWindow::GiveParameterToWindow() {
	try {
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
		SetLineWidth(m_hHalconID, 1);
		//设置窗口显示变量
		ui.lbl_Window->setHalconWnd(ho_Image, ho_ShowObj, isShowObj, m_hHalconID, ui.lbl_Status);
	}
	catch (...) {

	}
}

//打开图片
void PinWindow::on_pBtn_ReadImage_clicked() {
	//QFileDialog dlg;
	//dlg.setAcceptMode(QFileDialog::AcceptOpen);
	//dlg.setFileMode(QFileDialog::ExistingFile);
	////加文件过滤器 erik
	//QStringList filters;
	//filters << "Image Files (*.jpg *.jpeg *.bmp *.png)";
	//dlg.setNameFilters(filters);

	QFileDialog dlg;
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setFileMode(QFileDialog::ExistingFile);

	// 设置文件过滤器
	QStringList filters;
	filters << "Image Files (*.jpg *.jpeg *.bmp *.png)";
	dlg.setNameFilters(filters);

	// 构建默认路径：应用目录下的 images/Original
	QString defaultPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/images/Original");

	// 检查路径是否存在，不存在则创建
	QDir dir(defaultPath);
	if (!dir.exists()) {
		// 回退到用户文档目录
		defaultPath = QCoreApplication::applicationDirPath();
	}

	// 设置对话框默认路径
	dlg.setDirectory(defaultPath);

	// 显示对话框并等待用户选择
	if (dlg.exec() == QDialog::Accepted) {
		QString filePath = dlg.selectedFiles().first();
		//QString filePath = dlg.getOpenFileName();
		std::string str1 = filePath.toStdString();
		const char* str = str1.c_str();
		HTuple h_FilePath(str);
		if (!filePath.isEmpty()) {
			ReadImage(&ho_Image, h_FilePath);
			if (ui.checkBox_Gray->isChecked())
			{
				//勾选了灰度处理
				Rgb1ToGray(ho_Image, &ho_Image);
			}
			GenEmptyObj(&ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
		}
	}

	
}

//图像自适应窗口
void PinWindow::on_pBtn_FitImage_clicked() {
	if (ho_Image.Key() != nullptr) {
		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}
	}
}
//是否显示图形复选框
void PinWindow::on_ckb_ShowObj_stateChanged(int arg1) {
	if (arg1 == Qt::Checked) {
		isShowObj = true;
	}
	else {
		isShowObj = false;
	}
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	GiveParameterToWindow();
}
//清空窗口
void PinWindow::on_btn_ClearWindow_clicked() {
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	//清空窗口显示的图形变量
	GenEmptyObj(&ho_ShowObj);
	//显示原图
	if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
		showImg(ho_Image, "fill", "green");
	}
	else {
		showImg(ho_Image, "margin", "green");
	}
}
//绘制搜索区域
void PinWindow::on_pb_DrawRoi_clicked() {
	try {
		ui.lbl_Window->SetActionEnable(false);
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		//判断搜索区域是否为空
		int count = ui.cmb_SearchRegion->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请选择搜索区域编号!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		//当前绘制的区域数组里面的下标值
		int index1 = ui.cmb_SearchRegion->currentIndex();
		//绘制ROI
		HObject TempRoi;
		if (ui.cmb_ShapeSelect->currentText() == "矩形") {
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "仿射矩形") {
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&TempRoi, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "圆形") {
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&TempRoi, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "椭圆") {
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&TempRoi, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "多边形") {
			DrawRegion(&TempRoi, m_hHalconID);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else {
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
			return;
		}

		//判断输入参数结构体是否为空
		TypeParamsStruct_Pin m_type;
		HTuple regionNum;
		CountObj(RunParams_Pin.ho_SearchRegion, &regionNum);
		count = regionNum.TupleLength().I();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "搜索区域为空!", QMessageBox::Ok);
			return;
		}

		//区域检测类型
		if (ui.cmb_PinDetectType->currentText() == "Blob") {
			RunParams_Pin.hv_DetectType[index1] = m_type.DetectType_Blob;
		}
		else {
			RunParams_Pin.hv_DetectType[index1] = m_type.DetectType_Template;
		}

		//替换指定索引的搜索区域
		HObject replaceRegion;
		ReplaceObj(RunParams_Pin.ho_SearchRegion, TempRoi, &replaceRegion, index1 + 1);
		RunParams_Pin.ho_SearchRegion = replaceRegion;

		//二值化参数赋值
		if (ui.cmb_Type_Binarization->currentText().toStdString() == "固定阈值") {
			RunParams_Pin.hv_ThresholdType[index1] = m_type.Fixed_Threshold;
		}
		else if (ui.cmb_Type_Binarization->currentText().toStdString() == "自适应阈值") {
			RunParams_Pin.hv_ThresholdType[index1] = m_type.Auto_Threshold;
		}
		else if (ui.cmb_Type_Binarization->currentText().toStdString() == "动态阈值") {
			RunParams_Pin.hv_ThresholdType[index1] = m_type.Dyn_Threshold;
		}
		else {
			QMessageBox::information(this, "提示", "请选择二值化类型!", QMessageBox::Ok);
			return;
		}
		RunParams_Pin.hv_LowThreshold[index1] = ui.spb_LowThreshold->value();
		RunParams_Pin.hv_HighThreshold[index1] = ui.spb_HighThreshold->value();
		RunParams_Pin.hv_Sigma[index1] = ui.dspb_Sigma->value();
		RunParams_Pin.hv_CoreWidth[index1] = ui.dspb_DynThresholdWidth->value();
		RunParams_Pin.hv_CoreHeight[index1] = ui.dspb_DynThresholdHeight->value();
		RunParams_Pin.hv_DynThresholdContrast[index1] = ui.spb_DynThresholdContrast->value();

		if (ui.cmb_Polarity->currentText().toStdString() == "dark") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Dark;
		}
		else if (ui.cmb_Polarity->currentText().toStdString() == "light") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Light;
		}
		else if (ui.cmb_Polarity->currentText().toStdString() == "equal") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Equal;
		}
		else if (ui.cmb_Polarity->currentText().toStdString() == "not_equal") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_NotEqual;
		}
		else {
			//默认找白色区域
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Light;
		}
		//形态学参数赋值
		if (ui.cmb_OperatorType->currentText().toStdString() == "矩形开运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Opening_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "矩形闭运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Closing_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形开运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Opening_Cir;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形闭运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Closing_Cir;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "矩形腐蚀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Erosion_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "矩形膨胀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Dilation_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形腐蚀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Erosion_Cir;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形膨胀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Dilation_Cir;
		}
		else {
			RunParams_Pin.hv_OperatorType[index1] = m_type.hv_Null;
		}
		RunParams_Pin.hv_OperaRec_Width[index1] = ui.dsb_OperatorWidth->value();//数组
		RunParams_Pin.hv_OperaRec_Height[index1] = ui.dsb_OperatorHeight->value();//数组
		RunParams_Pin.hv_OperaCir_Radius[index1] = ui.dsb_OperatorRadius->value();//数组
		//孔洞填充
		RunParams_Pin.hv_FillUpShape_Min[index1] = ui.dspb_FillUpShape_Min->value();//数组
		RunParams_Pin.hv_FillUpShape_Max[index1] = ui.dspb_FillUpShape_Max->value();//数组
		//Blob筛选参数赋值
		RunParams_Pin.hv_AreaFilter_Min[index1] = ui.dspb_AreaFilter_Min->value();
		RunParams_Pin.hv_AreaFilter_Max[index1] = ui.dspb_AreaFilter_Max->value();
		RunParams_Pin.hv_RowFilter_Min[index1] = ui.dspb_RowFilter_Min->value();
		RunParams_Pin.hv_RowFilter_Max[index1] = ui.dspb_RowFilter_Max->value();
		RunParams_Pin.hv_ColumnFilter_Min[index1] = ui.dspb_ColumnFilter_Min->value();
		RunParams_Pin.hv_ColumnFilter_Max[index1] = ui.dspb_ColumnFilter_Max->value();
		RunParams_Pin.hv_RecLen1Filter_Min[index1] = ui.dspb_RecLen1Filter_Min->value();
		RunParams_Pin.hv_RecLen1Filter_Max[index1] = ui.dspb_RecLen1Filter_Max->value();
		RunParams_Pin.hv_RecLen2Filter_Min[index1] = ui.dspb_RecLen2Filter_Min->value();
		RunParams_Pin.hv_RecLen2Filter_Max[index1] = ui.dspb_RecLen2Filter_Max->value();
		RunParams_Pin.hv_CircularityFilter_Min[index1] = ui.dspb_CircularityFilter_Min->value();
		RunParams_Pin.hv_CircularityFilter_Max[index1] = ui.dspb_CircularityFilter_Max->value();
		RunParams_Pin.hv_RectangularityFilter_Min[index1] = ui.dspb_RectangularityFilter_Min->value();
		RunParams_Pin.hv_RectangularityFilter_Max[index1] = ui.dspb_RectangularityFilter_Max->value();
		RunParams_Pin.hv_WidthFilter_Min[index1] = ui.dspb_WidthFilter_Min->value();
		RunParams_Pin.hv_WidthFilter_Max[index1] = ui.dspb_WidthFilter_Max->value();
		RunParams_Pin.hv_HeightFilter_Min[index1] = ui.dspb_HeightFilter_Min->value();
		RunParams_Pin.hv_HeightFilter_Max[index1] = ui.dspb_HeightFilter_Max->value();
		//是否选择区域内面积最大值
		if (ui.ckb_SelectAreaMax->checkState() == Qt::Checked) {
			RunParams_Pin.hv_SelectAreaMax[index1] = 1;
		}
		else {
			RunParams_Pin.hv_SelectAreaMax[index1] = 0;
		}
		RunParams_Pin.hv_BlobCount[index1] = ui.spb_BlobCount->value();

		//排序方式
		if (ui.cmb_PinSortType->currentText() == "按行排序") {
			RunParams_Pin.hv_SortType[index1] = m_type.SortType_Row;
		}
		else if (ui.cmb_PinSortType->currentText() == "按列排序") {
			RunParams_Pin.hv_SortType[index1] = m_type.SortType_Column;
		}
		else {
			//默认按行排序
			RunParams_Pin.hv_SortType[index1] = m_type.SortType_Row;
		}

		//模板匹配参数
		if (ui.cmb_MatchMethod_Train->currentText() == "基于形状") {
			RunParams_Pin.hv_MatchMethod[index1] = m_type.ShapeModel;      //模板类型:"基于互相关
		}
		else {
			RunParams_Pin.hv_MatchMethod[index1] = m_type.NccModel;      //模板类型:"基于互相关
		}
		RunParams_Pin.hv_AngleStart[index1] = ui.dspb_AngleStart_Train->value();            //起始角度
		RunParams_Pin.hv_AngleExtent[index1] = ui.dspb_AngleExtent_Train->value();                //角度范围
		RunParams_Pin.hv_ScaleRMin[index1] = ui.dspb_ScaleRMin_Train->value();                 //最小行缩放
		RunParams_Pin.hv_ScaleRMax[index1] = ui.dspb_ScaleRMax_Train->value();                 //最大行缩放
		RunParams_Pin.hv_ScaleCMin[index1] = ui.dspb_ScaleCMin_Train->value();                 //最小列缩放
		RunParams_Pin.hv_ScaleCMax[index1] = ui.dspb_ScaleCMax_Train->value();				   //最大列缩放
		RunParams_Pin.hv_MinScore[index1] = ui.dspb_MinScore_Run->value();               //最低分数
		RunParams_Pin.hv_NumMatches[index1] = ui.spb_NumMatches_Run->value();               //匹配个数，0则自动选择，100则最多匹配100个
		RunParams_Pin.hv_LowThreshold[index1] = ui.spb_LowThreshold->value();
		//灰度处理
		if (ui.checkBox_Gray->isChecked())
		{
			RunParams_Pin.hv_ToGray = 1;
		}
		else
		{
			RunParams_Pin.hv_ToGray = 0;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "绘制搜索区域失败!", QMessageBox::Ok);
		return;
	}

}
//清除ROI
void PinWindow::on_pb_ClearRoi_clicked() {
	try {
		GenEmptyObj(&ho_ShowObj);
		//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
		GiveParameterToWindow();
		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}

		ui.cmb_SearchRegion->clear();
		ui.cmb_PinRegionBinding->clear();
		ui.cmb_BaseXBinding->clear();
		ui.cmb_BaseYBinding->clear();
		ui.textBrowser_BindingLog->clear();

		//区域检测类型清空
		RunParams_Pin.hv_DetectType.Clear();
		//二值化参数
		RunParams_Pin.hv_X_BindingNum.Clear();
		RunParams_Pin.hv_Y_BindingNum.Clear();
		RunParams_Pin.hv_SortType.Clear();
		GenEmptyObj(&RunParams_Pin.ho_SearchRegion);//数组,搜索区域
		RunParams_Pin.ho_SearchRegion.Clear();
		RunParams_Pin.hv_ThresholdType.Clear();		//数组,图像二值化类型
		RunParams_Pin.hv_LowThreshold.Clear();				//数组,低阈值(固定阈值)
		RunParams_Pin.hv_HighThreshold.Clear();				//数组,高阈值(固定阈值)
		RunParams_Pin.hv_Sigma.Clear();						//数组,平滑阈值
		RunParams_Pin.hv_CoreWidth.Clear();					//数组,动态阈值(滤波核宽度)
		RunParams_Pin.hv_CoreHeight.Clear();					//数组,动态阈值(滤波核高度)
		RunParams_Pin.hv_DynThresholdContrast.Clear();		//数组,动态阈值分割图像对比度
		RunParams_Pin.hv_DynThresholdPolarity.Clear();	//数组,动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark

		//筛选Blob参数
		RunParams_Pin.hv_AreaFilter_Min.Clear();	//数组
		RunParams_Pin.hv_RecLen1Filter_Min.Clear();//数组
		RunParams_Pin.hv_RecLen2Filter_Min.Clear();//数组
		RunParams_Pin.hv_CircularityFilter_Min.Clear();//数组
		RunParams_Pin.hv_RectangularityFilter_Min.Clear();//数组
		RunParams_Pin.hv_WidthFilter_Min.Clear();//数组
		RunParams_Pin.hv_HeightFilter_Min.Clear();//数组
		RunParams_Pin.hv_RowFilter_Min.Clear();//数组
		RunParams_Pin.hv_ColumnFilter_Min.Clear();//数组
		RunParams_Pin.hv_AreaFilter_Max.Clear();//数组
		RunParams_Pin.hv_RecLen1Filter_Max.Clear();//数组
		RunParams_Pin.hv_RecLen2Filter_Max.Clear();//数组
		RunParams_Pin.hv_CircularityFilter_Max.Clear();//数组
		RunParams_Pin.hv_RectangularityFilter_Max.Clear();//数组
		RunParams_Pin.hv_WidthFilter_Max.Clear();//数组
		RunParams_Pin.hv_HeightFilter_Max.Clear();//数组
		RunParams_Pin.hv_RowFilter_Max.Clear();//数组
		RunParams_Pin.hv_ColumnFilter_Max.Clear();//数组
		RunParams_Pin.hv_SelectAreaMax.Clear();
		RunParams_Pin.hv_BlobCount.Clear();
		//形态学类型
		RunParams_Pin.hv_OperatorType.Clear();//数组
		RunParams_Pin.hv_OperaRec_Width.Clear();//数组
		RunParams_Pin.hv_OperaRec_Height.Clear();//数组
		RunParams_Pin.hv_OperaCir_Radius.Clear();//数组
		//孔洞填充
		RunParams_Pin.hv_FillUpShape_Min.Clear();
		RunParams_Pin.hv_FillUpShape_Max.Clear();

		//模板匹配参数
		RunParams_Pin.hv_MatchMethod.Clear();      //模板类型:"基于形状"，"基于互相关	
		RunParams_Pin.hv_AngleStart.Clear();            //起始角度
		RunParams_Pin.hv_AngleExtent.Clear();                //角度范围
		RunParams_Pin.hv_ScaleRMin.Clear();                 //最小行缩放
		RunParams_Pin.hv_ScaleRMax.Clear();                 //最大行缩放
		RunParams_Pin.hv_ScaleCMin.Clear();                 //最小列缩放
		RunParams_Pin.hv_ScaleCMax.Clear();				   //最大列缩放
		RunParams_Pin.hv_MinScore.Clear();               //最低分数
		RunParams_Pin.hv_NumMatches.Clear();               //匹配个数，0则自动选择，100则最多匹配100个
		RunParams_Pin.ho_TrainRegion.Clear();
		RunParams_Pin.hv_ModelID.Clear();                     //模板句柄
	}
	catch (...) {
		QMessageBox::information(this, "提示", "清除ROI失败!", QMessageBox::Ok);
		return;
	}

}
//初始化Blob/工艺参数/结果参数表格
void PinWindow::IniTableData_Standard() {
	try {
		//Blob参数 
		QTableWidgetItem* hearderItem0;
		QStringList hearderText0;
		hearderText0 << "面积" << "X坐标" << "Y坐标" << "外接矩半长" << "外接矩半宽" << "圆度" << "矩形度" << "宽" << "高";
		ui.tablewidget_Results_Blob->setColumnCount(hearderText0.count());
		for (int i = 0; i < ui.tablewidget_Results_Blob->columnCount(); i++) {
			hearderItem0 = new QTableWidgetItem(hearderText0.at(i));
			QFont font = hearderItem0->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			hearderItem0->setForeground(Qt::red);//字体颜色
			hearderItem0->setFont(font);//设置字体
			ui.tablewidget_Results_Blob->setHorizontalHeaderItem(i, hearderItem0);
		}
		//绑定Blob结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_Results_Blob->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_Blob(int)));

		//工艺参数
		QTableWidgetItem* hearderItem;
		QStringList hearderText;
		hearderText << "距离基准X" << "距离基准Y" << "删除行";
		ui.tablewidget_StandardData->setColumnCount(hearderText.count());
		for (int i = 0; i < ui.tablewidget_StandardData->columnCount(); i++) {
			hearderItem = new QTableWidgetItem(hearderText.at(i));
			QFont font = hearderItem->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			hearderItem->setForeground(Qt::red);//字体颜色
			hearderItem->setFont(font);//设置字体
			ui.tablewidget_StandardData->setHorizontalHeaderItem(i, hearderItem);
		}

		//结果参数
		std::vector<std::string> vec_Name = PinDetect1->getModubleResultTitleList();
		QTableWidgetItem* hearderItem2;
		ui.tablewidget_ResultData->setColumnCount(vec_Name.size());
		for (int i = 0; i < ui.tablewidget_ResultData->columnCount(); i++) {
			hearderItem2 = new QTableWidgetItem(QString::fromStdString(vec_Name[i]));
			QFont font = hearderItem2->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			hearderItem2->setForeground(Qt::red);//字体颜色
			hearderItem2->setFont(font);//设置字体
			ui.tablewidget_ResultData->setHorizontalHeaderItem(i, hearderItem2);
		}
		//绑定结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_ResultData->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_ResultData(int)));

		//模板匹配结果表格
		QTableWidgetItem* hearderItem3;
		QStringList hearderText3;
		hearderText3 << "X坐标" << "Y坐标" << "角度" << "分数";
		ui.tablewidget_Results_Template->setColumnCount(hearderText3.count());
		for (int i = 0; i < ui.tablewidget_Results_Template->columnCount(); i++) {
			hearderItem3 = new QTableWidgetItem(hearderText3.at(i));
			QFont font = hearderItem3->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			hearderItem3->setForeground(Qt::red);//字体颜色
			hearderItem3->setFont(font);//设置字体
			ui.tablewidget_Results_Template->setHorizontalHeaderItem(i, hearderItem3);
		}
		//绑定模板匹配结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_Results_Template->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_TemplateData(int)));

		//模板匹配结果表格
		QTableWidgetItem* hearderItem4;
		QStringList hearderText4;
		hearderText4 << "Row" << "Column" << "Angle" << "Score";
		ui.tablewidget_Results_Template_2->setColumnCount(hearderText4.count());
		for (int i = 0; i < ui.tablewidget_Results_Template_2->columnCount(); i++) {
			hearderItem4 = new QTableWidgetItem(hearderText4.at(i));
			QFont font = hearderItem4->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			hearderItem4->setForeground(Qt::red);//字体颜色
			hearderItem4->setFont(font);//设置字体
			ui.tablewidget_Results_Template_2->setHorizontalHeaderItem(i, hearderItem4);
		}
		//绑定模板匹配结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_Results_Template_2->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_TemplateData2(int)));
	}
	catch (...) {
		QMessageBox::information(this, "提示", "初始化表格失败!", QMessageBox::Ok);
		return;
	}

}
//Blob结果表格每个Cell点击事件
void PinWindow::on_tablewidget_Results_Blob_cellClicked(int row, int column) {
	try {
		int SelectRow = row;
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (BlobCountShow > 0) {
			HObject ho_ObjTemp;
			SelectObj(BlobRegionShow, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, BlobRegionShow, &ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//Blob结果表格表头的点击事件槽函数
void PinWindow::slot_VerticalHeader_Blob(int) {
	try {
		int SelectRow = ui.tablewidget_Results_Blob->currentRow();
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (BlobCountShow > 0) {
			HObject ho_ObjTemp;
			SelectObj(BlobRegionShow, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, BlobRegionShow, &ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//Pin结果表格每个Cell点击事件
void PinWindow::on_tablewidget_ResultData_cellClicked(int row, int column) {
	try {
		int SelectRow = row;
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (mResultParams.hv_RetNum > 0) {
			HObject ho_ObjTemp;
			SelectObj(mResultParams.ho_Region_AllPin, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			if (mResultParams.ho_Region_AllPin.Key() != nullptr) {
				ConcatObj(ho_ShowObj, mResultParams.ho_Region_AllPin, &ho_ShowObj);
			}
			if (mResultParams.ho_Contour_Base.Key() != nullptr) {
				ConcatObj(ho_ShowObj, mResultParams.ho_Contour_Base, &ho_ShowObj);
			}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//Pin结果表格表头的点击事件槽函数
void PinWindow::slot_VerticalHeader_ResultData(int) {
	try {
		int SelectRow = ui.tablewidget_ResultData->currentRow();
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (mResultParams.hv_RetNum > 0) {
			HObject ho_ObjTemp;
			SelectObj(mResultParams.ho_Region_AllPin, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			if (mResultParams.ho_Region_AllPin.Key() != nullptr) {
				ConcatObj(ho_ShowObj, mResultParams.ho_Region_AllPin, &ho_ShowObj);
			}
			if (mResultParams.ho_Contour_Base.Key() != nullptr) {
				ConcatObj(ho_ShowObj, mResultParams.ho_Contour_Base, &ho_ShowObj);
			}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//模板匹配结果表格每个Cell点击事件
void PinWindow::on_tablewidget_Results_Template_cellClicked(int row, int column) {
	try {
		int SelectRow = row;
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (BlobCountShow > 0) {
			HObject ho_ObjTemp;
			SelectObj(BlobRegionShow, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			if (BlobRegionShow.Key() != nullptr) {
				ConcatObj(ho_ShowObj, BlobRegionShow, &ho_ShowObj);
			}
			if (BaseContourShow.Key() != nullptr) {
				ConcatObj(ho_ShowObj, BaseContourShow, &ho_ShowObj);
			}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//模板匹配结果表格表头的点击事件槽函数
void PinWindow::slot_VerticalHeader_TemplateData(int) {
	try {
		int SelectRow = ui.tablewidget_Results_Template->currentRow();
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (BlobCountShow > 0) {
			HObject ho_ObjTemp;
			SelectObj(BlobRegionShow, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			if (BlobRegionShow.Key() != nullptr) {
				ConcatObj(ho_ShowObj, BlobRegionShow, &ho_ShowObj);
			}
			if (BaseContourShow.Key() != nullptr) {
				ConcatObj(ho_ShowObj, BaseContourShow, &ho_ShowObj);
			}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//模板匹配结果表格表头的点击事件槽函数
void PinWindow::slot_VerticalHeader_TemplateData2(int) {
	try {
		int SelectRow = ui.tablewidget_Results_Template_2->currentRow();
		if (SelectRow < 0) {
			return;
		}
		//刷新结果区域
		if (BlobCountShow > 0) {
			HObject ho_ObjTemp;
			SelectObj(BlobRegionShow, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			if (BlobRegionShow.Key() != nullptr) {
				ConcatObj(ho_ShowObj, BlobRegionShow, &ho_ShowObj);
			}
			if (BaseContourShow.Key() != nullptr) {
				ConcatObj(ho_ShowObj, BaseContourShow, &ho_ShowObj);
			}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...) {

	}

}
//添加Blob搜索区域
void PinWindow::on_pb_AddRoi_clicked() {
	try {
		TypeParamsStruct_Pin m_Type;

		int count = ui.cmb_SearchRegion->count();
		QString nameBase = "ROI" + QString::number(count + 1);
		ui.cmb_SearchRegion->addItem(nameBase);
		count = ui.cmb_SearchRegion->count();
		ui.cmb_SearchRegion->setCurrentIndex(count - 1);

		//区域检测类型
		if (ui.cmb_PinDetectType->currentText() == "Blob") {
			RunParams_Pin.hv_DetectType.Append(m_Type.DetectType_Blob);
		}
		else {
			RunParams_Pin.hv_DetectType.Append(m_Type.DetectType_Template);
		}
		//******************二值化参数**********************
		//绑定基准的编号
		RunParams_Pin.hv_X_BindingNum.Append(0);
		RunParams_Pin.hv_Y_BindingNum.Append(0);
		//排序方式
		RunParams_Pin.hv_SortType.Append(m_Type.SortType_Row);
		HObject emptyRegion;
		GenEmptyObj(&emptyRegion);
		if (RunParams_Pin.ho_SearchRegion.Key() == nullptr) {
			GenEmptyObj(&RunParams_Pin.ho_SearchRegion);
		}
		ConcatObj(RunParams_Pin.ho_SearchRegion, emptyRegion, &RunParams_Pin.ho_SearchRegion);//数组,搜索区域
		RunParams_Pin.hv_ThresholdType.Append(m_Type.Fixed_Threshold);		//数组,图像二值化类型
		RunParams_Pin.hv_LowThreshold.Append(0);				//数组,低阈值(固定阈值)
		RunParams_Pin.hv_HighThreshold.Append(128);				//数组,高阈值(固定阈值)
		RunParams_Pin.hv_Sigma.Append(1);						//数组,平滑阈值
		RunParams_Pin.hv_CoreWidth.Append(1);					//数组,动态阈值(滤波核宽度)
		RunParams_Pin.hv_CoreHeight.Append(1);					//数组,动态阈值(滤波核高度)
		RunParams_Pin.hv_DynThresholdContrast.Append(10);		//数组,动态阈值分割图像对比度
		RunParams_Pin.hv_DynThresholdPolarity.Append(m_Type.DynPolarity_Light);	//数组,动态阈值极性选择(选择提取暗区域或者亮区域)，light,dark
		//形态学参数
		RunParams_Pin.hv_OperatorType.Append(m_Type.hv_Null);//数组
		RunParams_Pin.hv_OperaRec_Width.Append(1);//数组
		RunParams_Pin.hv_OperaRec_Height.Append(1);//数组
		RunParams_Pin.hv_OperaCir_Radius.Append(1);//数组
		//孔洞填充参数
		RunParams_Pin.hv_FillUpShape_Min.Append(1);//数组
		RunParams_Pin.hv_FillUpShape_Max.Append(10);//数组
		//筛选Blob参数
		RunParams_Pin.hv_AreaFilter_Min.Append(1);	//数组
		RunParams_Pin.hv_RecLen1Filter_Min.Append(1);//数组
		RunParams_Pin.hv_RecLen2Filter_Min.Append(1);//数组
		RunParams_Pin.hv_CircularityFilter_Min.Append(0);//数组
		RunParams_Pin.hv_RectangularityFilter_Min.Append(0);//数组
		RunParams_Pin.hv_WidthFilter_Min.Append(1);//数组
		RunParams_Pin.hv_HeightFilter_Min.Append(1);//数组
		RunParams_Pin.hv_RowFilter_Min.Append(1);//数组
		RunParams_Pin.hv_ColumnFilter_Min.Append(1);//数组
		RunParams_Pin.hv_AreaFilter_Max.Append(999999999);//数组
		RunParams_Pin.hv_RecLen1Filter_Max.Append(999999999);//数组
		RunParams_Pin.hv_RecLen2Filter_Max.Append(999999999);//数组
		RunParams_Pin.hv_CircularityFilter_Max.Append(1);//数组
		RunParams_Pin.hv_RectangularityFilter_Max.Append(1);//数组
		RunParams_Pin.hv_WidthFilter_Max.Append(999999999);//数组
		RunParams_Pin.hv_HeightFilter_Max.Append(999999999);//数组
		RunParams_Pin.hv_RowFilter_Max.Append(999999999);//数组
		RunParams_Pin.hv_ColumnFilter_Max.Append(999999999);//数组
		RunParams_Pin.hv_SelectAreaMax.Append(0);
		RunParams_Pin.hv_BlobCount.Append(1);
		//模板匹配参数
		RunParams_Pin.hv_MatchMethod.Append(m_Type.ShapeModel);      //模板类型:"基于形状"，"基于互相关	
		RunParams_Pin.hv_AngleStart.Append(-90);            //起始角度
		RunParams_Pin.hv_AngleExtent.Append(90);                //终止角度
		RunParams_Pin.hv_ScaleRMin.Append(0.9);                 //最小行缩放
		RunParams_Pin.hv_ScaleRMax.Append(1.1);                 //最大行缩放
		RunParams_Pin.hv_ScaleCMin.Append(0.9);                 //最小列缩放
		RunParams_Pin.hv_ScaleCMax.Append(1.1);				   //最大列缩放
		RunParams_Pin.hv_MinScore.Append(0.5);               //最低分数
		RunParams_Pin.hv_NumMatches.Append(1);               //匹配个数，0则自动选择，100则最多匹配100个
		HObject emptyTrainRegion;
		GenEmptyObj(&emptyTrainRegion);
		if (RunParams_Pin.ho_TrainRegion.Key() == nullptr) {
			GenEmptyObj(&RunParams_Pin.ho_TrainRegion);
		}
		ConcatObj(RunParams_Pin.ho_TrainRegion, emptyTrainRegion, &RunParams_Pin.ho_TrainRegion);//数组,模板训练区域
		HTuple modelID = NULL;
		RunParams_Pin.hv_ModelID.Append(modelID);                     //模板句柄
	}
	catch (...) {
		QMessageBox::information(this, "提示", "添加搜索区域失败!", QMessageBox::Ok);
		return;
	}

}
//设置当前区域Blob参数
void PinWindow::on_pb_SetBlobData_clicked() {
	try {
		TypeParamsStruct_Pin m_type;
		HTuple countRegion;
		CountObj(RunParams_Pin.ho_SearchRegion, &countRegion);
		if (countRegion <= 0) {
			QMessageBox::information(this, "提示", "Pin搜索区域为空!", QMessageBox::Ok);
			return;
		}
		int index1 = ui.cmb_SearchRegion->currentIndex();
		if (countRegion < (index1 + 1)) {
			QMessageBox::information(this, "提示", "Pin搜索区域个数与UI上数量不一致!", QMessageBox::Ok);
			return;
		}
		//区域检测类型
		if (ui.cmb_PinDetectType->currentText() == "Blob") {
			RunParams_Pin.hv_DetectType[index1] = m_type.DetectType_Blob;
		}
		else {
			RunParams_Pin.hv_DetectType[index1] = m_type.DetectType_Template;
		}
		//二值化参数赋值
		if (ui.cmb_Type_Binarization->currentText().toStdString() == "固定阈值") {
			RunParams_Pin.hv_ThresholdType[index1] = m_type.Fixed_Threshold;
		}
		else if (ui.cmb_Type_Binarization->currentText().toStdString() == "自适应阈值") {
			RunParams_Pin.hv_ThresholdType[index1] = m_type.Auto_Threshold;
		}
		else if (ui.cmb_Type_Binarization->currentText().toStdString() == "动态阈值") {
			RunParams_Pin.hv_ThresholdType[index1] = m_type.Dyn_Threshold;
		}
		else {
			QMessageBox::information(this, "提示", "请选择二值化类型!", QMessageBox::Ok);
			return;
		}
		RunParams_Pin.hv_LowThreshold[index1] = ui.spb_LowThreshold->value();
		RunParams_Pin.hv_HighThreshold[index1] = ui.spb_HighThreshold->value();
		RunParams_Pin.hv_Sigma[index1] = ui.dspb_Sigma->value();
		RunParams_Pin.hv_CoreWidth[index1] = ui.dspb_DynThresholdWidth->value();
		RunParams_Pin.hv_CoreHeight[index1] = ui.dspb_DynThresholdHeight->value();
		RunParams_Pin.hv_DynThresholdContrast[index1] = ui.spb_DynThresholdContrast->value();

		if (ui.cmb_Polarity->currentText().toStdString() == "dark") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Dark;
		}
		else if (ui.cmb_Polarity->currentText().toStdString() == "light") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Light;
		}
		else if (ui.cmb_Polarity->currentText().toStdString() == "equal") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Equal;
		}
		else if (ui.cmb_Polarity->currentText().toStdString() == "not_equal") {
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_NotEqual;
		}
		else {
			//默认找白色区域
			RunParams_Pin.hv_DynThresholdPolarity[index1] = m_type.DynPolarity_Light;
		}
		//形态学参数赋值
		if (ui.cmb_OperatorType->currentText().toStdString() == "矩形开运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Opening_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "矩形闭运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Closing_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形开运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Opening_Cir;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形闭运算") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Closing_Cir;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "矩形腐蚀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Erosion_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "矩形膨胀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Dilation_Rec;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形腐蚀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Erosion_Cir;
		}
		else if (ui.cmb_OperatorType->currentText().toStdString() == "圆形膨胀") {
			RunParams_Pin.hv_OperatorType[index1] = m_type.Dilation_Cir;
		}
		else {
			RunParams_Pin.hv_OperatorType[index1] = m_type.hv_Null;
		}
		RunParams_Pin.hv_OperaRec_Width[index1] = ui.dsb_OperatorWidth->value();//数组
		RunParams_Pin.hv_OperaRec_Height[index1] = ui.dsb_OperatorHeight->value();//数组
		RunParams_Pin.hv_OperaCir_Radius[index1] = ui.dsb_OperatorRadius->value();//数组
		//孔洞填充
		RunParams_Pin.hv_FillUpShape_Min[index1] = ui.dspb_FillUpShape_Min->value();//数组
		RunParams_Pin.hv_FillUpShape_Max[index1] = ui.dspb_FillUpShape_Max->value();//数组
		//Blob筛选参数赋值
		RunParams_Pin.hv_AreaFilter_Min[index1] = ui.dspb_AreaFilter_Min->value();
		RunParams_Pin.hv_AreaFilter_Max[index1] = ui.dspb_AreaFilter_Max->value();
		RunParams_Pin.hv_RowFilter_Min[index1] = ui.dspb_RowFilter_Min->value();
		RunParams_Pin.hv_RowFilter_Max[index1] = ui.dspb_RowFilter_Max->value();
		RunParams_Pin.hv_ColumnFilter_Min[index1] = ui.dspb_ColumnFilter_Min->value();
		RunParams_Pin.hv_ColumnFilter_Max[index1] = ui.dspb_ColumnFilter_Max->value();
		RunParams_Pin.hv_RecLen1Filter_Min[index1] = ui.dspb_RecLen1Filter_Min->value();
		RunParams_Pin.hv_RecLen1Filter_Max[index1] = ui.dspb_RecLen1Filter_Max->value();
		RunParams_Pin.hv_RecLen2Filter_Min[index1] = ui.dspb_RecLen2Filter_Min->value();
		RunParams_Pin.hv_RecLen2Filter_Max[index1] = ui.dspb_RecLen2Filter_Max->value();
		RunParams_Pin.hv_CircularityFilter_Min[index1] = ui.dspb_CircularityFilter_Min->value();
		RunParams_Pin.hv_CircularityFilter_Max[index1] = ui.dspb_CircularityFilter_Max->value();
		RunParams_Pin.hv_RectangularityFilter_Min[index1] = ui.dspb_RectangularityFilter_Min->value();
		RunParams_Pin.hv_RectangularityFilter_Max[index1] = ui.dspb_RectangularityFilter_Max->value();
		RunParams_Pin.hv_WidthFilter_Min[index1] = ui.dspb_WidthFilter_Min->value();
		RunParams_Pin.hv_WidthFilter_Max[index1] = ui.dspb_WidthFilter_Max->value();
		RunParams_Pin.hv_HeightFilter_Min[index1] = ui.dspb_HeightFilter_Min->value();
		RunParams_Pin.hv_HeightFilter_Max[index1] = ui.dspb_HeightFilter_Max->value();
		//是否选择区域内面积最大值
		if (ui.ckb_SelectAreaMax->checkState() == Qt::Checked) {
			RunParams_Pin.hv_SelectAreaMax[index1] = 1;
		}
		else {
			RunParams_Pin.hv_SelectAreaMax[index1] = 0;
		}
		auto blobCount = ui.spb_BlobCount->value();
		RunParams_Pin.hv_BlobCount[index1] = ui.spb_BlobCount->value();
		//排序方式
		if (ui.cmb_PinSortType->currentText() == "按行排序") {
			RunParams_Pin.hv_SortType[index1] = m_type.SortType_Row;
		}
		else if (ui.cmb_PinSortType->currentText() == "按列排序") {
			RunParams_Pin.hv_SortType[index1] = m_type.SortType_Column;
		}
		else {
			//默认按行排序
			RunParams_Pin.hv_SortType[index1] = m_type.SortType_Row;
		}

		//模板匹配参数
		if (ui.cmb_MatchMethod_Train->currentText() == "基于形状") {
			RunParams_Pin.hv_MatchMethod[index1] = m_type.ShapeModel;      //模板类型:"基于互相关
		}
		else {
			RunParams_Pin.hv_MatchMethod[index1] = m_type.NccModel;      //模板类型:"基于互相关
		}
		RunParams_Pin.hv_AngleStart[index1] = ui.dspb_AngleStart_Train->value();            //起始角度
		RunParams_Pin.hv_AngleExtent[index1] = ui.dspb_AngleExtent_Train->value();                //角度范围
		RunParams_Pin.hv_ScaleRMin[index1] = ui.dspb_ScaleRMin_Train->value();                 //最小行缩放
		RunParams_Pin.hv_ScaleRMax[index1] = ui.dspb_ScaleRMax_Train->value();                 //最大行缩放
		RunParams_Pin.hv_ScaleCMin[index1] = ui.dspb_ScaleCMin_Train->value();                 //最小列缩放
		RunParams_Pin.hv_ScaleCMax[index1] = ui.dspb_ScaleCMax_Train->value();				   //最大列缩放
		RunParams_Pin.hv_MinScore[index1] = ui.dspb_MinScore_Run->value();               //最低分数
		RunParams_Pin.hv_NumMatches[index1] = ui.spb_NumMatches_Run->value();               //匹配个数，0则自动选择，100则最多匹配100个
		//灰度处理
		if (ui.checkBox_Gray->isChecked())
		{
			RunParams_Pin.hv_ToGray = 1;
		}
		else
		{
			RunParams_Pin.hv_ToGray = 0;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "Pin检测参数设置失败!", QMessageBox::Ok);
		return;
	}
}
//Blob测试
void PinWindow::on_pb_BlobTest_clicked() {
	try {
		int count = ui.cmb_SearchRegion->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请选择区域编号!", QMessageBox::Ok);
			return;
		}

		HTuple numR;
		CountObj(RunParams_Pin.ho_SearchRegion, &numR);
		if (numR <= 0) {
			QMessageBox::information(this, "提示", "搜索区域为空!", QMessageBox::Ok);
			return;
		}

		//声明临时结构体变量
		RunParamsStruct_Pin structTemp;
		structTemp = RunParams_Pin;
		//判断是否启用位置矫正
		if (ui.ckb_PositionCorrection->checkState() == Qt::Checked) {
			//这里需要再试赋值，因为参数会被设置
			on_pb_SetData_PositionCorrection_clicked();
			structTemp = RunParams_Pin;

			int ret = 1;
			//查找模板
			HObject ModelRegion, ModelXLD;
			HTuple rowRet, colRet, angleRet, scoreRet;
			ret = PinDetect1->FindTemplate(ho_Image, 0, structTemp, ModelXLD, ModelRegion, rowRet, colRet, angleRet, scoreRet);
			if (ret == 0 && rowRet.TupleLength() == 1) {
				HTuple hv_ROIHomMat2D = NULL;//仿射变换ROI跟随
				VectorAngleToRigid(structTemp.hv_Row_PositionCorrection, structTemp.hv_Column_PositionCorrection,
					structTemp.hv_Angle_PositionCorrection.TupleRad(),
					rowRet, colRet, angleRet.TupleRad(),
					&hv_ROIHomMat2D);
				//仿射变换
				AffineTransRegion(structTemp.ho_SearchRegion, &structTemp.ho_SearchRegion, hv_ROIHomMat2D, "nearest_neighbor");
			}
		}

		//找区域代码运行
		BlobCountShow = 0;
		GenEmptyObj(&BlobRegionShow);
		TypeParamsStruct_Pin m_type;
		RunParamsStruct_Pin runParams;
		//搜索区域
		int index1 = ui.cmb_SearchRegion->currentIndex();
		SelectObj(structTemp.ho_SearchRegion, &runParams.ho_SearchRegion, index1 + 1);
		//区域检测类型
		runParams.hv_DetectType = structTemp.hv_DetectType[index1];
		//二值化参数赋值
		runParams.hv_SortType = structTemp.hv_SortType[index1];
		runParams.hv_ThresholdType = structTemp.hv_ThresholdType[index1];
		runParams.hv_LowThreshold = structTemp.hv_LowThreshold[index1];
		runParams.hv_HighThreshold = structTemp.hv_HighThreshold[index1];
		runParams.hv_Sigma = structTemp.hv_Sigma[index1];
		runParams.hv_CoreWidth = structTemp.hv_CoreWidth[index1];
		runParams.hv_CoreHeight = structTemp.hv_CoreHeight[index1];
		runParams.hv_DynThresholdContrast = structTemp.hv_DynThresholdContrast[index1];
		//默认找白色区域
		runParams.hv_DynThresholdPolarity = structTemp.hv_DynThresholdPolarity[index1];
		//形态学参数赋值
		runParams.hv_OperatorType = structTemp.hv_OperatorType[index1];
		runParams.hv_OperaRec_Width = structTemp.hv_OperaRec_Width[index1];
		runParams.hv_OperaRec_Height = structTemp.hv_OperaRec_Height[index1];
		runParams.hv_OperaCir_Radius = structTemp.hv_OperaCir_Radius[index1];
		//孔洞填充
		runParams.hv_FillUpShape_Min = structTemp.hv_FillUpShape_Min[index1];
		runParams.hv_FillUpShape_Max = structTemp.hv_FillUpShape_Max[index1];
		//Blob筛选参数赋值
		runParams.hv_AreaFilter_Min = structTemp.hv_AreaFilter_Min[index1];
		runParams.hv_AreaFilter_Max = structTemp.hv_AreaFilter_Max[index1];
		runParams.hv_RowFilter_Min = structTemp.hv_RowFilter_Min[index1];
		runParams.hv_RowFilter_Max = structTemp.hv_RowFilter_Max[index1];
		runParams.hv_ColumnFilter_Min = structTemp.hv_ColumnFilter_Min[index1];
		runParams.hv_ColumnFilter_Max = structTemp.hv_ColumnFilter_Max[index1];
		runParams.hv_RecLen1Filter_Min = structTemp.hv_RecLen1Filter_Min[index1];
		runParams.hv_RecLen1Filter_Max = structTemp.hv_RecLen1Filter_Max[index1];
		runParams.hv_RecLen2Filter_Min = structTemp.hv_RecLen2Filter_Min[index1];
		runParams.hv_RecLen2Filter_Max = structTemp.hv_RecLen2Filter_Max[index1];
		runParams.hv_CircularityFilter_Min = structTemp.hv_CircularityFilter_Min[index1];
		runParams.hv_CircularityFilter_Max = structTemp.hv_CircularityFilter_Max[index1];
		runParams.hv_RectangularityFilter_Min = structTemp.hv_RectangularityFilter_Min[index1];
		runParams.hv_RectangularityFilter_Max = structTemp.hv_RectangularityFilter_Max[index1];
		runParams.hv_WidthFilter_Min = structTemp.hv_WidthFilter_Min[index1];
		runParams.hv_WidthFilter_Max = structTemp.hv_WidthFilter_Max[index1];
		runParams.hv_HeightFilter_Min = structTemp.hv_HeightFilter_Min[index1];
		runParams.hv_HeightFilter_Max = structTemp.hv_HeightFilter_Max[index1];
		runParams.hv_SelectAreaMax = structTemp.hv_SelectAreaMax[index1];
		runParams.hv_BlobCount = structTemp.hv_BlobCount[index1];
		//排序方式
		runParams.hv_SortType = structTemp.hv_SortType[index1];

		//模板匹配参数
		runParams.hv_MatchMethod = structTemp.hv_MatchMethod[index1];
		runParams.hv_AngleStart = structTemp.hv_AngleStart[index1];            //起始角度
		runParams.hv_AngleExtent = structTemp.hv_AngleExtent[index1];                //角度范围
		runParams.hv_ScaleRMin = structTemp.hv_ScaleRMin[index1];                 //最小行缩放
		runParams.hv_ScaleRMax = structTemp.hv_ScaleRMax[index1];                 //最大行缩放
		runParams.hv_ScaleCMin = structTemp.hv_ScaleCMin[index1];                 //最小列缩放
		runParams.hv_ScaleCMax = structTemp.hv_ScaleCMax[index1];				   //最大列缩放
		runParams.hv_MinScore = structTemp.hv_MinScore[index1];					 //最低分数
		runParams.hv_NumMatches = structTemp.hv_NumMatches[index1];               //匹配个数，0则自动选择，100则最多匹配100个
		runParams.hv_ModelID = structTemp.hv_ModelID[index1];					//训练模型赋值

		//运行测试
		if (runParams.hv_DetectType == m_type.DetectType_Blob) {
			//Blob运行
			HObject regionPin;
			HTuple rowPin, columnPin;
			HTuple areaBlob, rowBlob, columnBlob, recLen1Blob, recLen2Blob, circularityBlob, rectangularityBlob, widthBlob, heightBlob;
			PinDetect1->PinBlob(ho_Image, runParams, regionPin, rowPin, columnPin,
				areaBlob, rowBlob, columnBlob, recLen1Blob, recLen2Blob, circularityBlob, rectangularityBlob, widthBlob, heightBlob);
			count = rowPin.TupleLength().I();
			BlobRegionShow = regionPin;
			BlobCountShow = count;
			//清空结果表格数据
			ui.tablewidget_Results_Blob->clearContents();
			//判断是否找到区域
			if (count <= 0) {
				//没有找到区域，退出
				GenEmptyObj(&ho_ShowObj);
				HObject roiContour;
				GenContourRegionXld(runParams.ho_SearchRegion, &roiContour, "border");
				ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj);
				//if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				//	showImg(ho_Image, "fill", "red");
				//}
				//else {
				//	showImg(ho_Image, "margin", "red");
				//}
				showImg(ho_Image, "margin", "red");
				QMessageBox::information(this, "提示", "没有找到Pin!", QMessageBox::Ok);
				return;
			}
			//刷新图像
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, regionPin, &ho_ShowObj);
			HObject roiContour;
			GenContourRegionXld(runParams.ho_SearchRegion, &roiContour, "border");
			ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//刷新所有Pin针中心点数据
			if (count > 0) {
				ui.spb_BlobCount->setValue(count);
				ui.tablewidget_Results_Blob->clearContents();
				ui.tablewidget_Results_Blob->setRowCount(count);
				for (int i = 0; i < count; i++) {
					QTableWidgetItem* item;
					QString str;
					//面积
					str = QString::number(areaBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 0, item);
					//X坐标
					str = QString::number(columnBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 1, item);
					//Y坐标
					str = QString::number(rowBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 2, item);
					//外接矩半长
					str = QString::number(recLen1Blob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 3, item);
					//外接矩半宽
					str = QString::number(recLen2Blob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 4, item);
					//圆度
					str = QString::number(circularityBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 5, item);
					//矩形度
					str = QString::number(rectangularityBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 6, item);
					//宽
					str = QString::number(widthBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 7, item);
					//高
					str = QString::number(heightBlob[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Blob->setItem(i, 8, item);

				}
			}
		}
		else {
			HObject ModelRegion, ModelXLD;
			HTuple row, col, angle, score;
			int ret = PinDetect1->FindTemplate(ho_Image, 1, runParams, ModelXLD, ModelRegion, row, col, angle, score);
			if (ret != 0) {
				QMessageBox::information(this, "提示", "查找模板失败", QMessageBox::Ok);
				return;
			}
			count = row.TupleLength().I();
			BlobRegionShow = ModelRegion;
			BlobCountShow = count;
			//刷新结果
			if (count <= 0) {
				//没有找到区域，退出
				GenEmptyObj(&ho_ShowObj);
				HObject roiContour;
				GenContourRegionXld(runParams.ho_SearchRegion, &roiContour, "border");
				ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj);
				//if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				//	showImg(ho_Image, "fill", "red");
				//}
				//else {
				//	showImg(ho_Image, "margin", "red");
				//}
				showImg(ho_Image, "margin", "red");
				QMessageBox::information(this, "提示", "没有找到Pin!", QMessageBox::Ok);
				return;
			}
			//刷新图像
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, ModelRegion, &ho_ShowObj);
			HObject roiContour;
			GenContourRegionXld(runParams.ho_SearchRegion, &roiContour, "border");
			ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			//刷新所有Pin针中心点数据
			if (count > 0) {
				ui.tablewidget_Results_Template->clearContents();
				ui.tablewidget_Results_Template->setRowCount(count);
				for (int i = 0; i < count; i++) {
					QTableWidgetItem* item;
					QString str;
					//X坐标
					str = QString::number(col[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Template->setItem(i, 0, item);
					//Y坐标
					str = QString::number(row[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Template->setItem(i, 1, item);
					//角度
					str = QString::number(angle[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Template->setItem(i, 2, item);
					//分数
					str = QString::number(score[i].D(), 'f', 2);
					item = new QTableWidgetItem(str, 1000);
					item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui.tablewidget_Results_Template->setItem(i, 3, item);
				}
			}
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "Pin检测算法运行测试失败!", QMessageBox::Ok);
		return;
	}
}
//基准参数设置
void PinWindow::on_pb_SetBaseData_clicked() {
	try {
		TypeParamsStruct_Pin m_type;
		if (RunParams_Pin.hv_BaseType.TupleLength() <= 0) {
			QMessageBox::information(this, "提示", "基准为空!", QMessageBox::Ok);
			return;
		}
		int index1 = ui.cmb_BaseSelect->currentIndex();
		if (RunParams_Pin.hv_BaseType.TupleLength() < (index1 + 1)) {
			QMessageBox::information(this, "提示", "基准参数与UI上数量不一致!", QMessageBox::Ok);
			return;
		}
		if (RunParams_Pin.hv_BaseType[index1] == m_type.BaseType_Line) {
			RunParams_Pin.hv_Row1_Base[index1] = ui.dspb_RowBegin_Line->value();
			RunParams_Pin.hv_Column1_Base[index1] = ui.dspb_ColBegin_Line->value();
			RunParams_Pin.hv_Row2_Base[index1] = ui.dspb_RowEnd_Line->value();
			RunParams_Pin.hv_Column2_Base[index1] = ui.dspb_ColEnd_Line->value();
			RunParams_Pin.hv_Radius_Base[index1] = 0;
		}
		else {
			RunParams_Pin.hv_Row1_Base[index1] = ui.dspb_CenterRow_Circle->value();
			RunParams_Pin.hv_Column1_Base[index1] = ui.dspb_CenterCol_Circle->value();
			RunParams_Pin.hv_Row2_Base[index1] = 0;
			RunParams_Pin.hv_Column2_Base[index1] = 0;
			RunParams_Pin.hv_Radius_Base[index1] = ui.dspb_CenterRadius_Circle->value();
		}
		RunParams_Pin.hv_MeasureLength1[index1] = ui.spb_MeasureLength1->value();		//数组,卡尺半长,
		RunParams_Pin.hv_MeasureLength2[index1] = ui.spb_MeasureLength2->value();		//数组,卡尺半宽
		RunParams_Pin.hv_MeasureSigma[index1] = ui.dspb_MeasureSigma->value();		//数组,平滑值 
		RunParams_Pin.hv_MeasureThreshold[index1] = ui.spb_MeasureThreshold->value();	//数组,边缘阈值
		RunParams_Pin.hv_MeasureTransition[index1] = HTuple(ui.cmb_MeasureTransition->currentText().toStdString().c_str());	//数组,卡尺极性
		RunParams_Pin.hv_MeasureSelect[index1] = HTuple(ui.cmb_MeasureSelect->currentText().toStdString().c_str());		//数组,边缘选择
		RunParams_Pin.hv_MeasureNum[index1] = ui.spb_MeasureNum->value();		    //数组,卡尺个数
		RunParams_Pin.hv_MeasureMinScore[index1] = ui.dspb_MinScore->value();     //数组,最小得分
		RunParams_Pin.hv_DistanceThreshold[index1] = ui.dspb_DistanceThreshold->value();   //数组,距离阈值
	}
	catch (...) {
		QMessageBox::information(this, "提示", "基准检测参数设置失败!", QMessageBox::Ok);
		return;
	}
}
//找基准测试
void PinWindow::on_pb_BaseTest_clicked() {
	try {
		int count = ui.cmb_BaseSelect->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请选择基准编号!", QMessageBox::Ok);
			return;
		}

		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		count = RunParams_Pin.hv_BaseType.TupleLength().I();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "基准为空!", QMessageBox::Ok);
			return;
		}

		int index1 = ui.cmb_BaseSelect->currentIndex();
		RunParamsStruct_Pin runParams;
		runParams.hv_BaseType = RunParams_Pin.hv_BaseType[index1];
		//模型需要的参数
		runParams.hv_Row1_Base = RunParams_Pin.hv_Row1_Base[index1];
		runParams.hv_Column1_Base = RunParams_Pin.hv_Column1_Base[index1];
		runParams.hv_Row2_Base = RunParams_Pin.hv_Row2_Base[index1];
		runParams.hv_Column2_Base = RunParams_Pin.hv_Column2_Base[index1];
		runParams.hv_Radius_Base = RunParams_Pin.hv_Radius_Base[index1];
		runParams.hv_MeasureLength1 = RunParams_Pin.hv_MeasureLength1[index1];
		runParams.hv_MeasureLength2 = RunParams_Pin.hv_MeasureLength2[index1];
		runParams.hv_MeasureSigma = RunParams_Pin.hv_MeasureSigma[index1];
		runParams.hv_MeasureThreshold = RunParams_Pin.hv_MeasureThreshold[index1];
		runParams.hv_MeasureTransition = RunParams_Pin.hv_MeasureTransition[index1];
		runParams.hv_MeasureSelect = RunParams_Pin.hv_MeasureSelect[index1];
		runParams.hv_MeasureNum = RunParams_Pin.hv_MeasureNum[index1];
		runParams.hv_MeasureMinScore = RunParams_Pin.hv_MeasureMinScore[index1];
		runParams.hv_DistanceThreshold = RunParams_Pin.hv_DistanceThreshold[index1];

		//找基准算法
		if (runParams.hv_BaseType == "直线") {
			HTuple row1Line, column1Line, row2Line, column2Line;
			HObject xldLine;
			PinDetect1->FindLine(ho_Image, runParams, row1Line, column1Line, row2Line, column2Line, xldLine);

			//判断是否找到直线
			if (row1Line.TupleLength() <= 0) {
				//没有找到直线，退出
				GenEmptyObj(&ho_ShowObj);
				showImg(ho_Image, "fill", "green");
				return;
			}
			//刷新图像
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, xldLine, &ho_ShowObj);
			showImg(ho_Image, "fill", "green");
			//刷新直线结果
			ui.txt_Row1_Line->setText(QString::number(row1Line.D(), 'f', 2));
			ui.txt_Col1_Line->setText(QString::number(column1Line.D(), 'f', 2));
			ui.txt_Row2_Line->setText(QString::number(row2Line.D(), 'f', 2));
			ui.txt_Col2_Line->setText(QString::number(column2Line.D(), 'f', 2));
		}
		else {
			HTuple row1Circle, column1Circle, radiusCircle;
			HObject xldCircle;
			PinDetect1->FindCircle(ho_Image, runParams, row1Circle, column1Circle, radiusCircle, xldCircle);
			//判断是否找到圆
			if (row1Circle.TupleLength() <= 0) {
				//没有找到圆，退出
				GenEmptyObj(&ho_ShowObj);
				showImg(ho_Image, "fill", "green");
				return;
			}
			//刷新图像
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, xldCircle, &ho_ShowObj);
			showImg(ho_Image, "fill", "green");
			//刷新圆心坐标和半径
			ui.txt_Row1_Circle->setText(QString::number(row1Circle.D(), 'f', 2));
			ui.txt_Col1_Circle->setText(QString::number(column1Circle.D(), 'f', 2));
			ui.txt_Radius_Circle->setText(QString::number(radiusCircle.D(), 'f', 2));
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "基准检测算法测试运行失败!", QMessageBox::Ok);
		return;
	}
}

//设置输入参数
void PinWindow::RefreshRunparams(const RunParamsStruct_Pin& RunParams) {
	try {
		TypeParamsStruct_Pin mType;
		//位置修正参数
		if (RunParams.hv_Check_PositionCorrection == 1) {
			ui.ckb_PositionCorrection->setCheckState(Qt::Checked);
		}
		else {
			ui.ckb_PositionCorrection->setCheckState(Qt::Unchecked);
		}
		ui.dspb_Row_PositionCorrection->setValue(RunParams.hv_Row_PositionCorrection.D());
		ui.dspb_Column_PositionCorrection->setValue(RunParams.hv_Column_PositionCorrection.D());
		ui.dspb_Angle_PositionCorrection->setValue(RunParams.hv_Angle_PositionCorrection.D());
		if (RunParams.hv_SortType_PositionCorrection == mType.SortType_Row) {
			ui.cmb_SortType->setCurrentText("按行排序");
		}
		else {
			ui.cmb_SortType->setCurrentText("按列排序");
		}
		if (RunParams.hv_MatchMethod_PositionCorrection == mType.ShapeModel) {
			ui.cmb_MatchMethod_Train_2->setCurrentText("基于形状");
		}
		else {
			ui.cmb_MatchMethod_Train_2->setCurrentText("基于互相关");
		}
		ui.dspb_AngleStart_Train_2->setValue(RunParams.hv_AngleStart_PositionCorrection.D());
		ui.dspb_AngleExtent_Train_2->setValue(RunParams.hv_AngleExtent_PositionCorrection.D());
		ui.dspb_ScaleRMin_Train_2->setValue(RunParams.hv_ScaleRMin_PositionCorrection.D());
		ui.dspb_ScaleRMax_Train_2->setValue(RunParams.hv_ScaleRMax_PositionCorrection.D());
		ui.dspb_ScaleCMin_Train_2->setValue(RunParams.hv_ScaleCMin_PositionCorrection.D());
		ui.dspb_ScaleCMax_Train_2->setValue(RunParams.hv_ScaleCMax_PositionCorrection.D());
		ui.dspb_MinScore_Run_2->setValue(RunParams.hv_MinScore_PositionCorrection.D());
		ui.spb_NumMatches_Run_2->setValue(RunParams.hv_NumMatches_PositionCorrection.TupleInt().I());

		//刷新ROI
		ui.cmb_SearchRegion->clear();
		GenEmptyObj(&ho_ShowObj);
		HTuple countRoi = 0;
		CountObj(RunParams.ho_SearchRegion, &countRoi);
		for (int i = 0; i < countRoi; i++) {
			QString nameRoi = "ROI" + QString::number(i + 1);
			ui.cmb_SearchRegion->addItem(nameRoi);
			ui.cmb_PinRegionBinding->addItem(nameRoi);

			HObject roiContour, roiRegion;
			SelectObj(RunParams.ho_SearchRegion, &roiRegion, i + 1);
			GenContourRegionXld(roiRegion, &roiContour, "border");
			ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj);
		}
		//刷新图像
		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}

		int count = ui.cmb_SearchRegion->count();
		if (count > 0) {
			ui.cmb_SearchRegion->setCurrentIndex(0);
			RefreshRunparams_ROI(RunParams, 0);
		}
		else {
			QMessageBox::information(this, "提示", "搜索区域不存在!", QMessageBox::Ok);
			return;
		}

		//刷新基准边
		ui.cmb_BaseSelect->clear();
		ui.cmb_BaseXBinding->clear();
		ui.cmb_BaseYBinding->clear();
		HTuple countBase = RunParams.hv_BaseType.TupleLength();
		if (countBase <= 0) {
			QMessageBox::information(this, "提示", "基准边不存在!", QMessageBox::Ok);
			return;
		}

		for (int i = 0; i < countBase; i++) {
			if (RunParams.hv_BaseType[i] == mType.BaseType_Line) {
				QString nameBase = "直线" + QString::number(i + 1);
				ui.cmb_BaseSelect->addItem(nameBase);
				ui.cmb_BaseXBinding->addItem(nameBase);
				ui.cmb_BaseYBinding->addItem(nameBase);
			}
			else if (RunParams.hv_BaseType[i] == mType.BaseType_Circle) {
				QString nameBase = "圆" + QString::number(i + 1);
				ui.cmb_BaseSelect->addItem(nameBase);
				ui.cmb_BaseXBinding->addItem(nameBase);
				ui.cmb_BaseYBinding->addItem(nameBase);
			}
			else {
				QMessageBox::information(this, "提示", "读取的基准类型不存在!", QMessageBox::Ok);
				return;
			}
		}
		RefreshRunparams_Base(RunParams, 0);

		//刷新像素当量和客户允许偏差
		ui.dspb_Tolerance_X->setValue(RunParams.hv_Tolerance_X.D());
		ui.dspb_Tolerance_Y->setValue(RunParams.hv_Tolerance_Y.D());
		ui.dspb_MmPixel->setValue(RunParams.hv_MmPixel.D());

		//刷新客户基准数据
		ui.tablewidget_StandardData->clearContents();
		count = RunParams_Pin.hv_StandardX_Array.TupleLength().I();
		if (count > 0) {
			ui.spb_PinNum->setValue(count);
			ui.tablewidget_StandardData->setRowCount(count);
			for (int i = 0; i < count; i++) {
				QTableWidgetItem* item;
				QString str;

				//距离基准X
				str = QString::number(RunParams_Pin.hv_StandardX_Array[i].D(), 'f', 3);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_StandardData->setItem(i, 0, item);
				//距离基准Y
				str = QString::number(RunParams_Pin.hv_StandardY_Array[i].D(), 'f', 3);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_StandardData->setItem(i, 1, item);
				//删除行
				QPushButton* deleteButton = new QPushButton;
				deleteButton->setText("删除");
				ui.tablewidget_StandardData->setCellWidget(i, 2, deleteButton);
				connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow()));
			}
		}
		else {
			QMessageBox::information(this, "提示", "基准数据不存在!", QMessageBox::Ok);
			return;
		}

		//刷新绑定信息
		ui.textBrowser_BindingLog->clear();
		for (int i = 0; i < countRoi; i++) {
			//被绑定的基准编号
			int X_BindingNum = HTuple(RunParams_Pin.hv_X_BindingNum[i]).TupleInt().I();
			int Y_BindingNum = HTuple(RunParams_Pin.hv_Y_BindingNum[i]).TupleInt().I();
			if (RunParams_Pin.hv_BaseType[X_BindingNum] == mType.BaseType_Line &&
				RunParams_Pin.hv_BaseType[Y_BindingNum] == mType.BaseType_Line) {
				ui.textBrowser_BindingLog->append(QDateTime::currentDateTime().toString("hh:mm:ss：") + "ROI" + QString::number(i + 1) + ",绑定X基准:" +
					"直线" + QString::number(X_BindingNum + 1) + ",绑定Y基准:" + "直线" + QString::number(Y_BindingNum + 1));
			}
			else if (RunParams_Pin.hv_BaseType[X_BindingNum] == mType.BaseType_Circle &&
				RunParams_Pin.hv_BaseType[Y_BindingNum] == mType.BaseType_Circle) {
				ui.textBrowser_BindingLog->append(QDateTime::currentDateTime().toString("hh:mm:ss：") + "ROI" + QString::number(i + 1) + ",绑定X基准:" +
					"圆" + QString::number(X_BindingNum + 1) + ",绑定Y基准:" + "圆" + QString::number(Y_BindingNum + 1));
			}
		}
		////Csv路径
		//ui.lineEdit_CsvPath->setText(QString::fromStdString(RunParams_Pin.FilePath_Csv));
		//灰度处理
		ui.checkBox_Gray->setChecked(HTuple(RunParams.hv_ToGray).TupleInt().I());
		if (HTuple(RunParams.hv_ToGray).TupleInt().I() == 1)
		{
			//灰度处理
			Rgb1ToGray(ho_Image, &ho_Image);
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "本地参数刷新失败!", QMessageBox::Ok);
		return;
	}
}
//设置输入参数
void PinWindow::RefreshRunparams_ROI(const RunParamsStruct_Pin& RunParams, int index1) {
	try {
		TypeParamsStruct_Pin mType;

		//刷新ROI 0的参数
		//二值化
		if (RunParams.hv_DetectType[index1] == mType.DetectType_Blob) {
			ui.cmb_PinDetectType->setCurrentText("Blob");
		}
		else {
			ui.cmb_PinDetectType->setCurrentText("模板匹配");
		}

		if (RunParams.hv_SortType[index1] == mType.SortType_Column) {
			ui.cmb_PinSortType->setCurrentText("按列排序");
		}
		else {
			ui.cmb_PinSortType->setCurrentText("按行排序");
		}

		if (RunParams.hv_ThresholdType[index1] == mType.Fixed_Threshold) {
			ui.cmb_Type_Binarization->setCurrentText("固定阈值");
		}
		else if (RunParams.hv_ThresholdType[index1] == mType.Auto_Threshold) {
			ui.cmb_Type_Binarization->setCurrentText("自适应阈值");
		}
		else {
			ui.cmb_Type_Binarization->setCurrentText("动态阈值");
		}
		ui.spb_LowThreshold->setValue(HTuple(RunParams.hv_LowThreshold[index1]).TupleInt().I());
		ui.spb_HighThreshold->setValue(HTuple(RunParams.hv_HighThreshold[index1]).TupleInt().I());
		ui.dspb_Sigma->setValue(RunParams.hv_Sigma[index1].D());
		ui.dspb_DynThresholdWidth->setValue(RunParams.hv_CoreWidth[index1].D());
		ui.dspb_DynThresholdHeight->setValue(RunParams.hv_CoreHeight[index1].D());
		ui.spb_DynThresholdContrast->setValue(HTuple(RunParams.hv_DynThresholdContrast[index1]).TupleInt().I());
		if (RunParams.hv_DynThresholdPolarity[index1] == mType.DynPolarity_Dark) {
			ui.cmb_Polarity->setCurrentText("dark");
		}
		else if (RunParams.hv_DynThresholdPolarity[index1] == mType.DynPolarity_Light) {
			ui.cmb_Polarity->setCurrentText("light");
		}
		else if (RunParams.hv_DynThresholdPolarity[index1] == mType.DynPolarity_Equal) {
			ui.cmb_Polarity->setCurrentText("equal");
		}
		else if (RunParams.hv_DynThresholdPolarity[index1] == mType.DynPolarity_NotEqual) {
			ui.cmb_Polarity->setCurrentText("not_equal");
		}

		//形态学
		if (RunParams.hv_OperatorType[index1] == mType.hv_Null) {
			ui.cmb_OperatorType->setCurrentText("null");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Opening_Rec) {
			ui.cmb_OperatorType->setCurrentText("矩形开运算");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Closing_Rec) {
			ui.cmb_OperatorType->setCurrentText("矩形闭运算");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Opening_Cir) {
			ui.cmb_OperatorType->setCurrentText("圆形开运算");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Closing_Cir) {
			ui.cmb_OperatorType->setCurrentText("圆形闭运算");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Erosion_Rec) {
			ui.cmb_OperatorType->setCurrentText("矩形腐蚀");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Dilation_Rec) {
			ui.cmb_OperatorType->setCurrentText("矩形膨胀");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Erosion_Cir) {
			ui.cmb_OperatorType->setCurrentText("圆形腐蚀");
		}
		else if (RunParams.hv_OperatorType[index1] == mType.Dilation_Cir) {
			ui.cmb_OperatorType->setCurrentText("圆形膨胀");
		}

		ui.dsb_OperatorWidth->setValue(RunParams.hv_OperaRec_Width[index1].D());
		ui.dsb_OperatorHeight->setValue(RunParams.hv_OperaRec_Height[index1].D());
		ui.dsb_OperatorRadius->setValue(RunParams.hv_OperaCir_Radius[index1].D());

		ui.dspb_AreaFilter_Min->setValue(RunParams.hv_AreaFilter_Min[index1].D());
		ui.dspb_AreaFilter_Max->setValue(RunParams.hv_AreaFilter_Max[index1].D());
		ui.dspb_ColumnFilter_Min->setValue(RunParams.hv_ColumnFilter_Min[index1].D());
		ui.dspb_ColumnFilter_Max->setValue(RunParams.hv_ColumnFilter_Max[index1].D());
		ui.dspb_RowFilter_Min->setValue(RunParams.hv_RowFilter_Min[index1].D());
		ui.dspb_RowFilter_Max->setValue(RunParams.hv_RowFilter_Max[index1].D());
		ui.dspb_RecLen1Filter_Min->setValue(RunParams.hv_RecLen1Filter_Min[index1].D());
		ui.dspb_RecLen1Filter_Max->setValue(RunParams.hv_RecLen1Filter_Max[index1].D());
		ui.dspb_RecLen2Filter_Min->setValue(RunParams.hv_RecLen2Filter_Min[index1].D());
		ui.dspb_RecLen2Filter_Max->setValue(RunParams.hv_RecLen2Filter_Max[index1].D());
		ui.dspb_CircularityFilter_Min->setValue(RunParams.hv_CircularityFilter_Min[index1].D());
		ui.dspb_CircularityFilter_Max->setValue(RunParams.hv_CircularityFilter_Max[index1].D());
		ui.dspb_RectangularityFilter_Min->setValue(RunParams.hv_RectangularityFilter_Min[index1].D());
		ui.dspb_RectangularityFilter_Max->setValue(RunParams.hv_RectangularityFilter_Max[index1].D());
		ui.dspb_WidthFilter_Min->setValue(RunParams.hv_WidthFilter_Min[index1].D());
		ui.dspb_WidthFilter_Max->setValue(RunParams.hv_WidthFilter_Max[index1].D());
		ui.dspb_HeightFilter_Min->setValue(RunParams.hv_HeightFilter_Min[index1].D());
		ui.dspb_HeightFilter_Max->setValue(RunParams.hv_HeightFilter_Max[index1].D());
		ui.dspb_FillUpShape_Min->setValue(RunParams.hv_FillUpShape_Min[index1].D());
		ui.dspb_FillUpShape_Max->setValue(RunParams.hv_FillUpShape_Max[index1].D());
		ui.spb_BlobCount->setValue(HTuple(RunParams.hv_BlobCount[index1]).TupleInt().I());

		if (RunParams.hv_SelectAreaMax[index1] == 1) {
			ui.ckb_SelectAreaMax->setCheckState(Qt::Checked);
		}
		else {
			ui.ckb_SelectAreaMax->setCheckState(Qt::Unchecked);
		}

		//模板匹配
		if (RunParams.hv_MatchMethod[index1] == mType.ShapeModel) {
			ui.cmb_MatchMethod_Train->setCurrentText("基于形状");
		}
		else {
			ui.cmb_MatchMethod_Train->setCurrentText("基于互相关");
		}
		ui.dspb_AngleStart_Train->setValue(RunParams.hv_AngleStart[index1].D());
		ui.dspb_AngleExtent_Train->setValue(RunParams.hv_AngleExtent[index1].D());
		ui.dspb_ScaleRMin_Train->setValue(RunParams.hv_ScaleRMin[index1].D());
		ui.dspb_ScaleRMax_Train->setValue(RunParams.hv_ScaleRMax[index1].D());
		ui.dspb_ScaleCMin_Train->setValue(RunParams.hv_ScaleCMin[index1].D());
		ui.dspb_ScaleCMax_Train->setValue(RunParams.hv_ScaleCMax[index1].D());
		ui.dspb_MinScore_Run->setValue(RunParams.hv_MinScore[index1].D());
		ui.spb_NumMatches_Run->setValue(HTuple(RunParams.hv_NumMatches[index1]).TupleInt().I());
		//灰度处理
		ui.checkBox_Gray->setChecked(HTuple(RunParams.hv_ToGray[index1]).TupleInt().I());
	}
	catch (...) {
		QMessageBox::information(this, "提示", "本地参数刷新失败!", QMessageBox::Ok);
		return;
	}
}
void PinWindow::RefreshRunparams_Base(const RunParamsStruct_Pin& RunParams, int index1) {
	try {
		TypeParamsStruct_Pin mType;

		//刷新基准
		if (RunParams.hv_BaseType[index1] == mType.BaseType_Line) {
			ui.cmb_BaseType->setCurrentText("直线");
			ui.dspb_RowBegin_Line->setValue(RunParams.hv_Row1_Base[index1].D());
			ui.dspb_ColBegin_Line->setValue(RunParams.hv_Column1_Base[index1].D());
			ui.dspb_RowEnd_Line->setValue(RunParams.hv_Row2_Base[index1].D());
			ui.dspb_ColEnd_Line->setValue(RunParams.hv_Column2_Base[index1].D());
		}
		else {
			ui.cmb_BaseType->setCurrentText("圆");
			ui.dspb_CenterRow_Circle->setValue(RunParams.hv_Row1_Base[index1].D());
			ui.dspb_CenterCol_Circle->setValue(RunParams.hv_Column1_Base[index1].D());
			ui.dspb_CenterRadius_Circle->setValue(RunParams.hv_Radius_Base[index1].D());
		}
		ui.spb_MeasureNum->setValue(HTuple(RunParams.hv_MeasureNum[index1]).TupleInt().I());
		ui.spb_MeasureLength1->setValue(HTuple(RunParams.hv_MeasureLength1[index1]).TupleInt().I());
		ui.spb_MeasureLength2->setValue(HTuple(RunParams.hv_MeasureLength2[index1]).TupleInt().I());
		ui.dspb_MeasureSigma->setValue(RunParams.hv_MeasureSigma[index1].D());
		ui.spb_MeasureThreshold->setValue(HTuple(RunParams.hv_MeasureThreshold[index1]).TupleInt().I());
		ui.dspb_MinScore->setValue(RunParams.hv_MeasureMinScore[index1].D());
		ui.dspb_DistanceThreshold->setValue(RunParams.hv_DistanceThreshold[index1].D());
		if (RunParams.hv_MeasureTransition[index1] == HTuple("positive")) {
			ui.cmb_MeasureTransition->setCurrentText("positive");
		}
		else if (RunParams.hv_MeasureTransition[index1] == HTuple("negative")) {
			ui.cmb_MeasureTransition->setCurrentText("negative");
		}
		else if (RunParams.hv_MeasureTransition[index1] == HTuple("all")) {
			ui.cmb_MeasureTransition->setCurrentText("all");
		}
		else if (RunParams.hv_MeasureTransition[index1] == HTuple("uniform")) {
			ui.cmb_MeasureTransition->setCurrentText("uniform");
		}

		if (RunParams.hv_MeasureSelect[index1] == HTuple("first")) {
			ui.cmb_MeasureSelect->setCurrentText("first");
		}
		else if (RunParams.hv_MeasureSelect[index1] == HTuple("last")) {
			ui.cmb_MeasureSelect->setCurrentText("last");
		}
		else if (RunParams.hv_MeasureSelect[index1] == HTuple("all")) {
			ui.cmb_MeasureSelect->setCurrentText("all");
		}

	}
	catch (...) {
		QMessageBox::information(this, "提示", "本地参数刷新失败!", QMessageBox::Ok);
		return;
	}
}
//设置输出参数
void PinWindow::SetResultparams(qint64 CT) {
	try {
		QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
		ui.plainTextEdit->appendPlainText(currenttime + ": " + "CT " + QString::number(CT) + "ms");
		if (mResultParams.hv_Ret == 1) {
			QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
			ui.plainTextEdit->appendPlainText(currenttime + ": " + "OK");
		}
		else {
			QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
			ui.plainTextEdit->appendPlainText(currenttime + ": " + "NG");
		}
		//刷新结果表格
		std::vector<std::map<int, std::string>> resultList = PinDetect1->getModubleResultList();
		int count = resultList.size();
		ui.tablewidget_ResultData->clearContents();
		ui.tablewidget_ResultData->setRowCount(count);
		for (int i = 0; i < count; i++) {
			QTableWidgetItem* item;
			for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++) {
				item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_ResultData->setItem(i, iter->first, item);
			}
		}
		//刷新图像
		count = HTuple(mResultParams.hv_RetNum).TupleInt().I();
		GenEmptyObj(&ho_ShowObj);
		if (mResultParams.ho_Region_AllPin.Key() != nullptr) {
			ConcatObj(ho_ShowObj, mResultParams.ho_Region_AllPin, &ho_ShowObj);
		}
		if (mResultParams.ho_Contour_Base.Key() != nullptr) {
			ConcatObj(ho_ShowObj, mResultParams.ho_Contour_Base, &ho_ShowObj);
		}
		if (mResultParams.ho_ModelXLD.Key() != nullptr) {
			ConcatObj(ho_ShowObj, mResultParams.ho_ModelXLD, &ho_ShowObj);
		}

		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}
		//再显示选择区域
		SetColor(m_hHalconID, "red");
		DispObj(mResultParams.ho_Region_NGPin, m_hHalconID);

		//判断是否找到区域
		if (count == 0) {
			QMessageBox::information(this, "提示", "没有找到Pin!", QMessageBox::Ok);
			return;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "结果刷新失败!", QMessageBox::Ok);
		return;
	}
}
//添加基准
void PinWindow::on_pb_AddBase_clicked() {
	try {
		TypeParamsStruct_Pin m_Type;
		if (ui.cmb_BaseType->currentText() == "直线") {
			RunParams_Pin.hv_BaseType.Append(m_Type.BaseType_Line);

			int count = ui.cmb_BaseSelect->count();
			QString nameBase = "直线" + QString::number(count + 1);
			ui.cmb_BaseSelect->addItem(nameBase);
			count = ui.cmb_BaseSelect->count();
			ui.cmb_BaseSelect->setCurrentIndex(count - 1);
		}
		else {
			RunParams_Pin.hv_BaseType.Append(m_Type.BaseType_Circle);

			int count = ui.cmb_BaseSelect->count();
			QString nameBase = "圆" + QString::number(count + 1);
			ui.cmb_BaseSelect->addItem(nameBase);
			count = ui.cmb_BaseSelect->count();
			ui.cmb_BaseSelect->setCurrentIndex(count - 1);
		}

		RunParams_Pin.hv_Row1_Base.Append(0);
		RunParams_Pin.hv_Column1_Base.Append(0);
		RunParams_Pin.hv_Row2_Base.Append(0);
		RunParams_Pin.hv_Column2_Base.Append(0);
		RunParams_Pin.hv_Radius_Base.Append(0);

		RunParams_Pin.hv_MeasureLength1.Append(50);		//数组,卡尺半长,
		RunParams_Pin.hv_MeasureLength2.Append(5);		//数组,卡尺半宽
		RunParams_Pin.hv_MeasureSigma.Append(1.0);		//数组,平滑值 
		RunParams_Pin.hv_MeasureThreshold.Append(20);	//数组,边缘阈值
		RunParams_Pin.hv_MeasureTransition.Append("all");	//数组,卡尺极性
		RunParams_Pin.hv_MeasureSelect.Append("first");		//数组,边缘选择
		RunParams_Pin.hv_MeasureNum.Append(10);		    //数组,卡尺个数
		RunParams_Pin.hv_MeasureMinScore.Append(0.5);     //数组,最小得分
		RunParams_Pin.hv_DistanceThreshold.Append(3.5);   //数组,距离阈值
	}
	catch (...) {
		QMessageBox::information(this, "提示", "添加基准失败!", QMessageBox::Ok);
		return;
	}

}
//删除基准
void PinWindow::on_pb_DeleteBase_clicked() {
	try {
		ui.cmb_BaseSelect->clear();

		RunParams_Pin.hv_BaseType.Clear();
		RunParams_Pin.hv_Row1_Base.Clear();
		RunParams_Pin.hv_Column1_Base.Clear();
		RunParams_Pin.hv_Row2_Base.Clear();
		RunParams_Pin.hv_Column2_Base.Clear();
		RunParams_Pin.hv_Radius_Base.Clear();
		RunParams_Pin.hv_MeasureLength1.Clear();		//数组,卡尺半长,
		RunParams_Pin.hv_MeasureLength2.Clear();		//数组,卡尺半宽
		RunParams_Pin.hv_MeasureSigma.Clear();		//数组,平滑值 
		RunParams_Pin.hv_MeasureThreshold.Clear();	//数组,边缘阈值
		RunParams_Pin.hv_MeasureTransition.Clear();	//数组,卡尺极性
		RunParams_Pin.hv_MeasureSelect.Clear();		//数组,边缘选择
		RunParams_Pin.hv_MeasureNum.Clear();		    //数组,卡尺个数
		RunParams_Pin.hv_MeasureMinScore.Clear();     //数组,最小得分
		RunParams_Pin.hv_DistanceThreshold.Clear();   //数组,距离阈值
	}
	catch (...) {
		QMessageBox::information(this, "提示", "清除基准失败!", QMessageBox::Ok);
		return;
	}

}
//绘制基准
void PinWindow::on_pb_DrawBase_clicked() {
	try {
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}

		int count = ui.cmb_BaseSelect->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请选择基准编号!", QMessageBox::Ok);
			return;
		}

		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		count = RunParams_Pin.hv_BaseType.TupleLength().I();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "基准为空!", QMessageBox::Ok);
			return;
		}
		TypeParamsStruct_Pin m_type;
		int index1 = ui.cmb_BaseSelect->currentIndex();

		if (RunParams_Pin.hv_BaseType[index1] == m_type.BaseType_Line) {
			HObject ho_XldLine;
			HTuple LineRow1, LineCol1, LineRow2, LineCol2;
			DrawLine(m_hHalconID, &LineRow1, &LineCol1, &LineRow2, &LineCol2);
			HTuple hv_ConcatRow, hv_ConcatCol;
			TupleConcat(LineRow1, LineRow2, &hv_ConcatRow);
			TupleConcat(LineCol1, LineCol2, &hv_ConcatCol);
			GenContourPolygonXld(&ho_XldLine, hv_ConcatRow, hv_ConcatCol);
			ConcatObj(ho_ShowObj, ho_XldLine, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(ho_XldLine, m_hHalconID);

			//画搜索方向
			ShowArrow_Line(m_hHalconID, LineRow1, LineCol1, LineRow2, LineCol2);
			//绘制直线参数赋值
			ui.dspb_RowBegin_Line->setValue(LineRow1.TupleReal().D());
			ui.dspb_ColBegin_Line->setValue(LineCol1.TupleReal().D());
			ui.dspb_RowEnd_Line->setValue(LineRow2.TupleReal().D());
			ui.dspb_ColEnd_Line->setValue(LineCol2.TupleReal().D());

			RunParams_Pin.hv_Row1_Base[index1] = LineRow1;
			RunParams_Pin.hv_Column1_Base[index1] = LineCol1;
			RunParams_Pin.hv_Row2_Base[index1] = LineRow2;
			RunParams_Pin.hv_Column2_Base[index1] = LineCol2;
			RunParams_Pin.hv_Radius_Base[index1] = 0;

		}
		else {
			HObject ho_XldCircle;
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&ho_XldCircle, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, ho_XldCircle, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(ho_XldCircle, m_hHalconID);

			//画搜索方向
			ShowArrow_Circle(m_hHalconID, CircleRow, CircleCol, CircleRadius);
			//绘制圆参数赋值
			ui.dspb_CenterRow_Circle->setValue(CircleRow.TupleReal().D());
			ui.dspb_CenterCol_Circle->setValue(CircleCol.TupleReal().D());
			ui.dspb_CenterRadius_Circle->setValue(CircleRadius.TupleReal().D());

			RunParams_Pin.hv_Row1_Base[index1] = CircleRow;
			RunParams_Pin.hv_Column1_Base[index1] = CircleCol;
			RunParams_Pin.hv_Row2_Base[index1] = 0;
			RunParams_Pin.hv_Column2_Base[index1] = 0;
			RunParams_Pin.hv_Radius_Base[index1] = CircleRadius;
		}

		RunParams_Pin.hv_MeasureLength1[index1] = ui.spb_MeasureLength1->value();		//数组,卡尺半长,
		RunParams_Pin.hv_MeasureLength2[index1] = ui.spb_MeasureLength2->value();		//数组,卡尺半宽
		RunParams_Pin.hv_MeasureSigma[index1] = ui.dspb_MeasureSigma->value();		//数组,平滑值 
		RunParams_Pin.hv_MeasureThreshold[index1] = ui.spb_MeasureThreshold->value();	//数组,边缘阈值
		RunParams_Pin.hv_MeasureTransition[index1] = HTuple(ui.cmb_MeasureTransition->currentText().toStdString().c_str());	//数组,卡尺极性
		RunParams_Pin.hv_MeasureSelect[index1] = HTuple(ui.cmb_MeasureSelect->currentText().toStdString().c_str());		//数组,边缘选择
		RunParams_Pin.hv_MeasureNum[index1] = ui.spb_MeasureNum->value();		    //数组,卡尺个数
		RunParams_Pin.hv_MeasureMinScore[index1] = ui.dspb_MinScore->value();     //数组,最小得分
		RunParams_Pin.hv_DistanceThreshold[index1] = ui.dspb_DistanceThreshold->value();   //数组,距离阈值
	}
	catch (...) {
		QMessageBox::information(this, "提示", "绘制基准失败!", QMessageBox::Ok);
		return;
	}

}
//添加工艺参数
void PinWindow::on_pb_AddValue_clicked() {
	try {
		if (ui.spb_PinNum->value() <= 0) {
			QMessageBox::information(this, "提示", "请先计算待检测Pin数量，再添加!", QMessageBox::Ok);
			return;
		}

		int count = ui.tablewidget_StandardData->rowCount();
		if (count >= ui.spb_PinNum->value()) {
			QMessageBox::information(this, "提示", "已经达到待检测Pin数量，禁止添加!", QMessageBox::Ok);
			return;
		}
		ui.tablewidget_StandardData->setRowCount(count + 1);

		QTableWidgetItem* item;
		QString str;

		//距离基准X
		str = "0.00";
		item = new QTableWidgetItem(str, 1000);
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui.tablewidget_StandardData->setItem(count, 0, item);
		//距离基准Y
		str = "0.00";
		item = new QTableWidgetItem(str, 1000);
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui.tablewidget_StandardData->setItem(count, 1, item);
		//删除行
		QPushButton* deleteButton = new QPushButton;
		deleteButton->setText("删除");
		ui.tablewidget_StandardData->setCellWidget(count, 2, deleteButton);
		connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow()));

		//添加X,Y方向标准数据
		RunParams_Pin.hv_StandardX_Array.Append(0);
		RunParams_Pin.hv_StandardY_Array.Append(0);
	}
	catch (...) {
		QMessageBox::information(this, "提示", "添加工艺参数失败!", QMessageBox::Ok);
		return;
	}
}
//清空工艺参数
void PinWindow::on_pb_ClearValue_clicked() {
	//清空结果表格数据
	ui.tablewidget_StandardData->clearContents();
	ui.tablewidget_StandardData->setRowCount(0);
	//清空X,Y方向标准数据
	RunParams_Pin.hv_StandardX_Array.Clear();
	RunParams_Pin.hv_StandardY_Array.Clear();
}
//计算Pin数量
void PinWindow::on_pb_CalculatePinNum_clicked() {
	try {
		int count = ui.cmb_SearchRegion->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请选择搜索区域编号!", QMessageBox::Ok);
			return;
		}

		HTuple numR;
		CountObj(RunParams_Pin.ho_SearchRegion, &numR);
		if (numR <= 0) {
			QMessageBox::information(this, "提示", "搜索区域为空!", QMessageBox::Ok);
			return;
		}
		int pinNumTemp = 0;
		for (int i = 0; i < count; i++) {
			TypeParamsStruct_Pin m_type;
			RunParamsStruct_Pin runParams;
			//搜索区域
			int index1 = i;
			SelectObj(RunParams_Pin.ho_SearchRegion, &runParams.ho_SearchRegion, index1 + 1);
			//区域检测类型
			runParams.hv_DetectType = RunParams_Pin.hv_DetectType[index1];
			//二值化参数赋值
			runParams.hv_SortType = RunParams_Pin.hv_SortType[index1];
			runParams.hv_ThresholdType = RunParams_Pin.hv_ThresholdType[index1];
			runParams.hv_LowThreshold = RunParams_Pin.hv_LowThreshold[index1];
			runParams.hv_HighThreshold = RunParams_Pin.hv_HighThreshold[index1];
			runParams.hv_Sigma = RunParams_Pin.hv_Sigma[index1];
			runParams.hv_CoreWidth = RunParams_Pin.hv_CoreWidth[index1];
			runParams.hv_CoreHeight = RunParams_Pin.hv_CoreHeight[index1];
			runParams.hv_DynThresholdContrast = RunParams_Pin.hv_DynThresholdContrast[index1];
			//默认找白色区域
			runParams.hv_DynThresholdPolarity = RunParams_Pin.hv_DynThresholdPolarity[index1];
			//形态学参数赋值
			runParams.hv_OperatorType = RunParams_Pin.hv_OperatorType[index1];
			runParams.hv_OperaRec_Width = RunParams_Pin.hv_OperaRec_Width[index1];
			runParams.hv_OperaRec_Height = RunParams_Pin.hv_OperaRec_Height[index1];
			runParams.hv_OperaCir_Radius = RunParams_Pin.hv_OperaCir_Radius[index1];
			//孔洞填充
			runParams.hv_FillUpShape_Min = RunParams_Pin.hv_FillUpShape_Min[index1];
			runParams.hv_FillUpShape_Max = RunParams_Pin.hv_FillUpShape_Max[index1];
			//Blob筛选参数赋值
			runParams.hv_AreaFilter_Min = RunParams_Pin.hv_AreaFilter_Min[index1];
			runParams.hv_AreaFilter_Max = RunParams_Pin.hv_AreaFilter_Max[index1];
			runParams.hv_RowFilter_Min = RunParams_Pin.hv_RowFilter_Min[index1];
			runParams.hv_RowFilter_Max = RunParams_Pin.hv_RowFilter_Max[index1];
			runParams.hv_ColumnFilter_Min = RunParams_Pin.hv_ColumnFilter_Min[index1];
			runParams.hv_ColumnFilter_Max = RunParams_Pin.hv_ColumnFilter_Max[index1];
			runParams.hv_CircularityFilter_Min = RunParams_Pin.hv_CircularityFilter_Min[index1];
			runParams.hv_CircularityFilter_Max = RunParams_Pin.hv_CircularityFilter_Max[index1];
			runParams.hv_RectangularityFilter_Min = RunParams_Pin.hv_RectangularityFilter_Min[index1];
			runParams.hv_RectangularityFilter_Max = RunParams_Pin.hv_RectangularityFilter_Max[index1];
			runParams.hv_WidthFilter_Min = RunParams_Pin.hv_WidthFilter_Min[index1];
			runParams.hv_WidthFilter_Max = RunParams_Pin.hv_WidthFilter_Max[index1];
			runParams.hv_HeightFilter_Min = RunParams_Pin.hv_HeightFilter_Min[index1];
			runParams.hv_HeightFilter_Max = RunParams_Pin.hv_HeightFilter_Max[index1];
			runParams.hv_RecLen1Filter_Min = RunParams_Pin.hv_RecLen1Filter_Min[index1];
			runParams.hv_RecLen1Filter_Max = RunParams_Pin.hv_RecLen1Filter_Max[index1];
			runParams.hv_RecLen2Filter_Min = RunParams_Pin.hv_RecLen2Filter_Min[index1];
			runParams.hv_RecLen2Filter_Max = RunParams_Pin.hv_RecLen2Filter_Max[index1];
			runParams.hv_SelectAreaMax = RunParams_Pin.hv_SelectAreaMax[index1];
			runParams.hv_BlobCount = RunParams_Pin.hv_BlobCount[index1];
			//排序方式
			runParams.hv_SortType = RunParams_Pin.hv_SortType[index1];

			//模板匹配参数
			runParams.hv_MatchMethod = RunParams_Pin.hv_MatchMethod[index1];
			runParams.hv_AngleStart = RunParams_Pin.hv_AngleStart[index1];            //起始角度
			runParams.hv_AngleExtent = RunParams_Pin.hv_AngleExtent[index1];                //角度范围
			runParams.hv_ScaleRMin = RunParams_Pin.hv_ScaleRMin[index1];                 //最小行缩放
			runParams.hv_ScaleRMax = RunParams_Pin.hv_ScaleRMax[index1];                 //最大行缩放
			runParams.hv_ScaleCMin = RunParams_Pin.hv_ScaleCMin[index1];                 //最小列缩放
			runParams.hv_ScaleCMax = RunParams_Pin.hv_ScaleCMax[index1];				   //最大列缩放
			runParams.hv_MinScore = RunParams_Pin.hv_MinScore[index1];					 //最低分数
			runParams.hv_NumMatches = RunParams_Pin.hv_NumMatches[index1];               //匹配个数，0则自动选择，100则最多匹配100个
			runParams.hv_ModelID = RunParams_Pin.hv_ModelID[index1];					//训练模型赋值

			int retNum = 0;
			//运行测试
			if (runParams.hv_DetectType == m_type.DetectType_Blob) {
				//Blob运行
				HObject regionPin;
				HTuple rowPin, columnPin;
				HTuple areaBlob, rowBlob, columnBlob, recLen1Blob, recLen2Blob, circularityBlob, rectangularityBlob, widthBlob, heightBlob;
				PinDetect1->PinBlob(ho_Image, runParams, regionPin, rowPin, columnPin,
					areaBlob, rowBlob, columnBlob, recLen1Blob, recLen2Blob, circularityBlob, rectangularityBlob, widthBlob, heightBlob);
				retNum = rowPin.TupleLength().I();


			}
			else {
				HObject ModelRegion, ModelXLD;
				HTuple row, col, angle, score;
				PinDetect1->FindTemplate(ho_Image, 1, runParams, ModelXLD, ModelRegion, row, col, angle, score);
				retNum = row.TupleLength().I();
			}
			if (retNum == 0) {
				QMessageBox::information(this, "提示", "Pin数量计算失败!", QMessageBox::Ok);
				return;
			}
			pinNumTemp += retNum;
		}
		ui.spb_PinNum->setValue(pinNumTemp);
	}
	catch (...) {
		QMessageBox::information(this, "提示", "Pin数量计算失败!", QMessageBox::Ok);
		return;
	}
}
//工艺参数一键导入
void PinWindow::on_pb_ImportStandardData_clicked() {
	try {
		int count = HTuple(mResultParams.hv_RetNum).TupleInt().I();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请先运行Pin检测后，再一键导入!", QMessageBox::Ok);
			return;
		}
		if (ui.spb_PinNum->value() <= 0) {
			QMessageBox::information(this, "提示", "请先计算待检测Pin数量，再一键导入!", QMessageBox::Ok);
			return;
		}
		if (count != ui.spb_PinNum->value()) {
			QMessageBox::information(this, "提示", "待检测Pin数量与Pin检测结果数量不一致!", QMessageBox::Ok);
			return;
		}
		//刷新所有Pin针数据
		if (count > 0) {
			//清空结果表格数据
			ui.tablewidget_StandardData->clearContents();
			ui.tablewidget_StandardData->setRowCount(0);
			//清空X,Y方向标准数据
			RunParams_Pin.hv_StandardX_Array.Clear();
			RunParams_Pin.hv_StandardY_Array.Clear();

			for (int i = 0; i < count; i++) {
				int Num = ui.tablewidget_StandardData->rowCount();
				ui.tablewidget_StandardData->setRowCount(Num + 1);

				QTableWidgetItem* item;
				QString str;

				//距离基准X
				str = QString::number(mResultParams.hv_DisX_MM_Array[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_StandardData->setItem(Num, 0, item);
				//距离基准Y
				str = QString::number(mResultParams.hv_DisY_MM_Array[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_StandardData->setItem(Num, 1, item);
				//删除行
				QPushButton* deleteButton = new QPushButton;
				deleteButton->setText("删除");
				ui.tablewidget_StandardData->setCellWidget(Num, 2, deleteButton);
				connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow()));

				//添加X,Y方向标准数据
				RunParams_Pin.hv_StandardX_Array.Append(mResultParams.hv_DisX_MM_Array[i]);
				RunParams_Pin.hv_StandardY_Array.Append(mResultParams.hv_DisY_MM_Array[i]);
			}
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "参数一键导入失败!", QMessageBox::Ok);
		return;
	}

}
//一键设置默认工艺参数
void PinWindow::on_pb_SetDefaultStandardData_clicked() {
	try {
		int count = ui.spb_PinNum->value();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请先计算待检测Pin数量，再设置默认参数!", QMessageBox::Ok);
			return;
		}
		//刷新所有Pin针数据
		if (count > 0) {
			//清空结果表格数据
			ui.tablewidget_StandardData->clearContents();
			ui.tablewidget_StandardData->setRowCount(0);
			//清空X,Y方向标准数据
			RunParams_Pin.hv_StandardX_Array.Clear();
			RunParams_Pin.hv_StandardY_Array.Clear();

			for (int i = 0; i < count; i++) {
				int Num = ui.tablewidget_StandardData->rowCount();
				ui.tablewidget_StandardData->setRowCount(Num + 1);

				QTableWidgetItem* item;
				QString str;

				//距离基准X
				str = "0.00";
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_StandardData->setItem(Num, 0, item);
				//距离基准Y
				str = "0.00";
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_StandardData->setItem(Num, 1, item);
				//删除行
				QPushButton* deleteButton = new QPushButton;
				deleteButton->setText("删除");
				ui.tablewidget_StandardData->setCellWidget(Num, 2, deleteButton);
				connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow()));

				//添加X,Y方向标准数据
				RunParams_Pin.hv_StandardX_Array.Append(0);
				RunParams_Pin.hv_StandardY_Array.Append(0);
			}
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "默认参数设置失败!", QMessageBox::Ok);
		return;
	}

}
//基准表格删除行槽函数
void PinWindow::slot_DeleteRow() {
	try {
		int selectRow = ui.tablewidget_StandardData->currentRow();
		if (selectRow < 0) {
			QMessageBox::information(this, "提示", "请选择被删除项!", QMessageBox::Ok);
			return;
		}
		ui.tablewidget_StandardData->removeRow(selectRow);
		//删除X,Y方向标准数据
		RunParams_Pin.hv_StandardX_Array.TupleRemove(selectRow);
		RunParams_Pin.hv_StandardY_Array.TupleRemove(selectRow);
	}
	catch (...) {
		QMessageBox::information(this, "提示", "基准删除失败!", QMessageBox::Ok);
		return;
	}

}

//刷新绑定基准的控件
void PinWindow::on_pb_RefreshBinding_clicked() {
	try {
		int countRegion = ui.cmb_SearchRegion->count();
		if (countRegion <= 0) {
			QMessageBox::information(this, "提示", "Pin检测区域为空!", QMessageBox::Ok);
			return;
		}
		int countBase = ui.cmb_BaseSelect->count();
		if (countBase <= 0) {
			QMessageBox::information(this, "提示", "基准为空!", QMessageBox::Ok);
			return;
		}
		ui.cmb_PinRegionBinding->clear();
		ui.cmb_BaseXBinding->clear();
		ui.cmb_BaseYBinding->clear();
		ui.textBrowser_BindingLog->clear();
		for (int i = 0; i < countRegion; i++) {
			QString name = ui.cmb_SearchRegion->itemText(i);
			ui.cmb_PinRegionBinding->addItem(name);
		}
		ui.cmb_PinRegionBinding->setCurrentIndex(0);
		for (int i = 0; i < countBase; i++) {
			QString name = ui.cmb_BaseSelect->itemText(i);
			ui.cmb_BaseXBinding->addItem(name);
			ui.cmb_BaseYBinding->addItem(name);
		}
		ui.cmb_BaseXBinding->setCurrentIndex(0);
		ui.cmb_BaseYBinding->setCurrentIndex(0);
		QMessageBox::information(this, "提示", "刷新完成，请重新绑定所有的基准!", QMessageBox::Ok);
	}
	catch (...) {
		QMessageBox::information(this, "提示", "基准刷新失败!", QMessageBox::Ok);
		return;
	}

}
//绑定基准
void PinWindow::on_pb_Binding_clicked() {
	try {
		int count = ui.cmb_PinRegionBinding->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "待绑定Pin检测区域为空!", QMessageBox::Ok);
			return;
		}
		count = ui.cmb_BaseXBinding->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "待绑定X基准为空!", QMessageBox::Ok);
			return;
		}
		count = ui.cmb_BaseYBinding->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "待绑定Y基准为空!", QMessageBox::Ok);
			return;
		}
		if (RunParams_Pin.hv_X_BindingNum.TupleLength() <= 0 || RunParams_Pin.hv_Y_BindingNum.TupleLength() <= 0) {
			QMessageBox::information(this, "提示", "待绑定XY基准编号的数组为空!", QMessageBox::Ok);
			return;
		}

		int index = ui.cmb_PinRegionBinding->currentIndex();
		RunParams_Pin.hv_X_BindingNum[index] = ui.cmb_BaseXBinding->currentIndex();
		RunParams_Pin.hv_Y_BindingNum[index] = ui.cmb_BaseYBinding->currentIndex();
		ui.textBrowser_BindingLog->append(QDateTime::currentDateTime().toString("hh:mm:ss：") + ui.cmb_PinRegionBinding->currentText() + ",绑定X基准:" +
			ui.cmb_BaseXBinding->currentText() + ",绑定Y基准:" + ui.cmb_BaseYBinding->currentText());
	}
	catch (...) {
		QMessageBox::information(this, "提示", "基准绑定失败!", QMessageBox::Ok);
		return;
	}

}
//设置标准参数
void PinWindow::on_pb_SetStandardData_clicked() {
	try {
		int count = ui.tablewidget_StandardData->rowCount();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "工艺参数表格为空!", QMessageBox::Ok);
			return;
		}
		if (count != ui.spb_PinNum->value()) {
			QMessageBox::information(this, "提示", "待检测Pin数量与工艺参数表格数量不一致!", QMessageBox::Ok);
			return;
		}
		//运行参数结构体赋值
		RunParams_Pin.hv_StandardX_Array.Clear();
		RunParams_Pin.hv_StandardY_Array.Clear();
		RunParams_Pin.hv_Tolerance_X = ui.dspb_Tolerance_X->value();
		RunParams_Pin.hv_Tolerance_Y = ui.dspb_Tolerance_Y->value();
		RunParams_Pin.hv_MmPixel = ui.dspb_MmPixel->value();

		for (int i = 0; i < count; i++) {
			//获取X,Y方向标准数据
			RunParams_Pin.hv_StandardX_Array.Append(ui.tablewidget_StandardData->model()->index(i, 0).data().toString().toDouble());
			RunParams_Pin.hv_StandardY_Array.Append(ui.tablewidget_StandardData->model()->index(i, 1).data().toString().toDouble());
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "标准工艺参数设置失败!", QMessageBox::Ok);
		return;
	}
}
//Pin针检测运行
void PinWindow::on_pb_Run_clicked() {
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}

	//运行
	int ret = slot_PinDetect();

	QDateTime startTime = QDateTime::currentDateTime();
	//HObject ho_Image_Copy;
	//CopyImage(ho_Image, &ho_Image_Copy); // 深拷贝

	//输出图片到主页
#ifdef DEBUG_LHC
	PointItemConfig a;
	mResultParams.ho_Image = ho_Image;
	emit showPinDeubg_signal(mResultParams, a);
#endif
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	//刷新结果
	SetResultparams(intervalTimeMS);
}

//找pin槽函数
int PinWindow::slot_PinDetect() {
	try {
		int ret = 1;
		//XML读取参数
		dllDatas1->setConfigPath(XmlPath);
		dllDatas1->setGetConfigPath(ConfigPath);
		ErrorCode_Xml errorCode = dllDatas1->ReadParams_PinDetect(RunParams_Pin, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml) {
			//XML读取失败
			return 1;
		}
		//调用算法
		PinDetect1->resultParams.ngIdx.clear(); //清一下ng模块
		int result = PinDetect1->PinDetectFunc(ho_Image, RunParams_Pin, PinDetect1->resultParams);
		mResultParams = PinDetect1->resultParams;
		return result;
	}
	catch (...) {
		return -1;
	}
}
//保存参数
void PinWindow::on_pb_SaveData_clicked() {
	TypeParamsStruct_Pin m_type;
	//if (ui.lineEdit_CsvPath->text().trimmed() == "") {
	//	QMessageBox::information(this, "提示", "Csv路径为空,保存失败", QMessageBox::Ok);
	//	return;
	//}
	//RunParams_Pin.FilePath_Csv = ui.lineEdit_CsvPath->text().trimmed().toStdString();

	//位置修正参数
	if (ui.cmb_MatchMethod_Train_2->currentText() == "基于形状") {
		RunParams_Pin.hv_MatchMethod_PositionCorrection = m_type.ShapeModel;      //模板类型:"基于形状
	}
	else {
		RunParams_Pin.hv_MatchMethod_PositionCorrection = m_type.NccModel;      //模板类型:"基于互相关
	}
	if (ui.cmb_SortType->currentText() == "按行排序") {
		RunParams_Pin.hv_SortType_PositionCorrection = m_type.SortType_Row;      //排序类型:"按行排序
	}
	else {
		RunParams_Pin.hv_SortType_PositionCorrection = m_type.SortType_Column;      //排序类型:"按列排序
	}
	RunParams_Pin.hv_AngleStart_PositionCorrection = ui.dspb_AngleStart_Train_2->value();            //起始角度
	RunParams_Pin.hv_AngleExtent_PositionCorrection = ui.dspb_AngleExtent_Train_2->value();                //角度范围
	RunParams_Pin.hv_ScaleRMin_PositionCorrection = ui.dspb_ScaleRMin_Train_2->value();                 //最小行缩放
	RunParams_Pin.hv_ScaleRMax_PositionCorrection = ui.dspb_ScaleRMax_Train_2->value();                 //最大行缩放
	RunParams_Pin.hv_ScaleCMin_PositionCorrection = ui.dspb_ScaleCMin_Train_2->value();                 //最小列缩放
	RunParams_Pin.hv_ScaleCMax_PositionCorrection = ui.dspb_ScaleCMax_Train_2->value();				   //最大列缩放
	RunParams_Pin.hv_MinScore_PositionCorrection = ui.dspb_MinScore_Run_2->value();               //最低分数
	RunParams_Pin.hv_NumMatches_PositionCorrection = ui.spb_NumMatches_Run_2->value();               //匹配个数，0则自动选择，100则最多匹配100个
	RunParams_Pin.hv_Row_PositionCorrection = ui.dspb_Row_PositionCorrection->value();
	RunParams_Pin.hv_Column_PositionCorrection = ui.dspb_Column_PositionCorrection->value();
	RunParams_Pin.hv_Angle_PositionCorrection = ui.dspb_Angle_PositionCorrection->value();
	if (ui.ckb_PositionCorrection->checkState() == Qt::Checked) {
		RunParams_Pin.hv_Check_PositionCorrection = 1;
	}
	else {
		RunParams_Pin.hv_Check_PositionCorrection = 0;
	}

	//灰度处理
	if (ui.checkBox_Gray->isChecked())
	{
		RunParams_Pin.hv_ToGray = 1;
	}
	else
	{
		RunParams_Pin.hv_ToGray = 0;
	}

	int ret = 1;
	//把保存参数改为自定义文件保存
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/2D_formulation";

	QDir dir2(folderPath);

	// 如果文件夹不存在，则创建文件夹
	if (!dir2.exists()) {
		if (!dir2.mkpath(".")) {
			QMessageBox::critical(nullptr, tr("Error"), tr("Failed to create the folder."));
			return;
		}
	}

	// 打开文件保存对话框
	QString filePath = QFileDialog::getSaveFileName(
		nullptr,
		tr("Save File"),
		folderPath,
		tr("XML Files (*.xml)")
	);

	// 如果用户没有选择文件，或者点击了取消
	if (filePath.isEmpty()) {
		return;
	}

	// 确保文件扩展名是 .xml
	if (!filePath.endsWith(".xml", Qt::CaseInsensitive)) {
		filePath += ".xml";
	}

	// 确保目录存在
	QDir dir(QFileInfo(filePath).absolutePath());
	if (!dir.exists()) {
		if (!dir.mkpath(".")) {
			QMessageBox::critical(nullptr, tr("Error"), tr("Failed to create the folder."));
			return;
		}
	}

	QFileInfo fileInfo(filePath);
	// 返回文件的绝对目录路径
	QString configPath = fileInfo.absoluteDir().path();
	if (!configPath.endsWith('/')) {
		configPath += '/';
	}

	//给到原来的存储路径
	ConfigPath = configPath.toLocal8Bit().data();
	XmlPath = filePath.toLocal8Bit().data();

	dllDatas1->setGetConfigPath(ConfigPath);
	dllDatas1->setConfigPath(XmlPath);

	//250304 模板匹配会用到这个 nodeName ；
	nodeName = fileInfo.baseName().toUtf8().data();
	
	//保存参数
	ret = dllDatas1->WriteParams_PinDetect(RunParams_Pin, processId, nodeName, processModbuleID);
	//refreshConfig(processId, processModbuleID, nodeName);
	if (ret != 0) {
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
}
//读取参数
void PinWindow::on_pb_ReadData_clicked() {

	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/2D_formulation";

	QDir dir2(folderPath);

	// 如果文件夹不存在，则创建文件夹
	if (!dir2.exists()) {
		if (!dir2.mkpath(".")) {
			QMessageBox::critical(nullptr, tr("Error"), tr("Failed to create the folder."));
			return;
		}
	}

	QString filePath = QFileDialog::getOpenFileName(
		nullptr,
		tr("Choose formula"),
		folderPath,
		tr("XML Files (*.xml)")
	);

	QFileInfo fileInfo(filePath);
	// 返回文件的绝对目录路径
	QString configPath = fileInfo.absoluteDir().path();
	if (!configPath.endsWith('/')) {
		configPath += '/';
	}

	ConfigPath = configPath.toLocal8Bit().data();
	XmlPath = filePath.toLocal8Bit().data();
	dllDatas1->setGetConfigPath(ConfigPath);
	dllDatas1->setConfigPath(XmlPath);
	//250304 模板匹配会用到这个 nodeName ；
	nodeName = fileInfo.baseName().toUtf8().data();

	ErrorCode_Xml errorCode;
	//读取参数
	errorCode = dllDatas1->ReadParams_PinDetect(RunParams_Pin, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml) {
		QMessageBox::information(this, "提示", "参数读取失败", QMessageBox::Ok);
		return;
	}
	//参数更新到窗口
	RefreshRunparams(RunParams_Pin);
}
//绘制箭头
void PinWindow::ShowArrow_Line(HTuple windowId, HTuple LineRow1, HTuple LineCol1, HTuple LineRow2, HTuple LineCol2) {
	try {
		//显示卡尺搜索方向
		HTuple hv_AngleLine;
		AngleLx(LineRow1, LineCol1, LineRow2, LineCol2, &hv_AngleLine);
		//箭头的起始点坐标(计算方式：以卡尺半长为斜边，以绘制的直线与水平轴夹角为角度。分别计算斜边的cos值给x,即col。计算斜边的sin值给y，即row)
		HTuple measureLength1 = HTuple(ui.spb_MeasureLength1->value());
		HTuple hv_Cos, hv_Sin, hv_ArrowRow1, hv_ArrowCol1, hv_ArrowRow2, hv_ArrowCol2;
		TupleCos(hv_AngleLine, &hv_Cos);
		TupleSin(hv_AngleLine, &hv_Sin);
		hv_ArrowRow1 = (LineRow1 + LineRow2) / 2 - measureLength1 * hv_Cos;
		hv_ArrowCol1 = (LineCol1 + LineCol2) / 2 - measureLength1 * hv_Sin;
		hv_ArrowRow2 = (LineRow1 + LineRow2) / 2 + measureLength1 * hv_Cos;
		hv_ArrowCol2 = (LineCol1 + LineCol2) / 2 + measureLength1 * hv_Sin;

		//显示搜索方向
		SetColor(m_hHalconID, "green");
		DispArrow(windowId, hv_ArrowRow1, hv_ArrowCol1, hv_ArrowRow2, hv_ArrowCol2, 5);
		//显示直线方向
		SetColor(m_hHalconID, "red");
		DispArrow(windowId, LineRow1, LineCol1, LineRow2, LineCol2, 5);
	}
	catch (...) {

	}

}
//绘制箭头
void PinWindow::ShowArrow_Circle(HTuple windowId, HTuple CircleRow, HTuple CircleCol, HTuple CircleRadius) {
	try {
		//显示卡尺搜索方向
//箭头的起始点坐标(计算方式：以圆心为起点，终点坐标等于圆心坐标加上半径后的坐标)
		HTuple hv_ArrowRow1, hv_ArrowCol1, hv_ArrowRow2, hv_ArrowCol2;
		hv_ArrowRow1 = CircleRow;
		hv_ArrowCol1 = CircleCol;
		hv_ArrowRow2 = CircleRow - CircleRadius - 50;
		hv_ArrowCol2 = CircleCol;

		//显示搜索方向
		SetColor(m_hHalconID, "green");
		DispArrow(windowId, hv_ArrowRow1, hv_ArrowCol1, hv_ArrowRow2, hv_ArrowCol2, 5);
	}
	catch (...) {

	}

}

//绘制模板训练区域
void PinWindow::on_btn_DrawTrainRoi_Template_clicked() {
	try {
		ui.lbl_Window->SetActionEnable(false);
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);
		if (ui.cmb_ShapeSelect->currentText() == "矩形") {
			HObject TempRoi;
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "仿射矩形") {
			HObject TempRoi;
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&TempRoi, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "圆形") {
			HObject TempRoi;
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&TempRoi, CircleRow, CircleCol, CircleRadius);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "椭圆") {
			HObject TempRoi;
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&TempRoi, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "多边形") {
			HObject TempRoi;
			DrawRegion(&TempRoi, m_hHalconID);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else {
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
			return;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "训练区域绘制失败!", QMessageBox::Ok);
		return;
	}

}
//绘制掩模区域
void PinWindow::on_btn_DrawShaddowRoi_Template_clicked() {
	try {
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "green");
		SetLineWidth(m_hHalconID, 1);
		if (ui.cmb_ShapeSelect->currentText() == "矩形") {
			HObject TempRoi;
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "仿射矩形") {
			HObject TempRoi;
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&TempRoi, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "圆形") {
			HObject TempRoi;
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&TempRoi, CircleRow, CircleCol, CircleRadius);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "椭圆") {
			HObject TempRoi;
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&TempRoi, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect->currentText() == "多边形") {
			HObject TempRoi;
			DrawRegion(&TempRoi, m_hHalconID);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else {
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
			return;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "掩膜区域绘制失败!", QMessageBox::Ok);
		return;
	}

}
//合并模板区域Roi
void PinWindow::on_btn_UnionRoi_Template_clicked() {
	try {
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		if (vecTemplateRegions.isEmpty()) {
			QMessageBox::information(this, "提示", "模板训练区域ROI为空!", QMessageBox::Ok);
			return;
		}
		else {
			SetColor(m_hHalconID, "red");
			SetLineWidth(m_hHalconID, 1);
			HObject TrainRoi_Template;
			//训练区域合并
			if (vecTemplateRegions.count() == 1) {
				TrainRoi_Template = vecTemplateRegions[0];
			}
			else {
				TrainRoi_Template = vecTemplateRegions[0];
				for (int i = 1; i < vecTemplateRegions.count(); i++) {
					Union2(TrainRoi_Template, vecTemplateRegions[i], &TrainRoi_Template);
				}
			}

			//掩模区域合并
			HObject ShadowRoi_Template;
			if (!vecShadowRegions.isEmpty()) {
				if (vecShadowRegions.count() == 1) {
					ShadowRoi_Template = vecShadowRegions[0];
				}
				else {
					ShadowRoi_Template = vecShadowRegions[0];
					for (int i = 1; i < vecShadowRegions.count(); i++) {
						Union2(ShadowRoi_Template, vecShadowRegions[i], &ShadowRoi_Template);
					}
				}
				//窗口显示图形赋值
				ConcatObj(ho_ShowObj, ShadowRoi_Template, &ho_ShowObj);
			}

			//抠出掩模区域
			if (!vecShadowRegions.isEmpty()) {
				Difference(TrainRoi_Template, ShadowRoi_Template, &TrainRoi_Template);
			}

			//当前绘制的区域数组里面的下标值
			int index1 = ui.cmb_SearchRegion->currentIndex();
			//替换指定索引的搜索区域
			HObject replaceRegion;
			ReplaceObj(RunParams_Pin.ho_TrainRegion, TrainRoi_Template, &replaceRegion, index1 + 1);
			RunParams_Pin.ho_TrainRegion = replaceRegion;

			//窗口显示图形赋值
			ConcatObj(ho_ShowObj, TrainRoi_Template, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();

			QMessageBox::information(this, "提示", "合并ROI成功，训练区域数量为:" + QString::number(vecTemplateRegions.count()), QMessageBox::Ok);

			//清除vector内存
			vecTemplateRegions.clear();
			vecShadowRegions.clear();
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "合并ROI失败", QMessageBox::Ok);
		return;
	}

}
//清除模板区域Roi
void PinWindow::on_btn_ClearRoi_Template_clicked() {
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	//清空区域集合
	vecTemplateRegions.clear();
	vecShadowRegions.clear();
	//清空窗口显示的图形变量
	GenEmptyObj(&ho_ShowObj);
	//显示原图
	showImg(ho_Image, "margin", "green");
}

//训练模板
void PinWindow::on_btn_Train_Template_clicked() {
	try {
		// --------------------- 1. 输入检查 ---------------------
		// 检查图像是否有效
		if (ho_Image.Key() == nullptr) {
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}

		// 检查区域编号下拉框是否有选项
		int count = ui.cmb_SearchRegion->count();
		if (count <= 0) {
			QMessageBox::information(this, "提示", "请选择区域编号!", QMessageBox::Ok);
			return;
		}

		// 检查搜索区域是否有效
		HTuple numR;
		CountObj(RunParams_Pin.ho_SearchRegion, &numR);
		if (numR <= 0) {
			QMessageBox::information(this, "提示", "Pin搜索区域为空!", QMessageBox::Ok);
			return;
		}

		// --------------------- 2. 初始化参数 ---------------------
		BlobCountShow = 0;          // 清空显示的数量
		GenEmptyObj(&BlobRegionShow); // 清空显示区域对象

		TypeParamsStruct_Pin m_type;   // 类型参数（未使用）
		RunParamsStruct_Pin runParams; // 运行时参数

		// 根据当前选择的区域索引，提取对应的搜索/训练区域和参数
		int index1 = ui.cmb_SearchRegion->currentIndex();
		SelectObj(RunParams_Pin.ho_SearchRegion, &runParams.ho_SearchRegion, index1 + 1); // 搜索区域
		SelectObj(RunParams_Pin.ho_TrainRegion, &runParams.ho_TrainRegion, index1 + 1);    // 训练区域
		runParams.hv_SortType = RunParams_Pin.hv_SortType[index1];    // 排序方式（如按分数降序）
		runParams.hv_DetectType = RunParams_Pin.hv_DetectType[index1];// 检测类型（如形状匹配/NCC）

		// --------------------- 3. 配置模板匹配参数 ---------------------
		runParams.hv_MatchMethod = RunParams_Pin.hv_MatchMethod[index1];  // 匹配方法（如 'shape'/'ncc'）
		runParams.hv_AngleStart = RunParams_Pin.hv_AngleStart[index1];    // 起始角度（弧度）
		runParams.hv_AngleExtent = RunParams_Pin.hv_AngleExtent[index1];  // 角度范围（弧度）
		runParams.hv_ScaleRMin = RunParams_Pin.hv_ScaleRMin[index1];      // 行方向最小缩放比例
		runParams.hv_ScaleRMax = RunParams_Pin.hv_ScaleRMax[index1];      // 行方向最大缩放比例
		runParams.hv_ScaleCMin = RunParams_Pin.hv_ScaleCMin[index1];      // 列方向最小缩放比例
		runParams.hv_ScaleCMax = RunParams_Pin.hv_ScaleCMax[index1];      // 列方向最大缩放比例
		runParams.hv_MinScore = RunParams_Pin.hv_MinScore[index1];        // 最低匹配分数（0~1）
		runParams.hv_NumMatches = RunParams_Pin.hv_NumMatches[index1];    // 最大匹配数量（0表示不限）

		// --------------------- 4. 创建模板 ---------------------
		int ret = PinDetect1->CreateTemplate(ho_Image, runParams, 1); // 调用模板创建接口
		if (ret != 0) {
			QMessageBox::information(this, "提示", "模板训练失败", QMessageBox::Ok);
			return;
		}

		// 保存模板ID到全局参数
		RunParams_Pin.hv_ModelID[index1] = runParams.hv_ModelID;

		// --------------------- 5. 查找模板 ---------------------
		HObject ModelRegion, ModelXLD;       // 匹配区域和轮廓对象
		HTuple row, col, angle, score;       // 匹配结果：行列坐标、角度、分数
		ret = PinDetect1->FindTemplate(ho_Image, 1, runParams, ModelXLD, ModelRegion, row, col, angle, score);
		if (ret != 0) {
			QMessageBox::information(this, "提示", "查找模板失败", QMessageBox::Ok);
			return;
		}

		// --------------------- 6. 处理匹配结果 ---------------------
		count = row.TupleLength().I();  // 匹配到的数量
		BlobRegionShow = ModelRegion;       // 更新显示区域
		BlobCountShow = count;              // 更新数量

		if (count <= 0) {
			// 未找到Pin：显示搜索区域轮廓并提示
			GenEmptyObj(&ho_ShowObj);
			HObject roiContour;
			GenContourRegionXld(runParams.ho_SearchRegion, &roiContour, "border"); // 生成搜索区域轮廓
			ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj); // 合并到显示对象

			// 根据复选框状态显示填充或边缘
			//if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			//	showImg(ho_Image, "fill", "red"); // 填充显示（红色）
			//}
			//else {
			//	showImg(ho_Image, "margin", "red"); // 边缘显示（红色）
			//}
			showImg(ho_Image, "margin", "red"); // 边缘显示（红色）
			QMessageBox::information(this, "提示", "没有找到Pin!", QMessageBox::Ok);
			return;
		}

		// --------------------- 7. 显示匹配结果 ---------------------
		GenEmptyObj(&ho_ShowObj);
		ConcatObj(ho_ShowObj, ModelRegion, &ho_ShowObj); // 合并匹配区域到显示对象

		HObject roiContour;
		GenContourRegionXld(runParams.ho_SearchRegion, &roiContour, "border"); // 生成搜索区域轮廓
		ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj); // 合并轮廓到显示对象

		// 根据复选框状态显示填充或边缘（绿色表示成功）
		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}

		// --------------------- 8. 更新结果表格 ---------------------
		if (count > 0) {
			ui.tablewidget_Results_Template->clearContents();
			ui.tablewidget_Results_Template->setRowCount(count);
			for (int i = 0; i < count; i++) {
				QTableWidgetItem* item;
				QString str;

				// X坐标（列）
				str = QString::number(col[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template->setItem(i, 0, item);

				// Y坐标（行）
				str = QString::number(row[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template->setItem(i, 1, item);

				// 角度（弧度转角度？需确认单位）
				str = QString::number(angle[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template->setItem(i, 2, item);

				// 分数（匹配置信度）
				str = QString::number(score[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template->setItem(i, 3, item);
			}
		}
	}
	catch (...) { // 捕获所有异常（需优化为具体异常类型）
		QMessageBox::information(this, "提示", "Pin模板训练失败!", QMessageBox::Ok);
		return;
	}
}

//切换ROI
void PinWindow::on_cmb_SearchRegion_activated(const QString& arg1) {
	try {
		//声明临时结构体变量
		RunParamsStruct_Pin structTemp;
		structTemp = RunParams_Pin;
		//判断是否启用位置矫正
		if (ui.ckb_PositionCorrection->checkState() == Qt::Checked) {
			//这里需要再试赋值，因为参数会被设置
			on_pb_SetData_PositionCorrection_clicked();
			structTemp = RunParams_Pin;

			int ret = 1;
			//查找模板
			HObject ModelRegion, ModelXLD;
			HTuple rowRet, colRet, angleRet, scoreRet;
			ret = PinDetect1->FindTemplate(ho_Image, 0, structTemp, ModelXLD, ModelRegion, rowRet, colRet, angleRet, scoreRet);
			if (ret == 0 && rowRet.TupleLength() == 1) {
				HTuple hv_ROIHomMat2D = NULL;//仿射变换ROI跟随
				VectorAngleToRigid(structTemp.hv_Row_PositionCorrection, structTemp.hv_Column_PositionCorrection,
					structTemp.hv_Angle_PositionCorrection.TupleRad(),
					rowRet, colRet, angleRet.TupleRad(),
					&hv_ROIHomMat2D);
				//仿射变换
				AffineTransRegion(structTemp.ho_SearchRegion, &structTemp.ho_SearchRegion, hv_ROIHomMat2D, "nearest_neighbor");
			}
		}
		//刷新ROI
		int index1 = ui.cmb_SearchRegion->currentIndex();
		HTuple count;
		CountObj(structTemp.ho_SearchRegion, &count);
		if (count > 0) {
			RefreshRunparams_ROI(structTemp, index1);

			GenEmptyObj(&ho_ShowObj);
			HObject roiContour, roiRegion;
			SelectObj(structTemp.ho_SearchRegion, &roiRegion, index1 + 1);
			GenContourRegionXld(roiRegion, &roiContour, "border");
			ConcatObj(ho_ShowObj, roiContour, &ho_ShowObj);
			//刷新图像
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "ROI切换失败!", QMessageBox::Ok);
		return;
	}

}
//切换基准
void PinWindow::on_cmb_BaseSelect_activated(const QString& arg1) {
	try {
		int index1 = ui.cmb_BaseSelect->currentIndex();
		RefreshRunparams_Base(RunParams_Pin, index1);
	}
	catch (...) {

	}
}


//选择保存CSV路径
void PinWindow::on_pb_CsvPath_clicked() {
	try {
		QString directory = QFileDialog::getExistingDirectory(nullptr, QObject::tr("Open Directory"), QDir::homePath());
		if (!directory.isEmpty()) {
			RunParams_Pin.FilePath_Csv = directory.toStdString() + "/";
			ui.lineEdit_CsvPath->setText(QString::fromStdString(RunParams_Pin.FilePath_Csv));
		}
		else {
			QMessageBox::information(this, "提示", "选择路径失败!", QMessageBox::Ok);
			return;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "选择路径失败!", QMessageBox::Ok);
		return;
	}
}

//位置修正绘制模板训练区域
void PinWindow::on_btn_DrawTrainRoi_Template_2_clicked() {
	try {
		ui.lbl_Window->SetActionEnable(false);
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);
		if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "矩形") {
			HObject TempRoi;
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "仿射矩形") {
			HObject TempRoi;
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&TempRoi, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "圆形") {
			HObject TempRoi;
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&TempRoi, CircleRow, CircleCol, CircleRadius);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "椭圆") {
			HObject TempRoi;
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&TempRoi, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "多边形") {
			HObject TempRoi;
			DrawRegion(&TempRoi, m_hHalconID);
			vecTemplateRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else {
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "绘制模板训练区域失败!", QMessageBox::Ok);
		return;
	}
}

//位置修正绘制掩模区域
void PinWindow::on_btn_DrawShaddowRoi_Template_2_clicked() {
	try {
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "green");
		SetLineWidth(m_hHalconID, 1);
		if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "矩形") {
			HObject TempRoi;
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "仿射矩形") {
			HObject TempRoi;
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&TempRoi, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "圆形") {
			HObject TempRoi;
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&TempRoi, CircleRow, CircleCol, CircleRadius);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "椭圆") {
			HObject TempRoi;
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&TempRoi, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "多边形") {
			HObject TempRoi;
			DrawRegion(&TempRoi, m_hHalconID);
			vecShadowRegions.push_back(TempRoi);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else {
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "绘制模板掩模区域失败!", QMessageBox::Ok);
		return;
	}
}

//位置修正合并模板区域Roi
void PinWindow::on_btn_UnionRoi_Template_2_clicked() {
	try {
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		if (vecTemplateRegions.isEmpty()) {
			QMessageBox::information(this, "提示", "模板训练区域ROI为空!", QMessageBox::Ok);
			return;
		}
		else {
			SetColor(m_hHalconID, "red");
			SetLineWidth(m_hHalconID, 1);
			HObject TrainRoi_Template;
			//训练区域合并
			if (vecTemplateRegions.count() == 1) {
				TrainRoi_Template = vecTemplateRegions[0];
			}
			else {
				TrainRoi_Template = vecTemplateRegions[0];
				for (int i = 1; i < vecTemplateRegions.count(); i++) {
					Union2(TrainRoi_Template, vecTemplateRegions[i], &TrainRoi_Template);
				}
			}

			//掩模区域合并
			HObject ShadowRoi_Template;
			if (!vecShadowRegions.isEmpty()) {
				if (vecShadowRegions.count() == 1) {
					ShadowRoi_Template = vecShadowRegions[0];
				}
				else {
					ShadowRoi_Template = vecShadowRegions[0];
					for (int i = 1; i < vecShadowRegions.count(); i++) {
						Union2(ShadowRoi_Template, vecShadowRegions[i], &ShadowRoi_Template);
					}
				}
				//窗口显示图形赋值
				ConcatObj(ho_ShowObj, ShadowRoi_Template, &ho_ShowObj);
			}

			//抠出掩模区域
			if (!vecShadowRegions.isEmpty()) {
				Difference(TrainRoi_Template, ShadowRoi_Template, &TrainRoi_Template);
			}

			//位置修正的模板
			RunParams_Pin.ho_TrainRegion_PositionCorrection = TrainRoi_Template;


			//窗口显示图形赋值
			ConcatObj(ho_ShowObj, TrainRoi_Template, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			QMessageBox::information(this, "提示", "合并ROI成功，训练区域数量为:" + QString::number(vecTemplateRegions.count()), QMessageBox::Ok);

			//清除vector内存
			vecTemplateRegions.clear();
			vecShadowRegions.clear();
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "合并模板训练区失败!", QMessageBox::Ok);
		return;
	}
}

//位置修正清除模板区域Roi
void PinWindow::on_btn_ClearRoi_Template_2_clicked() {
	if (ho_Image.Key() == nullptr) {
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	//清空区域集合
	vecTemplateRegions.clear();
	vecShadowRegions.clear();
	//清空窗口显示的图形变量
	GenEmptyObj(&ho_ShowObj);
	//显示原图
	showImg(ho_Image, "margin", "green");
}

//位置修正训练模板
void PinWindow::on_btn_Train_Template_2_clicked() {
	try {
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}

		//位置修正的模板
		on_pb_SetData_PositionCorrection_clicked();
		BlobCountShow = 0;
		GenEmptyObj(&BlobRegionShow);
		TypeParamsStruct_Pin m_type;

		int ret = 1;
		//创建模板
		ret = PinDetect1->CreateTemplate(ho_Image, RunParams_Pin, 0);
		if (ret != 0) {
			QMessageBox::information(this, "提示", "位置修正模板训练失败", QMessageBox::Ok);
			return;
		}
		//*******************************************************************************************************
		//查找模板
		HObject ModelRegion, ModelXLD;
		HTuple rowRet, colRet, angleRet, scoreRet;
		ret = PinDetect1->FindTemplate(ho_Image, 0, RunParams_Pin, ModelXLD, ModelRegion, rowRet, colRet, angleRet, scoreRet);

		if (ret != 0) {
			QMessageBox::information(this, "提示", "查找模板失败", QMessageBox::Ok);
			return;
		}
		//刷新结果
		int count = rowRet.TupleLength().I();
		BlobRegionShow = ModelRegion;
		BlobCountShow = count;
		if (count <= 0) {
			//没有找到区域，退出
			GenEmptyObj(&ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			QMessageBox::information(this, "提示", "没有找到模板!", QMessageBox::Ok);
			return;
		}
		//刷新图像
		GenEmptyObj(&ho_ShowObj);
		ConcatObj(ho_ShowObj, ModelRegion, &ho_ShowObj);
		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}
		//刷新数据
		if (count > 0) {
			ui.tablewidget_Results_Template_2->clearContents();
			ui.tablewidget_Results_Template_2->setRowCount(count);
			for (int i = 0; i < count; i++) {
				QTableWidgetItem* item;
				QString str;
				//行坐标
				str = QString::number(rowRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 0, item);
				//列坐标
				str = QString::number(colRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 1, item);
				//角度
				str = QString::number(angleRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 2, item);
				//分数
				str = QString::number(scoreRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 3, item);
			}
		}

		ui.dspb_Row_PositionCorrection->setValue(rowRet[0].D());
		ui.dspb_Column_PositionCorrection->setValue(colRet[0].D());
		ui.dspb_Angle_PositionCorrection->setValue(angleRet[0].D());
		RunParams_Pin.hv_Row_PositionCorrection = ui.dspb_Row_PositionCorrection->value();
		RunParams_Pin.hv_Column_PositionCorrection = ui.dspb_Column_PositionCorrection->value();
		RunParams_Pin.hv_Angle_PositionCorrection = ui.dspb_Angle_PositionCorrection->value();
	}
	catch (...) {
		QMessageBox::information(this, "提示", "模板训练失败!", QMessageBox::Ok);
		return;
	}
}

//是否启用位置修正
void PinWindow::on_ckb_PositionCorrection_stateChanged(int state) {
	if (state == Qt::Checked) {
		ui.pb_SearchROI_PositionCorrection->setEnabled(true);
		//ui.pb_SetData_PositionCorrection->setEnabled(true);
		ui.pb_Run_PositionCorrection->setEnabled(true);
	}
	else if (state == Qt::Unchecked) {
		ui.pb_SearchROI_PositionCorrection->setEnabled(false);
		//ui.pb_SetData_PositionCorrection->setEnabled(false);
		ui.pb_Run_PositionCorrection->setEnabled(false);
	}
}

//绘制位置修正搜索区域
void PinWindow::on_pb_SearchROI_PositionCorrection_clicked() {
	try {
		ui.lbl_Window->SetActionEnable(false);
		if (ho_Image.Key() == nullptr) {
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}

		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		//绘制ROI
		HObject TempRoi;
		if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "矩形") {
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "仿射矩形") {
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&TempRoi, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "圆形") {
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&TempRoi, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "椭圆") {
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&TempRoi, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else if (ui.cmb_ShapeSelect_PositionCorrection->currentText() == "多边形") {
			DrawRegion(&TempRoi, m_hHalconID);
			ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(TempRoi, m_hHalconID);
		}
		else {
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
			return;
		}
		RunParams_Pin.ho_SearchRegion_PositionCorrection = TempRoi;
	}
	catch (...) {
		QMessageBox::information(this, "提示", "绘制位置修正搜索区域失败!", QMessageBox::Ok);
		return;
	}
}

//设置位置修正参数
void PinWindow::on_pb_SetData_PositionCorrection_clicked() {
	try {
		TypeParamsStruct_Pin m_type;
		//模板匹配参数
		if (ui.cmb_MatchMethod_Train_2->currentText() == "基于形状") {
			RunParams_Pin.hv_MatchMethod_PositionCorrection = m_type.ShapeModel;      //模板类型:"基于互相关
		}
		else {
			RunParams_Pin.hv_MatchMethod_PositionCorrection = m_type.NccModel;      //模板类型:"基于互相关
		}
		if (ui.cmb_SortType->currentText() == "按行排序") {
			RunParams_Pin.hv_SortType_PositionCorrection = m_type.SortType_Row;      //排序类型:"按行排序
		}
		else {
			RunParams_Pin.hv_SortType_PositionCorrection = m_type.SortType_Column;      //排序类型:"按列排序
		}
		RunParams_Pin.hv_AngleStart_PositionCorrection = ui.dspb_AngleStart_Train_2->value();            //起始角度
		RunParams_Pin.hv_AngleExtent_PositionCorrection = ui.dspb_AngleExtent_Train_2->value();                //角度范围
		RunParams_Pin.hv_ScaleRMin_PositionCorrection = ui.dspb_ScaleRMin_Train_2->value();                 //最小行缩放
		RunParams_Pin.hv_ScaleRMax_PositionCorrection = ui.dspb_ScaleRMax_Train_2->value();                 //最大行缩放
		RunParams_Pin.hv_ScaleCMin_PositionCorrection = ui.dspb_ScaleCMin_Train_2->value();                 //最小列缩放
		RunParams_Pin.hv_ScaleCMax_PositionCorrection = ui.dspb_ScaleCMax_Train_2->value();				   //最大列缩放
		RunParams_Pin.hv_MinScore_PositionCorrection = ui.dspb_MinScore_Run_2->value();               //最低分数
		RunParams_Pin.hv_NumMatches_PositionCorrection = ui.spb_NumMatches_Run_2->value();               //匹配个数，0则自动选择，100则最多匹配100个
		RunParams_Pin.hv_Row_PositionCorrection = ui.dspb_Row_PositionCorrection->value();
		RunParams_Pin.hv_Column_PositionCorrection = ui.dspb_Column_PositionCorrection->value();
		RunParams_Pin.hv_Angle_PositionCorrection = ui.dspb_Angle_PositionCorrection->value();
		if (ui.ckb_PositionCorrection->checkState() == Qt::Checked) {
			RunParams_Pin.hv_Check_PositionCorrection = 1;
		}
		else {
			RunParams_Pin.hv_Check_PositionCorrection = 0;
		}
	}
	catch (...) {
		QMessageBox::information(this, "提示", "设置位置修正参数失败!", QMessageBox::Ok);
		return;
	}
}

//运行位置修正
void PinWindow::on_pb_Run_PositionCorrection_clicked() {
	try {
		on_pb_SetData_PositionCorrection_clicked();
		BlobCountShow = 0;
		GenEmptyObj(&BlobRegionShow);
		TypeParamsStruct_Pin m_type;
		int ret = 1;
		//*******************************************************************************************************
		//查找模板
		HObject ModelRegion, ModelXLD;
		HTuple rowRet, colRet, angleRet, scoreRet;
		ret = PinDetect1->FindTemplate(ho_Image, 0, RunParams_Pin, ModelXLD, ModelRegion, rowRet, colRet, angleRet, scoreRet);

		if (ret != 0) {
			QMessageBox::information(this, "提示", "查找模板失败", QMessageBox::Ok);
			return;
		}
		//刷新结果
		int count = rowRet.TupleLength().I();
		BlobRegionShow = ModelRegion;
		BlobCountShow = count;
		if (count <= 0) {
			//没有找到区域，退出
			GenEmptyObj(&ho_ShowObj);
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
				showImg(ho_Image, "fill", "green");
			}
			else {
				showImg(ho_Image, "margin", "green");
			}
			QMessageBox::information(this, "提示", "没有找到模板!", QMessageBox::Ok);
			return;
		}
		//刷新图像
		GenEmptyObj(&ho_ShowObj);
		ConcatObj(ho_ShowObj, ModelRegion, &ho_ShowObj);
		if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked) {
			showImg(ho_Image, "fill", "green");
		}
		else {
			showImg(ho_Image, "margin", "green");
		}
		//刷新数据
		if (count > 0) {
			ui.tablewidget_Results_Template_2->clearContents();
			ui.tablewidget_Results_Template_2->setRowCount(count);
			for (int i = 0; i < count; i++) {
				QTableWidgetItem* item;
				QString str;
				//行坐标
				str = QString::number(rowRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 0, item);
				//列坐标
				str = QString::number(colRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 1, item);
				//角度
				str = QString::number(angleRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 2, item);
				//分数
				str = QString::number(scoreRet[i].D(), 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Template_2->setItem(i, 3, item);
			}
		}

		ui.dspb_Row_PositionCorrection->setValue(rowRet[0].D());
		ui.dspb_Column_PositionCorrection->setValue(colRet[0].D());
		ui.dspb_Angle_PositionCorrection->setValue(angleRet[0].D());
		RunParams_Pin.hv_Row_PositionCorrection = ui.dspb_Row_PositionCorrection->value();
		RunParams_Pin.hv_Column_PositionCorrection = ui.dspb_Column_PositionCorrection->value();
		RunParams_Pin.hv_Angle_PositionCorrection = ui.dspb_Angle_PositionCorrection->value();
	}
	catch (...) {
		QMessageBox::information(this, "提示", "位置修正检测算法运行测试失败!", QMessageBox::Ok);
		return;
	}
}

//*****************************************************************************************************
//exe路径
string PinWindow::FolderPath() {
	char strBuf[256] = { 0 };
	GetModuleFileNameA(NULL, strBuf, sizeof(strBuf));
	string strTemp = strBuf;
	if (strTemp.empty())
		return strTemp;

	strTemp = strReplaceAll(strTemp, "\\", "/");
	string::size_type pos = strTemp.rfind("/");
	if (string::npos != pos)
		strTemp = strTemp.substr(0, pos);
	else
		strTemp = "";

	return strTemp;
}
//返回一个不含strOld
string PinWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew) {
	string::size_type pos = 0;
	string strTemp = strResource;
	do {
		if ((pos = strTemp.find(strOld)) != string::npos) {
			strTemp.replace(pos, strOld.length(), strNew);
		}
	} while (pos != string::npos);
	return strTemp;
}

// 定义一个函数，符合 std::string() 的签名
std::string PinWindow::getConfigPathTp(){
	return ConfigPath;
}

void PinWindow::wheelEvent(QWheelEvent* event){
	// 阻止滚轮事件，什么都不做
	//event->accept();
}