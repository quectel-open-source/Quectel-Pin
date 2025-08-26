#include "RobotCalibWindow.h"
#include "rs_motion.h"

RobotCalibWindow::RobotCalibWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
	//窗口初始化
	InitWindow();
	//标定类实例化
	RobotCalibDetect1 = new RobotCalibDetect();
	PaintDetect2 = new PaintDetect();
	Common1 = new Common();
	//高级参数默认不可见
	ui.widget_AdvancedData->setVisible(false);
	//初始化表格
	IniTableData();
	//给表格里面添加机器人标定行数据
	for (int i = 0; i < 9; i++)
	{
		on_pb_AddValue_clicked();
	}
	//给表格里面添加旋转中心标定行数据
	for (int i = 0; i < 9; i++)
	{
		on_pb_AddValue_RotateCenter_clicked();
	}

	if (!LSM->m_forMulaName.isEmpty())
	{
		ConfigPath = FolderPath() + "/Vision_formulation/Robot/";
		XmlPath = FolderPath() + "/Vision_formulation/Robot/" + LSM->m_forMulaName.toStdString() + ".xml";
		nodeName = LSM->m_forMulaName.toStdString();
	}
	else
	{
		//配置文件和XML路径
		ConfigPath = FolderPath() + "/Vision_formulation/Robot/";
		XmlPath = FolderPath() + "/Vision_formulation/Robot/Config.xml";
	}
}

RobotCalibWindow::~RobotCalibWindow()
{
	if(RobotCalibDetect1)
		delete RobotCalibDetect1;
	if(PaintDetect2)
		delete PaintDetect2;
	if(Common1)
		delete Common1;
}
void RobotCalibWindow::getmessage(QString value)
{
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}
//初始化显示窗口
void RobotCalibWindow::InitWindow()
{
	//生成空图像
	GenEmptyObj(&ho_Image);
	GenEmptyObj(&ho_CropModelImg);
	GenEmptyObj(&TrainRoi_Template);
	GenEmptyObj(&SearchRoi_Template);
	GenEmptyObj(&ShadowRoi_Template);
	ho_Image.Clear();
	ho_CropModelImg.Clear();
	TrainRoi_Template.Clear();
	SearchRoi_Template.Clear();
	ShadowRoi_Template.Clear();

	vec_RoiSearch_Calib.clear();
	vec_RoiTrain_Template.clear();
	vec_RoiSearch_Template.clear();
	vec_RoiShaddow_Template.clear();

	//图像显示
	view = new QGraphicsViews;
	ui.imageLayout->addWidget(view);
	view->ClearObj();
}
//打开图片
void RobotCalibWindow::on_pb_ReadImage_clicked()
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
			MatImage = PaintDetect2->HObject2Mat(ho_Image);
			Q_InputImg = QImage(str1.c_str());
			view->DispImage(Q_InputImg);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "图像打开失败!", QMessageBox::Ok);
		return;
	}
}
//添加标定roi
void RobotCalibWindow::on_pb_AddCalibRoi_clicked()
{
	view->ClearObj();
	vec_RoiSearch_Calib.clear();
	HTuple width = 0, height = 0;
	if (ho_Image.Key() != nullptr)
	{
		GetImageSize(ho_Image, &width, &height);
	}
	if (width == 0)
	{
		RectangleItem * Roi = new RectangleItem(10, 10, 500, 500);
		Roi->ItemDiscrib = "圆搜索区域";
		view->AddItems(Roi);
		vec_RoiSearch_Calib.push_back(Roi);
	}
	else
	{
		RectangleItem * Roi = new RectangleItem(width / 2 - width / 4, height / 2 - height / 4, width / 2, height / 2);
		Roi->ItemDiscrib = "圆搜索区域";
		view->AddItems(Roi);
		vec_RoiSearch_Calib.push_back(Roi);
	}
}
//清除标定roi
void RobotCalibWindow::on_pb_ClearCalibRoi_clicked()
{
	view->ClearObj();
	vec_RoiSearch_Calib.clear();
}
//找圆
void RobotCalibWindow::on_pb_FindCircleCalib_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "图像是空，请先加载图像!", QMessageBox::Ok);
		return;
	}
	if (vec_RoiSearch_Calib.size() != 1)
	{
		QMessageBox::information(this, "提示", "运行区域是空，请先绘制运行区域!", QMessageBox::Ok);
		return;
	}

	double x1 = 0.00, y1 = 0.00;
	HObject objTemp;
	//计算像素坐标
	int ret = MarkCenter(ho_Image, x1, y1, objTemp);
	if (ret == 0)
	{
		//画结果图
		cv::Mat destImg;
		RunParamsStruct_PaintDetect runParams_Pain;
		runParams_Pain.vec_InputRegion.clear();
		runParams_Pain.vec_Ret.clear();
		runParams_Pain.hv_LineWidth = 5;
		runParams_Pain.hv_Word_Size = 10;
		runParams_Pain.hv_Word_X = 100;
		runParams_Pain.hv_Word_Y = 100;
		//不画ok/ng字符串
		runParams_Pain.hv_Ret = -1;
		runParams_Pain.vec_Ret.push_back(1);
		HObject unionRegion;
		GenRegionContourXld(objTemp, &unionRegion,"margin");
		Union1(unionRegion, &unionRegion);
		runParams_Pain.vec_InputRegion.push_back(unionRegion);
		PaintDetect2->PaintImage(ho_Image, runParams_Pain, destImg);
		Q_OutputImg = Common1->Mat2QImage(destImg);
		view->DispImage(Q_OutputImg);
	}
	else
	{
		QMessageBox::information(this, "提示", "找圆失败!", QMessageBox::Ok);
		return;
	}
}

