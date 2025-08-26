#include "BarCodeWindow.h"

BarCodeWindow::BarCodeWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	////��������
	//QFile f("./qdarkstyle/theme/darkstyle.qss");
	//if (!f.exists()) {
	//	printf("Unable to set stylesheet, file not found\n");
	//}
	//else {
	//	f.open(QFile::ReadOnly | QFile::Text);
	//	QTextStream ts(&f);
	//	qApp->setStyleSheet(ts.readAll());
	//}
	setWindowFlags(Qt::FramelessWindowHint);//���ر߿� 
	//���ڳ�ʼ��
	InitWindow();
	barCodeParams1 = new BarCodeDetect();
	//һά����
	IniTableData_BarCode();
	//�����ļ���XML·��
	//ConfigPath = FolderPath() + "/config/";
	//XmlPath = FolderPath() + "/config/config.xml";
}

BarCodeWindow::BarCodeWindow(HObject image, int _processID, int modubleID, std::string _modubleName)
{
	barCodeParams1 = new BarCodeDetect();
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processId = _processID;
	if (image.Key() != nullptr)
		ho_Image = image;
}

BarCodeWindow::~BarCodeWindow()
{
	//�ͷ�ֱ��new���ڴ�
	if (barCodeParams1)
	{
		delete barCodeParams1;
	}
}
void BarCodeWindow::on_toolButton_clicked() {
	BarCodeWindow::showMinimized(); //��С��
}

void BarCodeWindow::on_toolButton_2_clicked() {
	if (BarCodeWindow::isMaximized()) {
		BarCodeWindow::showNormal();//��ԭ�¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		BarCodeWindow::showMaximized();//����¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void BarCodeWindow::on_toolButton_3_clicked() {
	this->close();
}

void BarCodeWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void BarCodeWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//��������󻯻���С��ʱҲ�����д���
		if (BarCodeWindow::isMaximized() || BarCodeWindow::isMinimized()) {
			return;
		}
		else {
			//���ڰ�ť֮����Ҫ�������ĵط������д���(����)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse()|| ui.cmb_IsFollow->underMouse()
				|| ui.lbl_Window->underMouse() || ui.btn_DrawRoi_BarCode->underMouse() || ui.cmb_Tolerance_BarCode->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//BarCodeWindow::move(BarCodeWindow::mapToGlobal(event->pos() - whereismouse));//�ƶ�
			}
		}
	}
}
void BarCodeWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

void BarCodeWindow::resizeEvent(QResizeEvent* event)
{
	try
	{
		// �����ﴦ���ڴ�С�ı��¼�
	//����halcon���ļ�·��Ϊutf8�������������
		SetSystem("filename_encoding", "utf8");
		//�������д��룬����������ƶ�ʱ������mouseMove����ʱ���Կ������ꡣ����ֻ������갴��ʱ�ſ��Կ�������
		ui.lbl_Window->setMouseTracking(true);
		m_hHalconID = NULL;
		m_hLabelID = (Hlong)ui.lbl_Window->winId();
		on_pBtn_FitImage_clicked();
	}
	catch (...)
	{

	}

}

//��ʼ����ʾ����
void BarCodeWindow::InitWindow()
{
	//����halcon���ļ�·��Ϊutf8�������������
	SetSystem("filename_encoding", "utf8");
	//�������д��룬����������ƶ�ʱ������mouseMove����ʱ���Կ������ꡣ����ֻ������갴��ʱ�ſ��Կ�������
	ui.lbl_Window->setMouseTracking(true);
	//���ɿ�ͼ��
	GenEmptyObj(&ho_Image);
	GenEmptyObj(&ho_ShowObj);
	GenEmptyObj(&RetRegionShow);
	RetRegionShow.Clear();
	ho_Image.Clear();
	m_hHalconID = NULL;
	m_hLabelID = (Hlong)ui.lbl_Window->winId();
	ui.frame->hide();
}

