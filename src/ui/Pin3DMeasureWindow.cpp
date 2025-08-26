#include "Pin3DMeasureWindow.h"

Pin3DMeasureWindow::Pin3DMeasureWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.frame->hide();
	//this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
	IniPin3DMeasureClassObject();
	this->setWindowFlags(this->windowFlags() | Qt::CustomizeWindowHint);
	this->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	this->setWindowFlag(Qt::WindowCloseButtonHint, false);
	//this->setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	modubleDatas1 = new modubleDatas();
	connect(ui.cbB_LocateMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(LocateChoose()));
	LocateChoose();

	viewer.reset(new pcl::visualization::PCLVisualizer(ren, renderWindow, "Cloud 3D Viewer", false));
}

Pin3DMeasureWindow::Pin3DMeasureWindow(pcl::PointCloud<pcl::PointXYZ>::Ptr image3D, int _processID, int modubleID, std::string _modubleName
	, std::function<void(int processID, int modubleID, std::string modubleName)> _refreshConfig, std::string getConfigPath) {
	//ui.setupUi(this);

	ori_cloud = image3D;

	//this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processID = _processID;

	Pin3DMeasureParams1 = new Pin3DMeasure(nodeName, processModbuleID, processID);
	modubleDatas1 = new modubleDatas();
	modubleDatas1->setGetConfigPath(ConfigPath);
	//this->setWindowFlags(this->windowFlags() | Qt::CustomizeWindowHint);
	//this->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	//this->setWindowFlag(Qt::WindowCloseButtonHint, false);
	//this->setWindowFlags(Qt::FramelessWindowHint);//隐藏边框 
	//ui.frame->hide();

	refreshConfig = _refreshConfig;

	//connect(ui.cbB_LocateMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(LocateChoose()));
	//LocateChoose();
}

Pin3DMeasureWindow::~Pin3DMeasureWindow()
{
	delete Pin3DMeasureParams1;
	if (modubleDatas1)
		delete modubleDatas1;
}

void Pin3DMeasureWindow::IniPin3DMeasureClassObject()
{
	Pin3DMeasureParams1 = new Pin3DMeasure();
}

void Pin3DMeasureWindow::on_toolButton_clicked()
{
	Pin3DMeasureWindow::showMinimized(); //最小化
}

void Pin3DMeasureWindow::on_toolButton_2_clicked() {
	//if (Pin3DMeasureWindow::isMaximized()) {
	//	Pin3DMeasureWindow::showNormal();//还原事件
	//	//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	//}
	//else {
	//	Pin3DMeasureWindow::showMaximized();//最大化事件
	//	//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	//}
}

void Pin3DMeasureWindow::on_toolButton_3_clicked() {
	this->close();
}


//点云显示
bool Pin3DMeasureWindow::has_suffix(const std::string& filename, const std::string& suffix)
{
	std::size_t index = filename.find(suffix, filename.size() - suffix.size());
	return (index != std::string::npos);
}

void Pin3DMeasureWindow::LoadShowCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	ui.openGLWidget->clearMask();
	viewer->removeAllPointClouds();
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud(cloud, "cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud");
	viewer->addCoordinateSystem(5.0);
	viewer->initCameraParameters();
	std::string str = "Number of point clouds: " + std::to_string(cloud->points.size());
	viewer->setShowFPS(false);
	viewer->addText(str, 0, 0, 20, 1.0, 1.0, 1.0, "sre");
	ui.openGLWidget->SetRenderWindow(viewer->getRenderWindow());
}

void Pin3DMeasureWindow::ShowMultiCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2)
{
	ui.openGLWidget->update();
	vtkRenderer* ren = vtkRenderer::New();
	vtkGenericOpenGLRenderWindow* renderWindow = vtkGenericOpenGLRenderWindow::New();
	pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer(ren, renderWindow, "MultiCloudViewer", false));
	ui.openGLWidget->SetRenderWindow(viewer->getRenderWindow());
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ>(cloud1, "sample cloud1");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud1");
	viewer->addCoordinateSystem(5.0);
	viewer->setShowFPS(false);
	viewer->addPointCloud<pcl::PointXYZ>(cloud2, "sample cloud2");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1, 0, 0, "sample cloud2");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud2");
}

void Pin3DMeasureWindow::on_pBtn_OpenCloud_clicked()
{
	QFileDialog dlg;
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setFileMode(QFileDialog::ExistingFile);


	// 构建默认路径：应用目录下的 3D_Photo
	QString defaultPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/3D_Photo");

	// 检查路径是否存在
	QDir dir(defaultPath);
	if (!dir.exists()) {
		// 回退到用户文档目录（或当前目录）
		defaultPath = QCoreApplication::applicationDirPath();
	}

	// 设置对话框默认路径
	dlg.setDirectory(defaultPath);

	QString filePath = dlg.getOpenFileName();
	QByteArray cdata = filePath.toLocal8Bit();
	std::string str1 = cdata.toStdString();
	const char *str = str1.c_str();
	if (!filePath.isEmpty())
	{
		if (has_suffix(str, "ply"))
		{
			//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
			//if (pcl::io::loadPLYFile<pcl::PointXYZ>(str, *cloud) == -1)
			//{
			//	QMessageBox::information(this, "提示", "点云格式不正确", QMessageBox::Ok);
			//}
			//pcl::io::loadPLYFile<pcl::PointXYZ>(str, *cloud);
			//ori_cloud.reset();
			//ori_cloud = cloud;
			//LoadShowCloud(cloud);

			pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
			if (pcl::io::loadPLYFile<pcl::PointXYZ>(str, *cloud) == -1)
			{
				QMessageBox::information(this, "提示", "点云格式不正确", QMessageBox::Ok);
				return;
			}
			ori_cloud.reset();
			ori_cloud = std::move(cloud);
			cloud.reset();
			LoadShowCloud(ori_cloud);
		}
		else if (has_suffix(str, "pcd"))
		{
			//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
			//if (pcl::io::loadPCDFile<pcl::PointXYZ>(str, *cloud) == -1)
			//{
			//	QMessageBox::information(this, "提示", "点云格式不正确", QMessageBox::Ok);
			//}
			//pcl::io::loadPCDFile(str, *cloud);
			//ori_cloud.reset();
			//ori_cloud = cloud;
			//LoadShowCloud(cloud);

			pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
			if (pcl::io::loadPCDFile<pcl::PointXYZ>(str, *cloud) == -1)
			{
				QMessageBox::information(this, "提示", "点云格式不正确", QMessageBox::Ok);
				return;
			}
			ori_cloud.reset();
			current_cloud.reset();
			ori_cloud = cloud;
			current_cloud = ori_cloud;
			LoadShowCloud(cloud);
		}
		else
		{
			QMessageBox::information(this, "提示", "仅支持PCD和PLY格式点云", QMessageBox::Ok);
		}
	}
}