//保存参数
void RobotCalibWindow::on_pb_SaveData_clicked()
{
	if (LSM->m_version == EdPin)
	{
		//确认弹窗
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "保存确认",
			"是否确认保存为治具配方 [" + LSM->m_forMulaName +"]对应方案 ,是否保存或覆盖？",
			QMessageBox::Yes | QMessageBox::No);
		if (reply != QMessageBox::Yes) {
			return;
		}
		//易鼎丰，根据配方名写死
		if (!LSM->m_forMulaName.isEmpty())
		{
			ConfigPath = FolderPath() + "/Vision_formulation/Robot/";
			XmlPath = FolderPath() + "/Vision_formulation/Robot/" + LSM->m_forMulaName.toStdString() + ".xml";;
			nodeName = LSM->m_forMulaName.toStdString();
		}
	}
	else
	{

		QString defaultFileName = QCoreApplication::applicationDirPath();

		// 打开文件对话框，选择保存路径
		QString filePath = QFileDialog::getSaveFileName(this, "Save File", defaultFileName, "Files (*.xml);;All Files (*)");
		if (filePath.isEmpty() == false) {

			QFileInfo fileInfo(filePath);

			//            std::string dirName = fileInfo.baseName().toStdString();
			//            std::string configPath = fileInfo.absolutePath().toStdString() + "/" + dirName + "/";
			//            std::string xmlPath = fileInfo.absolutePath().toStdString() + "/" + dirName + "/" + fileInfo.fileName().toStdString();

			ConfigPath = fileInfo.path().toStdString() + "/";
			XmlPath = fileInfo.absoluteFilePath().toStdString();
			nodeName = fileInfo.baseName().toStdString();
		}
	}

	int ret = 1;
	//设置参数
	ret = SetRunParams_Calib(RunParams_RobotCalib);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
	ret = SetRunParams_Template(RunParams_RobotCalib);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
	//保存运行参数
	ret = RobotCalibDetect1->WriteParams_RobotCalib(ConfigPath, XmlPath, RunParams_RobotCalib, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
	RobotCalibDetect1->configPath = ConfigPath;
	RobotCalibDetect1->XMLPath = XmlPath;
	RobotCalibDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//读取参数
void RobotCalibWindow::on_pb_ReadData_clicked()
{
	if (!LSM->m_forMulaName.isEmpty())
	{
		ConfigPath = FolderPath() + "/Vision_formulation/Robot/";
		XmlPath = FolderPath() + "/Vision_formulation/Robot/" + LSM->m_forMulaName.toStdString() + ".xml";
		nodeName = LSM->m_forMulaName.toStdString();
	}

    QString defaultFileName = QCoreApplication::applicationDirPath() + "/Vision_formulation/Robot/";

    // 打开文件对话框，选择保存路径
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", defaultFileName, "Files (*.xml);;All Files (*)");

    if (filePath.isEmpty() == false) {
        QFileInfo fileInfo(filePath);
        ConfigPath = fileInfo.path().toStdString() + "/";
        XmlPath = fileInfo.absoluteFilePath().toStdString();
    }
	else
	{
		return;
	}


	ErrorCode_Xml errorCode;
	//读取参数
	errorCode = RobotCalibDetect1->ReadParams_RobotCalib(ConfigPath, XmlPath, RunParams_RobotCalib, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "提示", "参数读取失败", QMessageBox::Ok);
		return;
	}
	//参数更新到窗口
	UpDataWindow(RunParams_RobotCalib);
}
//设置标定参数
int RobotCalibWindow::SetRunParams_Calib(RunParamsStruct_RobotCalib &runParams)
{
	try
	{
		if (runParams.hv_HomMat2D == NULL)
		{
			QMessageBox::information(this, "提示", "机器人标定矩阵为空", QMessageBox::Ok);
			return 1;
		}
		if (ui.lineEdit_ModelX->text() == "" || ui.lineEdit_ModelY->text() == "" || ui.lineEdit_ModelA->text() == "")
		{
			QMessageBox::information(this, "提示", "模板参数为空", QMessageBox::Ok);
			return 1;
		}
		if (ui.lineEdit_CenterX->text() == "" || ui.lineEdit_CenterY->text() == "" || ui.lineEdit_CenterR->text() == "")
		{
			QMessageBox::information(this, "提示", "旋转中心参数为空", QMessageBox::Ok);
			return 1;
		}

		//九点标定参数赋值
		int count = ui.tablewidget_CoordinateData->rowCount();
		if (count < 9)
		{
			QMessageBox::information(this, "提示", "数据点小于9!", QMessageBox::Ok);
			return 1;
		}
		runParams.hv_NPointCamera_X.Clear();
		runParams.hv_NPointCamera_Y.Clear();
		runParams.hv_NPointRobot_X.Clear();
		runParams.hv_NPointRobot_Y.Clear();
		if (ui.cmb_locateMode->currentText() == "偏移量补偿" && ui.cmb_CamInHand->currentText() == "眼在手上")
		{
			for (int i = 0; i < count; i++)
			{
				//获取像素坐标
				runParams.hv_NPointCamera_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 0).data().toString().toDouble());
				runParams.hv_NPointCamera_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 1).data().toString().toDouble());
				//获取机器坐标
				if (i == 0)
				{
					runParams.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 3).data().toString().toDouble());
					runParams.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 4).data().toString().toDouble());
				}
				else
				{
					//坐标翻转
					if (i >= 1 && i <= 4)
					{
						runParams.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i + 4, 3).data().toString().toDouble());
						runParams.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i + 4, 4).data().toString().toDouble());
					}
					else if (i >= 5 && i <= 8)
					{
						runParams.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i - 4, 3).data().toString().toDouble());
						runParams.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i - 4, 4).data().toString().toDouble());
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < count; i++)
			{
				//获取像素坐标
				runParams.hv_NPointCamera_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 0).data().toString().toDouble());
				runParams.hv_NPointCamera_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 1).data().toString().toDouble());
				//获取机器坐标
				runParams.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 3).data().toString().toDouble());
				runParams.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 4).data().toString().toDouble());
			}
		}
		//计算偏移量场景
		if (ui.cmb_Scene->currentText() == "先拍后抓")
		{
			runParams.hv_IsCamFirst = true;
		}
		else
		{
			runParams.hv_IsCamFirst = false;
		}
		//补偿角度是否取反
		if (ui.cmb_ReverseAngle->currentText() == "角度不取反")
		{
			runParams.hv_IsReverseAngle = false;
		}
		else
		{
			runParams.hv_IsReverseAngle = true;
		}
		//旋转中心标定参数赋值
		runParams.hv_Center_X.Clear();
		runParams.hv_Center_Y.Clear();
		runParams.hv_Center_Radius.Clear();
		runParams.hv_ModelX.Clear();
		runParams.hv_ModelY.Clear();
		runParams.hv_ModelA.Clear();
		runParams.hv_Center_X = ui.lineEdit_CenterX->text().trimmed().toDouble();
		runParams.hv_Center_Y = ui.lineEdit_CenterY->text().trimmed().toDouble();
		runParams.hv_Center_Radius = ui.lineEdit_CenterR->text().trimmed().toDouble();
		runParams.hv_ModelX = ui.lineEdit_ModelX->text().trimmed().toDouble();
		runParams.hv_ModelY = ui.lineEdit_ModelY->text().trimmed().toDouble();
		runParams.hv_ModelA = ui.lineEdit_ModelA->text().trimmed().toDouble();
		runParams.hv_ModelPosition_RobotX = ui.lineEdit_ModelPosition_RobotX->text().trimmed().toDouble();
		runParams.hv_ModelPosition_RobotY = ui.lineEdit_ModelPosition_RobotY->text().trimmed().toDouble();
		runParams.hv_ModelPosition_RobotA = ui.lineEdit_ModelPosition_RobotA->text().trimmed().toDouble();
		runParams.hv_CenterPosition_RobotX = ui.dspb_RobotOriX->value();
		runParams.hv_CenterPosition_RobotY = ui.dspb_RobotOriY->value();
		runParams.hv_CenterPosition_RobotA = ui.dspb_RobotOriA->value(); 
		
		runParams.hv_CenterCamera_X.Clear();
		runParams.hv_CenterCamera_Y.Clear();
		runParams.hv_CenterRobot_X.Clear();
		runParams.hv_CenterRobot_Y.Clear();
	
		count = ui.tablewidget_CoordinateData_RotateCenter->rowCount();
		if (count < 3)
		{
			QMessageBox::information(this, "提示", "数据点小于3!", QMessageBox::Ok);
			return 1;
		}

		for (int i = 0; i < count; i++)
		{
			//获取像素坐标
			runParams.hv_CenterCamera_X.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 0).data().toString().toDouble());
			runParams.hv_CenterCamera_Y.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 1).data().toString().toDouble());
			//获取机器坐标
			runParams.hv_CenterRobot_X.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 2).data().toString().toDouble());
			runParams.hv_CenterRobot_Y.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 3).data().toString().toDouble());
		}
		//是否计算绝对坐标或者偏移量
		if (ui.cmb_locateMode->currentText() == "绝对坐标引导")
		{
			runParams.hv_IsAbsCoordinate = true;
		}
		else
		{
			runParams.hv_IsAbsCoordinate = false;
		}
		//是否启用旋转中心
		if (ui.ckb_UseCenterCalib->checkState() == Qt::Checked)
		{
			runParams.hv_IsUseCenterCalib = true;
		}
		else
		{
			runParams.hv_IsUseCenterCalib = false;
		}
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}
//设置模板匹配参数
int RobotCalibWindow::SetRunParams_Template(RunParamsStruct_RobotCalib &runParams)
{
	try
	{
		ModelTypeStruct_Template_RobotCalib typeStructTemplate;
		string hv_typeTemp;

		if (ui.cmb_MatchMethod_Train->currentIndex() == 0)
		{
			hv_typeTemp = typeStructTemplate.Aniso;
		}
		else if (ui.cmb_MatchMethod_Train->currentIndex() == 1)
		{
			hv_typeTemp = typeStructTemplate.Ncc;
		}
		else
		{
			hv_typeTemp = typeStructTemplate.Aniso;
		}
		runParams.hv_MatchMethod = hv_typeTemp;   //比例模式:"none"无缩放,"scaled"同步缩放,"aniso"异步缩放,"auto"自动

		runParams.hv_NumLevels = ui.spb_NumLevels_Train->value();  //金字塔级别

		runParams.hv_AngleStart = ui.dspb_AngleStart_Train->value();             //起始角度
		runParams.hv_AngleExtent = ui.dspb_AngleExtent_Train->value();            //角度范围

		runParams.hv_ScaleRMin = ui.dspb_ScaleRMin_Train->value();              //最小行缩放
		runParams.hv_ScaleRMax = ui.dspb_ScaleRMax_Train->value();              //最大行缩放

		runParams.hv_ScaleCMin = ui.dspb_ScaleCMin_Train->value();              //最小列缩放
		runParams.hv_ScaleCMax = ui.dspb_ScaleCMax_Train->value();              //最大列缩放

		runParams.hv_Contrast = ui.spb_Contrast_Train->value();               //对比度
		runParams.hv_MinContrast = ui.spb_MinContrast_Train->value();   //最小对比度

		runParams.hv_MinScore = ui.dspb_MinScore_Run->value();               //最小分数
		runParams.hv_NumMatches = ui.spb_NumMatches_Run->value();             //匹配个数，0则自动选择，100则最多匹配100个
		runParams.hv_TimeOut = ui.spb_Timeout->value();		//超时时间

		//***********搜索区域保存*******************************************************************
		if (vec_RoiSearch_Template.size() == 1)
		{
			HObject roiTemp;
			MRectangle rec1;
			vec_RoiSearch_Template[0]->GetRect(rec1);
			GenRectangle1(&roiTemp, rec1.row, rec1.col, rec1.row + rec1.height, rec1.col + rec1.width);
			runParams.P1_SearchRoi.x = rec1.col;
			runParams.P1_SearchRoi.y = rec1.row;
			runParams.P2_SearchRoi.x = rec1.col + rec1.width;
			runParams.P2_SearchRoi.y = rec1.row + rec1.height;
			runParams.hv_IsUsSearchRoi = 1;
		}
		else
		{
			runParams.hv_IsUsSearchRoi = 0;
		}
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}
//更新参数到窗口
void RobotCalibWindow::UpDataWindow(const RunParamsStruct_RobotCalib &runParams)
{
	try
	{
		//****************************更新标定参数**********************************************
		view->ClearObj();
		on_pb_ClearValue_clicked();
		for (int i = 0; i < runParams.hv_NPointCamera_X.TupleLength().I(); i++)
		{
			ui.tablewidget_CoordinateData->setRowCount(i + 1);

			QTableWidgetItem *item;
			QString str;
			
			//像素X
			str = QString::number(runParams.hv_NPointCamera_X[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData->setItem(i, 0, item);
			//像素Y
			str = QString::number(runParams.hv_NPointCamera_Y[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData->setItem(i, 1, item);
			//手动取值
			QPushButton *getPixelButton = new QPushButton;
			getPixelButton->setText("获取");
			ui.tablewidget_CoordinateData->setCellWidget(i, 2, getPixelButton);
			connect(getPixelButton, SIGNAL(clicked()), this, SLOT(slot_GetPixel()));
			//机器X
			str = QString::number(runParams.hv_NPointRobot_X[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData->setItem(i, 3, item);
			//机器Y
			str = QString::number(runParams.hv_NPointRobot_Y[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData->setItem(i, 4, item);
			//删除行
			QPushButton *deleteButton = new QPushButton;
			deleteButton->setText("删除");
			ui.tablewidget_CoordinateData->setCellWidget(i, 5, deleteButton);
			connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow()));
		}

		on_pb_ClearValue_RotateCenter_clicked();
		for (int i = 0; i < runParams.hv_CenterCamera_X.TupleLength().I(); i++)
		{
			ui.tablewidget_CoordinateData_RotateCenter->setRowCount(i + 1);

			QTableWidgetItem *item;
			QString str;

			//像素X
			str = QString::number(runParams.hv_CenterCamera_X[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData_RotateCenter->setItem(i, 0, item);
			//像素Y
			str = QString::number(runParams.hv_CenterCamera_Y[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData_RotateCenter->setItem(i, 1, item);
			//机器X
			str = QString::number(runParams.hv_CenterRobot_X[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData_RotateCenter->setItem(i, 2, item);
			//机器Y
			str = QString::number(runParams.hv_CenterRobot_Y[i].D(), 'f', 3);
			item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData_RotateCenter->setItem(i, 3, item);
			//手动取值
			QPushButton *getPixelButton = new QPushButton;
			getPixelButton->setText("获取");
			ui.tablewidget_CoordinateData_RotateCenter->setCellWidget(i, 4, getPixelButton);
			connect(getPixelButton, SIGNAL(clicked()), this, SLOT(slot_GetRobot_RotateCenter()));
			//删除行
			QPushButton *deleteButton = new QPushButton;
			deleteButton->setText("删除");
			ui.tablewidget_CoordinateData_RotateCenter->setCellWidget(i, 5, deleteButton);
			connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow_RotateCenter()));
		}

		ui.lineEdit_CenterX->setText(QString::number(runParams.hv_Center_X.D(), 'f', 3));
		ui.lineEdit_CenterY->setText(QString::number(runParams.hv_Center_Y.D(), 'f', 3));
		ui.lineEdit_CenterR->setText(QString::number(runParams.hv_Center_Radius.D(), 'f', 3));

		ui.lineEdit_ModelX->setText(QString::number(runParams.hv_ModelX.D(), 'f', 3));
		ui.lineEdit_ModelY->setText(QString::number(runParams.hv_ModelY.D(), 'f', 3));
		ui.lineEdit_ModelA->setText(QString::number(runParams.hv_ModelA.D(), 'f', 3));

		ui.lineEdit_ModelPosition_RobotX->setText(QString::number(runParams.hv_ModelPosition_RobotX.D(), 'f', 3));
		ui.lineEdit_ModelPosition_RobotY->setText(QString::number(runParams.hv_ModelPosition_RobotY.D(), 'f', 3));
		ui.lineEdit_ModelPosition_RobotA->setText(QString::number(runParams.hv_ModelPosition_RobotA.D(), 'f', 3));

		ui.dspb_RobotOriX->setValue(runParams.hv_CenterPosition_RobotX.D());
		ui.dspb_RobotOriY->setValue(runParams.hv_CenterPosition_RobotY.D());
		ui.dspb_RobotOriA->setValue(runParams.hv_CenterPosition_RobotA.D());

		if (runParams.hv_IsCamFirst)
		{
			ui.cmb_Scene->setCurrentText("先拍后抓");
		}
		else
		{
			ui.cmb_Scene->setCurrentText("先抓后拍");
		}

		if (runParams.hv_IsReverseAngle)
		{
			ui.cmb_ReverseAngle->setCurrentText("角度取反");
		}
		else
		{
			ui.cmb_ReverseAngle->setCurrentText("角度不取反");
		}

		if (runParams.hv_IsAbsCoordinate)
		{
			ui.cmb_locateMode->setCurrentText("绝对坐标引导");
		}
		else
		{
			ui.cmb_locateMode->setCurrentText("偏移量补偿");
		}

		if (runParams.hv_IsUseCenterCalib == true)
		{
			ui.ckb_UseCenterCalib->setCheckState(Qt::Checked);
		}
		else
		{
			ui.ckb_UseCenterCalib->setCheckState(Qt::Unchecked);
		}

		//**********************更新模板匹配参数***********************************************
		ModelTypeStruct_Template_RobotCalib typeStructTemplate;
		if (runParams.hv_MatchMethod == typeStructTemplate.Aniso)
		{
			ui.cmb_MatchMethod_Train->setCurrentIndex(0);
		}
		else if (runParams.hv_MatchMethod == typeStructTemplate.Ncc)
		{
			ui.cmb_MatchMethod_Train->setCurrentIndex(1);
		}
		else
		{
			ui.cmb_MatchMethod_Train->setCurrentIndex(0);
		}

		ui.spb_NumLevels_Train->setValue(runParams.hv_NumLevels);

		ui.dspb_AngleStart_Train->setValue(runParams.hv_AngleStart);
		ui.dspb_AngleExtent_Train->setValue(runParams.hv_AngleExtent);

		ui.dspb_ScaleRMin_Train->setValue(runParams.hv_ScaleRMin);
		ui.dspb_ScaleRMax_Train->setValue(runParams.hv_ScaleRMax);

		ui.dspb_ScaleCMin_Train->setValue(runParams.hv_ScaleCMin);
		ui.dspb_ScaleCMax_Train->setValue(runParams.hv_ScaleCMax);

		ui.spb_Contrast_Train->setValue(runParams.hv_Contrast);

		ui.spb_MinContrast_Train->setValue(runParams.hv_MinContrast);

		//更新运行参数
		ui.dspb_MinScore_Run->setValue(runParams.hv_MinScore);
		ui.spb_NumMatches_Run->setValue(runParams.hv_NumMatches);
		ui.spb_Timeout->setValue(runParams.hv_TimeOut);
		if (runParams.hv_IsUsSearchRoi == 1)
		{
			ui.cmb_SearchRoi->setCurrentIndex(1);
			if (vec_RoiSearch_Template.size() > 0)
			{
				vec_RoiSearch_Template.clear();
			}
			RectangleItem * Roi = new RectangleItem(runParams.P1_SearchRoi.x, runParams.P1_SearchRoi.y,
				runParams.P2_SearchRoi.x - runParams.P1_SearchRoi.x, runParams.P2_SearchRoi.y - runParams.P1_SearchRoi.y);
			Roi->ItemDiscrib = "搜索区域";
			view->AddItems(Roi);
			vec_RoiSearch_Template.push_back(Roi);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "参数刷新到窗口失败", QMessageBox::Ok);
		return;
	}
}
//初始化数据显示表格
void RobotCalibWindow::IniTableData()
{
	//机器人标定
	QTableWidgetItem   *hearderItem;
	QStringList hearderText;
	hearderText << "像素X" << "像素Y" << "手动取值" << "机器X" << "机器Y" << "删除行";
	ui.tablewidget_CoordinateData->setColumnCount(hearderText.count());
	for (int i = 0; i < ui.tablewidget_CoordinateData->columnCount(); i++)
	{
		hearderItem = new QTableWidgetItem(hearderText.at(i));
		QFont font = hearderItem->font();
		font.setBold(true);//设置为粗体
		font.setPointSize(12);//设置字体大小
		//hearderItem->setTextColor(Qt::red);//字体颜色
		hearderItem->setForeground(QBrush(Qt::red));//字体颜色
		hearderItem->setFont(font);//设置字体
		ui.tablewidget_CoordinateData->setHorizontalHeaderItem(i, hearderItem);
	}
	//旋转中心标定
	QTableWidgetItem   *hearderItem2;
	QStringList hearderText2;
	hearderText2 << "像素X" << "像素Y" << "机器X" << "机器Y" << "手动取值" << "删除行";
	ui.tablewidget_CoordinateData_RotateCenter->setColumnCount(hearderText2.count());
	for (int i = 0; i < ui.tablewidget_CoordinateData_RotateCenter->columnCount(); i++)
	{
		hearderItem2 = new QTableWidgetItem(hearderText2.at(i));
		QFont font = hearderItem2->font();
		font.setBold(true);//设置为粗体
		font.setPointSize(12);//设置字体大小
		//hearderItem2->setTextColor(Qt::red);//字体颜色
		hearderItem2->setForeground(QBrush(Qt::red));//字体颜色
		hearderItem2->setFont(font);//设置字体
		ui.tablewidget_CoordinateData_RotateCenter->setHorizontalHeaderItem(i, hearderItem2);
	}
	//模板匹配
	QTableWidgetItem   *hearderItem3;
	QStringList hearderText3;
	hearderText3 << "X" << "Y" << "角度" << "分数";
	ui.tablewidget_Result_Template->setColumnCount(hearderText3.count());
	for (int i = 0; i < ui.tablewidget_Result_Template->columnCount(); i++)
	{
		hearderItem3 = new QTableWidgetItem(hearderText3.at(i));
		QFont font = hearderItem3->font();
		font.setBold(true);//设置为粗体
		font.setPointSize(12);//设置字体大小
		//hearderItem->setTextColor(Qt::red);//字体颜色
		hearderItem3->setForeground(QBrush(Qt::red));//字体颜色
		hearderItem3->setFont(font);//设置字体
		ui.tablewidget_Result_Template->setHorizontalHeaderItem(i, hearderItem3);
	}
}
//机器人标定添加变量
void RobotCalibWindow::on_pb_AddValue_clicked()
{
	int count = ui.tablewidget_CoordinateData->rowCount();
	ui.tablewidget_CoordinateData->setRowCount(count + 1);

	QTableWidgetItem *item;
	QString str;

	//像素X
	str = "0.00";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData->setItem(count, 0, item);
	//像素Y
	str = "0.00";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData->setItem(count, 1, item);
	//手动取值
	QPushButton *getPixelButton = new QPushButton;
	getPixelButton->setText("获取");
	ui.tablewidget_CoordinateData->setCellWidget(count, 2, getPixelButton);
	connect(getPixelButton, SIGNAL(clicked()), this, SLOT(slot_GetPixel()));
	//机器X
	str = "0.00";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData->setItem(count, 3, item);
	//机器Y
	str = "0.00";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData->setItem(count, 4, item);
	//删除行
	QPushButton *deleteButton = new QPushButton;
	deleteButton->setText("删除");
	ui.tablewidget_CoordinateData->setCellWidget(count, 5, deleteButton);
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow()));
}
//机器人标定删除行槽函数
void RobotCalibWindow::slot_DeleteRow()
{
	int selectRow = ui.tablewidget_CoordinateData->currentRow();
	if (selectRow < 0)
	{
		QMessageBox::information(this, "提示", "请选择被删除项!", QMessageBox::Ok);
		return;
	}
	ui.tablewidget_CoordinateData->removeRow(selectRow);
}
//机器人标定清空变量
void RobotCalibWindow::on_pb_ClearValue_clicked()
{
	//清空结果表格数据
	ui.tablewidget_CoordinateData->clearContents();
	ui.tablewidget_CoordinateData->setRowCount(0);
}
//获取像素坐标槽函数
void RobotCalibWindow::slot_GetPixel()
{

	int selectRow = ui.tablewidget_CoordinateData->currentRow();
	if (selectRow < 0)
	{
		QMessageBox::information(this, "提示", "请选择获取像素坐标的行!", QMessageBox::Ok);
		return;
	}
	double x1 = 0.00, y1 = 0.00;
	HObject objTemp;
	//计算像素坐标
	int ret = MarkCenter(ho_Image, x1, y1, objTemp);
	if (ret == 0)
	{
		//画结果图
		cv::Mat destImg;
		RunParamsStruct_PaintDetect runParams_Pain;
		runParams_Pain.vec_InputRegion.clear();
		runParams_Pain.vec_Ret.clear();
		runParams_Pain.hv_LineWidth = 5;
		runParams_Pain.hv_Word_Size = 10;
		runParams_Pain.hv_Word_X = 100;
		runParams_Pain.hv_Word_Y = 100;
		//不画ok/ng字符串
		runParams_Pain.hv_Ret = -1;
		runParams_Pain.vec_Ret.push_back(1);
		HObject unionRegion;
		GenRegionContourXld(objTemp, &unionRegion, "margin");
		Union1(unionRegion, &unionRegion);
		runParams_Pain.vec_InputRegion.push_back(unionRegion);
		PaintDetect2->PaintImage(ho_Image, runParams_Pain, destImg);
		Q_OutputImg = Common1->Mat2QImage(destImg);
		view->DispImage(Q_OutputImg);
	}
	else
	{
		QMessageBox::information(this, "提示", "找圆失败!", QMessageBox::Ok);
		return;
	}
	//刷新第1,2列函数，像素坐标
	QTableWidgetItem *item1;
	item1 = new QTableWidgetItem(QString::number(x1, 'f', 3), 1000);
	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData->setItem(selectRow, 0, item1);
	item1 = new QTableWidgetItem(QString::number(y1, 'f', 3), 1000);
	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData->setItem(selectRow, 1, item1);
}

//机器人标定开始
void RobotCalibWindow::on_pb_Calibrate_clicked()
{
	try
	{
		int count = ui.tablewidget_CoordinateData->rowCount();
		if (count != 9)
		{
			QMessageBox::information(this, "提示", "数据点不等于9!", QMessageBox::Ok);
			return;
		}
		//运行参数结构体赋值
		RunParams_RobotCalib.hv_NPointCamera_X.Clear();
		RunParams_RobotCalib.hv_NPointCamera_Y.Clear();
		RunParams_RobotCalib.hv_NPointRobot_X.Clear();
		RunParams_RobotCalib.hv_NPointRobot_Y.Clear();
		RunParams_RobotCalib.hv_HomMat2D.Clear();
		if (ui.cmb_locateMode->currentText() == "偏移量补偿" && ui.cmb_CamInHand->currentText() == "眼在手上")
		{
			for (int i = 0; i < count; i++)
			{
				//获取像素坐标
				RunParams_RobotCalib.hv_NPointCamera_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 0).data().toString().toDouble());
				RunParams_RobotCalib.hv_NPointCamera_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 1).data().toString().toDouble());
				//获取机器坐标
				if (i == 0)
				{
					RunParams_RobotCalib.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 3).data().toString().toDouble());
					RunParams_RobotCalib.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 4).data().toString().toDouble());
				}
				else
				{
					//坐标翻转
					if (i >= 1 && i <= 4)
					{
						RunParams_RobotCalib.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i + 4, 3).data().toString().toDouble());
						RunParams_RobotCalib.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i + 4, 4).data().toString().toDouble());
					}
					else if (i >= 5 && i <= 8)
					{
						RunParams_RobotCalib.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i - 4, 3).data().toString().toDouble());
						RunParams_RobotCalib.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i - 4, 4).data().toString().toDouble());
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < count; i++)
			{
				//获取像素坐标
				RunParams_RobotCalib.hv_NPointCamera_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 0).data().toString().toDouble());
				RunParams_RobotCalib.hv_NPointCamera_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 1).data().toString().toDouble());
				//获取机器坐标
				RunParams_RobotCalib.hv_NPointRobot_X.Append(ui.tablewidget_CoordinateData->model()->index(i, 3).data().toString().toDouble());
				RunParams_RobotCalib.hv_NPointRobot_Y.Append(ui.tablewidget_CoordinateData->model()->index(i, 4).data().toString().toDouble());
			}
		}

		//标定
		int ret = RobotCalibDetect1->NPointCalibrate(RunParams_RobotCalib);
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "机器人标定失败!", QMessageBox::Ok);
		}
		else
		{
			//计算标定误差
			HTuple rX1, rY1;
			AffineTransPoint2d(RunParams_RobotCalib.hv_HomMat2D, RunParams_RobotCalib.hv_NPointCamera_X[0], RunParams_RobotCalib.hv_NPointCamera_Y[0], &rX1, &rY1);
			double rms = (abs(RunParams_RobotCalib.hv_NPointRobot_X[0].D() - rX1.D()) > abs(RunParams_RobotCalib.hv_NPointRobot_Y[0].D() - rY1.D())) 
				? abs(RunParams_RobotCalib.hv_NPointRobot_X[0].D() - rX1.D()) : abs(RunParams_RobotCalib.hv_NPointRobot_Y[0].D() - rY1.D());
			
			QMessageBox::information(this, "提示", "机器人标定成功,RMS：" + QString::number(rms, 'f', 4), QMessageBox::Ok);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "机器人标定失败!", QMessageBox::Ok);
	}
}
//旋转中心添加变量
void RobotCalibWindow::on_pb_AddValue_RotateCenter_clicked()
{
	int count = ui.tablewidget_CoordinateData_RotateCenter->rowCount();
	ui.tablewidget_CoordinateData_RotateCenter->setRowCount(count + 1);

	QTableWidgetItem *item;
	QString str;

	//像素X
	str = "0.000";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(count, 0, item);
	//像素Y
	str = "0.000";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(count, 1, item);
	//机器X
	str = "0.000";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(count, 2, item);
	//机器Y
	str = "0.000";
	item = new QTableWidgetItem(str, 1000);
	item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(count, 3, item);
	//手动取值
	QPushButton *getPixelButton = new QPushButton;
	getPixelButton->setText("获取");
	ui.tablewidget_CoordinateData_RotateCenter->setCellWidget(count, 4, getPixelButton);
	connect(getPixelButton, SIGNAL(clicked()), this, SLOT(slot_GetRobot_RotateCenter()));
	//删除行
	QPushButton *deleteButton = new QPushButton;
	deleteButton->setText("删除");
	ui.tablewidget_CoordinateData_RotateCenter->setCellWidget(count, 5, deleteButton);
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_DeleteRow_RotateCenter()));
}
//旋转中心清空变量
void RobotCalibWindow::on_pb_ClearValue_RotateCenter_clicked()
{
	//清空结果表格数据
	ui.tablewidget_CoordinateData_RotateCenter->clearContents();
	ui.tablewidget_CoordinateData_RotateCenter->setRowCount(0);
}
//旋转中心标定开始
void RobotCalibWindow::on_pb_Calibrate_RotateCenter_clicked()
{
	try
	{
		ui.lineEdit_CenterX->setText("");
		ui.lineEdit_CenterY->setText("");
		ui.lineEdit_CenterR->setText("");
		int count = ui.tablewidget_CoordinateData_RotateCenter->rowCount();
		if (count < 3)
		{
			QMessageBox::information(this, "提示", "数据点小于3!", QMessageBox::Ok);
			return;
		}
		//运行参数结构体赋值
		RunParams_RobotCalib.hv_CenterCamera_X.Clear();
		RunParams_RobotCalib.hv_CenterCamera_Y.Clear();
		RunParams_RobotCalib.hv_CenterRobot_X.Clear();
		RunParams_RobotCalib.hv_CenterRobot_Y.Clear();
	
		RunParams_RobotCalib.hv_Center_X.Clear();
		RunParams_RobotCalib.hv_Center_Y.Clear();
		RunParams_RobotCalib.hv_Center_Radius.Clear();
		for (int i = 0; i < count; i++)
		{
            double x1,y1,x2,y2;
			//获取像素坐标
			RunParams_RobotCalib.hv_CenterCamera_X.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 0).data().toString().toDouble());
			RunParams_RobotCalib.hv_CenterCamera_Y.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 1).data().toString().toDouble());
            x1 = ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 0).data().toString().toDouble();
            y1 = ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 1).data().toString().toDouble();
			//获取机器坐标
			RunParams_RobotCalib.hv_CenterRobot_X.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 2).data().toString().toDouble());
			RunParams_RobotCalib.hv_CenterRobot_Y.Append(ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 3).data().toString().toDouble());
            x2 = ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 2).data().toString().toDouble();
            y2 = ui.tablewidget_CoordinateData_RotateCenter->model()->index(i, 3).data().toString().toDouble();
		}
		//标定
		int ret = RobotCalibDetect1->CenterCalibrate(RunParams_RobotCalib);
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "旋转中心标定失败!", QMessageBox::Ok);
			return;
		}
		else
		{
			ui.lineEdit_CenterX->setText(QString::number(RunParams_RobotCalib.hv_Center_X.D(), 'f', 3));
			ui.lineEdit_CenterY->setText(QString::number(RunParams_RobotCalib.hv_Center_Y.D(), 'f', 3));
			ui.lineEdit_CenterR->setText(QString::number(RunParams_RobotCalib.hv_Center_Radius.D(), 'f', 3));

			//画结果图
			cv::Mat destImg;
			RunParamsStruct_PaintDetect runParams_Pain;
			runParams_Pain.vec_InputRegion.clear();
			runParams_Pain.vec_Ret.clear();
			runParams_Pain.hv_LineWidth = 5;
			runParams_Pain.hv_Word_Size = 10;
			runParams_Pain.hv_Word_X = 100;
			runParams_Pain.hv_Word_Y = 100;
			//不画ok/ng字符串
			runParams_Pain.hv_Ret = -1;
			runParams_Pain.vec_Ret.push_back(1);
			HObject unionRegion;
			GenRegionContourXld(RunParams_RobotCalib.ho_CenterCircle, &unionRegion, "margin");
			Union1(unionRegion, &unionRegion);
			runParams_Pain.vec_InputRegion.push_back(unionRegion);
			PaintDetect2->PaintImage(ho_Image, runParams_Pain, destImg);
			Q_OutputImg = Common1->Mat2QImage(destImg);
			view->DispImage(Q_OutputImg);
			QMessageBox::information(this, "提示", "旋转中心标定成功!", QMessageBox::Ok);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "旋转中心标定失败!", QMessageBox::Ok);
	}
}
//旋转中心删除行槽函数
void RobotCalibWindow::slot_DeleteRow_RotateCenter()
{
	int selectRow = ui.tablewidget_CoordinateData_RotateCenter->currentRow();
	if (selectRow < 0)
	{
		QMessageBox::information(this, "提示", "请选择被删除项!", QMessageBox::Ok);
		return;
	}
	ui.tablewidget_CoordinateData_RotateCenter->removeRow(selectRow);
}
//旋转中心获取机器坐标槽函数
void RobotCalibWindow::slot_GetRobot_RotateCenter()
{
	int selectRow = ui.tablewidget_CoordinateData_RotateCenter->currentRow();
	if (selectRow < 0)
	{
		QMessageBox::information(this, "提示", "请选择获取像素坐标的行!", QMessageBox::Ok);
		return;
	}
	double x1 = 0.00, y1 = 0.00;

	//计算像素坐标
	HObject objTemp;
	int ret = MarkCenter(ho_Image, x1, y1, objTemp);
	if (ret == 0)
	{
		//画结果图
		cv::Mat destImg;
		RunParamsStruct_PaintDetect runParams_Pain;
		runParams_Pain.vec_InputRegion.clear();
		runParams_Pain.vec_Ret.clear();
		runParams_Pain.hv_LineWidth = 5;
		runParams_Pain.hv_Word_Size = 10;
		runParams_Pain.hv_Word_X = 100;
		runParams_Pain.hv_Word_Y = 100;
		//不画ok/ng字符串
		runParams_Pain.hv_Ret = -1;
		runParams_Pain.vec_Ret.push_back(1);
		HObject unionRegion;
		GenRegionContourXld(objTemp, &unionRegion, "margin");
		Union1(unionRegion, &unionRegion);
		runParams_Pain.vec_InputRegion.push_back(unionRegion);
		PaintDetect2->PaintImage(ho_Image, runParams_Pain, destImg);
		Q_OutputImg = Common1->Mat2QImage(destImg);
		view->DispImage(Q_OutputImg);
	}
	else
	{
		QMessageBox::information(this, "提示", "找圆失败!", QMessageBox::Ok);
		return;
	}
	//刷新第1,2列函数，像素坐标
	QTableWidgetItem *item1;
	item1 = new QTableWidgetItem(QString::number(x1, 'f', 3), 1000);
	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(selectRow, 0, item1);
	item1 = new QTableWidgetItem(QString::number(y1, 'f', 3), 1000);
	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(selectRow, 1, item1);
	//转换到机器坐标
	HTuple Qx, Qy;
	AffineTransPoint2d(RunParams_RobotCalib.hv_HomMat2D, x1, y1, &Qx, &Qy);
	//刷新第3,4列函数，机器坐标
	item1 = new QTableWidgetItem(QString::number(Qx, 'f', 3), 1000);
	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(selectRow, 2, item1);
	item1 = new QTableWidgetItem(QString::number(Qy, 'f', 3), 1000);
	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.tablewidget_CoordinateData_RotateCenter->setItem(selectRow, 3, item1);
}
//计算Mark点中心
int RobotCalibWindow::MarkCenter(const HObject &ho_Img, double &X, double &Y, HObject &retContour)
{
	try
	{
		HObject GrayImg;
		Rgb1ToGray(ho_Img, &GrayImg);
		MedianRect(GrayImg, &GrayImg, 3, 3);
		GenEmptyObj(&retContour);
		retContour.Clear();
		X = 0.00;
		Y = 0.00;

		HObject roiTemp;
		MRectangle rec1;
		vec_RoiSearch_Calib[0]->GetRect(rec1);
		GenRectangle1(&roiTemp, rec1.row, rec1.col, rec1.row + rec1.height, rec1.col + rec1.width);
		HObject reduceImg;
		ReduceDomain(GrayImg, roiTemp, &reduceImg);
		HObject region;
		HTuple usedThreshold;
		BinaryThreshold(reduceImg, &region, "max_separability", "dark", &usedThreshold);
		HObject connectRegion;
		Connection(region, &connectRegion);
		HObject selectRegion;
		SelectShapeStd(connectRegion, &selectRegion, "max_area", 70);
		HTuple row, col, radius;
		SmallestCircle(selectRegion, &row, &col, &radius);
		//检测圆形
		HTuple hv_Index;            //模型id(例如同一个模型一次抓两个圆，hv_Index就是[0,1])
		HTuple hv_MetrologyHandle;        //测量句柄
		//创建测量句柄
		CreateMetrologyModel(&hv_MetrologyHandle);
		//获取模型尺寸
		HTuple hv_Width, hv_Height;
		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		//添加测量对象
		SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
		AddMetrologyObjectCircleMeasure(hv_MetrologyHandle, row, col, radius,
			50, 2, 1, 20, "measure_transition", "positive", &hv_Index);
		//设置测量对象的参数
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_select", "all");
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_measures", 30);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_instances", 1);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "min_score", 0.1);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "distance_threshold", 3.5);
		SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_interpolation", "bilinear");
		//执行抓圆模型
		ApplyMetrologyModel(ho_Image, hv_MetrologyHandle);
		//获取抓圆结果
		HTuple hv_CircleRowCenter, hv_CircleColumnCenter, hv_CircleRadius;
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "row", &hv_CircleRowCenter);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "column", &hv_CircleColumnCenter);
		GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "result_type", "radius", &hv_CircleRadius);
		//获取圆轮廓
		HObject ho_Contour_Circle;
		GetMetrologyObjectResultContour(&ho_Contour_Circle, hv_MetrologyHandle, 0, "all", 1.5);
		//判断是否找到圆
		HTuple hv_Length;
		TupleLength(hv_CircleRowCenter, &hv_Length);
		if (hv_Length.I() > 0)
		{
			X = hv_CircleColumnCenter.D();
			Y = hv_CircleRowCenter.D();
			HObject cross;
            GenCrossContourXld(&cross, hv_CircleRowCenter, hv_CircleColumnCenter, 16, 0.78);
			ConcatObj(ho_Contour_Circle, cross, &cross);
			retContour = cross;
			return 0;
		}
		else
		{
			GenEmptyObj(&retContour);
			retContour.Clear();
			X = 0.00;
			Y = 0.00;
			return 1;
		}
	
	}
	catch (...)
	{
		GenEmptyObj(&retContour);
		retContour.Clear();
		X = 0.00;
		Y = 0.00;
		return -1;
	}
}
//计算机器人抓取偏移量
void RobotCalibWindow::on_pb_GetOffset_clicked()
{
	ui.lineEdit_OffsetX->setText("");
	ui.lineEdit_OffsetY->setText("");
	ui.lineEdit_OffsetA->setText("");

	if (ui.lineEdit_NowX->text() == "" || ui.lineEdit_NowY->text() == "" || ui.lineEdit_NowA->text() == "")
	{
		//图像为空，或者图像被clear
		QMessageBox::information(this, "提示", "当前坐标不能为空!", QMessageBox::Ok);
		return;
	}
	//当前值赋值到结构体
	RunParams_RobotCalib.hv_NowX = ui.lineEdit_NowX->text().trimmed().toDouble();
	RunParams_RobotCalib.hv_NowY = ui.lineEdit_NowY->text().trimmed().toDouble();
	RunParams_RobotCalib.hv_NowA = ui.lineEdit_NowA->text().trimmed().toDouble();
	RunParamsStruct_RobotCalib paramsTemp = RunParams_RobotCalib;
	paramsTemp.hv_IsAbsCoordinate = false;
	//计算偏移量
	ResultParamsStruct_RobotCalib resultParams;
	int ret = slot_CalculateLocate(paramsTemp, resultParams);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "偏移量计算失败!", QMessageBox::Ok);
		return;
	}
	//刷新结果
	SetOffsetData(RunParams_RobotCalib, resultParams);
}
//计算偏移量函数
int RobotCalibWindow::slot_CalculateLocate(RunParamsStruct_RobotCalib &RunParams, ResultParamsStruct_RobotCalib &ResultParams)
{
	try
	{
		//XML读取参数
		ErrorCode_Xml errorCode = RobotCalibDetect1->ReadParams_RobotCalib(ConfigPath, XmlPath, RunParams, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML读取失败
			return 1;
		}
		//调用算法
		return RobotCalibDetect1->CalculateFunc(RunParams, ResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//刷新偏移量结果
void RobotCalibWindow::SetOffsetData(const RunParamsStruct_RobotCalib &runParams, const ResultParamsStruct_RobotCalib &resultParams)
{
	if (runParams.hv_IsAbsCoordinate)
	{
		ui.lineEdit_RobotX_Abs->setText(QString::number(resultParams.hv_X_Robot, 'f', 3));
		ui.lineEdit_RobotY_Abs->setText(QString::number(resultParams.hv_Y_Robot, 'f', 3));
		ui.lineEdit_RobotA_Abs->setText(QString::number(resultParams.hv_A_Robot, 'f', 3));
	}
	else
	{
		ui.lineEdit_OffsetX->setText(QString::number(resultParams.hv_X_Robot, 'f', 3));
		ui.lineEdit_OffsetY->setText(QString::number(resultParams.hv_Y_Robot, 'f', 3));
		ui.lineEdit_OffsetA->setText(QString::number(resultParams.hv_A_Robot, 'f', 3));
	}
}
//计算机器人绝对坐标
void RobotCalibWindow::on_pb_TransPoint_clicked()
{
	try
	{
		ui.lineEdit_RobotX_Abs->setText("");
		ui.lineEdit_RobotY_Abs->setText("");
		ui.lineEdit_RobotA_Abs->setText("");

		if (ui.lineEdit_PixelX_Abs->text() == "" || ui.lineEdit_PixelY_Abs->text() == "" || ui.lineEdit_PixelA_Abs->text() == "")
		{
			//图像为空，或者图像被clear
			QMessageBox::information(this, "提示", "像素坐标不能为空!", QMessageBox::Ok);
			return;
		}
        RunParams_RobotCalib.hv_NowX = ui.lineEdit_PixelX_Abs->text().trimmed().toDouble();
        RunParams_RobotCalib.hv_NowY = ui.lineEdit_PixelY_Abs->text().trimmed().toDouble();
        RunParams_RobotCalib.hv_NowA = ui.lineEdit_PixelA_Abs->text().trimmed().toDouble();
		RunParamsStruct_RobotCalib paramsTemp = RunParams_RobotCalib;
		paramsTemp.hv_IsAbsCoordinate = true;
		//计算机器人坐标
		ResultParamsStruct_RobotCalib resultParams;
		int ret = slot_CalculateLocate(paramsTemp, resultParams);
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "绝对坐标计算失败!", QMessageBox::Ok);
			return;
		}
		//刷新结果
		SetOffsetData(RunParams_RobotCalib, resultParams);
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "绝对坐标计算失败!", QMessageBox::Ok);
		return;
	}
}
//引导模式
void RobotCalibWindow::on_cmb_locateMode_activated(const QString &arg1)
{
	if (ui.cmb_locateMode->currentText() == "偏移量补偿")
	{
		ui.gbx_CenterCalib->setEnabled(true);
	}
	else if (ui.cmb_locateMode->currentText() == "绝对坐标引导")
	{
		ui.gbx_CenterCalib->setEnabled(false);
	}
	else
	{
		QMessageBox::information(this, "提示", "引导模式错误!", QMessageBox::Ok);
	}
}


