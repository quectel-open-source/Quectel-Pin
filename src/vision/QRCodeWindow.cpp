#include "QRCodeWindow.h"

QRCodeWindow::QRCodeWindow(QWidget *parent)
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
	code2dParams1 = new QRCodeDetect();
	//二维码检测
	IniTableData_Code2d();
	//配置文件和XML路径
	//ConfigPath = FolderPath() + "/config/";
	//XmlPath = FolderPath() + "/config/Config.xml";
}

QRCodeWindow::QRCodeWindow(HObject image, int _processID, int modubleID, std::string _modubleName)
{
	code2dParams1 = new QRCodeDetect();
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processId = _processID;
	if (image.Key() != nullptr)
		ho_Image = image;
}

QRCodeWindow::~QRCodeWindow()
{
	//释放二维码内存
	delete code2dParams1;
}
void QRCodeWindow::on_toolButton_clicked() {
	QRCodeWindow::showMinimized(); //最小化
}

void QRCodeWindow::on_toolButton_2_clicked() {
	if (QRCodeWindow::isMaximized()) {
		QRCodeWindow::showNormal();//还原事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		QRCodeWindow::showMaximized();//最大化事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void QRCodeWindow::on_toolButton_3_clicked() {
	this->close();
}

void QRCodeWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void QRCodeWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//当窗口最大化或最小化时也不进行触发
		if (QRCodeWindow::isMaximized() || QRCodeWindow::isMinimized()) {
			return;
		}
		else {
			//当在按钮之类需要鼠标操作的地方不进行触发(防误触)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() || ui.cmb_IsFollow->underMouse()
				|| ui.lbl_Window->underMouse() || ui.btn_DrawRoi_Code2d->underMouse() || ui.cmb_Tolerance_Code2d->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//QRCodeWindow::move(QRCodeWindow::mapToGlobal(event->pos() - whereismouse));//移动
			}
		}
	}
}
void QRCodeWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

void QRCodeWindow::resizeEvent(QResizeEvent* event)
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
	}
	catch (...)
	{

	}

}

//初始化显示窗口
void QRCodeWindow::InitWindow()
{
	//设置halcon的文件路径为utf8，解决中文乱码
	SetSystem("filename_encoding", "utf8");
	//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
	ui.lbl_Window->setMouseTracking(true);
	ui.frame->hide();
	//生成空图像
	GenEmptyObj(&ho_Image);
	GenEmptyObj(&ho_ShowObj);
	GenEmptyObj(&RetRegionShow);
	RetRegionShow.Clear();
	RetCountShow = 0;
	ho_Image.Clear();
	m_hHalconID = NULL;
	m_hLabelID = (Hlong)ui.lbl_Window->winId();

}

//显示图像
void QRCodeWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
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
void QRCodeWindow::GiveParameterToWindow()
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
void QRCodeWindow::on_pBtn_ReadImage_clicked()
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
		QMessageBox::information(this, "提示", "图像打开失败!", QMessageBox::Ok);
		return;
	}

}