void Pin3DMeasureWindow::on_pBtn_SaveCloud_clicked()
{
	if (current_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "当前窗口点云为空或未运行程序，请先加载点云并运行", QMessageBox::Ok);
	}
	else
	{
		//获取点云保存路径
		QString file_path = QFileDialog::getSaveFileName(this, "save file", "C:\\", "ply files(*.ply)");
		if (file_path.size() <= 0)
		{
			return;
		}
		//根据类型存点云
		QStringList list = file_path.split(".");
		if (list.size() > 0)
		{
			std::string SaveType = list[list.size() - 1].toStdString();
			QByteArray cdata = file_path.toLocal8Bit();
			std::string FileName = cdata.toStdString();
			pcl::io::savePLYFileBinary(FileName, *current_cloud);
		}
	}
}

int Pin3DMeasureWindow::on_pBtn_PickModel_clicked()
{
	QFileDialog dlg;
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setFileMode(QFileDialog::ExistingFile);
	QString filePath = dlg.getOpenFileName();
	QByteArray cdata = filePath.toLocal8Bit();
	std::string str1 = cdata.toStdString();
	const char *str = str1.c_str();
	if (!filePath.isEmpty())
	{
      ui.lineEdit_ModelName->setText(str);
	}
	return 0;
}

void Pin3DMeasureWindow::LocateChoose()
{
	if (ui.cbB_LocateMethod->currentText() == "机械定位")
	{
		ui.pBtn_PickModel->setEnabled(false);
		ui.lineEdit_ModelName->setEnabled(false);
		ui.dSpB_DownSamplingVoxel->setEnabled(false);
		ui.spB_MatchIterations->setEnabled(false);
		ui.pBtn_RunMatchCloud->setEnabled(false);
		ui.ckB_PopWindowShowMatchCloud->setEnabled(false);
	}
	else {
		ui.pBtn_PickModel->setEnabled(true);
		ui.lineEdit_ModelName->setEnabled(true);
		ui.dSpB_DownSamplingVoxel->setEnabled(true);
		ui.spB_MatchIterations->setEnabled(true);
		ui.pBtn_RunMatchCloud->setEnabled(true);
		ui.ckB_PopWindowShowMatchCloud->setEnabled(true);
	}
}

void Pin3DMeasureWindow::on_pBtn_ClearData_clicked()
{
	ui.plainTextEdit->clear();
	return;
}

int Pin3DMeasureWindow::on_pBtn_AddPinROI_clicked()
{
	bool createSucess;
	QString text = QInputDialog::getText(this, tr("新增PIN参数"), tr("输入参数名称："), QLineEdit::Normal, "", &createSucess);

	if (createSucess && !text.isEmpty())
	{
		for (int a = 0; a < this->ui.tbeW_PInParams->columnCount(); a++)
		{
			QString columnName = ui.tbeW_PInParams->horizontalHeaderItem(a)->text();
			if (text == columnName)
			{
				QMessageBox::information(this, "提示", "当前名称已存在", QMessageBox::Ok);
				return 0;
			}
		}
		int ColumCount = ui.tbeW_PInParams->columnCount();
		ui.tbeW_PInParams->insertColumn(ColumCount);
		QTableWidgetItem* columnHeaderItem = new QTableWidgetItem(text);
		ui.tbeW_PInParams->setHorizontalHeaderItem(ColumCount, columnHeaderItem);
		CreateDoubleSpinBoxMM(0, ColumCount, 0.0);
		CreateDoubleSpinBoxMM(1, ColumCount, 0.0);
		CreateDoubleSpinBoxMM(2, ColumCount, 0.0);
		CreateDoubleSpinBoxMM(3, ColumCount, 10.0);
		CreateDoubleSpinBoxMM(4, ColumCount, 10.0);
		CreateDoubleSpinBoxMM(5, ColumCount, 10.0);
		CreateDoubleSpinBoxDegrees(6, ColumCount, 0.0);
		CreateDoubleSpinBoxDegrees(7, ColumCount, 0.0);
		CreateDoubleSpinBoxDegrees(8, ColumCount, 0.0);
		CreateDoubleSpinBox(9, ColumCount, 0.1);
		CreateSpinBox(10, ColumCount, 5);
		CreateDoubleSpinBox(11, ColumCount, 0.1);
		CreateDoubleSpinBox(12, ColumCount, 0.3);
		CreateSpinBox(13, ColumCount, 10);
		CreateSpinBox(14, ColumCount, 200);
		CreateDoubleSpinBoxMM(15, ColumCount, 0.0);
		CreateDoubleSpinBoxMM(16, ColumCount, 0.0);
		CreateDoubleSpinBoxMM(17, ColumCount, 0.0);
		CreateDoubleSpinBoxMM(18, ColumCount, 0.0);
		CreateSpinBox(19, ColumCount, 1);
		CreateComboBox(20, ColumCount, "沿X升序");
		CreateSpinBox(21, ColumCount, 0);
		CreateSpinBox(22, ColumCount, 0);

	}
	return 0;
}

int Pin3DMeasureWindow::on_pBtn_DeletePinROI_clicked()
{
	int currentColumn = ui.tbeW_PInParams->currentColumn();
	if (currentColumn == -1)
	{
		return 0;
	}
	QString currentColumnName = ui.tbeW_PInParams->horizontalHeaderItem(currentColumn)->text();
	QMessageBox::StandardButton result;
	result = QMessageBox::question(this, "删除PIN参数", "是否删除当前名为："+ currentColumnName, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::NoButton);
	if (result == QMessageBox::Yes)
	{
		this->ui.tbeW_PInParams->removeColumn(currentColumn);
	}
	return 0;
}

