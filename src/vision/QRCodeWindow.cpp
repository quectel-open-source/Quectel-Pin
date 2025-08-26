#include "QRCodeWindow.h"

QRCodeWindow::QRCodeWindow(QWidget *parent)
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
	code2dParams1 = new QRCodeDetect();
	//��ά����
	IniTableData_Code2d();
	//�����ļ���XML·��
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
	//�ͷŶ�ά���ڴ�
	delete code2dParams1;
}
void QRCodeWindow::on_toolButton_clicked() {
	QRCodeWindow::showMinimized(); //��С��
}

void QRCodeWindow::on_toolButton_2_clicked() {
	if (QRCodeWindow::isMaximized()) {
		QRCodeWindow::showNormal();//��ԭ�¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		QRCodeWindow::showMaximized();//����¼�
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
		//��������󻯻���С��ʱҲ�����д���
		if (QRCodeWindow::isMaximized() || QRCodeWindow::isMinimized()) {
			return;
		}
		else {
			//���ڰ�ť֮����Ҫ�������ĵط������д���(����)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() || ui.cmb_IsFollow->underMouse()
				|| ui.lbl_Window->underMouse() || ui.btn_DrawRoi_Code2d->underMouse() || ui.cmb_Tolerance_Code2d->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//QRCodeWindow::move(QRCodeWindow::mapToGlobal(event->pos() - whereismouse));//�ƶ�
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
void QRCodeWindow::InitWindow()
{
	//����halcon���ļ�·��Ϊutf8�������������
	SetSystem("filename_encoding", "utf8");
	//�������д��룬����������ƶ�ʱ������mouseMove����ʱ���Կ������ꡣ����ֻ������갴��ʱ�ſ��Կ�������
	ui.lbl_Window->setMouseTracking(true);
	ui.frame->hide();
	//���ɿ�ͼ��
	GenEmptyObj(&ho_Image);
	GenEmptyObj(&ho_ShowObj);
	GenEmptyObj(&RetRegionShow);
	RetRegionShow.Clear();
	RetCountShow = 0;
	ho_Image.Clear();
	m_hHalconID = NULL;
	m_hLabelID = (Hlong)ui.lbl_Window->winId();

}

//��ʾͼ��
void QRCodeWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
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
void QRCodeWindow::GiveParameterToWindow()
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
		QMessageBox::information(this, "��ʾ", "ͼ���ʧ��!", QMessageBox::Ok);
		return;
	}

}

//ͼ������Ӧ����
void QRCodeWindow::on_pBtn_FitImage_clicked()
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
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	GiveParameterToWindow();
}
//��մ���
void QRCodeWindow::on_btn_ClearWindow_clicked()
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
//****************��ά������غ���***********************
//���в��Ҷ�ά��ṹ�������ֵ����
void QRCodeWindow::SetRunParams_Code2d(RunParamsStruct_Code2d &runParams)
{
	try
	{
		//���ö�ά�빤������
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

		if (ui.cmb_Tolerance_Code2d->currentText() == "��׼")
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
		GenEmptyObj(&SearchRoi_Code2d);
		//��������ϲ�
		if (!ho_Code2dRegions.isEmpty())
		{
			//һά����������ϲ�
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
			//������ʾͼ�θ�ֵ
			ConcatObj(ho_ShowObj, SearchRoi_Code2d, &ho_ShowObj);
			runParams.ho_SearchRegion = SearchRoi_Code2d;
		}
		else
		{
			//���û�л�����������,�ÿ�
			GenEmptyObj(&SearchRoi_Code2d);
			SearchRoi_Code2d.Clear();
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
//���ö�ά������ʾ�������
void QRCodeWindow::SetTableData_Code2d(const ResultParamsStruct_Code2d &resultStruct, qint64 Ct)
{
	try
	{
		getmessage("CT:" + QString::number(Ct) + "ms");
		//�ж��Ƿ��ҵ���ά��
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
		//ˢ�½�����
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
//���¶�ά�������в���������
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
		QMessageBox::information(this, "��ʾ", "����ˢ��ʧ��!", QMessageBox::Ok);
		return;

	}
}

//�����ά�������
void QRCodeWindow::on_pb_SaveData_clicked()
{
	//�ѱ��������Ϊ�Զ����ļ�����
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/QRCode_formulation";

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
	//���ö�ά�����в���
	SetRunParams_Code2d(hv_RunParamsCode2d);
	//�����ά�����в���
	ret = dataIOC.WriteParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��", QMessageBox::Ok);
		return;
	}
	code2dParams1->configPath = ConfigPath;
	code2dParams1->XMLPath = XmlPath;
	code2dParams1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//��ȡ��ά�������
void QRCodeWindow::on_pb_LoadData_clicked()
{
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/QRCode_formulation";

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
	//������Ҫ���
	hv_RunParamsCode2d.hv_CodeType_Run.Clear();
	//��ȡ��ά�����
	errorCode = dataIOC.ReadParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "��ʾ", "������ȡʧ��", QMessageBox::Ok);
		return;
	}
	//�������µ�����
	UpdateCode2dDataWindow(hv_RunParamsCode2d);
}
//���ƶ�ά����������
void QRCodeWindow::on_btn_DrawRoi_Code2d_clicked()
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
		ho_Code2dRegions.push_back(TempRoi);
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
void QRCodeWindow::on_btn_ClearRoi_Code2d_clicked()
{
	ho_Code2dRegions.clear();
	GenEmptyObj(&SearchRoi_Code2d);
	GenEmptyObj(&ho_ShowObj);
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	GiveParameterToWindow();
	showImg(ho_Image, "margin", "green");
}
//��ʼ����ά������ʾ���
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
			font.setBold(true);//����Ϊ����
			font.setPointSize(12);//���������С
			//hearderItem->setTextColor(Qt::red);//������ɫ
			hearderItem->setForeground(QBrush(Qt::red));//������ɫ
			hearderItem->setFont(font);//��������
			ui.tablewidget_Results_Code2d->setHorizontalHeaderItem(i, hearderItem);
		}
		//�󶨽�����ı�ͷ����¼����źŲ�
		connect(ui.tablewidget_Results_Code2d->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_Code2d(int)));
	}
	catch (...)
	{

	}
}
//��ά����ۺ���
void QRCodeWindow::on_pb_Run_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	int ret = 1;
	//ʶ���ά��
	QDateTime startTime = QDateTime::currentDateTime();
	ret = slot_FindCode2d();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);

	//ˢ�½��
	SetTableData_Code2d(code2dParams1->mResultParams, intervalTimeMS);
}
//����ά��ۺ���
int QRCodeWindow::slot_FindCode2d()
{
	try
	{
		int ret = 1;
		//XML��ȡ����
		hv_RunParamsCode2d.hv_CodeType_Run.Clear();
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML��ȡʧ�ܣ���Ĭ��ֵ
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
			//�����ά�����в���
			ret = dataIOC.WriteParams_Code2d(ConfigPath, XmlPath, hv_RunParamsCode2d, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//�����㷨
		return code2dParams1->FindCode2dFunc(ho_Image, hv_RunParamsCode2d, code2dParams1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//������ÿ��Cell����¼�
void QRCodeWindow::on_tablewidget_Results_Code2d_cellClicked(int row, int column)
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
void QRCodeWindow::slot_VerticalHeader_Code2d(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Results_Code2d->currentRow();
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
//����һ������strOld
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
