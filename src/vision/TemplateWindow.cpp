#include "TemplateWindow.h"

TemplateWindow::TemplateWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	////加载主题
	//QFile f("./qdarkstyle/theme/darkstyle.qss");
	//if (!f.exists()) {
	//	printf("Unable to set stylesheet, file not found\n");
	//}
	//else {
	//	f.open(QFile::ReadOnly | QFile::Text);
	//	QTextStream ts(&f);
	//	qApp->setStyleSheet(ts.readAll());
	//}
	setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	//窗口初始化
	InitWindow();
	TemplateDetect1 = new TemplateDetect();
	//初始化结果显示表格
	IniTableData_Template();
	//默认隐藏高级参数
	ui.widget_AdvancedData->setVisible(false);

	ui.groupBox_3->hide();

	////配置文件和XML路径
	//ConfigPath = FolderPath() + "/config/";
	//XmlPath = FolderPath() + "/config/Config.xml";
}

TemplateWindow::TemplateWindow(HObject image, int _processID, int modubleID, std::string _modubleName)
{
	TemplateDetect1 = new TemplateDetect();
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processId = _processID;
	if (image.Key() != nullptr)
		ho_Image = image;
}

TemplateWindow::~TemplateWindow()
{
	//释放模板new的内存
	delete TemplateDetect1;
}
void TemplateWindow::on_toolButton_clicked() {
	TemplateWindow::showMinimized(); //最小化
}

void TemplateWindow::on_toolButton_2_clicked() {
	if (TemplateWindow::isMaximized()) {
		TemplateWindow::showNormal();//还原事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		TemplateWindow::showMaximized();//最大化事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void TemplateWindow::on_toolButton_3_clicked() {
	this->close();
}

void TemplateWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void TemplateWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//当窗口最大化或最小化时也不进行触发
		if (TemplateWindow::isMaximized() || TemplateWindow::isMinimized()) {
			return;
		}
		else {
			//当在按钮之类需要鼠标操作的地方不进行触发(防误触)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() ||
				ui.lbl_Window->underMouse() || ui.cmb_RoiShape_Template->underMouse() || ui.cmb_ModelType->underMouse() || ui.cmb_MatchMethod_Train->underMouse() ||
				ui.cmb_Metric_Train->underMouse() || ui.cmb_Optimization_Train->underMouse() || ui.cmb_SubPixel_Run->underMouse()
				) {

			}
			else if (keypoint == 1) {

			}
			else {
				//TemplateWindow::move(TemplateWindow::mapToGlobal(event->pos() - whereismouse));//移动
			}
		}
	}
}
void TemplateWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

void TemplateWindow::resizeEvent(QResizeEvent* event)
{
	try
	{
		// 在这里处理窗口大小改变事件
	//设置halcon的文件路径为utf8，解决中文乱码
		SetSystem("filename_encoding", "utf8");
		//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
		ui.lbl_Window->setMouseTracking(true);
		m_hHalconID = NULL;
		m_hLabelID = (Hlong)ui.lbl_Window->winId();
		on_pBtn_FitImage_clicked();

		//显示模板小图
		if (ho_CropModelImg.Key() != nullptr)
		{
			DisplyModelImg(ho_CropModelImg);
		}
	}
	catch (...)
	{

	}

}

//初始化显示窗口
void TemplateWindow::InitWindow()
{
	try
	{
		//设置halcon的文件路径为utf8，解决中文乱码
		SetSystem("filename_encoding", "utf8");
		//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
		ui.lbl_Window->setMouseTracking(true);
		//生成空图像
		GenEmptyObj(&ho_Image);
		GenEmptyObj(&ho_ShowObj);
		GenEmptyObj(&RetRegionShow);
		RetRegionShow.Clear();
		GenEmptyObj(&RetContourShow);
		RetContourShow.Clear();
		ho_Image.Clear();
		GenEmptyObj(&ho_CropModelImg);
		ho_CropModelImg.Clear();
		m_hHalconID = NULL;
		m_hLabelID = (Hlong)ui.lbl_Window->winId();
		ui.frame->hide();
	}
	catch (...)
	{

	}
}