//图像自适应窗口
void QRCodeWindow::on_pBtn_FitImage_clicked()
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
void QRCodeWindow::on_ckb_ShowObj_stateChanged(int arg1)
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
void QRCodeWindow::on_btn_ClearWindow_clicked()
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
//****************二维码检测相关函数***********************
//运行查找二维码结构体参数赋值函数
void QRCodeWindow::SetRunParams_Code2d(RunParamsStruct_Code2d &runParams)
{
	try
	{
		//设置二维码工具名称
		runParams.hv_CodeNum = ui.spb_CodeNum_Code2d->value();
	
		string strTemp = ui.txt_HeadChar_Code2d->text().trimmed().toStdString().c_str();
		runParams.hv_HeadChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_EndChar_Code2d->text().trimmed().toStdString().c_str();
		runParams.hv_EndChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_ContainChar_Code2d->text().trimmed().toStdString().c_str();
		runParams.hv_ContainChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_NotContainChar_Code2d->text().trimmed().toStdString().c_str();
		runParams.hv_NotContainChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_NotContainChar_Code2d->text().trimmed().toStdString().c_str();
		runParams.hv_NotContainChar = HTuple(strTemp.c_str());

		runParams.hv_CodeLength_Run = HTuple(ui.spb_CodeLength_Code2d->value());
		runParams.hv_TimeOut = ui.spb_TimeOut_Code2d->value();

		CodeTypeStruct_Code2d CodeTypes;
		runParams.hv_CodeType_Run.Clear();
		if (ui.ckb_Aztec_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.Aztec);
		}
		if (ui.ckb_DM_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.DM);
		}
		if (ui.ckb_GS1Aztec_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.GS1Aztec);
		}
		if (ui.ckb_GS1DM_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.GS1DM);
		}
		if (ui.ckb_GS1QR_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.GS1QR);
		}
		if (ui.ckb_MicroQR_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.MicroQR);
		}
		if (ui.ckb_PDF417_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.PDF417);
		}
		if (ui.ckb_QR_Code2d->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.QR);
		}
		if (runParams.hv_CodeType_Run.TupleLength().I() == 0)
		{
			runParams.hv_CodeType_Run = CodeTypes.QR;
		}

		if (ui.cmb_Tolerance_Code2d->currentText() == "标准")
		{
			runParams.hv_Tolerance = "low";
		}
		else
		{
			runParams.hv_Tolerance = "high";
		}
		if (ui.cmb_IsFollow->currentText() == "Yes")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
		//*******************************************************************************
		//搜索区域赋值
		GenEmptyObj(&SearchRoi_Code2d);
		//搜索区域合并
		if (!ho_Code2dRegions.isEmpty())
		{
			//一维码搜索区域合并
			if (ho_Code2dRegions.count() == 1)
			{
				SearchRoi_Code2d = ho_Code2dRegions[0];
			}
			else
			{
				SearchRoi_Code2d = ho_Code2dRegions[0];
				for (int i = 1; i < ho_Code2dRegions.count(); i++)
				{
					Union2(SearchRoi_Code2d, ho_Code2dRegions[i], &SearchRoi_Code2d);
				}
			}
			//窗口显示图形赋值
			ConcatObj(ho_ShowObj, SearchRoi_Code2d, &ho_ShowObj);
			runParams.ho_SearchRegion = SearchRoi_Code2d;
		}
		else
		{
			//如果没有绘制搜索区域,置空
			GenEmptyObj(&SearchRoi_Code2d);
			SearchRoi_Code2d.Clear();
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "参数设置失败!", QMessageBox::Ok);
		return;
	}

}
//设置二维码结果显示表格数据
void QRCodeWindow::SetTableData_Code2d(const ResultParamsStruct_Code2d &resultStruct, qint64 Ct)
{
	try
	{
		getmessage("CT:" + QString::number(Ct) + "ms");
		//判断是否找到二维码
		GenEmptyObj(&ho_ShowObj);
		if (resultStruct.hv_RetNum > 0)
		{
			RetCountShow = resultStruct.hv_RetNum;
			RetRegionShow = resultStruct.ho_RegionCode2d;
			ConcatObj(ho_ShowObj, resultStruct.ho_RegionCode2d, &ho_ShowObj);
		}
		else
		{
			RetCountShow = 0;
			GenEmptyObj(&RetRegionShow);
			RetRegionShow.Clear();
		}
		showImg(ho_Image, "margin", "green");
		//刷新结果表格
		std::vector<std::map<int, std::string>> resultList = code2dParams1->getModubleResultList();
		int count = resultList.size();
		ui.tablewidget_Results_Code2d->clearContents();
		ui.tablewidget_Results_Code2d->setRowCount(count);
		for (int i = 0; i < count; i++)
		{
			QTableWidgetItem* item;
			for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++)
			{
				item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Code2d->setItem(i, iter->first, item);
			}
		}
	}
	catch (...)
	{

	}

}
//更新二维码检测运行参数到窗口
void QRCodeWindow::UpdateCode2dDataWindow(const RunParamsStruct_Code2d &runParams)
{
	try
	{
		ui.spb_CodeNum_Code2d->setValue(runParams.hv_CodeNum.I());
		SearchRoi_Code2d = runParams.ho_SearchRegion;

		QString contentTemp = "";
		contentTemp = QString::fromUtf8(runParams.hv_HeadChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_HeadChar_Code2d->setText(contentTemp);
		}

		contentTemp = QString::fromUtf8(runParams.hv_EndChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_EndChar_Code2d->setText(contentTemp);
		}

		contentTemp = QString::fromUtf8(runParams.hv_ContainChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_ContainChar_Code2d->setText(contentTemp);
		}

		contentTemp = QString::fromUtf8(runParams.hv_NotContainChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_NotContainChar_Code2d->setText(contentTemp);
		}

		ui.spb_CodeLength_Code2d->setValue(runParams.hv_CodeLength_Run.I());
		ui.spb_TimeOut_Code2d->setValue(runParams.hv_TimeOut.I());

		CodeTypeStruct_Code2d codeTypeStruct;
		int len = runParams.hv_CodeType_Run.TupleLength().I();
		for (int i = 0; i < len; i++)
		{
			if (runParams.hv_CodeType_Run[i] == codeTypeStruct.Aztec)
			{
				ui.ckb_Aztec_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.DM)
			{
				ui.ckb_DM_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.GS1Aztec)
			{
				ui.ckb_GS1Aztec_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.GS1DM)
			{
				ui.ckb_GS1DM_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.GS1QR)
			{
				ui.ckb_GS1QR_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.MicroQR)
			{
				ui.ckb_MicroQR_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.PDF417)
			{
				ui.ckb_PDF417_Code2d->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.QR)
			{
				ui.ckb_QR_Code2d->setCheckState(Qt::Checked);
			}
			else
			{
				string str = runParams.hv_CodeType_Run[i].S().Text();
			}
		}
		if (runParams.isFollow)
		{
			ui.cmb_IsFollow->setCurrentText("Yes");
		}
		else
		{
			ui.cmb_IsFollow->setCurrentText("No");
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "参数刷新失败!", QMessageBox::Ok);
		return;

	}
}

