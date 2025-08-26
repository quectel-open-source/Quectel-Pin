#include "BlobWindow.h"

BlobWindow::BlobWindow(QWidget *parent)
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
	BlobDetect1 = new BlobDetect();
	IniTableData_Blob();
	IniBlobTools();
	GenEmptyObj(&SearchRoi_Blob);
	GenRectangle1(&SearchRoi_Blob, 10, 10, 500, 500);

	//注册信号自定义类型，可以使用信号槽传递自定义类型
	qRegisterMetaType<ResultParamsStruct_Blob>("ResultParamsStruct_Blob");
	connect(this, SIGNAL(sig_BlobResults(ResultParamsStruct_Blob, qint64)), this, SLOT(slot_BlobResults(ResultParamsStruct_Blob, qint64)));

	////配置文件和XML路径
	//ConfigPath = FolderPath() + "/config/";
	//XmlPath = FolderPath() + "/config/config.xml";
}

BlobWindow::BlobWindow(HObject image, int _processID, int modubleID, std::string _modubleName)
{
	BlobDetect1 = new BlobDetect();
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processId = _processID;
	if (image.Key() != nullptr)
		ho_Image = image;
}

BlobWindow::~BlobWindow()
{
	delete BlobDetect1;
}
void BlobWindow::on_toolButton_clicked() {
	BlobWindow::showMinimized(); //最小化
}

void BlobWindow::on_toolButton_2_clicked() {
	if (BlobWindow::isMaximized()) {
		BlobWindow::showNormal();//还原事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		BlobWindow::showMaximized();//最大化事件
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void BlobWindow::on_toolButton_3_clicked() {
	this->close();
}

void BlobWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void BlobWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//当窗口最大化或最小化时也不进行触发
		if (BlobWindow::isMaximized() || BlobWindow::isMinimized()) {
			return;
		}
		else {
			//当在按钮之类需要鼠标操作的地方不进行触发(防误触)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() || ui.cmb_Type_Binarization_Blob->underMouse() ||
				ui.lbl_Window->underMouse() || ui.pb_DrawRoi_Blob->underMouse() || ui.cmb_IsFollow->underMouse() || ui.cmb_OperatorType_Morphology_Blob->underMouse() || 
				ui.cmb_Polarity_Blob->underMouse() || ui.cmb_DestRegion->underMouse() || ui.cmb_FeaturesFilter_Blob->underMouse() ||
				ui.cmb_AndOr_Blob->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//BlobWindow::move(BlobWindow::mapToGlobal(event->pos() - whereismouse));//移动
			}
		}
	}
}
void BlobWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

void BlobWindow::resizeEvent(QResizeEvent* event)
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
void BlobWindow::InitWindow()
{
	//设置halcon的文件路径为utf8，解决中文乱码
	SetSystem("filename_encoding", "utf8");
	//设置这行代码，可以让鼠标移动时，触发mouseMove，随时可以看到坐标。否则，只有在鼠标按下时才可以看到坐标
	ui.lbl_Window->setMouseTracking(true);
	//生成空图像
	GenEmptyObj(&ho_Image);
	GenEmptyObj(&ho_ShowObj);
	ho_Image.Clear();
	m_hHalconID = NULL;
	m_hLabelID = (Hlong)ui.lbl_Window->winId();
	ui.frame->hide();
}

//显示图像
void BlobWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
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
void BlobWindow::GiveParameterToWindow()
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
void BlobWindow::on_pBtn_ReadImage_clicked()
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
void BlobWindow::on_pBtn_FitImage_clicked()
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
void BlobWindow::on_ckb_ShowObj_stateChanged(int arg1)
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
void BlobWindow::on_btn_ClearWindow_clicked()
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