//显示图像
void TemplateWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
{
	try
	{
		if (ho_Img.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetSystem("flush_graphic", "false");
		//判断窗口句柄
		if (m_hHalconID != NULL)
		{
			//如果有图像，则先清空图像
			DetachBackgroundFromWindow(m_hHalconID);
		}
		else
		{
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
		if (1.0 * ui.lbl_Window->width() / m_imgWidth < 1.0 * ui.lbl_Window->height() / m_imgHeight)
		{
			SetWindowExtents(m_hHalconID, ui.lbl_Window->height() / 2.0 - m_scaledHeight / 2.0, 0, ui.lbl_Window->width(), m_scaledHeight);
		}
		else
		{
			SetWindowExtents(m_hHalconID, 0, ui.lbl_Window->width() / 2.0 - m_scaledWidth / 2.0, m_scaledWidth, ui.lbl_Window->height());

		}
		SetPart(m_hHalconID, 0, 0, m_imgHeight - 1, m_imgWidth - 1);
		//这句显示图像，不会闪烁(把图像设置成窗口的背景图)
		//AttachBackgroundToWindow(ho_Img, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(ho_Img, m_hHalconID);
		//显示图形
		if (isShowObj)
		{
			SetCheck("give_error");
			try
			{
				DispObj(ho_ShowObj, m_hHalconID);
			}
			catch (...)
			{
				return;
			}
			SetCheck("give_error");
		}
	}
	catch (...)
	{

	}
}

//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
void TemplateWindow::GiveParameterToWindow()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}

		//判断窗口句柄
		if (m_hHalconID != NULL)
		{
			//如果有图像，则先清空图像
			//DetachBackgroundFromWindow(m_hHalconID);
		}
		else
		{
			//打开窗口
			OpenWindow(0, 0, ui.lbl_Window->width(), ui.lbl_Window->height(), m_hLabelID, "visible", "", &m_hHalconID);
		}

		//设置图形显示属性
		SetDraw(m_hHalconID, "margin");
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);
		//设置窗口显示变量
		ui.lbl_Window->setHalconWnd(ho_Image, ho_ShowObj, isShowObj, m_hHalconID, ui.lbl_Status);
	}
	catch (...)
	{

	}
}

//打开图片
void TemplateWindow::on_pBtn_ReadImage_clicked()
{
	try
	{
		QFileDialog dlg;
		dlg.setAcceptMode(QFileDialog::AcceptOpen);
		dlg.setFileMode(QFileDialog::ExistingFile);
		QString filePath = dlg.getOpenFileName();
		std::string str1 = filePath.toStdString();
		const char *str = str1.c_str();
		HTuple h_FilePath(str);
		if (!filePath.isEmpty())
		{
			ReadImage(&ho_Image, h_FilePath);
			GenEmptyObj(&ho_ShowObj);
			showImg(ho_Image, "margin", "green");
		}
	}
	catch (...)
	{

	}
}

//图像自适应窗口
void TemplateWindow::on_pBtn_FitImage_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		//QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	showImg(ho_Image, "margin", "green");
}
//是否显示图形复选框
void TemplateWindow::on_ckb_ShowObj_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked)
	{
		isShowObj = true;
	}
	else
	{
		isShowObj = false;
	}
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	GiveParameterToWindow();
}
//清空窗口
void TemplateWindow::on_btn_ClearWindow_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	//清空窗口显示的图形变量
	GenEmptyObj(&ho_ShowObj);
	//显示原图
	showImg(ho_Image, "margin", "green");
}