int Pin3DMeasureWindow::CreateComboBox(int row, int column, QString value)
{
	QComboBox *combobox = new QComboBox;
	combobox->addItems({ "沿X升序","沿X降序","沿Y升序","沿Y降序" });
	combobox->setFixedSize(100, 30);
	combobox->setCurrentText(value);
	ui.tbeW_PInParams->setCellWidget(row, column, combobox);
	return 0;
}

int Pin3DMeasureWindow::CreateSpinBox(int row, int column, int value)
{
	QSpinBox *spinBox = new QSpinBox;
	spinBox->setRange(0, 99999999);
	spinBox->setFixedSize(100, 30);
	spinBox->setSingleStep(1);
	spinBox->setValue(value);
	ui.tbeW_PInParams->setCellWidget(row, column, spinBox);
	return 0;
}

int Pin3DMeasureWindow::CreateDoubleSpinBoxMM(int row, int column, double value)
{
	QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox;
	doubleSpinBox->setRange(-99999999.999, 99999999.999);
	doubleSpinBox->setDecimals(3);
	doubleSpinBox->setSingleStep(0.1);
	doubleSpinBox->setSuffix("mm");
	doubleSpinBox->setFixedSize(100,30);
	doubleSpinBox->setValue(value);
	ui.tbeW_PInParams->setCellWidget(row, column, doubleSpinBox);
	return 0;
}

int Pin3DMeasureWindow::CreateDoubleSpinBoxDegrees(int row, int column, double value)
{
	QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox;
	doubleSpinBox->setRange(-180, 180);
	doubleSpinBox->setDecimals(3);
	doubleSpinBox->setSingleStep(0.1);
	doubleSpinBox->setSuffix("°");
	doubleSpinBox->setFixedSize(100, 30);
	doubleSpinBox->setValue(value);
	ui.tbeW_PInParams->setCellWidget(row, column, doubleSpinBox);
	return 0;
}

int Pin3DMeasureWindow::CreateDoubleSpinBox(int row, int column, double value)
{
	QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox;
	doubleSpinBox->setRange(0, 99999999.999);
	doubleSpinBox->setDecimals(3);
	doubleSpinBox->setSingleStep(0.1);
	doubleSpinBox->setFixedSize(100, 30);
	doubleSpinBox->setValue(value);
	ui.tbeW_PInParams->setCellWidget(row, column, doubleSpinBox);
	return 0;
}


int Pin3DMeasureWindow::GetSpinBoxValue(QTableWidget *tableWidget, int row, int column)
{
	QSpinBox *spinBox = static_cast<QSpinBox*>(tableWidget->cellWidget(row, column));
	if (spinBox) 
	{
		return spinBox->value();
	}
	return 0; 
}


double Pin3DMeasureWindow::GetDoubleSpinBoxValue(QTableWidget *tableWidget, int row, int column)
{
	QDoubleSpinBox *doublespinBox = static_cast<QDoubleSpinBox*>(tableWidget->cellWidget(row, column));
	if (doublespinBox)
	{
		return doublespinBox->value();
	}
	return 0;
}

QString Pin3DMeasureWindow::GetComboBoxValue(QTableWidget *tableWidget, int row, int column)
{
	QComboBox *comboBox = static_cast<QComboBox*>(tableWidget->cellWidget(row, column));
	if (comboBox)
	{
		return comboBox->currentText();
	}
	return "0";
}