//Blob添加形态学操作类型
void BlobWindow::on_pb_AddOperatorType_Morphology_Blob_clicked()
{
	try
	{
		TypeStruct_RegionOperator_Blob mType;
		if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形膨胀" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形腐蚀" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形开运算" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形闭运算" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "底帽运算矩形结构体" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "顶帽运算矩形结构体")
		{
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形膨胀")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Dilation_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形腐蚀")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Erosion_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形开运算")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Opening_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形闭运算")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Closing_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "底帽运算矩形结构体")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_BottomHat_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "顶帽运算矩形结构体")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_TopHat_Rectangle);
			}

			hv_RunParamsBlob.hv_Width_Operator.Append(ui.dsb_StructWidth_Blob->value());
			hv_RunParamsBlob.hv_Height_Operator.Append(ui.dsb_StructHeight_Blob->value());

			QListWidgetItem *aItem;
			QString str = ui.cmb_OperatorType_Morphology_Blob->currentText() + "Width:" + QString::number(ui.dsb_StructWidth_Blob->value(), 'f', 2) +
				" Height:" + QString::number(ui.dsb_StructHeight_Blob->value(), 'f', 2);
			aItem = new QListWidgetItem();
			aItem->setText(str);
			aItem->setCheckState(Qt::Checked);
			aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
		}
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形膨胀" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形腐蚀" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形开运算" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形闭运算" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "底帽运算圆形结构体" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "顶帽运算圆形结构体")
		{
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形膨胀")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Dilation_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形腐蚀")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Erosion_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形开运算")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Opening_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形闭运算")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Closing_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "底帽运算圆形结构体")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_BottomHat_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "顶帽运算圆形结构体")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_TopHat_Circle);
			}
			hv_RunParamsBlob.hv_CircleRadius_Operator.Append(ui.dsb_StructRadius_Blob->value());

			QListWidgetItem *aItem;
			QString str = ui.cmb_OperatorType_Morphology_Blob->currentText() + "Radius:" + QString::number(ui.dsb_StructRadius_Blob->value(), 'f', 2);
			aItem = new QListWidgetItem();
			aItem->setText(str);
			aItem->setCheckState(Qt::Checked);
			aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
		}
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "孔洞面积填充")
		{
			hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_FillUp_Shape);
			hv_RunParamsBlob.hv_MinArea_FillUpShape_Operator.Append(ui.dsb_MinArea_Blob->value());
			hv_RunParamsBlob.hv_MaxArea_FillUpShape_Operator.Append(ui.dsb_MaxArea_Blob->value());

			QListWidgetItem *aItem;
			QString str = ui.cmb_OperatorType_Morphology_Blob->currentText() + "MinArea:" + QString::number(ui.dsb_MinArea_Blob->value(), 'f', 2)
				+ "MaxArea:" + QString::number(ui.dsb_MaxArea_Blob->value(), 'f', 2);
			aItem = new QListWidgetItem();
			aItem->setText(str);
			aItem->setCheckState(Qt::Checked);
			aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
		}
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "孔洞全部填充" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "计算连通域" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "所有区域合并" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "补集")
		{
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "孔洞全部填充")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Fillup);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "计算连通域")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Connection);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "所有区域合并")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Union1);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "补集")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Complement);
			}
			QListWidgetItem *aItem;
			QString str = ui.cmb_OperatorType_Morphology_Blob->currentText();
			aItem = new QListWidgetItem();
			aItem->setText(str);
			aItem->setCheckState(Qt::Checked);
			aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
		}
		else
		{
			QMessageBox::information(this, "提示", "选择筛选特征类型为空!", QMessageBox::Ok);
			return;
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "添加形态学失败!", QMessageBox::Ok);
		return;
	}

}

//Blob清空形态学操作类型
void BlobWindow::on_pb_ClearOperatorType_Morphology_Blob_clicked()
{
	try
	{
		ui.listWidget_OperatorTypes_Morphology_Blob->clear();
		if (hv_RunParamsBlob.hv_RegionOperator_Type.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_RegionOperator_Type.Clear();
		}
		if (hv_RunParamsBlob.hv_Width_Operator.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_Width_Operator.Clear();
		}
		if (hv_RunParamsBlob.hv_Height_Operator.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_Height_Operator.Clear();
		}
		if (hv_RunParamsBlob.hv_CircleRadius_Operator.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_CircleRadius_Operator.Clear();
		}
		if (hv_RunParamsBlob.hv_MinArea_FillUpShape_Operator.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_MinArea_FillUpShape_Operator.Clear();
		}
		if (hv_RunParamsBlob.hv_MaxArea_FillUpShape_Operator.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_MaxArea_FillUpShape_Operator.Clear();
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "清空形态学失败", QMessageBox::Ok);
		return;
	}
}
//**********************************************************************************************************
//绘制	roi函数
void BlobWindow::DrawRoiFunc(int RoiShape, HObject &OutRegion)
{
	try
	{
		GenEmptyObj(&OutRegion);
		OutRegion.Clear();

		ui.lbl_Window->SetActionEnable(false);
		SetLineWidth(m_hHalconID, 1);
		if (RoiShape == 0)
		{
			HTuple RecRow1, RecCol1, RecRow2, RecCol2;
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&OutRegion, RecRow1, RecCol1, RecRow2, RecCol2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 1)
		{
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&OutRegion, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 2)
		{
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&OutRegion, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 3)
		{
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&OutRegion, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//给窗口刷新图像传递参数(主要作用是为了滚轮缩放时显示图形)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 4)
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
//绘制ROI
void BlobWindow::on_pb_DrawRoi_Blob_clicked()
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
		DrawRoiFunc(0, TempRoi);
		SearchRoi_Blob = TempRoi;
	}
	catch (...)
	{

	}

}
//清空ROI
void BlobWindow::on_pb_ClearRoi_Blob_clicked()
{
	GenEmptyObj(&SearchRoi_Blob);
	SearchRoi_Blob.Clear();
}
//运行Blob函数
void BlobWindow::on_pb_Run_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	int ret = 1;
	//运行
	QDateTime startTime = QDateTime::currentDateTime();
	ret = slot_BlobDetect();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	
	//开线程刷新结果表格
	QtConcurrent::run([=]()
	{
		emit sig_BlobResults(BlobDetect1->mResultParams, intervalTimeMS);
	});

}
//刷新Blob结果槽函数
void BlobWindow::slot_BlobResults(ResultParamsStruct_Blob ResultParams_Blob, qint64 Ct)
{
	try
	{
		getmessage("CT:" + QString::number(Ct) + "ms");
		//刷新图片
		GenEmptyObj(&ho_ShowObj);
		if (ResultParams_Blob.hv_RetNum > 0)
		{
			ConcatObj(ho_ShowObj, ResultParams_Blob.ho_DestRegions, &ho_ShowObj);
		}
		else
		{
			getmessage("NG");
		}
		showImg(ho_Image, "fill", "green");//margin
		//刷新结果表格
		std::vector<std::map<int, std::string>> resultList = BlobDetect1->getModubleResultList();
		int count = resultList.size();
		ui.tablewidget_Results_Blob->clearContents();
		ui.tablewidget_Results_Blob->setRowCount(count);
		for (int i = 0; i < count; i++)
		{
			QTableWidgetItem* item;
			for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++)
			{
				item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_Blob->setItem(i, iter->first, item);
			}
		}
	}
	catch (...)
	{
	}
}
//添加筛选特征
void BlobWindow::on_pb_AddFeature_Blob_clicked()
{
	try
	{
		QListWidgetItem *aItem;
		QString str = ui.cmb_FeaturesFilter_Blob->currentText() + " Min:" + QString::number(ui.dspb_MinValue_FeaturesFilter_Blob->value(), 'f', 2) +
			" Max:" + QString::number(ui.dspb_MaxValue_FeaturesFilter_Blob->value(), 'f', 2);
		aItem = new QListWidgetItem();
		aItem->setText(str);
		aItem->setCheckState(Qt::Checked);
		aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		ui.listWidget_FeaturesFilter_Blob->addItem(aItem);

		TypeStruct_FeaturesFilter_Blob mType;
		if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "面积")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Area_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "X")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Column_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "Y")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Row_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "角度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Angle_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "圆度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Circularity_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "紧密度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Compactness_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "凸度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Convexity_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "矩形度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rectangularity_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "高度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Height_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "宽度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Width_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "最小外接圆半径")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_OuterRadius_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "最小外接矩半长")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rect2Len1_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "最小外接矩半宽")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rect2Len2_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "最小外接矩角度")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rect2Angle_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else
		{
			QMessageBox::information(this, "提示", "选择筛选特征类型为空!", QMessageBox::Ok);
			return;
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "添加筛选特征失败!", QMessageBox::Ok);
		return;
	}

}