//*****************模板匹配相关代码****************************************************************
//ROI形状选择
void TemplateWindow::on_cmb_RoiShape_Template_activated(const QString &arg1)
{
	RoiShape = arg1;
}
//绘制	roi函数
void TemplateWindow::DrawRoiFunc(HObject &OutRegion)
{
	try
	{
		GenEmptyObj(&OutRegion);
		OutRegion.Clear();

		ui.lbl_Window->SetActionEnable(false);
		SetLineWidth(m_hHalconID, 1);
		if (RoiShape == "rectangle1")
		{
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			//会自动进入让鼠标可以开始画矩形ROI的状态，不需要鼠标事件控制
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&OutRegion, RecRow1, RecCol1, RecRow2, RecCol2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "rectangle2")
		{
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&OutRegion, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "circle")
		{
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&OutRegion, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "ellipse")
		{
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&OutRegion, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "polygon")
		{
			DrawRegion(&OutRegion, m_hHalconID);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else
		{
			QMessageBox::information(this, "提示", "请选择区域形状!", QMessageBox::Ok);
		}
	}
	catch (...)
	{
		GenEmptyObj(&OutRegion);
		OutRegion.Clear();
	}

}
//绘制模板训练区域
void TemplateWindow::on_btn_DrawTrainRoi_Template_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "red");
	HObject TempRoi;
	DrawRoiFunc(TempRoi);
	if (TempRoi.Key() == nullptr)
	{
		QMessageBox::information(this, "提示", "ROI绘制失败!", QMessageBox::Ok);
		return;
	}
	ho_TemplateRegions.push_back(TempRoi);
}
//绘制模板搜索区域
void TemplateWindow::on_btn_DrawSearchRoi_Template_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "blue");
	HObject TempRoi;
	DrawRoiFunc(TempRoi);
	if (TempRoi.Key() == nullptr)
	{
		QMessageBox::information(this, "提示", "ROI绘制失败!", QMessageBox::Ok);
		return;
	}
	ho_SearchRegions.push_back(TempRoi);
}
//绘制掩模区域
void TemplateWindow::on_btn_DrawShaddowRoi_Template_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "green");
	HObject TempRoi;
	DrawRoiFunc(TempRoi);
	if (TempRoi.Key() == nullptr)
	{
		QMessageBox::information(this, "提示", "ROI绘制失败!", QMessageBox::Ok);
		return;
	}
	ho_ShadowRegions.push_back(TempRoi);
}
//合并模板区域Roi
void TemplateWindow::on_btn_UnionRoi_Template_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		if (ho_TemplateRegions.isEmpty())
		{
			QMessageBox::information(this, "提示", "模板训练区域ROI为空!", QMessageBox::Ok);
			return;
		}
		else
		{
			SetColor(m_hHalconID, "red");
			SetLineWidth(m_hHalconID, 1);
			//训练区域合并
			if (ho_TemplateRegions.count() == 1)
			{
				TrainRoi_Template = ho_TemplateRegions[0];
			}
			else
			{
				TrainRoi_Template = ho_TemplateRegions[0];
				for (int i = 1; i < ho_TemplateRegions.count(); i++)
				{
					Union2(TrainRoi_Template, ho_TemplateRegions[i], &TrainRoi_Template);
				}
			}

			//掩模区域合并
			if (!ho_ShadowRegions.isEmpty())
			{
				if (ho_ShadowRegions.count() == 1)
				{
					ShadowRoi_Template = ho_ShadowRegions[0];
				}
				else
				{
					ShadowRoi_Template = ho_ShadowRegions[0];
					for (int i = 1; i < ho_ShadowRegions.count(); i++)
					{
						Union2(ShadowRoi_Template, ho_ShadowRegions[i], &ShadowRoi_Template);
					}
				}
				//窗口显示图形赋值
				ConcatObj(ho_ShowObj, ShadowRoi_Template, &ho_ShowObj);
			}

			//抠出掩模区域
			if (!ho_ShadowRegions.isEmpty())
			{
				Difference(TrainRoi_Template, ShadowRoi_Template, &TrainRoi_Template);
			}
			//窗口显示图形赋值
			ConcatObj(ho_ShowObj, TrainRoi_Template, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			
			//显示模板小图
			GenEmptyObj(&ho_CropModelImg);
			HObject imgReduce;
			ReduceDomain(ho_Image, TrainRoi_Template, &imgReduce);
			CropDomain(imgReduce, &ho_CropModelImg);
			DisplyModelImg(ho_CropModelImg);
			int countRoi = ho_TemplateRegions.count();
			//清除vector内存
			ho_TemplateRegions.clear();
			ho_ShadowRegions.clear();
			QMessageBox::information(this, "提示", "合并ROI成功，训练区域数量为:" + QString::number(countRoi), QMessageBox::Ok);
		}
	}
	catch (...)
	{

	}
}
//清除模板区域Roi
void TemplateWindow::on_btn_ClearRoi_Template_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		//清空区域集合
		ho_TemplateRegions.clear();
		ho_SearchRegions.clear();
		ho_ShadowRegions.clear();
		//清空区域变量
		GenEmptyObj(&TrainRoi_Template);
		GenEmptyObj(&SearchRoi_Template);
		GenEmptyObj(&ShadowRoi_Template);
		//清空窗口显示的图形变量
		GenEmptyObj(&ho_ShowObj);
		//显示原图
		showImg(ho_Image, "margin", "green");
	}
	catch (...)
	{

	}
}
//训练模板
void TemplateWindow::on_btn_Train_Template_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		int ret = 1;
		//创建模板
		//设置查找模板的运行参数
		SetRunParams_Template(hv_RunParamsTemplate);
		ret = slot_CreateTemplate(hv_RunParamsTemplate);
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "模板训练失败", QMessageBox::Ok);
			return;
		}
		QMessageBox::information(this, "提示", "模板训练成功", QMessageBox::Ok);
		//*******************************************************************************************************
		////查找模板
		//QDateTime startTime = QDateTime::currentDateTime();
		//ret = slot_FindTemplate();
		//QDateTime endTime = QDateTime::currentDateTime();
		//qint64 intervalTimeMS = startTime.msecsTo(endTime);

		////刷新结果
		//SetTableData_Template(TemplateDetect1->mResultParams, intervalTimeMS);
	}
	catch (...)
	{

	}
}
//查找模板
void TemplateWindow::on_pb_Run_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		int ret = 1;
		//查找模板
		QDateTime startTime = QDateTime::currentDateTime();
		ret = slot_FindTemplate();
		QDateTime endTime = QDateTime::currentDateTime();
		qint64 intervalTimeMS = startTime.msecsTo(endTime);

		//刷新结果
		SetTableData_Template(TemplateDetect1->mResultParams, intervalTimeMS);
	}
	catch (...)
	{

	}
}
//模板参数保存
void TemplateWindow::on_pb_SaveData_clicked()
{
	//把保存参数改为自定义文件保存
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/Vision_formulation/Template_formulation";

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

	//250304 模板匹配会用到这个 nodeName ；
	nodeName = fileInfo.baseName().toUtf8().data();

	int ret = 1;
	//设置查找模板的运行参数
	SetRunParams_Template(hv_RunParamsTemplate);
	//保存模板匹配训练参数和运行参数
	ret = dataIOC.WriteParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
	TemplateDetect1->configPath = ConfigPath;
	TemplateDetect1->XMLPath = XmlPath;
	TemplateDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//模板参数读取
void TemplateWindow::on_pb_ReadData_clicked()
{
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/Vision_formulation/Template_formulation";

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
	//250304 模板匹配会用到这个 nodeName ；
	nodeName = fileInfo.baseName().toUtf8().data();

	ErrorCode_Xml errorCode; 
	//读取模板匹配参数
	errorCode = dataIOC.ReadParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "提示", "参数读取失败", QMessageBox::Ok);
		return;
	}
	//参数更新到窗口
	UpdateTemplateDataWindow(hv_RunParamsTemplate);
}