void Pin3DMeasureWindow::SetRunParams_Pin3DMeasure(RunParamsStruct_Pin3DMeasure &runparams)
{
	runparams.ROI_CenterX = this->ui.dSpB_ROI_CenterX->value();
	runparams.ROI_CenterY = this->ui.dSpB_ROI_CenterY->value();
	runparams.ROI_CenterZ = this->ui.dSpB_ROI_CenterZ->value();
	runparams.ROI_LengthX = this->ui.dSpB_ROI_LengthX->value();
	runparams.ROI_LengthY = this->ui.dSpB_ROI_LengthY->value();
	runparams.ROI_LengthZ = this->ui.dSpB_ROI_LengthZ->value();
	runparams.ROI_RX = this->ui.dSpB_ROI_RX->value();
	runparams.ROI_RY = this->ui.dSpB_ROI_RY->value();
	runparams.ROI_RZ = this->ui.dSpB_ROI_RZ->value();

	runparams.LocateMethod = this->ui.cbB_LocateMethod->currentText().toStdString();

	runparams.ModelName = this->ui.lineEdit_ModelName->text().toStdString();
	runparams.DownSamplingVoxel = this->ui.dSpB_DownSamplingVoxel->value();
	runparams.MatchIterations = this->ui.spB_MatchIterations->value();
	runparams.PopWindowShowMatchCloud = this->ui.ckB_PopWindowShowMatchCloud->isChecked();
	
	runparams.BasePlanePoints[0][0] = this->ui.dSB_BasePoint1CenterX->value();
	runparams.BasePlanePoints[0][1] = this->ui.dSB_BasePoint1CenterY->value();
	runparams.BasePlanePoints[0][2] = this->ui.dSB_BasePoint1CenterZ->value();
	runparams.BasePlanePoints[1][0] = this->ui.dSB_BasePoint2CenterX->value();
	runparams.BasePlanePoints[1][1] = this->ui.dSB_BasePoint2CenterY->value();
	runparams.BasePlanePoints[1][2] = this->ui.dSB_BasePoint2CenterZ->value();
	runparams.BasePlanePoints[2][0] = this->ui.dSB_BasePoint3CenterX->value();
	runparams.BasePlanePoints[2][1] = this->ui.dSB_BasePoint3CenterY->value();
	runparams.BasePlanePoints[2][2] = this->ui.dSB_BasePoint3CenterZ->value();
	runparams.BasePlanePoints[3][0] = this->ui.dSB_BasePoint4CenterX->value();
	runparams.BasePlanePoints[3][1] = this->ui.dSB_BasePoint4CenterY->value();
	runparams.BasePlanePoints[3][2] = this->ui.dSB_BasePoint4CenterZ->value();
	runparams.PopWindowShowBasePointCloud = this->ui.ckB_PopWindowShowBasePointCloud->isChecked();

	runparams.PopWindowShowCropPinCloud = this->ui.ckB_PopWindowShowCropPinCloud->isChecked();
	runparams.PopWindowShowPinSORCloud = this->ui.ckB_PopWindowShowPinSORCloud->isChecked();
	runparams.PopWindowShowPinDownCloud = this->ui.ckB_PopWindowShowPinDownCloud->isChecked();
	runparams.PopWindowShowPinClusterCloud = this->ui.ckB_PopWindowShowPinClusterCloud->isChecked();
	runparams.ShowResult = this->ui.ckB_ShowResult->isChecked();
	runparams.PinParamsNum = this->ui.tbeW_PInParams->columnCount();

	if (this->ui.tbeW_PInParams->columnCount() == 0)
	{
		return;
	}
	else
	{
		runparams.PinParamsName.clear();
		runparams.PinROI_CenterX_Vector.clear();
		runparams.PinROI_CenterY_Vector.clear();
		runparams.PinROI_CenterZ_Vector.clear();
		runparams.PinROI_LengthX_Vector.clear();
		runparams.PinROI_LengthY_Vector.clear();
		runparams.PinROI_LengthZ_Vector.clear();
		runparams.PinROI_RX_Vector.clear();
		runparams.PinROI_RY_Vector.clear();
		runparams.PinROI_RZ_Vector.clear();
		runparams.PinDownSamplingVoxel_Vector.clear();
		runparams.SORNeighborPoints_Vector.clear();
		runparams.SOROutlierThreshold_Vector.clear();
		runparams.EuclideanClusterTolerance_Vector.clear();
		runparams.EuclideanClusterMinPoints_Vector.clear();
		runparams.EuclideanClusterMaxPoints_Vector.clear();
		runparams.PinStandard_Vector.clear();
		runparams.PinUpperTol_Vector.clear();
		runparams.PinLowerTol_Vector.clear();
		runparams.PinCorrect_Vector.clear();
		runparams.PinNum_Vector.clear();
		runparams.SortOrder_Vector.clear();
		runparams.HeadAddZeroNum_Vector.clear();
		runparams.TailAddZeroNum_Vector.clear();
		for (int a = 0; a < this->ui.tbeW_PInParams->columnCount(); a++)
		{
			QTableWidgetItem *item = this->ui.tbeW_PInParams->horizontalHeaderItem(a);
			runparams.PinParamsName.push_back(item->text().toStdString());
			runparams.PinROI_CenterX_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 0, a));
			runparams.PinROI_CenterY_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 1, a));
			runparams.PinROI_CenterZ_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 2, a));
			runparams.PinROI_LengthX_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 3, a));
			runparams.PinROI_LengthY_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 4, a));
			runparams.PinROI_LengthZ_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 5, a));
			runparams.PinROI_RX_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 6, a));
			runparams.PinROI_RY_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 7, a));
			runparams.PinROI_RZ_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 8, a));
			runparams.PinDownSamplingVoxel_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 9, a));
			runparams.SORNeighborPoints_Vector.push_back(GetSpinBoxValue(this->ui.tbeW_PInParams, 10, a));
			runparams.SOROutlierThreshold_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 11, a));
			runparams.EuclideanClusterTolerance_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 12, a));
			runparams.EuclideanClusterMinPoints_Vector.push_back(GetSpinBoxValue(this->ui.tbeW_PInParams, 13, a));
			runparams.EuclideanClusterMaxPoints_Vector.push_back(GetSpinBoxValue(this->ui.tbeW_PInParams, 14, a));
			runparams.PinStandard_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 15, a));
			runparams.PinUpperTol_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 16, a));
			runparams.PinLowerTol_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 17, a));
			runparams.PinCorrect_Vector.push_back(GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 18, a));
			runparams.PinNum_Vector.push_back(GetSpinBoxValue(this->ui.tbeW_PInParams, 19, a));
			runparams.SortOrder_Vector.push_back(GetComboBoxValue(this->ui.tbeW_PInParams, 20, a).toStdString());
			runparams.HeadAddZeroNum_Vector.push_back(GetSpinBoxValue(this->ui.tbeW_PInParams, 21, a));
			runparams.TailAddZeroNum_Vector.push_back(GetSpinBoxValue(this->ui.tbeW_PInParams, 22, a));
		}
	}
}