//��ʾͼ��
void BarCodeWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
{
	try
	{
		if (ho_Img.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		SetSystem("flush_graphic", "false");
		//�жϴ��ھ��
		if (m_hHalconID != NULL)
		{
			//�����ͼ���������ͼ��
			DetachBackgroundFromWindow(m_hHalconID);
		}
		else
		{
			//�򿪴���
			OpenWindow(0, 0, ui.lbl_Window->width(), ui.lbl_Window->height(), m_hLabelID, "visible", "", &m_hHalconID);
		}

		//����ͼ����ʾ����
		SetDraw(m_hHalconID, hv_FillType);
		SetColor(m_hHalconID, hv_Color);
		SetLineWidth(m_hHalconID, 1);
		//���ô�����ʾ����
		ui.lbl_Window->setHalconWnd(ho_Img, ho_ShowObj, isShowObj, m_hHalconID, ui.lbl_Status);

		//��ȡͼ���С
		GetImageSize(ho_Img, &m_imgWidth, &m_imgHeight);
		//��ȡ����ϵ��
		TupleMin2(1.0 * ui.lbl_Window->width() / m_imgWidth, 1.0 * ui.lbl_Window->height() / m_imgHeight, &m_hvScaledRate);
		//����ͼ��
		ZoomImageFactor(ho_Img, &m_hResizedImg, m_hvScaledRate, m_hvScaledRate, "constant");
		//��ȡ���ź�Ĵ�С
		GetImageSize(m_hResizedImg, &m_scaledWidth, &m_scaledHeight);
		//�򿪴���
		if (1.0 * ui.lbl_Window->width() / m_imgWidth < 1.0 * ui.lbl_Window->height() / m_imgHeight)
		{
			SetWindowExtents(m_hHalconID, ui.lbl_Window->height() / 2.0 - m_scaledHeight / 2.0, 0, ui.lbl_Window->width(), m_scaledHeight);
		}
		else
		{
			SetWindowExtents(m_hHalconID, 0, ui.lbl_Window->width() / 2.0 - m_scaledWidth / 2.0, m_scaledWidth, ui.lbl_Window->height());

		}
		SetPart(m_hHalconID, 0, 0, m_imgHeight - 1, m_imgWidth - 1);
		//�����ʾͼ�񣬲�����˸(��ͼ�����óɴ��ڵı���ͼ)
				//AttachBackgroundToWindow(ho_Img, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(ho_Img, m_hHalconID);

		//��ʾͼ��
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

//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
void BarCodeWindow::GiveParameterToWindow()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}

		//�жϴ��ھ��
		if (m_hHalconID != NULL)
		{
			//�����ͼ���������ͼ��
			//DetachBackgroundFromWindow(m_hHalconID);
		}
		else
		{
			//�򿪴���
			OpenWindow(0, 0, ui.lbl_Window->width(), ui.lbl_Window->height(), m_hLabelID, "visible", "", &m_hHalconID);
		}

		//����ͼ����ʾ����
		SetDraw(m_hHalconID, "margin");
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);
		//���ô�����ʾ����
		ui.lbl_Window->setHalconWnd(ho_Image, ho_ShowObj, isShowObj, m_hHalconID, ui.lbl_Status);

	}
	catch (...)
	{

	}

}

//��ͼƬ
void BarCodeWindow::on_pBtn_ReadImage_clicked()
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
		QMessageBox::information(this, "��ʾ", "ͼ���ʧ��!", QMessageBox::Ok);
		return;
	}
}