//更新模板训练参数和运行参数窗口
void TemplateWindow::UpdateTemplateDataWindow(const RunParamsStruct_Template &RunParams)
{
	try
	{
		//更新训练参数
		ModelTypeStruct_Template typeStructTemplate;
		if (RunParams.hv_MatchMethod == typeStructTemplate.None)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("基于形状无缩放");
		}
		else if (RunParams.hv_MatchMethod == typeStructTemplate.Scaled)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("基于形状同步缩放");
		}
		else if (RunParams.hv_MatchMethod == typeStructTemplate.Aniso)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("基于形状异步缩放");
		}
		else if (RunParams.hv_MatchMethod == typeStructTemplate.Ncc)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("基于互相关");
		}

		//判断tuple类型数据是不是数字
		HTuple hv_isNumber;
		TupleIsNumber(RunParams.hv_NumLevels, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			ui.spb_NumLevels_Train->setValue(RunParams.hv_NumLevels.TupleInt().I());
		}
		else
		{
			ui.spb_NumLevels_Train->setValue(-1);
		}
		ui.dspb_AngleStart_Train->setValue(RunParams.hv_AngleStart.TupleReal().D());
		ui.dspb_AngleExtent_Train->setValue(RunParams.hv_AngleExtent.TupleReal().D());
		TupleIsNumber(RunParams.hv_AngleStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			ui.dspb_AngleStep_Train->setValue(RunParams.hv_AngleStep.TupleReal().D());
		}
		else
		{
			ui.dspb_AngleStep_Train->setValue(0);
		}
		ui.dspb_ScaleRMin_Train->setValue(RunParams.hv_ScaleRMin.TupleReal().D());
		ui.dspb_ScaleRMax_Train->setValue(RunParams.hv_ScaleRMax.TupleReal().D());
		TupleIsNumber(RunParams.hv_ScaleRStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			ui.dspb_ScaleRStep_Train->setValue(RunParams.hv_ScaleRStep.TupleReal().D());
		}
		else
		{
			ui.dspb_ScaleRStep_Train->setValue(0);
		}
		ui.dspb_ScaleCMin_Train->setValue(RunParams.hv_ScaleCMin.TupleReal().D());
		ui.dspb_ScaleCMax_Train->setValue(RunParams.hv_ScaleCMax.TupleReal().D());
		TupleIsNumber(RunParams.hv_ScaleCStep, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			ui.dspb_ScaleCStep_Train->setValue(RunParams.hv_ScaleCStep.TupleReal().D());
		}
		else
		{
			ui.dspb_ScaleCStep_Train->setValue(0);
		}
		ui.cmb_Optimization_Train->setCurrentText(QString::fromUtf8(RunParams.hv_Optimization.S().Text()));
		ui.cmb_Metric_Train->setCurrentText(QString::fromUtf8(RunParams.hv_Metric.S().Text()));
		TupleIsNumber(RunParams.hv_Contrast, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			ui.spb_Contrast_Train->setValue(RunParams.hv_Contrast.TupleInt().I());
		}
		else
		{
			ui.spb_Contrast_Train->setValue(0);
		}
		TupleIsNumber(RunParams.hv_MinContrast, &hv_isNumber);
		if (hv_isNumber == 1)
		{
			ui.spb_MinContrast_Train->setValue(RunParams.hv_MinContrast.TupleInt().I());
		}
		else
		{
			ui.spb_MinContrast_Train->setValue(0);
		}
		TrainRoi_Template = RunParams.ho_TrainRegion;
		ho_CropModelImg = RunParams.ho_CropModelImg;
		//刷新模板小图
		if (ho_CropModelImg.Key() != nullptr)
		{
			DisplyModelImg(ho_CropModelImg);
		}

		//更新运行参数
		ui.dspb_MinScore_Run->setValue(RunParams.hv_MinScore.TupleReal().D());
		ui.spb_NumMatches_Run->setValue(RunParams.hv_NumMatches.TupleInt().I());
		ui.dspb_MaxOverlap_Run->setValue(RunParams.hv_MaxOverlap.TupleReal().D());
		ui.cmb_SubPixel_Run->setCurrentText(QString::fromUtf8(RunParams.hv_SubPixel.S().Text()));
		ui.dspb_Greediness_Run->setValue(RunParams.hv_Greediness.TupleReal().D());
		ui.spb_Timeout->setValue(RunParams.hv_TimeOut.TupleInt().I());
		SearchRoi_Template = RunParams.ho_SearchRegion;
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "参数刷新失败", QMessageBox::Ok);
		return;
	}
}
//查找模板结构体参数赋值函数
void TemplateWindow::SetRunParams_Template(RunParamsStruct_Template &runParams)
{
	try
	{
		ModelTypeStruct_Template typeStructTemplate;
		HTuple hv_typeTemp;

		if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "基于形状无缩放")
		{
			hv_typeTemp = typeStructTemplate.None;
		}
		else if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "基于形状同步缩放")
		{
			hv_typeTemp = typeStructTemplate.Scaled;
		}
		else if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "基于形状异步缩放")
		{
			hv_typeTemp = typeStructTemplate.Aniso;
		}
		else if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "基于互相关")
		{
			hv_typeTemp = typeStructTemplate.Ncc;
		}
		else
		{
			hv_typeTemp = typeStructTemplate.Scaled;
		}
		runParams.hv_MatchMethod = hv_typeTemp;   //比例模式:"none"无缩放,"scaled"同步缩放,"aniso"异步缩放,"auto"自动

		if (ui.spb_NumLevels_Train->value() == -1)
		{
			runParams.hv_NumLevels = typeStructTemplate.Auto;  //金字塔级别("auto")
		}
		else
		{
			runParams.hv_NumLevels = ui.spb_NumLevels_Train->value();  //金字塔级别
		}
		runParams.hv_AngleStart = ui.dspb_AngleStart_Train->value();             //起始角度
		runParams.hv_AngleExtent = ui.dspb_AngleExtent_Train->value();            //角度范围
		if (ui.dspb_AngleStep_Train->value() == 0)
		{
			runParams.hv_AngleStep = typeStructTemplate.Auto;   //角度步长("auto")
		}
		else
		{
			runParams.hv_AngleStep = ui.dspb_AngleStep_Train->value();             //角度步长
		}
		runParams.hv_ScaleRMin = ui.dspb_ScaleRMin_Train->value();              //最小行缩放
		runParams.hv_ScaleRMax = ui.dspb_ScaleRMax_Train->value();              //最大行缩放
		if (ui.dspb_ScaleRStep_Train->value() == 0)
		{
			runParams.hv_ScaleRStep = typeStructTemplate.Auto;     //行方向缩放步长("auto")
		}
		else
		{
			runParams.hv_ScaleRStep = ui.dspb_ScaleRStep_Train->value();       //行方向缩放步长
		}
		runParams.hv_ScaleCMin = ui.dspb_ScaleCMin_Train->value();              //最小列缩放
		runParams.hv_ScaleCMax = ui.dspb_ScaleCMax_Train->value();              //最大列缩放
		if (ui.dspb_ScaleCStep_Train->value() == 0)
		{
			runParams.hv_ScaleCStep = typeStructTemplate.Auto;     //列方向缩放步长("auto")
		}
		else
		{
			runParams.hv_ScaleCStep = ui.dspb_ScaleCStep_Train->value();       //列方向缩放步长
		}
		runParams.hv_Optimization = HTuple(ui.cmb_Optimization_Train->currentText().toStdString().c_str());           //优化算法
		runParams.hv_Metric = HTuple(ui.cmb_Metric_Train->currentText().toStdString().c_str());                 //极性/度量
		if (ui.spb_Contrast_Train->value() == 0)
		{
			runParams.hv_Contrast = typeStructTemplate.Auto;               //对比度("auto")
		}
		else
		{
			runParams.hv_Contrast = ui.spb_Contrast_Train->value();               //对比度
		}

		if (ui.spb_MinContrast_Train->value() == 0)
		{
			runParams.hv_MinContrast = typeStructTemplate.Auto;       //最小对比度("auto")
		}
		else
		{
			runParams.hv_MinContrast = ui.spb_MinContrast_Train->value();   //最小对比度
		}
		runParams.ho_TrainRegion = TrainRoi_Template;				//模板训练区域
		runParams.ho_CropModelImg = ho_CropModelImg;				//模版训练小图
		runParams.hv_MinScore = ui.dspb_MinScore_Run->value();               //最小分数
		runParams.hv_NumMatches = ui.spb_NumMatches_Run->value();             //匹配个数，0则自动选择，100则最多匹配100个
		runParams.hv_MaxOverlap = ui.dspb_MaxOverlap_Run->value();             //要找到模型实例的最大重叠
		runParams.hv_SubPixel = HTuple(ui.cmb_SubPixel_Run->currentText().toStdString().c_str());               //亚像素精度
		runParams.hv_Greediness = ui.dspb_Greediness_Run->value();             //贪婪系数
		runParams.hv_TimeOut = ui.spb_Timeout->value();		//超时时间

		//*******************************************************************************
		//搜索区域赋值
		if (!ho_SearchRegions.isEmpty())
		{
			if (ho_SearchRegions.count() == 1)
			{
				SearchRoi_Template = ho_SearchRegions[0];
			}
			else
			{
				SearchRoi_Template = ho_SearchRegions[0];
				for (int i = 1; i < ho_SearchRegions.count(); i++)
				{
					Union2(SearchRoi_Template, ho_SearchRegions[i], &SearchRoi_Template);
				}
			}
			//窗口显示图形赋值
			ConcatObj(ho_ShowObj, SearchRoi_Template, &ho_ShowObj);
			runParams.ho_SearchRegion = SearchRoi_Template;
		}
		else
		{
			//如果没有绘制搜索区域，则全图搜索
			GenEmptyObj(&SearchRoi_Template);
			SearchRoi_Template.Clear();
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "模板参数设置失败", QMessageBox::Ok);
		return;
	}
}