void Pin3DMeasureWindow::UpdatePin3DMeasureWindow(const RunParamsStruct_Pin3DMeasure &runparams)
{
	ui.dSpB_ROI_CenterX->setValue(runparams.ROI_CenterX);
	ui.dSpB_ROI_CenterY->setValue(runparams.ROI_CenterY);
	ui.dSpB_ROI_CenterZ->setValue(runparams.ROI_CenterZ);
	ui.dSpB_ROI_LengthX->setValue(runparams.ROI_LengthX);
	ui.dSpB_ROI_LengthY->setValue(runparams.ROI_LengthY);
	ui.dSpB_ROI_LengthZ->setValue(runparams.ROI_LengthZ);
	ui.dSpB_ROI_RX->setValue(runparams.ROI_RX);
	ui.dSpB_ROI_RY->setValue(runparams.ROI_RY);
	ui.dSpB_ROI_RZ->setValue(runparams.ROI_RZ);

	ui.cbB_LocateMethod->setCurrentText(runparams.LocateMethod.c_str());

	ui.lineEdit_ModelName->setText(QString::fromStdString(runparams.ModelName));
	ui.dSpB_DownSamplingVoxel->setValue(runparams.DownSamplingVoxel);
	ui.spB_MatchIterations->setValue(runparams.MatchIterations);
	ui.ckB_PopWindowShowMatchCloud->setChecked(runparams.PopWindowShowMatchCloud);

	ui.dSB_BasePoint1CenterX->setValue(runparams.BasePlanePoints[0][0]);
	ui.dSB_BasePoint1CenterY->setValue(runparams.BasePlanePoints[0][1]);
	ui.dSB_BasePoint1CenterZ->setValue(runparams.BasePlanePoints[0][2]);
	ui.dSB_BasePoint2CenterX->setValue(runparams.BasePlanePoints[1][0]);
	ui.dSB_BasePoint2CenterY->setValue(runparams.BasePlanePoints[1][1]);
	ui.dSB_BasePoint2CenterZ->setValue(runparams.BasePlanePoints[1][2]);
	ui.dSB_BasePoint3CenterX->setValue(runparams.BasePlanePoints[2][0]);
	ui.dSB_BasePoint3CenterY->setValue(runparams.BasePlanePoints[2][1]);
	ui.dSB_BasePoint3CenterZ->setValue(runparams.BasePlanePoints[2][2]);
	ui.dSB_BasePoint4CenterX->setValue(runparams.BasePlanePoints[3][0]);
	ui.dSB_BasePoint4CenterY->setValue(runparams.BasePlanePoints[3][1]);
	ui.dSB_BasePoint4CenterZ->setValue(runparams.BasePlanePoints[3][2]);

	ui.ckB_PopWindowShowBasePointCloud->setChecked(runparams.PopWindowShowBasePointCloud);
	ui.ckB_PopWindowShowCropPinCloud->setChecked(runparams.PopWindowShowCropPinCloud);
	ui.ckB_PopWindowShowPinSORCloud->setChecked(runparams.PopWindowShowPinSORCloud);
	ui.ckB_PopWindowShowPinDownCloud->setChecked(runparams.PopWindowShowPinDownCloud);
	ui.ckB_PopWindowShowPinClusterCloud->setChecked(runparams.PopWindowShowPinClusterCloud);
	ui.ckB_ShowResult->setChecked(runparams.ShowResult);

	for (int i = 0; i < runparams.PinParamsNum; i++)
	{
		ui.tbeW_PInParams->insertColumn(i);
		QTableWidgetItem* columnHeaderItem = new QTableWidgetItem(QString::fromStdString(runparams.PinParamsName[i]));
		ui.tbeW_PInParams->setHorizontalHeaderItem(i, columnHeaderItem);

		CreateDoubleSpinBoxMM(0, i, runparams.PinROI_CenterX_Vector[i]);
		CreateDoubleSpinBoxMM(1, i, runparams.PinROI_CenterY_Vector[i]);
		CreateDoubleSpinBoxMM(2, i, runparams.PinROI_CenterZ_Vector[i]);
		CreateDoubleSpinBoxMM(3, i, runparams.PinROI_LengthX_Vector[i]);
		CreateDoubleSpinBoxMM(4, i, runparams.PinROI_LengthY_Vector[i]);
		CreateDoubleSpinBoxMM(5, i, runparams.PinROI_LengthZ_Vector[i]);
		CreateDoubleSpinBoxDegrees(6, i, runparams.PinROI_RX_Vector[i]);
		CreateDoubleSpinBoxDegrees(7, i, runparams.PinROI_RX_Vector[i]);
		CreateDoubleSpinBoxDegrees(8, i, runparams.PinROI_RX_Vector[i]);
		CreateDoubleSpinBox(9, i, runparams.PinDownSamplingVoxel_Vector[i]);
		CreateSpinBox(10, i, runparams.SORNeighborPoints_Vector[i]);
		CreateDoubleSpinBox(11, i, runparams.SOROutlierThreshold_Vector[i]);
		CreateDoubleSpinBox(12, i, runparams.EuclideanClusterTolerance_Vector[i]);
		CreateSpinBox(13, i, runparams.EuclideanClusterMinPoints_Vector[i]);
		CreateSpinBox(14, i, runparams.EuclideanClusterMaxPoints_Vector[i]);
		CreateDoubleSpinBoxMM(15, i, runparams.PinStandard_Vector[i]);
		CreateDoubleSpinBoxMM(16, i, runparams.PinUpperTol_Vector[i]);
		CreateDoubleSpinBoxMM(17, i, runparams.PinLowerTol_Vector[i]);
		CreateDoubleSpinBoxMM(18, i, runparams.PinCorrect_Vector[i]);
		CreateSpinBox(19, i, runparams.PinNum_Vector[i]);
		CreateComboBox(20, i, QString::fromStdString(runparams.SortOrder_Vector[i]));
		CreateSpinBox(21, i, runparams.HeadAddZeroNum_Vector[i]);
		CreateSpinBox(22, i, runparams.TailAddZeroNum_Vector[i]);
	}

}

void Pin3DMeasureWindow::on_btn_SavePinMeasureData_clicked()
{
	int ret = 1;
	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	//把保存参数改为自定义文件保存
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/3D_formulation";

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

	modubleDatas1->setGetConfigPath(ConfigPath);
	modubleDatas1->setConfigPath(XmlPath);
	nodeName = fileInfo.baseName().toUtf8().data();
	ret = modubleDatas1->WriteParams_Pin3DMeasure(RunParamsPin3DMeasure, processID, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "提示", "参数保存失败", QMessageBox::Ok);
		return;
	}
}

void Pin3DMeasureWindow::on_btn_LoadPinMeasureData_clicked()
{
	// 获取可执行文件的路径
	QString exePath = QCoreApplication::applicationDirPath();

	// 在可执行文件路径下创建文件夹路径
	QString folderPath = exePath + "/3D_formulation";

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
	modubleDatas1->setGetConfigPath(ConfigPath);
	modubleDatas1->setConfigPath(XmlPath);
	nodeName = fileInfo.baseName().toUtf8().data();

	ErrorCode_Xml errorCode;
	//清一下数据
	RunParamsPin3DMeasure.clear();
	errorCode = modubleDatas1->ReadParams_Pin3DMeasure(RunParamsPin3DMeasure, processID, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "提示", "参数读取失败", QMessageBox::Ok);
		return;
	}

	int columns = ui.tbeW_PInParams->columnCount();
	for (int i = columns; i > -1; i--)
	{
		ui.tbeW_PInParams->removeColumn(i);
	}
	//参数更新到窗口
	UpdatePin3DMeasureWindow(RunParamsPin3DMeasure);
}