//ͼ������Ӧ����
void BarCodeWindow::on_pBtn_FitImage_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		//QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	showImg(ho_Image, "margin", "green");
}
//�Ƿ���ʾͼ�θ�ѡ��
void BarCodeWindow::on_ckb_ShowObj_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked)
	{
		isShowObj = true;
	}
	else
	{
		isShowObj = false;
	}
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	GiveParameterToWindow();
}
//��մ���
void BarCodeWindow::on_btn_ClearWindow_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	//��մ�����ʾ��ͼ�α���
	GenEmptyObj(&ho_ShowObj);
	//��ʾԭͼ
	showImg(ho_Image, "margin", "green");
}
//****************************************************************************************************
//****************һά������غ���***********************
//һά����ۺ���
void BarCodeWindow::on_pb_Run_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	int ret = 1;
	//ʶ��һά��
	QDateTime startTime = QDateTime::currentDateTime();
	 ret = slot_FindBarCode();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);

	//ˢ�½��
	SetTableData_BarCode(barCodeParams1->mResultParams, intervalTimeMS);
	mResultParams = barCodeParams1->mResultParams;
}
//��һά��ۺ���
int BarCodeWindow::slot_FindBarCode()
{
	try
	{
		int ret = 1;
		//XML��ȡ����
		hv_RunParamsBarCode.hv_CodeType_Run.Clear();
		ErrorCode_Xml errorCode = dataIOC.ReadParams_BarCode(ConfigPath, XmlPath, hv_RunParamsBarCode, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML��ȡʧ�ܣ���Ĭ��ֵ
			GenEmptyObj(&hv_RunParamsBarCode.ho_SearchRegion);
			hv_RunParamsBarCode.ho_SearchRegion.Clear();
			hv_RunParamsBarCode.hv_CodeType_Run = "auto";
			hv_RunParamsBarCode.hv_CodeNum = 1;
			hv_RunParamsBarCode.hv_Tolerance = "high";
			hv_RunParamsBarCode.hv_HeadChar = "null";
			hv_RunParamsBarCode.hv_EndChar = "null";
			hv_RunParamsBarCode.hv_ContainChar = "null";
			hv_RunParamsBarCode.hv_NotContainChar = "null";
			hv_RunParamsBarCode.hv_CodeLength_Run = 2;
			hv_RunParamsBarCode.hv_TimeOut = 100;
			//����һά�����в���
			ret = dataIOC.WriteParams_BarCode(ConfigPath, XmlPath, hv_RunParamsBarCode, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//�����㷨
		return barCodeParams1->FindBarCodeFunc(ho_Image, hv_RunParamsBarCode, barCodeParams1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//���в���һά��ṹ�������ֵ����
void BarCodeWindow::SetRunParams_BarCode(RunParamsStruct_BarCode &runParams)
{
	try
	{
		runParams.hv_CodeNum = ui.spb_CodeNum_BarCode->value();

		string strTemp = ui.txt_HeadChar_BarCode->text().trimmed().toStdString().c_str();
		runParams.hv_HeadChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_EndChar_BarCode->text().trimmed().toStdString().c_str();
		runParams.hv_EndChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_ContainChar_BarCode->text().trimmed().toStdString().c_str();
		runParams.hv_ContainChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_NotContainChar_BarCode->text().trimmed().toStdString().c_str();
		runParams.hv_NotContainChar = HTuple(strTemp.c_str());

		strTemp = ui.txt_NotContainChar_BarCode->text().trimmed().toStdString().c_str();
		runParams.hv_NotContainChar = HTuple(strTemp.c_str());


		runParams.hv_CodeLength_Run = HTuple(ui.spb_CodeLength_BarCode->value());
		runParams.hv_TimeOut = ui.spb_TimeOut_BarCode->value();

		CodeTypeStruct_BarCode CodeTypes;
		runParams.hv_CodeType_Run.Clear();
		if (ui.ckb_CODABAR_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.CODABAR);
		}
		if (ui.ckb_CODE128_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.CODE128);
		}
		if (ui.ckb_CODE39_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.CODE39);
		}
		if (ui.ckb_CODE93_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.CODE93);
		}
		if (ui.ckb_EAN13_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.EAN13);
		}
		if (ui.ckb_EAN8_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.EAN8);
		}
		if (ui.ckb_ITF25_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.ITF25);
		}
		if (ui.ckb_PHARMACODE_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.PHARMACODE);
		}
		if (ui.ckb_UPCA_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.UPCA);
		}
		if (ui.ckb_UPCE_BarCode->isChecked())
		{
			runParams.hv_CodeType_Run.Append(CodeTypes.UPCE);
		}
		if (runParams.hv_CodeType_Run.TupleLength() == 0)
		{
			runParams.hv_CodeType_Run = CodeTypes.AUTO;
		}

		if (ui.cmb_Tolerance_BarCode->currentText() == "��׼")
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
		//��������ֵ
		GenEmptyObj(&SearchRoi_BarCode);
		//��������ϲ�
		if (!ho_BarCodeRegions.isEmpty())
		{
			//һά����������ϲ�
			if (ho_BarCodeRegions.count() == 1)
			{
				SearchRoi_BarCode = ho_BarCodeRegions[0];
			}
			else
			{
				SearchRoi_BarCode = ho_BarCodeRegions[0];
				for (int i = 1; i < ho_BarCodeRegions.count(); i++)
				{
					Union2(SearchRoi_BarCode, ho_BarCodeRegions[i], &SearchRoi_BarCode);
				}
			}
			//������ʾͼ�θ�ֵ
			ConcatObj(ho_ShowObj, SearchRoi_BarCode, &ho_ShowObj);
			runParams.ho_SearchRegion = SearchRoi_BarCode;
		}
		else
		{
			//���û�л�����������,�ÿ�
			GenEmptyObj(&SearchRoi_BarCode);
			SearchRoi_BarCode.Clear();
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��!", QMessageBox::Ok);
		return;
	}
}
//����һά����������
void BarCodeWindow::on_btn_DrawRoi_BarCode_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		HObject TempRoi;
		HTuple RecRow1, RecCol1, RecRow2, RecCol2;
		DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
		GenRectangle1(&TempRoi, RecRow1, RecCol1, RecRow2, RecCol2);
		ho_BarCodeRegions.push_back(TempRoi);
		ConcatObj(ho_ShowObj, TempRoi, &ho_ShowObj);
		//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
		GiveParameterToWindow();
		DispObj(TempRoi, m_hHalconID);
	}
	catch (...)
	{
	}
}
//���ROI
void BarCodeWindow::on_btn_ClearRoi_BarCode_clicked()
{
	ho_BarCodeRegions.clear();
	GenEmptyObj(&SearchRoi_BarCode);
	SearchRoi_BarCode.Clear();
	GenEmptyObj(&ho_ShowObj);
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	GiveParameterToWindow();
	showImg(ho_Image, "margin", "green");
}
//��ʼ��һά������ʾ���
void BarCodeWindow::IniTableData_BarCode()
{
	try
	{
		std::vector<std::string> vec_Name = barCodeParams1->getModubleResultTitleList();
		QTableWidgetItem   *hearderItem;
		ui.tablewidget_Results_BarCode->setColumnCount(vec_Name.size());
		for (int i = 0; i < ui.tablewidget_Results_BarCode->columnCount(); i++)
		{
			hearderItem = new QTableWidgetItem(QString::fromStdString(vec_Name[i]));
			QFont font = hearderItem->font();
			font.setBold(true);//����Ϊ����
			font.setPointSize(12);//���������С
			//hearderItem->setTextColor(Qt::red);//������ɫ
			hearderItem->setForeground(QBrush(Qt::red));//������ɫ
			hearderItem->setFont(font);//��������
			ui.tablewidget_Results_BarCode->setHorizontalHeaderItem(i, hearderItem);
		}
		//�󶨽�����ı�ͷ����¼����źŲ�
		connect(ui.tablewidget_Results_BarCode->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_BarCode(int)));
	}
	catch (...)
	{

	}
}
//����һά������ʾ�������
void BarCodeWindow::SetTableData_BarCode(const ResultParamsStruct_BarCode &resultStruct, qint64 Ct)
{
	try
	{
		getmessage("CT:" + QString::number(Ct) + "ms");
		//�ж��Ƿ��ҵ�����
		GenEmptyObj(&ho_ShowObj);
		if (resultStruct.hv_RetNum > 0)
		{
			RetCountShow = resultStruct.hv_RetNum;
			RetRegionShow = resultStruct.ho_RegionBarCode;
			ConcatObj(ho_ShowObj, resultStruct.ho_RegionBarCode, &ho_ShowObj);
		}
		else
		{
			RetCountShow = 0;
			GenEmptyObj(&RetRegionShow);
			RetRegionShow.Clear();
		}
		showImg(ho_Image, "margin", "green");
		//ˢ�½�����
		std::vector<std::map<int, std::string>> resultList = barCodeParams1->getModubleResultList();
		int count = resultList.size();
		ui.tablewidget_Results_BarCode->clearContents();
		ui.tablewidget_Results_BarCode->setRowCount(count);
		for (int i = 0; i < count; i++)
		{
			QTableWidgetItem* item;
			for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++)
			{
				item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_Results_BarCode->setItem(i, iter->first, item);
			}
		}
	}
	catch (...)
	{

	}
}
//����һά�������
void BarCodeWindow::on_pb_SaveData_clicked()
{
	//�ѱ��������Ϊ�Զ����ļ�����
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/BarCode_formulation";

	QDir dir2(folderPath);

	// ����ļ��в����ڣ��򴴽��ļ���
	if (!dir2.exists()) {
		if (!dir2.mkpath(".")) {
			QMessageBox::critical(nullptr, tr("Error"), tr("Failed to create the folder."));
			return;
		}
	}

	// ���ļ�����Ի���
	QString filePath = QFileDialog::getSaveFileName(
		nullptr,
		tr("Save File"),
		folderPath,
		tr("XML Files (*.xml)")
	);

	// ����û�û��ѡ���ļ������ߵ����ȡ��
	if (filePath.isEmpty()) {
		return;
	}

	// ȷ���ļ���չ���� .xml
	if (!filePath.endsWith(".xml", Qt::CaseInsensitive)) {
		filePath += ".xml";
	}

	// ȷ��Ŀ¼����
	QDir dir(QFileInfo(filePath).absolutePath());
	if (!dir.exists()) {
		if (!dir.mkpath(".")) {
			QMessageBox::critical(nullptr, tr("Error"), tr("Failed to create the folder."));
			return;
		}
	}

	QFileInfo fileInfo(filePath);
	// �����ļ��ľ���Ŀ¼·��
	QString configPath = fileInfo.absoluteDir().path();
	if (!configPath.endsWith('/')) {
		configPath += '/';
	}

	//����ԭ���Ĵ洢·��
	ConfigPath = configPath.toLocal8Bit().data();
	XmlPath = filePath.toLocal8Bit().data();

	//250304 ģ��ƥ����õ���� nodeName ��
	nodeName = fileInfo.baseName().toUtf8().data();

	int ret = 1;
	//����һά�����в���
	SetRunParams_BarCode(hv_RunParamsBarCode);
	//����һά�����в���
	ret = dataIOC.WriteParams_BarCode(ConfigPath, XmlPath, hv_RunParamsBarCode, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��", QMessageBox::Ok);
		return;
	}
	barCodeParams1->configPath = ConfigPath;
	barCodeParams1->XMLPath = XmlPath;
	barCodeParams1->getRefreshConfig(processId, processModbuleID, nodeName);
}