//初始化模板显示表格
void TemplateWindow::IniTableData_Template()
{
	try
	{
		std::vector<std::string> vec_Name = TemplateDetect1->getModubleResultTitleList();
		QTableWidgetItem   *hearderItem;
		ui.tablewidget_Result_Template->setColumnCount(vec_Name.size());
		for (int i = 0; i < ui.tablewidget_Result_Template->columnCount(); i++)
		{
			hearderItem = new QTableWidgetItem(QString::fromStdString(vec_Name[i]));
			QFont font = hearderItem->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			//hearderItem->setTextColor(Qt::red);//字体颜色
			hearderItem->setForeground(QBrush(Qt::red));//字体颜色
			hearderItem->setFont(font);//设置字体
			ui.tablewidget_Result_Template->setHorizontalHeaderItem(i, hearderItem);
		}
		//绑定模板匹配结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_Result_Template->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_TemplateData(int)));
	}
	catch (...)
	{

	}
}
//设置模板显示表格数据
void TemplateWindow::SetTableData_Template(const ResultParamsStruct_Template &resultStruct, qint64 Ct)
{
	try
	{
		//刷新时间
		getmessage("CT:" + QString::number(Ct) + "ms");
		//判断找模板结果
		GenEmptyObj(&ho_ShowObj);
		if (resultStruct.hv_RetNum > 0)
		{
			ho_ShowObj = resultStruct.ho_Region_Template;
			RetRegionShow = resultStruct.ho_Region_Template;
			RetContourShow = resultStruct.ho_Contour_Template;
			RetCountShow = resultStruct.hv_RetNum;
		}
		else
		{
			RetCountShow = 0;
			GenEmptyObj(&RetRegionShow);
			RetRegionShow.Clear();
			GenEmptyObj(&RetContourShow);
			RetContourShow.Clear();
			getmessage("NG");
		}
		//刷新图像
		showImg(ho_Image, "margin", "green");
		//刷新结果表格
		std::vector<std::map<int, std::string>> resultList = TemplateDetect1->getModubleResultList();
		int count = resultList.size();
		ui.tablewidget_Result_Template->clearContents();
		ui.tablewidget_Result_Template->setRowCount(count);
		for (int i = 0; i < count; i++)
		{
			QTableWidgetItem* item;
			for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++)
			{
				item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Result_Template->setItem(i, iter->first, item);
			}
		}
	}
	catch (...)
	{
	}
}
//查找模板槽函数
int TemplateWindow::slot_FindTemplate()
{
	try
	{
		int ret = 1;
		//XML读取参数
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML读取失败，给默认值
			GenEmptyObj(&hv_RunParamsTemplate.ho_SearchRegion);
			hv_RunParamsTemplate.ho_SearchRegion.Clear();
			hv_RunParamsTemplate.hv_MatchMethod = "auto_scaled";  
			hv_RunParamsTemplate.hv_NumLevels = "auto";           
			hv_RunParamsTemplate.hv_AngleStart = -30;             
			hv_RunParamsTemplate.hv_AngleExtent = 30;             
			hv_RunParamsTemplate.hv_AngleStep = "auto";           
			hv_RunParamsTemplate.hv_ScaleRMin = 0.9;              
			hv_RunParamsTemplate.hv_ScaleRMax = 1.1;              
			hv_RunParamsTemplate.hv_ScaleRStep = "auto";          
			hv_RunParamsTemplate.hv_ScaleCMin = 0.9;              
			hv_RunParamsTemplate.hv_ScaleCMax = 1.1;				
			hv_RunParamsTemplate.hv_ScaleCStep = "auto";          
			hv_RunParamsTemplate.hv_Optimization = "auto";        
			hv_RunParamsTemplate.hv_Metric = "use_polarity";      
			hv_RunParamsTemplate.hv_Contrast = "auto";            
			hv_RunParamsTemplate.hv_MinContrast = "auto";         
		
			hv_RunParamsTemplate.hv_Angle_Original = 0;			
			hv_RunParamsTemplate.hv_CenterRow_Original = 0;		
			hv_RunParamsTemplate.hv_CenterCol_Original = 0;		
		
			hv_RunParamsTemplate.hv_MinScore = 0.5;               
			hv_RunParamsTemplate.hv_NumMatches = 1;               
			hv_RunParamsTemplate.hv_MaxOverlap = 0.5;             
			hv_RunParamsTemplate.hv_SubPixel = "least_squares";   
			hv_RunParamsTemplate.hv_Greediness = 0.9;             
			//保存一维码运行参数
			ret = dataIOC.WriteParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//调用算法
		return TemplateDetect1->FindTemplate(ho_Image, hv_RunParamsTemplate, TemplateDetect1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//训练模板槽函数
int TemplateWindow::slot_CreateTemplate(RunParamsStruct_Template &runParams)
{
	try
	{
		//if (ConfigPath.empty() || XmlPath.empty())
		//{
		//	QMessageBox::information(this, "提示", "请先保存参数", QMessageBox::Ok);
		//	return -1;
		//}
		int ret = 1;
		//创建模板
		ret = TemplateDetect1->CreateTemplate(ho_Image, runParams);
		if (ret != 0)
		{
			return ret;
		}
		////保存模板匹配训练参数和运行参数
		//ret = dataIOC.WriteParams_Template(ConfigPath, XmlPath, runParams, processId, nodeName, processModbuleID);
		//if (ret != 0)
		//{
		//	return ret;
		//}
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

//选择保存模型的路径
void TemplateWindow::on_pb_SelectModelPath_clicked()
{
	try
	{
		ui.lineEdit_ModelPath->clear();
		// 获取文件夹路径
		QString filepath = QFileDialog::getExistingDirectory(this, "Input the Model path", "./");
		if (filepath.isEmpty())
		{
			return;
		}
		else
		{
			QString pathTemp = filepath + "/";
			ui.lineEdit_ModelPath->setText(pathTemp);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "模板存储路径选择失败", QMessageBox::Ok);
		return;
	}

}
//保存模型到指定路径
void TemplateWindow::on_pb_SaveModelFile_clicked()
{
	try
	{
		if (ui.lineEdit_ModelName->text() == "")
		{
			QMessageBox::information(this, "提示", "模板名称是空", QMessageBox::Ok);
			return;
		}
		if (ui.lineEdit_ModelPath->text() == "")
		{
			QMessageBox::information(this, "提示", "模板存储路径是空", QMessageBox::Ok);
			return;
		}
		if (ui.cmb_ModelType->currentText() == "基于形状")
		{
			string path = ui.lineEdit_ModelPath->text().trimmed().toStdString();
			string name = ui.lineEdit_ModelName->text().trimmed().toStdString();
			HTuple modelIDPath = HTuple(path.c_str()) + HTuple(name.c_str()) + ".shm";
			WriteShapeModel(hv_RunParamsTemplate.hv_ModelID, modelIDPath);
		}
		else if(ui.cmb_ModelType->currentText() == "基于互相关")
		{
			string path = ui.lineEdit_ModelPath->text().trimmed().toStdString();
			string name = ui.lineEdit_ModelName->text().trimmed().toStdString();
			HTuple modelIDPath = HTuple(path.c_str()) + HTuple(name.c_str()) + ".ncm";
			WriteNccModel(hv_RunParamsTemplate.hv_ModelID, modelIDPath);
		}
		else
		{
			QMessageBox::information(this, "提示", "模板存储类型不存在", QMessageBox::Ok);
			return;
		}
		QMessageBox::information(this, "提示", "模板存储成功", QMessageBox::Ok);
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "模板存储到指定路径失败", QMessageBox::Ok);
		return;
	}
}
//是否启用高级参数
void TemplateWindow::on_ckb_AdvancedData_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked)
	{
		ui.widget_AdvancedData->setVisible(true);
	}
	else
	{
		ui.widget_AdvancedData->setVisible(false);
	}
}
//exe路径
string TemplateWindow::FolderPath()
{
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
string TemplateWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
{
	string::size_type pos = 0;
	string strTemp = strResource;
	do
	{
		if ((pos = strTemp.find(strOld)) != string::npos)
		{
			strTemp.replace(pos, strOld.length(), strNew);
		}
	} while (pos != string::npos);
	return strTemp;
}
//*******************************************************************************************************************************************************
//模板匹配结果表格每个Cell点击事件
void TemplateWindow::on_tablewidget_Result_Template_cellClicked(int row, int column)
{
	try
	{
		int SelectRow = row;
		if (SelectRow < 0)
		{
			return;
		}
		//刷新结果区域
		if (RetCountShow > 0)
		{
			//先显示所有区域
			HObject ho_ObjTemp;
			GenEmptyObj(&ho_ObjTemp);
			GenEmptyObj(&ho_ShowObj);
			if (RetRegionShow.Key() != nullptr)
			{
				SelectObj(RetRegionShow, &ho_ObjTemp, SelectRow + 1);
				ConcatObj(ho_ShowObj, RetRegionShow, &ho_ShowObj);
			}
			//if (RetContourShow.Key() != nullptr)
			//{
			//	ConcatObj(ho_ShowObj, RetContourShow, &ho_ShowObj);
			//}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked)
			{
				showImg(ho_Image, "fill", "green");
			}
			else
			{
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//模板匹配结果表格表头的点击事件槽函数
void TemplateWindow::slot_VerticalHeader_TemplateData(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Result_Template->currentRow();
		if (SelectRow < 0)
		{
			return;
		}
		//刷新结果区域
		if (RetCountShow > 0)
		{
			//先显示所有区域
			HObject ho_ObjTemp;
			GenEmptyObj(&ho_ObjTemp);
			GenEmptyObj(&ho_ShowObj);
			if (RetRegionShow.Key() != nullptr)
			{
				SelectObj(RetRegionShow, &ho_ObjTemp, SelectRow + 1);
				ConcatObj(ho_ShowObj, RetRegionShow, &ho_ShowObj);
			}
			//if (RetContourShow.Key() != nullptr)
			//{
			//	ConcatObj(ho_ShowObj, RetContourShow, &ho_ShowObj);
			//}
			if (ui.ckb_ShowObj_Fill->checkState() == Qt::Checked)
			{
				showImg(ho_Image, "fill", "green");
			}
			else
			{
				showImg(ho_Image, "margin", "green");
			}
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//显示模板图
void TemplateWindow::DisplyModelImg(const HObject &image)
{
	try
	{
		if (image.Key() == nullptr)
		{
			return;
		}
		SetSystem("flush_graphic", "false");
		//打开窗口
		OpenWindow(0, 0, (Hlong)ui.lbl_Window_ModelImg->width(), (Hlong)ui.lbl_Window_ModelImg->height(), (Hlong)ui.lbl_Window_ModelImg->winId(), "visible", "", &hv_WindowHandle_ModelImg);
		//获取图像大小
		GetImageSize(image, &m_imgWidth_Crop, &m_imgHeight_Crop);
		//获取缩放系数
		TupleMin2(1.0 * ui.lbl_Window_ModelImg->width() / m_imgWidth_Crop, 1.0 * ui.lbl_Window_ModelImg->height() / m_imgHeight_Crop, &m_hvScaledRate_Crop);
		//缩放图像
		ZoomImageFactor(image, &m_hResizedImg_Crop, m_hvScaledRate_Crop, m_hvScaledRate_Crop, "constant");
		//获取缩放后的大小
		GetImageSize(m_hResizedImg_Crop, &m_scaledWidth_Crop, &m_scaledHeight_Crop);
		//打开窗口
		if (1.0 * ui.lbl_Window_ModelImg->width() / m_imgWidth_Crop < 1.0 * ui.lbl_Window_ModelImg->height() / m_imgHeight_Crop)
		{
			SetWindowExtents(hv_WindowHandle_ModelImg, ui.lbl_Window_ModelImg->height() / 2.0 - m_scaledHeight_Crop / 2.0, 0, ui.lbl_Window_ModelImg->width(), m_scaledHeight_Crop);
		}
		else
		{
			SetWindowExtents(hv_WindowHandle_ModelImg, 0, ui.lbl_Window_ModelImg->width() / 2.0 - m_scaledWidth_Crop / 2.0, m_scaledWidth_Crop, ui.lbl_Window_ModelImg->height());

		}
		SetPart(hv_WindowHandle_ModelImg, 0, 0, m_imgHeight_Crop - 1, m_imgWidth_Crop - 1);
		ClearWindow(hv_WindowHandle_ModelImg);
		SetSystem("flush_graphic", "true");
		DispObj(image, hv_WindowHandle_ModelImg);
	}
	catch (...)
	{
	}
}