int Pin3DMeasureWindow::on_pBtn_RunCropModule_clicked()
{
	if (ori_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "原始点云为空，请先加载点云", QMessageBox::Ok);
		return 0;
	}
	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_module(new pcl::PointCloud<pcl::PointXYZ>);
	QDateTime startTime = QDateTime::currentDateTime();
	Pin3DMeasureParams1->CropModuleCloud(ori_cloud, cloud_module, RunParamsPin3DMeasure);
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	ui.plainTextEdit->appendPlainText("单步执行模块裁剪运行时间：" + QString::number(intervalTimeMS) + "ms");
	match_cloud.reset();
	module_cloud = cloud_module;
	LoadShowCloud(module_cloud);
	current_cloud = module_cloud;
}

int Pin3DMeasureWindow::on_pBtn_RunMatchCloud_clicked()
{
	if (module_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "裁剪模块特征点云为空，请先加载并裁剪模块点云", QMessageBox::Ok);
		return 0;
	}
	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_trans(new pcl::PointCloud<pcl::PointXYZ>);
	QDateTime startTime = QDateTime::currentDateTime();
	Pin3DMeasureParams1->MatchTransCloud(module_cloud, RunParamsPin3DMeasure, cloud_trans);
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	ui.plainTextEdit->appendPlainText("单步执行匹配运行时间：" + QString::number(intervalTimeMS) + "ms");
	match_cloud = cloud_trans;
	LoadShowCloud(match_cloud);
	current_cloud = match_cloud;
}

int Pin3DMeasureWindow::on_pBtn_RunFourPointFitPlane_clicked()
{
	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	if (this->ui.cbB_LocateMethod->currentText() == "机械定位")
	{
		match_cloud = module_cloud;
	}
	if (match_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "裁剪模块特征点云为空，请先加载并裁剪模块点云", QMessageBox::Ok);
		return 0;
	}
	QDateTime startTime = QDateTime::currentDateTime();
	Pin3DMeasureParams1->FourPointFitPlane(match_cloud, RunParamsPin3DMeasure, BasePlaneCoefficient);
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	ui.plainTextEdit->appendPlainText("单步执行建基准运行时间：" + QString::number(intervalTimeMS) + "ms");
	return 0;
}


int Pin3DMeasureWindow::on_pBtn_RunPinCal_clicked()
{
	if (this->ui.cbB_LocateMethod->currentText() == "机械定位")
	{
		match_cloud = module_cloud;
	}
	if (match_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "裁剪模块特征点云为空，请先加载并裁剪模块点云", QMessageBox::Ok);
		return 0;
	}

	if (BasePlaneCoefficient == Eigen::Vector4f(0, 0, 0, 0))
	{
		QMessageBox::information(this, "提示", "未建立基准，请先建立基准", QMessageBox::Ok);
		return 0;
	}

	if (this->ui.tbeW_PInParams->columnCount() == 0)
	{
		QMessageBox::information(this, "提示", "未加载或创建PIN参数，无法进行计算，请先加载或创建PIN参数", QMessageBox::Ok);
		return 0;
	}

	int currentColumn = ui.tbeW_PInParams->currentColumn();
	if (currentColumn == -1)
	{
		QMessageBox::information(this, "提示", "未选择需要运行的PIN参数，请选择后运行", QMessageBox::Ok);
		return 0;
	}

	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	ResultParamsStruct_Pin3DMeasure ResultParamsPinMeasure;

	QDateTime startTime = QDateTime::currentDateTime();
	pcl::PointCloud<pcl::PointXYZ>::Ptr pin_cloud(new pcl::PointCloud<pcl::PointXYZ>);
	Pin3DMeasureParams1->CropPinCalCoord(match_cloud, pin_cloud, RunParamsPin3DMeasure, BasePlaneCoefficient, ResultParamsPinMeasure, currentColumn);
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	ui.plainTextEdit->appendPlainText("单组PIN针计算运行时间：" + QString::number(intervalTimeMS) + "ms");
	LoadShowCloud(pin_cloud);
	current_cloud = pin_cloud;

	if (RunParamsPin3DMeasure.ShowResult == 1)
	{
		for (int i = 0; i < ResultParamsPinMeasure.PinHeight.size(); i++)
		{
			ui.plainTextEdit->appendPlainText(QString::number(ResultParamsPinMeasure.PinHeight[i]));
		}

		for (int i = 0; i < ResultParamsPinMeasure.PinFlag.size(); i++)
		{
			ui.plainTextEdit->appendPlainText(QString::number(ResultParamsPinMeasure.PinFlag[i]));
		}
	}

	return 0;
}

int Pin3DMeasureWindow::on_pBtn_PinMeasureAlgo_clicked()
{
	if (ori_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "点云为空，请先加载点云", QMessageBox::Ok);
		return 0;
	}

	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	ResultParamsStruct_Pin3DMeasure ResultParamsPinMeasure;

	QDateTime startTime = QDateTime::currentDateTime();
	// 3D识别，获取结果
	Pin3DMeasureParams1->PinMeasureAlgo(ori_cloud, RunParamsPin3DMeasure, ResultParamsPinMeasure);
	
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	ui.plainTextEdit->appendPlainText("PIN计算总时间：" + QString::number(intervalTimeMS) + "ms");


	if (RunParamsPin3DMeasure.ShowResult == 1)
	{
		for (int i = 0; i < ResultParamsPinMeasure.PinHeight.size(); i++)
		{
			ui.plainTextEdit->appendPlainText(QString::number(ResultParamsPinMeasure.PinHeight[i]));
		}

		for (int i = 0; i < ResultParamsPinMeasure.PinFlag.size(); i++)
		{
			ui.plainTextEdit->appendPlainText(QString::number(ResultParamsPinMeasure.PinFlag[i]));
		}
	}

	return 0;
}

void Pin3DMeasureWindow::PP_Callback(const pcl::visualization::PointPickingEvent& event, void* args)
{
	struct PinMeasCallbackArgs* data = (struct PinMeasCallbackArgs *)args;
	if (event.getPointIndex() == -1)
		return;
	PointT current_point;
	event.getPoint(current_point.x, current_point.y, current_point.z);
	data->clicked_points_3d->points.push_back(current_point);
	pcl::visualization::PointCloudColorHandlerCustom<PointT> red(data->clicked_points_3d, 255, 0, 0);
	data->viewerPtr->removePointCloud("clicked_points");
	data->viewerPtr->addPointCloud(data->clicked_points_3d, red, "clicked_points");
	data->viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
	ui.dSpB_ROI_CenterX->setValue(current_point.x);
	ui.dSpB_ROI_CenterY->setValue(current_point.y);
	ui.dSpB_ROI_CenterZ->setValue(current_point.z);
}