//切换查看图片
void RobotCalibWindow::on_cmb_ImgSelect_activated(const QString &arg1)
{
	try
	{
		int index1 = ui.cmb_ImgSelect->currentIndex();
		if (index1 == 0)
		{
			if (!Q_InputImg.isNull())
			{
				view->DispImage(Q_InputImg);
			}
		}
		else if (index1 == 1)
		{
			if (!Q_OutputImg.isNull())
			{
				view->DispImage(Q_OutputImg);
			}
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "图片切换失败!", QMessageBox::Ok);
		return;
	}
}
//切换搜索区域
void RobotCalibWindow::on_cmb_SearchRoi_activated(const QString &arg1)
{
	try
	{
		HTuple width = 0, height = 0;
		if (ho_Image.Key() != nullptr)
		{
			GetImageSize(ho_Image, &width, &height);
		}
		int index1 = ui.cmb_SearchRoi->currentIndex();
		if (index1 == 1)
		{
			if (width == 0)
			{
				RectangleItem * Roi = new RectangleItem(10, 10, 500, 500);
				Roi->ItemDiscrib = "搜索区域";
				view->AddItems(Roi);
				vec_RoiSearch_Template.push_back(Roi);
			}
			else
			{
				RectangleItem * Roi = new RectangleItem(width / 2 - width / 4, height / 2 - height / 4, width / 2, height / 2);
				Roi->ItemDiscrib = "搜索区域";
				view->AddItems(Roi);
				vec_RoiSearch_Template.push_back(Roi);
			}
		}
		else
		{
			if (vec_RoiSearch_Template.size() > 0)
			{
				if (vec_RoiSearch_Template.size() == 1)
				{
					view->DeleteObj(vec_RoiSearch_Template[0]);
					vec_RoiSearch_Template.clear();
				}
				else
				{
					QMessageBox::information(this, "提示", "搜索区域数组数量不等于1!", QMessageBox::Ok);
					return;
				}
			}
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "搜索区域切换失败!", QMessageBox::Ok);
		return;
	}
}
//绘制训练区域
void RobotCalibWindow::on_pb_DrawTrainRoi_clicked()
{
	try
	{
		HTuple width = 0, height = 0;
		if (ho_Image.Key() != nullptr)
		{
			GetImageSize(ho_Image, &width, &height);
		}
		if (width == 0)
		{
			RectangleItem * Roi = new RectangleItem(10, 10, 500, 500);
			Roi->ItemDiscrib = "训练区域";
			view->AddItems(Roi);
			vec_RoiTrain_Template.push_back(Roi);
		}
		else
		{
			RectangleItem * Roi = new RectangleItem(width / 2 - width / 4, height / 2 - height / 4, width / 2, height / 2);
			Roi->ItemDiscrib = "训练区域";
			view->AddItems(Roi);
			vec_RoiTrain_Template.push_back(Roi);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "训练区域添加失败!", QMessageBox::Ok);
		return;
	}
}
//绘制掩膜区域
void RobotCalibWindow::on_pb_DrawShaddowRoi_clicked()
{
	try
	{
		HTuple width = 0, height = 0;
		if (ho_Image.Key() != nullptr)
		{
			GetImageSize(ho_Image, &width, &height);
		}
		if (width == 0)
		{
			RectangleItem * Roi = new RectangleItem(10, 10, 500, 500);
			Roi->ItemDiscrib = "掩模区域";
			view->AddItems(Roi);
			vec_RoiShaddow_Template.push_back(Roi);
		}
		else
		{
			RectangleItem * Roi = new RectangleItem(width / 2 - width / 4, height / 2 - height / 4, width / 2, height / 2);
			Roi->ItemDiscrib = "掩模区域";
			view->AddItems(Roi);
			vec_RoiShaddow_Template.push_back(Roi);
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "掩模区域添加失败!", QMessageBox::Ok);
		return;
	}
}
//清除训练区域
void RobotCalibWindow::on_pb_ClearTrainRoi_clicked()
{
	if (vec_RoiTrain_Template.size() > 0)
	{
		for (int i = 0; i < vec_RoiTrain_Template.size(); i++)
		{
			view->DeleteObj(vec_RoiTrain_Template[i]);
		}
		vec_RoiTrain_Template.clear();
	}
	if (vec_RoiShaddow_Template.size() > 0)
	{
		for (int i = 0; i < vec_RoiShaddow_Template.size(); i++)
		{
			view->DeleteObj(vec_RoiShaddow_Template[i]);
		}
		vec_RoiShaddow_Template.clear();
	}
}
//训练模板
void RobotCalibWindow::on_pb_Train_Template_clicked()
{
	try
	{
		view->ClearObj();
		if (vec_RoiTrain_Template.size() <= 0)
		{
			QMessageBox::information(this, "提示", "训练区域为空!", QMessageBox::Ok);
			return;
		}
		//取出训练区域
		HObject ho_RegionTrain;
		GenEmptyObj(&ho_RegionTrain);
		MRectangle rec1;
		vec_RoiTrain_Template[0]->GetRect(rec1);
		GenRectangle1(&ho_RegionTrain, rec1.row, rec1.col, rec1.row + rec1.height, rec1.col + rec1.width);
		for (int i = 1; i < vec_RoiTrain_Template.size(); i++)
		{
			HObject roiTemp;
			vec_RoiTrain_Template[i]->GetRect(rec1);
			GenRectangle1(&roiTemp, rec1.row, rec1.col, rec1.row + rec1.height, rec1.col + rec1.width);
			Union2(ho_RegionTrain, roiTemp, &ho_RegionTrain);
		}
		for (int i = 0; i < vec_RoiTrain_Template.size(); i++)
		{
			view->DeleteObj(vec_RoiTrain_Template[i]);
		}
		vec_RoiTrain_Template.clear();
		//取出掩模区域
		HObject ho_RegionShaddow;
		GenEmptyObj(&ho_RegionShaddow);
		if (vec_RoiShaddow_Template.size() > 0)
		{
			vec_RoiShaddow_Template[0]->GetRect(rec1);
			GenRectangle1(&ho_RegionShaddow, rec1.row, rec1.col, rec1.row + rec1.height, rec1.col + rec1.width);
			for (int i = 1; i < vec_RoiShaddow_Template.size(); i++)
			{
				HObject roiTemp;
				vec_RoiShaddow_Template[i]->GetRect(rec1);
				GenRectangle1(&roiTemp, rec1.row, rec1.col, rec1.row + rec1.height, rec1.col + rec1.width);
				Union2(ho_RegionShaddow, roiTemp, &ho_RegionShaddow);
			}
			//抠掉掩模区域
			Difference(ho_RegionTrain, ho_RegionShaddow, &ho_RegionTrain);
		}
		if (vec_RoiShaddow_Template.size() > 0)
		{
			for (int i = 0; i < vec_RoiShaddow_Template.size(); i++)
			{
				view->DeleteObj(vec_RoiShaddow_Template[i]);
			}
			vec_RoiShaddow_Template.clear();
		}
		//训练区域导出
		RunParams_RobotCalib.VecRows_TrainRoi.clear();
		RunParams_RobotCalib.VecColumns_TrainRoi.clear();
		HTuple rows, columns;
		GetRegionPoints(ho_RegionTrain, &rows, &columns);
		int ll = rows.TupleLength().I();
		if (ll > 0)
		{
			vector<long long> vecRows(ll);
			vector<long long> vecCols(ll);
			std::memcpy(vecRows.data(), rows.ToLArr(), ll * sizeof(long long));
			std::memcpy(vecCols.data(), columns.ToLArr(), ll * sizeof(long long));
			RunParams_RobotCalib.VecRows_TrainRoi = vecRows;
			RunParams_RobotCalib.VecColumns_TrainRoi = vecCols;
		}
		HObject ho_ReduceImg, ho_CutImg;
		TrainRoi_Template = ho_RegionTrain;
		ReduceDomain(ho_Image, ho_RegionTrain, &ho_ReduceImg);
		CropDomain(ho_ReduceImg, &ho_CutImg);
		ho_CropModelImg = ho_CutImg;
		//设置查找模板的运行参数
		int ret = SetRunParams_Template(RunParams_RobotCalib);
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "模板参数设置失败", QMessageBox::Ok);
			return;
		}
		ret = slot_CreateTemplate(RunParams_RobotCalib);
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "模板训练失败", QMessageBox::Ok);
			return;
		}
		//查找模板
		ret = slot_FindTemplate();
		if (ret != 0)
		{
			QMessageBox::information(this, "提示", "模板查找失败", QMessageBox::Ok);
			return;
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "训练失败", QMessageBox::Ok);
		return;
	}
}
//查找模板
void RobotCalibWindow::on_pb_FindTemplate_clicked()
{
	//设置查找模板的运行参数
	int ret = SetRunParams_Template(RunParams_RobotCalib);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "模板参数设置失败", QMessageBox::Ok);
		return;
	}
	//查找模板
	ret = slot_FindTemplate();
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "模板查找失败", QMessageBox::Ok);
		return;
	}
}
//查找模板槽函数
int RobotCalibWindow::slot_FindTemplate()
{
	try
	{
		//调用算法
		QDateTime startTime = QDateTime::currentDateTime();
		int ret = RobotCalibDetect1->FindTemplate(MatImage, RunParams_RobotCalib, RobotCalibDetect1->mResultParams);
		QDateTime endTime = QDateTime::currentDateTime();
		qint64 intervalTimeMS = startTime.msecsTo(endTime);
		//刷新找模板结果
		getmessage("CT:" + QString::number(intervalTimeMS) + "ms");
		//显示结果图
		Q_OutputImg = Common1->Mat2QImage(RobotCalibDetect1->mResultParams.DestImg);
		view->DispImage(Q_OutputImg);
		if (RobotCalibDetect1->mResultParams.hv_RetNum > 0)
		{
			getmessage("OK");
		}
		else
		{
			getmessage("NG");
		}
		//刷新结果表格
		ui.tablewidget_Result_Template->clearContents();
		ui.tablewidget_Result_Template->setRowCount(0);
		int count = RobotCalibDetect1->mResultParams.hv_RetNum;
		if (count > 0)
		{
			for (int i = 0; i < count; i++)
			{
				int count = ui.tablewidget_Result_Template->rowCount();
				ui.tablewidget_Result_Template->setRowCount(count + 1);

				QTableWidgetItem *item;
				QString str;

				//X
				str = QString::number(RobotCalibDetect1->mResultParams.hv_Column[i], 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Result_Template->setItem(count, 0, item);
				//Y
				str = QString::number(RobotCalibDetect1->mResultParams.hv_Row[i], 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Result_Template->setItem(count, 1, item);
				//角度
				str = QString::number(RobotCalibDetect1->mResultParams.hv_Angle[i], 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Result_Template->setItem(count, 2, item);
				//分数
				str = QString::number(RobotCalibDetect1->mResultParams.hv_Score[i], 'f', 2);
				item = new QTableWidgetItem(str, 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Result_Template->setItem(count, 3, item);
			}
		}

		return ret;
	}
	catch (...)
	{
		return -1;
	}
}
//训练模板槽函数
int RobotCalibWindow::slot_CreateTemplate(RunParamsStruct_RobotCalib &runParams)
{
	try
	{
		int ret = 1;
		//创建模板
		ret = RobotCalibDetect1->CreateTemplate(MatImage, runParams);
		return ret;
	}
	catch (...)
	{
		return -1;
	}
}
//是否启用高级参数
void RobotCalibWindow::on_ckb_AdvancedData_stateChanged(int arg1)
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

//计算机器人九点坐标
void RobotCalibWindow::on_pb_CalculateRobot_clicked()
{
	try
	{
		//九点标定参数赋值
		int count = ui.tablewidget_CoordinateData->rowCount();
		if (count < 9)
		{
			QMessageBox::information(this, "提示", "数据点小于9!", QMessageBox::Ok);
			return;
		}
		double x_RoboxOri = ui.dspb_RobotOriX->value();
		double y_RobotOri = ui.dspb_RobotOriY->value();
		double dis_Robot = ui.dspb_distanceCalib->value();
		for (int i = 0; i < count; i++)
		{
			double x = x_RoboxOri;
			double y = y_RobotOri;

			if (i == 1)
			{
				x += dis_Robot;
				y += 0;
			}
			else if (i == 2)
			{
				x += dis_Robot;
				y += dis_Robot;
			}
			else if (i == 3)
			{
				x += 0;
				y += dis_Robot;
			}
			else if (i == 4)
			{
				x -= dis_Robot;
				y += dis_Robot;
			}
			else if (i == 5)
			{
				x -= dis_Robot;
				y += 0;
			}
			else if (i == 6)
			{
				x -= dis_Robot;
				y -= dis_Robot;
			}
			else if (i == 7)
			{
				x += 0;
				y -= dis_Robot;
			}
			else if (i == 8)
			{
				x += dis_Robot;
				y -= dis_Robot;
			}

			QString str = QString::number(x);
			QTableWidgetItem *item = new QTableWidgetItem(str, 1000);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData->setItem(i, 3, item);

			str = QString::number(y);
			QTableWidgetItem* item2 = new QTableWidgetItem(str, 1000);
			item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tablewidget_CoordinateData->setItem(i, 4, item2);
		}
		
	}
	catch (...)
	{
		QMessageBox::information(this, "提示", "计算失败!", QMessageBox::Ok);
		return;
	}
}
//按照指定字符分割字符串到vector
void RobotCalibWindow::stringToken(const string sToBeToken, const string sSeperator, vector<string>& vToken)
{
	string sCopy = sToBeToken;
	int iPosEnd = 0;
	while (true)
	{
		iPosEnd = sCopy.find(sSeperator);
		if (iPosEnd == -1)
		{
			vToken.push_back(sCopy);
			break;
		}
		vToken.push_back(sCopy.substr(0, iPosEnd));
		sCopy = sCopy.substr(iPosEnd + 1);
	}
}

//线程延迟(不阻塞主线程，QThread::Sleep()会阻塞主线程)
void RobotCalibWindow::Delay_MSec(unsigned int msec) //毫秒
{
	QTime _Timer = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < _Timer)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
//exe路径
string RobotCalibWindow::FolderPath()
{
	char strBuf[256] = { 0 };
#ifdef WIN32
    GetModuleFileNameA(NULL, strBuf, sizeof(strBuf));
#else
    ssize_t len =readlink("/proc/self/exe", strBuf, sizeof(strBuf) - 1);
#endif
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
string RobotCalibWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
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