//��ȡһά�������
void BarCodeWindow::on_pb_LoadData_clicked()
{	
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/BarCode_formulation";

	QDir dir2(folderPath);

	// ����ļ��в����ڣ��򴴽��ļ���
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
	// �����ļ��ľ���Ŀ¼·��
	QString configPath = fileInfo.absoluteDir().path();
	if (!configPath.endsWith('/')) {
		configPath += '/';
	}

	ConfigPath = configPath.toLocal8Bit().data();
	XmlPath = filePath.toLocal8Bit().data();
	//250304 ģ��ƥ����õ���� nodeName ��
	nodeName = fileInfo.baseName().toUtf8().data();

	ErrorCode_Xml errorCode;
	//��ȡһά�����
	errorCode = dataIOC.ReadParams_BarCode(ConfigPath, XmlPath, hv_RunParamsBarCode, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "��ʾ", "������ȡʧ��", QMessageBox::Ok);
		return;
	}
	//�������µ�����
	UpdateBarCodeDataWindow(hv_RunParamsBarCode);
}

//����һά�������в���������
void BarCodeWindow::UpdateBarCodeDataWindow(const RunParamsStruct_BarCode &runParams)
{
	try
	{
		ui.spb_CodeNum_BarCode->setValue(runParams.hv_CodeNum.I());
		SearchRoi_BarCode = runParams.ho_SearchRegion;

		QString contentTemp = "";
		contentTemp = QString::fromUtf8(runParams.hv_HeadChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_HeadChar_BarCode->setText(contentTemp);
		}

		contentTemp = QString::fromUtf8(runParams.hv_EndChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_EndChar_BarCode->setText(contentTemp);
		}

		contentTemp = QString::fromUtf8(runParams.hv_ContainChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_ContainChar_BarCode->setText(contentTemp);
		}

		contentTemp = QString::fromUtf8(runParams.hv_NotContainChar.S().Text());
		if (contentTemp != "null" && contentTemp != "")
		{
			ui.txt_NotContainChar_BarCode->setText(contentTemp);
		}

		ui.spb_CodeLength_BarCode->setValue(runParams.hv_CodeLength_Run.I());
		ui.spb_TimeOut_BarCode->setValue(runParams.hv_TimeOut.I());

		CodeTypeStruct_BarCode codeTypeStruct;
		int len = runParams.hv_CodeType_Run.TupleLength().I();
		for (int i = 0; i < len; i++)
		{
			if (runParams.hv_CodeType_Run[i] == codeTypeStruct.CODABAR)
			{
				ui.ckb_CODABAR_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.CODE128)
			{
				ui.ckb_CODE128_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.CODE39)
			{
				ui.ckb_CODE39_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.CODE93)
			{
				ui.ckb_CODE93_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.EAN13)
			{
				ui.ckb_EAN13_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.EAN8)
			{
				ui.ckb_EAN8_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.ITF25)
			{
				ui.ckb_ITF25_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.PHARMACODE)
			{
				ui.ckb_PHARMACODE_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.UPCA)
			{
				ui.ckb_UPCA_BarCode->setCheckState(Qt::Checked);
			}
			else if (runParams.hv_CodeType_Run[i] == codeTypeStruct.UPCE)
			{
				ui.ckb_UPCE_BarCode->setCheckState(Qt::Checked);
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
		QMessageBox::information(this, "��ʾ", "����ˢ��ʧ��", QMessageBox::Ok);
		return;
	}
}
//������ÿ��Cell����¼�
void BarCodeWindow::on_tablewidget_Results_BarCode_cellClicked(int row, int column)
{
	try
	{
		int SelectRow = row;
		if (SelectRow < 0)
		{
			return;
		}
		//ˢ�½������
		if (RetCountShow > 0)
		{
			//����ʾ��������
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
			//����ʾѡ������
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//�������ͷ�ĵ���¼��ۺ���
void BarCodeWindow::slot_VerticalHeader_BarCode(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Results_BarCode->currentRow();
		if (SelectRow < 0)
		{
			return;
		}
		//ˢ�½������
		if (RetCountShow > 0)
		{
			//����ʾ��������
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
			//����ʾѡ������
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//exe·��
string BarCodeWindow::FolderPath()
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
//����һ������strOld
string BarCodeWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
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