void Pin3DMeasureWindow::PPB_Callback(const pcl::visualization::PointPickingEvent& event, void* args)
{
	struct PinMeasCallbackArgs* data = (struct PinMeasCallbackArgs *)args;
	if (event.getPointIndex() == -1)
		return;
	PointT current_point;
	event.getPoint(current_point.x, current_point.y, current_point.z);
	data->clicked_points_3d->points.push_back(current_point);
	pcl::visualization::PointCloudColorHandlerCustom<PointT> red(data->clicked_points_3d, 255, 0, 0);
	data->viewerPtr->removePointCloud("clicked_points");
	data->viewerPtr->addPointCloud(data->clicked_points_3d, red, "clicked_points");
	data->viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
	BasePointPickOrder++;
	if (BasePointPickOrder == 1)
	{
		ui.dSB_BasePoint1CenterX->setValue(current_point.x);
		ui.dSB_BasePoint1CenterY->setValue(current_point.y);
		ui.dSB_BasePoint1CenterZ->setValue(current_point.z);
	}
	else if (BasePointPickOrder == 2)
	{
		ui.dSB_BasePoint2CenterX->setValue(current_point.x);
		ui.dSB_BasePoint2CenterY->setValue(current_point.y);
		ui.dSB_BasePoint2CenterZ->setValue(current_point.z);
	}
	else if (BasePointPickOrder == 3)
	{
		ui.dSB_BasePoint3CenterX->setValue(current_point.x);
		ui.dSB_BasePoint3CenterY->setValue(current_point.y);
		ui.dSB_BasePoint3CenterZ->setValue(current_point.z);
	}
	else
	{
		ui.dSB_BasePoint4CenterX->setValue(current_point.x);
		ui.dSB_BasePoint4CenterY->setValue(current_point.y);
		ui.dSB_BasePoint4CenterZ->setValue(current_point.z);
	}
}

void Pin3DMeasureWindow::PPPin_Callback(const pcl::visualization::PointPickingEvent& event, void* args)
{
	struct PinMeasCallbackArgs* data = (struct PinMeasCallbackArgs *)args;
	if (event.getPointIndex() == -1)
		return;
	PointT current_point;
	event.getPoint(current_point.x, current_point.y, current_point.z);
	data->clicked_points_3d->points.push_back(current_point);
	pcl::visualization::PointCloudColorHandlerCustom<PointT> red(data->clicked_points_3d, 255, 0, 0);
	data->viewerPtr->removePointCloud("clicked_points");
	data->viewerPtr->addPointCloud(data->clicked_points_3d, red, "clicked_points");
	data->viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");	
	int currentColumn = ui.tbeW_PInParams->currentColumn();
	if (currentColumn == -1)
	{
		return;
	}
	CreateDoubleSpinBoxMM(0, currentColumn, current_point.x);
	CreateDoubleSpinBoxMM(1, currentColumn, current_point.y);
	CreateDoubleSpinBoxMM(2, currentColumn, current_point.z);
}


void Pin3DMeasureWindow::on_pBtn_PickCropModulePoint_clicked()
{
	if (ori_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "原始点云为空，请先加载点云", QMessageBox::Ok);
		return;
	}
	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	match_cloud.reset();
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_module(new pcl::PointCloud<pcl::PointXYZ>);
	Eigen::Matrix3f inverseRotationMatrix = Eigen::Matrix3f::Identity();
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ>(ori_cloud, "sample cloud1");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud1");
	viewer->addCoordinateSystem(5.0);
	viewer->setShowFPS(false);
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(10000));
		std::string cube;
		viewer->removePointCloud("sample cloud2");

		RunParamsPin3DMeasure.ROI_RX = this->ui.dSpB_ROI_RX->value();
		RunParamsPin3DMeasure.ROI_RY = this->ui.dSpB_ROI_RY->value();
		RunParamsPin3DMeasure.ROI_RZ = this->ui.dSpB_ROI_RZ->value();
		RunParamsPin3DMeasure.ROI_CenterX = this->ui.dSpB_ROI_CenterX->value();
		RunParamsPin3DMeasure.ROI_CenterY = this->ui.dSpB_ROI_CenterY->value();
		RunParamsPin3DMeasure.ROI_CenterZ = this->ui.dSpB_ROI_CenterZ->value();
		RunParamsPin3DMeasure.ROI_LengthX = this->ui.dSpB_ROI_LengthX->value();
		RunParamsPin3DMeasure.ROI_LengthY = this->ui.dSpB_ROI_LengthY->value();
		RunParamsPin3DMeasure.ROI_LengthZ = this->ui.dSpB_ROI_LengthZ->value();
		

		Pin3DMeasureParams1->Create3DRoi(ori_cloud, cloud_module, RunParamsPin3DMeasure, inverseRotationMatrix);
		Eigen::Quaternionf rotation(inverseRotationMatrix);
		Eigen::Vector3f center(RunParamsPin3DMeasure.ROI_CenterX, RunParamsPin3DMeasure.ROI_CenterY, RunParamsPin3DMeasure.ROI_CenterZ);
		viewer->removeShape(cube);
		viewer->addCube(center, rotation, RunParamsPin3DMeasure.ROI_LengthX, RunParamsPin3DMeasure.ROI_LengthY, RunParamsPin3DMeasure.ROI_LengthZ, cube);
		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, cube);
		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 0, 1, cube);
		viewer->addPointCloud<pcl::PointXYZ>(cloud_module, "sample cloud2");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1, 0, 0, "sample cloud2");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud2");
	}
}