//保存二维码检测参数
void QRCodeWindow::on_pb_SaveData_clicked()
{
	//把保存参数改为自定义文件保存
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/Vision_formulation/QRCode_formulation";

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
	//设置二维码运行参数
	SetRunParams_Code2d(hv_RunParamsCode2d);
	//保存二维码运行参数
	ret = dataIOC.WriteParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
	code2dParams1->configPath = ConfigPath;
	code2dParams1->XMLPath = XmlPath;
	code2dParams1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//读取二维码检测参数
void QRCodeWindow::on_pb_LoadData_clicked()
{
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/Vision_formulation/QRCode_formulation";

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
	//数组需要清空
	hv_RunParamsCode2d.hv_CodeType_Run.Clear();
	//读取二维码参数
	errorCode = dataIOC.ReadParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "提示", "参数读取失败", QMessageBox::Ok);
		return;
	}
	//参数更新到窗口
	UpdateCode2dDataWindow(hv_RunParamsCode2d);
}
//绘制二维码搜索区域
void QRCodeWindow::on_btn_DrawRoi_Code2d_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		HObject TempRoi;
		HTuple RecRow1, RecCol1, RecRow2, RecCol2;
		DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
		GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
		ho_Code2dRegions.push_back(TempRoi);
		ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
		//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
		GiveParameterToWindow();
		DispObj(TempRoi, m_hHalconID);

	}
	catch (...)
	{

	}

}
//清空ROI
void QRCodeWindow::on_btn_ClearRoi_Code2d_clicked()
{
	ho_Code2dRegions.clear();
	GenEmptyObj(&SearchRoi_Code2d);
	GenEmptyObj(&ho_ShowObj);
	//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
	GiveParameterToWindow();
	showImg(ho_Image, "margin", "green");
}
//初始化二维码结果显示表格
void QRCodeWindow::IniTableData_Code2d()
{
	try
	{
		std::vector<std::string> vec_Name = code2dParams1->getModubleResultTitleList();
		QTableWidgetItem   *hearderItem;
		ui.tablewidget_Results_Code2d->setColumnCount(vec_Name.size());
		for (int i = 0; i < ui.tablewidget_Results_Code2d->columnCount(); i++)
		{
			hearderItem = new QTableWidgetItem(QString::fromStdString(vec_Name[i]));
			QFont font = hearderItem->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			//hearderItem->setTextColor(Qt::red);//字体颜色
			hearderItem->setForeground(QBrush(Qt::red));//字体颜色
			hearderItem->setFont(font);//设置字体
			ui.tablewidget_Results_Code2d->setHorizontalHeaderItem(i, hearderItem);
		}
		//绑定结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_Results_Code2d->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_Code2d(int)));
	}
	catch (...)
	{

	}
}
//二维码检测槽函数
void QRCodeWindow::on_pb_Run_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	int ret = 1;
	//识别二维码
	QDateTime startTime = QDateTime::currentDateTime();
	ret = slot_FindCode2d();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);

	//刷新结果
	SetTableData_Code2d(code2dParams1->mResultParams, intervalTimeMS);
}
//检测二维码槽函数
int QRCodeWindow::slot_FindCode2d()
{
	try
	{
		int ret = 1;
		//XML读取参数
		hv_RunParamsCode2d.hv_CodeType_Run.Clear();
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML读取失败，给默认值
			GenEmptyObj(&hv_RunParamsCode2d.ho_SearchRegion);
			hv_RunParamsCode2d.ho_SearchRegion.Clear();
			hv_RunParamsCode2d.hv_CodeType_Run = "QR Code";
			hv_RunParamsCode2d.hv_CodeNum = 1;
			hv_RunParamsCode2d.hv_Tolerance = "high";
			hv_RunParamsCode2d.hv_HeadChar = "null";
			hv_RunParamsCode2d.hv_EndChar = "null";
			hv_RunParamsCode2d.hv_ContainChar = "null";
			hv_RunParamsCode2d.hv_NotContainChar = "null";
			hv_RunParamsCode2d.hv_CodeLength_Run = 2;
			hv_RunParamsCode2d.hv_TimeOut = 100;
			//保存二维码运行参数
			ret = dataIOC.WriteParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//调用算法
		return code2dParams1->FindCode2dFunc(ho_Image, hv_RunParamsCode2d, code2dParams1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//结果表格每个Cell点击事件
void QRCodeWindow::on_tablewidget_Results_Code2d_cellClicked(int row, int column)
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
//结果表格表头的点击事件槽函数
void QRCodeWindow::slot_VerticalHeader_Code2d(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Results_Code2d->currentRow();
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
//exe路径
string QRCodeWindow::FolderPath()
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
string QRCodeWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
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