//清空筛选特征
void BlobWindow::on_pb_ClearFeature_Blob_clicked()
{
	try
	{
		ui.listWidget_FeaturesFilter_Blob->clear();
		if (hv_RunParamsBlob.hv_FeaturesFilter_Type.TupleLength() > 0)
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Clear();
			hv_RunParamsBlob.hv_MinValue_Filter.Clear();
			hv_RunParamsBlob.hv_MaxValue_Filter.Clear();
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "清空筛选特征失败!", QMessageBox::Ok);
		return;
	}

}
//初始化Blob结果表格
void BlobWindow::IniTableData_Blob()
{
	try
	{
		std::vector<std::string> vec_Name = BlobDetect1->getModubleResultTitleList();
		QTableWidgetItem   *hearderItem;
		ui.tablewidget_Results_Blob->setColumnCount(vec_Name.size());
		for (int i = 0; i < ui.tablewidget_Results_Blob->columnCount(); i++)
		{
			hearderItem = new QTableWidgetItem(QString::fromStdString(vec_Name[i]));
			QFont font = hearderItem->font();
			font.setBold(true);//设置为粗体
			font.setPointSize(12);//设置字体大小
			//hearderItem->setTextColor(Qt::red);//字体颜色
			hearderItem->setForeground(QBrush(Qt::red));//字体颜色
			hearderItem->setFont(font);//设置字体
			ui.tablewidget_Results_Blob->setHorizontalHeaderItem(i, hearderItem);
		}
		//绑定Blob结果表格的表头点击事件的信号槽
		connect(ui.tablewidget_Results_Blob->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_Blob(int)));

		//形态学表格右键菜单事件
		ui.listWidget_OperatorTypes_Morphology_Blob->setContextMenuPolicy(Qt::CustomContextMenu);
		Action1_Morphology = new QAction("删除", this);
		Menu_Morphology = new QMenu(this);
		Menu_Morphology->addAction(Action1_Morphology);
		//给动作设置信号槽
		connect(Action1_Morphology, &QAction::triggered, [=]()
		{
			try
			{
				//获取被移除的索引值
				int row = ui.listWidget_OperatorTypes_Morphology_Blob->currentRow();
				if (row < 0)
				{
					QMessageBox::information(this, "提示", "请选择被移除的选项", QMessageBox::Ok);
					return;
				}
				//获取将要被移除的区域操作类型
				HTuple strType = hv_RunParamsBlob.hv_RegionOperator_Type[row];
				//移除控件中的指定选项
				QListWidgetItem *aItem = ui.listWidget_OperatorTypes_Morphology_Blob->takeItem(row);
				delete aItem;
				//移除相对应的类型
				if (hv_RunParamsBlob.hv_RegionOperator_Type.TupleLength() > 0)
				{
					TupleRemove(hv_RunParamsBlob.hv_RegionOperator_Type, row, &hv_RunParamsBlob.hv_RegionOperator_Type);
				}
				//移除相对应的类型的参数
				TypeStruct_RegionOperator_Blob mTypeRegionOperator;
				if (hv_RunParamsBlob.hv_Width_Operator.TupleLength() > 0)
				{
					if (strType == mTypeRegionOperator.hv_Dilation_Rectangle || strType == mTypeRegionOperator.hv_Erosion_Rectangle ||
						strType == mTypeRegionOperator.hv_Opening_Rectangle || strType == mTypeRegionOperator.hv_Closing_Rectangle ||
						strType == mTypeRegionOperator.hv_BottomHat_Rectangle || strType == mTypeRegionOperator.hv_TopHat_Rectangle)
					{
						TupleRemove(hv_RunParamsBlob.hv_Width_Operator, row, &hv_RunParamsBlob.hv_Width_Operator);
					}
				}
				if (hv_RunParamsBlob.hv_Height_Operator.TupleLength() > 0)
				{
					if (strType == mTypeRegionOperator.hv_Dilation_Rectangle || strType == mTypeRegionOperator.hv_Erosion_Rectangle ||
						strType == mTypeRegionOperator.hv_Opening_Rectangle || strType == mTypeRegionOperator.hv_Closing_Rectangle ||
						strType == mTypeRegionOperator.hv_BottomHat_Rectangle || strType == mTypeRegionOperator.hv_TopHat_Rectangle)
					{
						TupleRemove(hv_RunParamsBlob.hv_Height_Operator, row, &hv_RunParamsBlob.hv_Height_Operator);
					}
				}
				if (hv_RunParamsBlob.hv_CircleRadius_Operator.TupleLength() > 0)
				{
					if (strType == mTypeRegionOperator.hv_Dilation_Circle || strType == mTypeRegionOperator.hv_Erosion_Circle ||
						strType == mTypeRegionOperator.hv_Opening_Circle || strType == mTypeRegionOperator.hv_Closing_Circle ||
						strType == mTypeRegionOperator.hv_BottomHat_Circle || strType == mTypeRegionOperator.hv_TopHat_Circle)
					{
						TupleRemove(hv_RunParamsBlob.hv_CircleRadius_Operator, row, &hv_RunParamsBlob.hv_CircleRadius_Operator);
					}
				}
				if (hv_RunParamsBlob.hv_MinArea_FillUpShape_Operator.TupleLength() > 0)
				{
					if (strType == mTypeRegionOperator.hv_FillUp_Shape)
					{
						TupleRemove(hv_RunParamsBlob.hv_MinArea_FillUpShape_Operator, row, &hv_RunParamsBlob.hv_MinArea_FillUpShape_Operator);
					}
				}
				if (hv_RunParamsBlob.hv_MaxArea_FillUpShape_Operator.TupleLength() > 0)
				{
					if (strType == mTypeRegionOperator.hv_FillUp_Shape)
					{
						TupleRemove(hv_RunParamsBlob.hv_MaxArea_FillUpShape_Operator, row, &hv_RunParamsBlob.hv_MaxArea_FillUpShape_Operator);
					}
				}
			}
			catch (...)
			{
				QMessageBox::information(this, "提示", "删除形态学失败", QMessageBox::Ok);
				return;
			}
		});
		connect(ui.listWidget_OperatorTypes_Morphology_Blob, &QListWidget::customContextMenuRequested, [=](const QPoint &pos)
		{
			Menu_Morphology->exec(QCursor::pos());
		});

		//筛选表格右键菜单事件
		ui.listWidget_FeaturesFilter_Blob->setContextMenuPolicy(Qt::CustomContextMenu);
		Action1_Filter = new QAction("删除", this);
		Menu_Filter = new QMenu(this);
		Menu_Filter->addAction(Action1_Filter);
		//给动作设置信号槽
		connect(Action1_Filter, &QAction::triggered, [=]()
		{
			try
			{
				int row = ui.listWidget_FeaturesFilter_Blob->currentRow();
				if (row < 0)
				{
					QMessageBox::information(this, "提示", "请选择被移除的选项", QMessageBox::Ok);
					return;
				}
				QListWidgetItem *aItem = ui.listWidget_FeaturesFilter_Blob->takeItem(row);
				delete aItem;
				if (hv_RunParamsBlob.hv_FeaturesFilter_Type.TupleLength() > 0)
				{
					TupleRemove(hv_RunParamsBlob.hv_FeaturesFilter_Type, row, &hv_RunParamsBlob.hv_FeaturesFilter_Type);
					TupleRemove(hv_RunParamsBlob.hv_MinValue_Filter, row, &hv_RunParamsBlob.hv_MinValue_Filter);
					TupleRemove(hv_RunParamsBlob.hv_MaxValue_Filter, row, &hv_RunParamsBlob.hv_MaxValue_Filter);
				}
			}
			catch (...)
			{
				QMessageBox::information(this, "提示", "删除筛选特征失败!", QMessageBox::Ok);
				return;
			}
		});
		connect(ui.listWidget_FeaturesFilter_Blob, &QListWidget::customContextMenuRequested, [=](const QPoint &pos)
		{
			Menu_Filter->exec(QCursor::pos());
		});
	}
	catch (...)
	{
	}
}
//初始化Blob窗口控件
void BlobWindow::IniBlobTools()
{
	try
	{
		//二值化
		ui.cmb_Type_Binarization_Blob->setCurrentIndex(0);
		ui.label_151->setVisible(true);
		ui.spb_LowThreshold_Blob->setVisible(true);
		ui.label_190->setVisible(true);
		ui.spb_HighThreshold_Blob->setVisible(true);
		ui.label_191->setVisible(false);
		ui.dspb_Sigma_Blob->setVisible(false);
		ui.label_192->setVisible(false);
		ui.dspb_DynThresholdWidth_Blob->setVisible(false);
		ui.label_193->setVisible(false);
		ui.dspb_DynThresholdHeight_Blob->setVisible(false);
		ui.label_194->setVisible(false);
		ui.spb_DynThresholdContrast_Blob->setVisible(false);
		ui.label_195->setVisible(false);
		ui.cmb_Polarity_Blob->setVisible(false);
		//形态学
		ui.cmb_OperatorType_Morphology_Blob->setCurrentIndex(0);
		ui.label_197->setVisible(true);
		ui.dsb_StructWidth_Blob->setVisible(true);
		ui.label_198->setVisible(true);
		ui.dsb_StructHeight_Blob->setVisible(true);
		ui.label_199->setVisible(false);
		ui.dsb_StructRadius_Blob->setVisible(false);
		ui.label_200->setVisible(false);
		ui.dsb_MinArea_Blob->setVisible(false);
		ui.label_201->setVisible(false);
		ui.dsb_MaxArea_Blob->setVisible(false);
		//特征筛选
		ui.cmb_FeaturesFilter_Blob->setCurrentIndex(0);
	}
	catch (...)
	{
	}
}
//保存Blob参数
void BlobWindow::on_pb_SaveData_clicked()
{
	//把保存参数改为自定义文件保存
// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/Vision_formulation/Blob_formulation";

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
	//设置Blob参数
	SetRunParams_Blob(hv_RunParamsBlob);
	//保存运行参数
	ret = dataIOC.WriteParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
	BlobDetect1->configPath = ConfigPath;
	BlobDetect1->XMLPath = XmlPath;
	BlobDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//读取Blob参数
void BlobWindow::on_pb_LoadData_clicked()
{
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/Vision_formulation/Blob_formulation";

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
	//读取直线参数
	errorCode = dataIOC.ReadParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "提示", "参数读取失败", QMessageBox::Ok);
		return;
	}
	//参数更新到窗口
	UpdateBlobDataWindow(hv_RunParamsBlob);

}
//Blob二值化类型选择事件
void BlobWindow::on_cmb_Type_Binarization_Blob_currentTextChanged(const QString &arg1)
{
	try
	{
		TypeStruct_BinarizationDetect mType;
		if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "固定阈值")
		{
			ui.label_151->setVisible(true);
			ui.spb_LowThreshold_Blob->setVisible(true);
			ui.label_190->setVisible(true);
			ui.spb_HighThreshold_Blob->setVisible(true);
			ui.label_191->setVisible(false);
			ui.dspb_Sigma_Blob->setVisible(false);
			ui.label_192->setVisible(false);
			ui.dspb_DynThresholdWidth_Blob->setVisible(false);
			ui.label_193->setVisible(false);
			ui.dspb_DynThresholdHeight_Blob->setVisible(false);
			ui.label_194->setVisible(false);
			ui.spb_DynThresholdContrast_Blob->setVisible(false);
			ui.label_195->setVisible(false);
			ui.cmb_Polarity_Blob->setVisible(false);
		}
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "自适应阈值")
		{
			ui.label_151->setVisible(false);
			ui.spb_LowThreshold_Blob->setVisible(false);
			ui.label_190->setVisible(false);
			ui.spb_HighThreshold_Blob->setVisible(false);
			ui.label_191->setVisible(true);
			ui.dspb_Sigma_Blob->setVisible(true);
			ui.label_192->setVisible(false);
			ui.dspb_DynThresholdWidth_Blob->setVisible(false);
			ui.label_193->setVisible(false);
			ui.dspb_DynThresholdHeight_Blob->setVisible(false);
			ui.label_194->setVisible(false);
			ui.spb_DynThresholdContrast_Blob->setVisible(false);
			ui.label_195->setVisible(false);
			ui.cmb_Polarity_Blob->setVisible(false);
		}
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "动态阈值")
		{
			ui.label_151->setVisible(false);
			ui.spb_LowThreshold_Blob->setVisible(false);
			ui.label_190->setVisible(false);
			ui.spb_HighThreshold_Blob->setVisible(false);
			ui.label_191->setVisible(false);
			ui.dspb_Sigma_Blob->setVisible(false);
			ui.label_192->setVisible(true);
			ui.dspb_DynThresholdWidth_Blob->setVisible(true);
			ui.label_193->setVisible(true);
			ui.dspb_DynThresholdHeight_Blob->setVisible(true);
			ui.label_194->setVisible(true);
			ui.spb_DynThresholdContrast_Blob->setVisible(true);
			ui.label_195->setVisible(true);
			ui.cmb_Polarity_Blob->setVisible(true);
		}
		else
		{
			ui.label_151->setVisible(false);
			ui.spb_LowThreshold_Blob->setVisible(false);
			ui.label_190->setVisible(false);
			ui.spb_HighThreshold_Blob->setVisible(false);
			ui.label_191->setVisible(false);
			ui.dspb_Sigma_Blob->setVisible(false);
			ui.label_192->setVisible(false);
			ui.dspb_DynThresholdWidth_Blob->setVisible(false);
			ui.label_193->setVisible(false);
			ui.dspb_DynThresholdHeight_Blob->setVisible(false);
			ui.label_194->setVisible(false);
			ui.spb_DynThresholdContrast_Blob->setVisible(false);
			ui.label_195->setVisible(false);
			ui.cmb_Polarity_Blob->setVisible(false);
		}
	}
	catch (...)
	{

	}

}
//Blob形态学区域操作类型选择事件
void BlobWindow::on_cmb_OperatorType_Morphology_Blob_activated(const QString &arg1)
{
	try
	{
		if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形膨胀" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形腐蚀" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形开运算" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "矩形闭运算" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "底帽运算矩形结构体" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "顶帽运算矩形结构体")
		{
			ui.label_197->setVisible(true);
			ui.dsb_StructWidth_Blob->setVisible(true);
			ui.label_198->setVisible(true);
			ui.dsb_StructHeight_Blob->setVisible(true);

			ui.label_199->setVisible(false);
			ui.dsb_StructRadius_Blob->setVisible(false);
			ui.label_200->setVisible(false);
			ui.dsb_MinArea_Blob->setVisible(false);
			ui.label_201->setVisible(false);
			ui.dsb_MaxArea_Blob->setVisible(false);
		}
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形膨胀" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形腐蚀" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形开运算" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "圆形闭运算" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "底帽运算圆形结构体" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "顶帽运算圆形结构体")
		{
			ui.label_197->setVisible(false);
			ui.dsb_StructWidth_Blob->setVisible(false);
			ui.label_198->setVisible(false);
			ui.dsb_StructHeight_Blob->setVisible(false);

			ui.label_199->setVisible(true);
			ui.dsb_StructRadius_Blob->setVisible(true);
			ui.label_200->setVisible(false);
			ui.dsb_MinArea_Blob->setVisible(false);
			ui.label_201->setVisible(false);
			ui.dsb_MaxArea_Blob->setVisible(false);
		}
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "孔洞面积填充")
		{
			ui.label_197->setVisible(false);
			ui.dsb_StructWidth_Blob->setVisible(false);
			ui.label_198->setVisible(false);
			ui.dsb_StructHeight_Blob->setVisible(false);

			ui.label_199->setVisible(false);
			ui.dsb_StructRadius_Blob->setVisible(false);
			ui.label_200->setVisible(true);
			ui.dsb_MinArea_Blob->setVisible(true);
			ui.label_201->setVisible(true);
			ui.dsb_MaxArea_Blob->setVisible(true);
		}
		else
		{
			ui.label_197->setVisible(false);
			ui.dsb_StructWidth_Blob->setVisible(false);
			ui.label_198->setVisible(false);
			ui.dsb_StructHeight_Blob->setVisible(false);

			ui.label_199->setVisible(false);
			ui.dsb_StructRadius_Blob->setVisible(false);
			ui.label_200->setVisible(false);
			ui.dsb_MinArea_Blob->setVisible(false);
			ui.label_201->setVisible(false);
			ui.dsb_MaxArea_Blob->setVisible(false);
		}
	}
	catch (...)
	{

	}

}
//Blob结果表格每个Cell点击事件
void BlobWindow::on_tablewidget_Results_Blob_cellClicked(int row, int column)
{
	try
	{
		int SelectRow = row;
		if (SelectRow < 0)
		{
			return;
		}
		//刷新结果区域
		if (BlobDetect1->mResultParams.hv_RetNum > 0)
		{
			HObject ho_ObjTemp;
			SelectObj(BlobDetect1->mResultParams.ho_DestRegions, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, BlobDetect1->mResultParams.ho_DestRegions, &ho_ShowObj);
			showImg(ho_Image, "fill", "green");//margin
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//Blob结果表格表头的点击事件槽函数
void BlobWindow::slot_VerticalHeader_Blob(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Results_Blob->currentRow();
		if (SelectRow < 0)
		{
			return;
		}
		//刷新结果区域
		if (BlobDetect1->mResultParams.hv_RetNum > 0)
		{
			HObject ho_ObjTemp;
			SelectObj(BlobDetect1->mResultParams.ho_DestRegions, &ho_ObjTemp, SelectRow + 1);
			//先显示所有区域
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, BlobDetect1->mResultParams.ho_DestRegions, &ho_ShowObj);
			showImg(ho_Image, "fill", "green");//margin
			//再显示选择区域
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//运行Blob参数赋值函数
void BlobWindow::SetRunParams_Blob(RunParamsStruct_Blob &runParams)
{
	try
	{
		//搜索区域保存
		if (ui.ckb_UseROI->isChecked())
		{
			HTuple r1, c1, r2, c2;
			SmallestRectangle1(SearchRoi_Blob, &r1, &c1, &r2, &c2);

			runParams.P1_SearchRoi.x = c1;
			runParams.P1_SearchRoi.y = r1;
			runParams.P2_SearchRoi.x = c2;
			runParams.P2_SearchRoi.y = r2;
			runParams.hv_IsUsSearchRoi = 1;
		}
		else
		{
			runParams.hv_IsUsSearchRoi = 0;
		}
		//********************************二值化参数赋值*******************************************
		//二值化类型
		TypeStruct_Binarization_Blob typeBinarization;
		HTuple hv_typeTemp;
		if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "固定阈值")
		{
			hv_typeTemp = typeBinarization.Fixed_Threshold;
		}
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "自适应阈值")
		{
			hv_typeTemp = typeBinarization.Auto_Threshold;
		}
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "动态阈值")
		{
			hv_typeTemp = typeBinarization.Dyn_Threshold;
		}
		else
		{
			hv_typeTemp = typeBinarization.hv_Null;
		}
		runParams.hv_ThresholdType = hv_typeTemp;

		runParams.hv_CoreWidth = ui.dspb_DynThresholdWidth_Blob->value();
		runParams.hv_CoreHeight = ui.dspb_DynThresholdHeight_Blob->value();
		runParams.hv_Sigma = ui.dspb_Sigma_Blob->value();
		runParams.hv_DynThresholdContrast = ui.spb_DynThresholdContrast_Blob->value();
		runParams.hv_LowThreshold = ui.spb_LowThreshold_Blob->value();
		runParams.hv_HighThreshold = ui.spb_HighThreshold_Blob->value();
		if (ui.cmb_Polarity_Blob->currentText().toStdString() == "dark")
		{
			hv_typeTemp = typeBinarization.DynPolarity_Dark;
		}
		else if (ui.cmb_Polarity_Blob->currentText().toStdString() == "light")
		{
			hv_typeTemp = typeBinarization.DynPolarity_Light;
		}
		else if (ui.cmb_Polarity_Blob->currentText().toStdString() == "equal")
		{
			hv_typeTemp = typeBinarization.DynPolarity_Equal;
		}
		else if (ui.cmb_Polarity_Blob->currentText().toStdString() == "not_equal")
		{
			hv_typeTemp = typeBinarization.DynPolarity_NotEqual;
		}
		else
		{
			hv_typeTemp = typeBinarization.DynPolarity_Equal;
		}
		runParams.hv_DynThresholdPolarity = hv_typeTemp;
		//与或赋值
		runParams.hv_And_Or_Filter = HTuple(ui.cmb_AndOr_Blob->currentText().toStdString().c_str());
		if (ui.cmb_IsFollow->currentText() == "Yes")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
		runParams.hv_SetBlobCount = ui.spinBox_SetBlobCount->value();
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "参数设置失败", QMessageBox::Ok);
		return;
	}

}
//运行参数更新到窗口
void BlobWindow::UpdateBlobDataWindow(const RunParamsStruct_Blob &runParams)
{
	try
	{
		//**********************Blob二值化参数更新到窗口*************************************************************
		ui.cmb_Type_Binarization_Blob->setCurrentText(QString::fromUtf8(runParams.hv_ThresholdType.S().Text()));
		ui.spb_LowThreshold_Blob->setValue(runParams.hv_LowThreshold.TupleInt().I());
		ui.spb_HighThreshold_Blob->setValue(runParams.hv_HighThreshold.TupleInt().I());
		ui.dspb_Sigma_Blob->setValue(runParams.hv_Sigma.TupleReal().D());
		ui.dspb_DynThresholdWidth_Blob->setValue(runParams.hv_CoreWidth.TupleReal().D());
		ui.dspb_DynThresholdHeight_Blob->setValue(runParams.hv_CoreHeight.TupleReal().D());
		ui.spb_DynThresholdContrast_Blob->setValue(runParams.hv_DynThresholdContrast.TupleInt().I());
		ui.cmb_Polarity_Blob->setCurrentText(QString::fromUtf8(runParams.hv_DynThresholdPolarity.S().Text()));

		//******************************Blob形态学参数更新到窗口**********************************************************************
		//区域操作类型数量
		int typeCount = runParams.hv_RegionOperator_Type.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			ui.listWidget_OperatorTypes_Morphology_Blob->clear();
			int RectangleNum = 0;
			int CircleNum = 0;
			int FillUpShapeNum = 0;
			TypeStruct_RegionOperator_Blob mType;
			for (int i = 0; i < typeCount; i++)
			{
				if (runParams.hv_RegionOperator_Type[i] == mType.hv_Dilation_Rectangle || runParams.hv_RegionOperator_Type[i] == mType.hv_Erosion_Rectangle ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_Opening_Rectangle || runParams.hv_RegionOperator_Type[i] == mType.hv_Closing_Rectangle ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_BottomHat_Rectangle || runParams.hv_RegionOperator_Type[i] == mType.hv_TopHat_Rectangle)
				{
					HTuple widthTemp, heightTemp;
					widthTemp = runParams.hv_Width_Operator[RectangleNum];
					heightTemp = runParams.hv_Height_Operator[RectangleNum];
					QListWidgetItem *aItem;
					QString str = QString::fromUtf8(runParams.hv_RegionOperator_Type[i].S().Text()) + " Width:" + QString::number(widthTemp.TupleReal().D(), 'f', 2) +
						" Height:" + QString::number(heightTemp.TupleReal().D(), 'f', 2);
					aItem = new QListWidgetItem();
					aItem->setText(str);
					aItem->setCheckState(Qt::Checked);
					aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
					ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
					RectangleNum += 1;
				}
				else if (runParams.hv_RegionOperator_Type[i] == mType.hv_Dilation_Circle || runParams.hv_RegionOperator_Type[i] == mType.hv_Erosion_Circle ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_Opening_Circle || runParams.hv_RegionOperator_Type[i] == mType.hv_Closing_Circle ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_BottomHat_Circle || runParams.hv_RegionOperator_Type[i] == mType.hv_TopHat_Circle)
				{
					HTuple circleTemp;
					circleTemp = runParams.hv_CircleRadius_Operator[CircleNum];
					QListWidgetItem *aItem;
					QString str = QString::fromUtf8(runParams.hv_RegionOperator_Type[i].S().Text()) + " Radius:" + QString::number(circleTemp.TupleReal().D(), 'f', 2);
					aItem = new QListWidgetItem();
					aItem->setText(str);
					aItem->setCheckState(Qt::Checked);
					aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
					ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
					CircleNum += 1;
				}
				else if (runParams.hv_RegionOperator_Type[i] == mType.hv_FillUp_Shape)
				{
					HTuple minTemp, maxTemp;
					minTemp = runParams.hv_MinArea_FillUpShape_Operator[FillUpShapeNum];
					maxTemp = runParams.hv_MaxArea_FillUpShape_Operator[FillUpShapeNum];
					QListWidgetItem *aItem;
					QString str = QString::fromUtf8(runParams.hv_RegionOperator_Type[i].S().Text()) + " MinAreaValue:" + QString::number(minTemp.TupleReal().D(), 'f', 2) +
						+" MaxAreaValue:" + QString::number(maxTemp.TupleReal().D(), 'f', 2);
					aItem = new QListWidgetItem();
					aItem->setText(str);
					aItem->setCheckState(Qt::Checked);
					aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
					ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
					FillUpShapeNum += 1;
				}
				else if (runParams.hv_RegionOperator_Type[i] == mType.hv_Fillup ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_Connection ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_Complement ||
					runParams.hv_RegionOperator_Type[i] == mType.hv_Union1)
				{
					QListWidgetItem *aItem;
					QString str = QString::fromUtf8(runParams.hv_RegionOperator_Type[i].S().Text());
					aItem = new QListWidgetItem();
					aItem->setText(str);
					aItem->setCheckState(Qt::Checked);
					aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
					ui.listWidget_OperatorTypes_Morphology_Blob->addItem(aItem);
				}

			}
		}

		//区域筛选类型数量
		typeCount = runParams.hv_FeaturesFilter_Type.TupleLength().TupleInt().I();
		if (typeCount > 0)
		{
			ui.listWidget_FeaturesFilter_Blob->clear();
			for (int i = 0; i < typeCount; i++)
			{
				HTuple minTemp, maxTemp;
				minTemp = runParams.hv_MinValue_Filter[i];
				maxTemp = runParams.hv_MaxValue_Filter[i];
				QListWidgetItem *aItem;
				QString str = QString::fromUtf8(runParams.hv_FeaturesFilter_Type[i].S().Text()) + " MinValue:" + QString::number(minTemp.TupleReal().D(), 'f', 2) +
					" MaxValue:" + QString::number(maxTemp.TupleReal().D(), 'f', 2);
				aItem = new QListWidgetItem();
				aItem->setText(str);
				aItem->setCheckState(Qt::Checked);
				aItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				ui.listWidget_FeaturesFilter_Blob->addItem(aItem);
			}
		}
		//与或
		ui.cmb_AndOr_Blob->setCurrentText(QString::fromUtf8(runParams.hv_And_Or_Filter.S().Text()));
		if (runParams.isFollow)
		{
			ui.cmb_IsFollow->setCurrentText("Yes");
		}
		else
		{
			ui.cmb_IsFollow->setCurrentText("No");
		}
		if (runParams.hv_IsUsSearchRoi == 1)
		{
			GenRectangle1(&SearchRoi_Blob, runParams.P1_SearchRoi.y, runParams.P1_SearchRoi.x, runParams.P2_SearchRoi.y, runParams.P2_SearchRoi.x);
			ui.ckb_UseROI->setChecked(true);
		}
		else
		{
			ui.ckb_UseROI->setChecked(false);
		}
		ui.spinBox_SetBlobCount->setValue(runParams.hv_SetBlobCount.I());
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "参数刷新失败", QMessageBox::Ok);
		return;
	}

}
//Blob检测槽函数
int BlobWindow::slot_BlobDetect()
{
	try
	{
		int ret = 1;
		//XML读取参数
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML读取失败，给默认值(只给二值化的值)
			TypeStruct_Binarization_Blob mType;
			hv_RunParamsBlob.isFollow = false;
			hv_RunParamsBlob.hv_IsUsSearchRoi = 0;
			hv_RunParamsBlob.hv_ThresholdType = mType.Fixed_Threshold;
			hv_RunParamsBlob.hv_LowThreshold = 50;
			hv_RunParamsBlob.hv_HighThreshold = 128;
			hv_RunParamsBlob.hv_Sigma = 1;
			hv_RunParamsBlob.hv_CoreWidth = 3;
			hv_RunParamsBlob.hv_CoreHeight = 3;
			hv_RunParamsBlob.hv_DynThresholdContrast = 10;
			hv_RunParamsBlob.hv_DynThresholdPolarity = "dark";

			//区域操作类型清空，筛选类型清空
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Clear();
			hv_RunParamsBlob.hv_RegionOperator_Type.Clear();

			//保存Blob检测运行参数
			ret = dataIOC.WriteParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//调用算法
		return BlobDetect1->BlobFunc(ho_Image, hv_RunParamsBlob, BlobDetect1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//exe路径
string BlobWindow::FolderPath()
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
string BlobWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
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