void Pin3DMeasureWindow::on_pBtn_PickBaseFourPoint_clicked()
{
	if (this->ui.cbB_LocateMethod->currentText() == "机械定位")
	{
		match_cloud = module_cloud;
	}
	if (match_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "点云为空，请先加载点云或运行前序流程", QMessageBox::Ok);
		return;
	}
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("viewer"));
	viewer->addPointCloud(match_cloud, "cloud");
	viewer->setBackgroundColor(0, 0, 0);
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud");
	viewer->addCoordinateSystem(5.0);
	viewer->initCameraParameters();
	std::string str = "Number of point clouds: " + std::to_string(match_cloud->points.size());
	viewer->setShowFPS(false);
	viewer->addText(str, 0, 0, 20, 1.0, 1.0, 1.0, "sre");
	BasePointPickOrder = 0;
	cloud_mutex.lock();    
	struct PinMeasCallbackArgs cb_args;
	PointCloudT::Ptr clicked_points_3d(new PointCloudT);
	cb_args.clicked_points_3d = clicked_points_3d;
	cb_args.viewerPtr = pcl::visualization::PCLVisualizer::Ptr(viewer);
	viewer->registerPointPickingCallback(&Pin3DMeasureWindow::PPB_Callback, *this, (void*)&cb_args);
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	cloud_mutex.unlock();
}


int Pin3DMeasureWindow::on_pBtn_PickCropPinPoint_clicked()
{
	if (this->ui.cbB_LocateMethod->currentText() == "机械定位")
	{
		match_cloud = module_cloud;
	}
	if (match_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "点云为空，请先加载点云", QMessageBox::Ok);
		return 0;
	}
	int currentColumn = ui.tbeW_PInParams->currentColumn();
	if (currentColumn == -1)
	{
		QMessageBox::information(this, "提示", "当前未选中PIN参数，请加载或创建PIN参数后选中", QMessageBox::Ok);
		return 0;
	}
	SetRunParams_Pin3DMeasure(RunParamsPin3DMeasure);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_pin(new pcl::PointCloud<pcl::PointXYZ>);
	Eigen::Matrix3f inverseRotationMatrix = Eigen::Matrix3f::Identity();

	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZ>(match_cloud, "sample cloud1");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud1");
	viewer->addCoordinateSystem(5.0);
	viewer->setShowFPS(false);
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(10000));
		std::string cube;
		viewer->removePointCloud("sample cloud2");

		RunParamsPin3DMeasure.ROI_CenterX = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 0, currentColumn);
		RunParamsPin3DMeasure.ROI_CenterY = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 1, currentColumn);
		RunParamsPin3DMeasure.ROI_CenterZ = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 2, currentColumn);
		RunParamsPin3DMeasure.ROI_LengthX = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 3, currentColumn);
		RunParamsPin3DMeasure.ROI_LengthY = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 4, currentColumn);
		RunParamsPin3DMeasure.ROI_LengthZ = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 5, currentColumn);
		RunParamsPin3DMeasure.ROI_RX = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 6, currentColumn);
		RunParamsPin3DMeasure.ROI_RY = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 7, currentColumn);
		RunParamsPin3DMeasure.ROI_RZ = GetDoubleSpinBoxValue(this->ui.tbeW_PInParams, 8, currentColumn);

		Pin3DMeasureParams1->Create3DRoi(match_cloud, cloud_pin, RunParamsPin3DMeasure, inverseRotationMatrix);
		Eigen::Quaternionf rotation(inverseRotationMatrix);
		Eigen::Vector3f center(RunParamsPin3DMeasure.ROI_CenterX, RunParamsPin3DMeasure.ROI_CenterY, RunParamsPin3DMeasure.ROI_CenterZ);
		viewer->removeShape(cube);
		viewer->addCube(center, rotation, RunParamsPin3DMeasure.ROI_LengthX, RunParamsPin3DMeasure.ROI_LengthY, RunParamsPin3DMeasure.ROI_LengthZ, cube);
		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, cube);
		viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 0, 1, cube);
		viewer->addPointCloud<pcl::PointXYZ>(cloud_pin, "sample cloud2");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1, 0, 0, "sample cloud2");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud2");
	}
	return 0;
}

/*
int PinMeasureWindow::on_pBtn_PickCropPinPoint_clicked()
{
	if (this->ui.cbB_LocateMethod->currentText() == "机械定位")
	{
		match_cloud = module_cloud;
	}
	if (match_cloud == NULL)
	{
		QMessageBox::information(this, "提示", "点云为空，请先加载点云", QMessageBox::Ok);
		return 0;
	}
	int currentColumn = ui.tbeW_PInParams->currentColumn();
	if (currentColumn == -1)
	{
		QMessageBox::information(this, "提示", "当前无PIN参数，请加载或创建PIN参数", QMessageBox::Ok);
		return 0;
	}
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("viewer"));
	viewer->addPointCloud(match_cloud, "cloud");
	viewer->setBackgroundColor(0, 0, 0);
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud");
	viewer->addCoordinateSystem(5.0);
	viewer->initCameraParameters();
	std::string str = "Number of point clouds: " + std::to_string(match_cloud->points.size());
	viewer->setShowFPS(false);
	viewer->addText(str, 0, 0, 20, 1.0, 1.0, 1.0, "sre");
	cloud_mutex.lock();
	struct PinMeasCallbackArgs cb_args;
	PointCloudT::Ptr clicked_points_3d(new PointCloudT);
	cb_args.clicked_points_3d = clicked_points_3d;
	cb_args.viewerPtr = pcl::visualization::PCLVisualizer::Ptr(viewer);
	viewer->registerPointPickingCallback(&PinMeasureWindow::PPPin_Callback, *this, (void*)&cb_args);
	while (!viewer->wasStopped()) {
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	cloud_mutex.unlock();
	return 0;
}
*/

//exe路径
string Pin3DMeasureWindow::FolderPath()
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
string Pin3DMeasureWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
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

void Pin3DMeasureWindow::wheelEvent(QWheelEvent* event){
	QWidget* focusedWidget = QApplication::focusWidget();

	// 检查鼠标是否在 QOpenGLWidget 范围内
	if (focusedWidget && focusedWidget->inherits("QOpenGLWidget")) {
		// 如果当前焦点是 QOpenGLWidget，则不让父窗口滚动
		event->accept();
	}
	else {
		QMainWindow::wheelEvent(event);
	}
}