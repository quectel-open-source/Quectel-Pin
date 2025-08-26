#include "TemplateWindow.h"

TemplateWindow::TemplateWindow(QWidget *parent)
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
	TemplateDetect1 = new TemplateDetect();
	//��ʼ�������ʾ���
	IniTableData_Template();
	//Ĭ�����ظ߼�����
	ui.widget_AdvancedData->setVisible(false);

	ui.groupBox_3->hide();

	////�����ļ���XML·��
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
	//�ͷ�ģ��new���ڴ�
	delete TemplateDetect1;
}
void TemplateWindow::on_toolButton_clicked() {
	TemplateWindow::showMinimized(); //��С��
}

void TemplateWindow::on_toolButton_2_clicked() {
	if (TemplateWindow::isMaximized()) {
		TemplateWindow::showNormal();//��ԭ�¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		TemplateWindow::showMaximized();//����¼�
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
		//��������󻯻���С��ʱҲ�����д���
		if (TemplateWindow::isMaximized() || TemplateWindow::isMinimized()) {
			return;
		}
		else {
			//���ڰ�ť֮����Ҫ�������ĵط������д���(����)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() ||
				ui.lbl_Window->underMouse() || ui.cmb_RoiShape_Template->underMouse() || ui.cmb_ModelType->underMouse() || ui.cmb_MatchMethod_Train->underMouse() ||
				ui.cmb_Metric_Train->underMouse() || ui.cmb_Optimization_Train->underMouse() || ui.cmb_SubPixel_Run->underMouse()
				) {

			}
			else if (keypoint == 1) {

			}
			else {
				//TemplateWindow::move(TemplateWindow::mapToGlobal(event->pos() - whereismouse));//�ƶ�
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
		// �����ﴦ���ڴ�С�ı��¼�
	//����halcon���ļ�·��Ϊutf8�������������
		SetSystem("filename_encoding", "utf8");
		//�������д��룬����������ƶ�ʱ������mouseMove����ʱ���Կ������ꡣ����ֻ������갴��ʱ�ſ��Կ�������
		ui.lbl_Window->setMouseTracking(true);
		m_hHalconID = NULL;
		m_hLabelID = (Hlong)ui.lbl_Window->winId();
		on_pBtn_FitImage_clicked();

		//��ʾģ��Сͼ
		if (ho_CropModelImg.Key() != nullptr)
		{
			DisplyModelImg(ho_CropModelImg);
		}
	}
	catch (...)
	{

	}

}

//��ʼ����ʾ����
void TemplateWindow::InitWindow()
{
	try
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

//��ʾͼ��
void TemplateWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
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
void TemplateWindow::GiveParameterToWindow()
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

//ͼ������Ӧ����
void TemplateWindow::on_pBtn_FitImage_clicked()
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
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	GiveParameterToWindow();
}
//��մ���
void TemplateWindow::on_btn_ClearWindow_clicked()
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

//*****************ģ��ƥ����ش���****************************************************************
//ROI��״ѡ��
void TemplateWindow::on_cmb_RoiShape_Template_activated(const QString &arg1)
{
	RoiShape = arg1;
}
//����	roi����
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
			//���Զ������������Կ�ʼ������ROI��״̬������Ҫ����¼�����
			DrawRectangle1(m_hHalconID, &RecRow1, &RecCol1, &RecRow2, &RecCol2);
			GenRectangle1(&OutRegion, RecRow1, RecCol1, RecRow2, RecCol2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "rectangle2")
		{
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&OutRegion, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "circle")
		{
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&OutRegion, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "ellipse")
		{
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&OutRegion, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == "polygon")
		{
			DrawRegion(&OutRegion, m_hHalconID);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else
		{
			QMessageBox::information(this, "��ʾ", "��ѡ��������״!", QMessageBox::Ok);
		}
	}
	catch (...)
	{
		GenEmptyObj(&OutRegion);
		OutRegion.Clear();
	}

}
//����ģ��ѵ������
void TemplateWindow::on_btn_DrawTrainRoi_Template_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "red");
	HObject TempRoi;
	DrawRoiFunc(TempRoi);
	if (TempRoi.Key() == nullptr)
	{
		QMessageBox::information(this, "��ʾ", "ROI����ʧ��!", QMessageBox::Ok);
		return;
	}
	ho_TemplateRegions.push_back(TempRoi);
}
//����ģ����������
void TemplateWindow::on_btn_DrawSearchRoi_Template_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "blue");
	HObject TempRoi;
	DrawRoiFunc(TempRoi);
	if (TempRoi.Key() == nullptr)
	{
		QMessageBox::information(this, "��ʾ", "ROI����ʧ��!", QMessageBox::Ok);
		return;
	}
	ho_SearchRegions.push_back(TempRoi);
}
//������ģ����
void TemplateWindow::on_btn_DrawShaddowRoi_Template_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	SetColor(m_hHalconID, "green");
	HObject TempRoi;
	DrawRoiFunc(TempRoi);
	if (TempRoi.Key() == nullptr)
	{
		QMessageBox::information(this, "��ʾ", "ROI����ʧ��!", QMessageBox::Ok);
		return;
	}
	ho_ShadowRegions.push_back(TempRoi);
}
//�ϲ�ģ������Roi
void TemplateWindow::on_btn_UnionRoi_Template_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		if (ho_TemplateRegions.isEmpty())
		{
			QMessageBox::information(this, "��ʾ", "ģ��ѵ������ROIΪ��!", QMessageBox::Ok);
			return;
		}
		else
		{
			SetColor(m_hHalconID, "red");
			SetLineWidth(m_hHalconID, 1);
			//ѵ������ϲ�
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

			//��ģ����ϲ�
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
				//������ʾͼ�θ�ֵ
				ConcatObj(ho_ShowObj, ShadowRoi_Template, &ho_ShowObj);
			}

			//�ٳ���ģ����
			if (!ho_ShadowRegions.isEmpty())
			{
				Difference(TrainRoi_Template, ShadowRoi_Template, &TrainRoi_Template);
			}
			//������ʾͼ�θ�ֵ
			ConcatObj(ho_ShowObj, TrainRoi_Template, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			
			//��ʾģ��Сͼ
			GenEmptyObj(&ho_CropModelImg);
			HObject imgReduce;
			ReduceDomain(ho_Image, TrainRoi_Template, &imgReduce);
			CropDomain(imgReduce, &ho_CropModelImg);
			DisplyModelImg(ho_CropModelImg);
			int countRoi = ho_TemplateRegions.count();
			//���vector�ڴ�
			ho_TemplateRegions.clear();
			ho_ShadowRegions.clear();
			QMessageBox::information(this, "��ʾ", "�ϲ�ROI�ɹ���ѵ����������Ϊ:" + QString::number(countRoi), QMessageBox::Ok);
		}
	}
	catch (...)
	{

	}
}
//���ģ������Roi
void TemplateWindow::on_btn_ClearRoi_Template_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		//������򼯺�
		ho_TemplateRegions.clear();
		ho_SearchRegions.clear();
		ho_ShadowRegions.clear();
		//����������
		GenEmptyObj(&TrainRoi_Template);
		GenEmptyObj(&SearchRoi_Template);
		GenEmptyObj(&ShadowRoi_Template);
		//��մ�����ʾ��ͼ�α���
		GenEmptyObj(&ho_ShowObj);
		//��ʾԭͼ
		showImg(ho_Image, "margin", "green");
	}
	catch (...)
	{

	}
}
//ѵ��ģ��
void TemplateWindow::on_btn_Train_Template_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		int ret = 1;
		//����ģ��
		//���ò���ģ������в���
		SetRunParams_Template(hv_RunParamsTemplate);
		ret = slot_CreateTemplate(hv_RunParamsTemplate);
		if (ret != 0)
		{
			QMessageBox::information(this, "��ʾ", "ģ��ѵ��ʧ��", QMessageBox::Ok);
			return;
		}
		QMessageBox::information(this, "��ʾ", "ģ��ѵ���ɹ�", QMessageBox::Ok);
		//*******************************************************************************************************
		////����ģ��
		//QDateTime startTime = QDateTime::currentDateTime();
		//ret = slot_FindTemplate();
		//QDateTime endTime = QDateTime::currentDateTime();
		//qint64 intervalTimeMS = startTime.msecsTo(endTime);

		////ˢ�½��
		//SetTableData_Template(TemplateDetect1->mResultParams, intervalTimeMS);
	}
	catch (...)
	{

	}
}
//����ģ��
void TemplateWindow::on_pb_Run_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		int ret = 1;
		//����ģ��
		QDateTime startTime = QDateTime::currentDateTime();
		ret = slot_FindTemplate();
		QDateTime endTime = QDateTime::currentDateTime();
		qint64 intervalTimeMS = startTime.msecsTo(endTime);

		//ˢ�½��
		SetTableData_Template(TemplateDetect1->mResultParams, intervalTimeMS);
	}
	catch (...)
	{

	}
}
//ģ���������
void TemplateWindow::on_pb_SaveData_clicked()
{
	//�ѱ��������Ϊ�Զ����ļ�����
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/Template_formulation";

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
	//���ò���ģ������в���
	SetRunParams_Template(hv_RunParamsTemplate);
	//����ģ��ƥ��ѵ�����������в���
	ret = dataIOC.WriteParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��", QMessageBox::Ok);
		return;
	}
	TemplateDetect1->configPath = ConfigPath;
	TemplateDetect1->XMLPath = XmlPath;
	TemplateDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//ģ�������ȡ
void TemplateWindow::on_pb_ReadData_clicked()
{
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/Template_formulation";

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
	//��ȡģ��ƥ�����
	errorCode = dataIOC.ReadParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "��ʾ", "������ȡʧ��", QMessageBox::Ok);
		return;
	}
	//�������µ�����
	UpdateTemplateDataWindow(hv_RunParamsTemplate);
}

//����ģ��ѵ�����������в�������
void TemplateWindow::UpdateTemplateDataWindow(const RunParamsStruct_Template &RunParams)
{
	try
	{
		//����ѵ������
		ModelTypeStruct_Template typeStructTemplate;
		if (RunParams.hv_MatchMethod == typeStructTemplate.None)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("������״������");
		}
		else if (RunParams.hv_MatchMethod == typeStructTemplate.Scaled)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("������״ͬ������");
		}
		else if (RunParams.hv_MatchMethod == typeStructTemplate.Aniso)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("������״�첽����");
		}
		else if (RunParams.hv_MatchMethod == typeStructTemplate.Ncc)
		{
			ui.cmb_MatchMethod_Train->setCurrentText("���ڻ����");
		}

		//�ж�tuple���������ǲ�������
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
		//ˢ��ģ��Сͼ
		if (ho_CropModelImg.Key() != nullptr)
		{
			DisplyModelImg(ho_CropModelImg);
		}

		//�������в���
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
		QMessageBox::information(this, "��ʾ", "����ˢ��ʧ��", QMessageBox::Ok);
		return;
	}
}
//����ģ��ṹ�������ֵ����
void TemplateWindow::SetRunParams_Template(RunParamsStruct_Template &runParams)
{
	try
	{
		ModelTypeStruct_Template typeStructTemplate;
		HTuple hv_typeTemp;

		if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "������״������")
		{
			hv_typeTemp = typeStructTemplate.None;
		}
		else if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "������״ͬ������")
		{
			hv_typeTemp = typeStructTemplate.Scaled;
		}
		else if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "������״�첽����")
		{
			hv_typeTemp = typeStructTemplate.Aniso;
		}
		else if (ui.cmb_MatchMethod_Train->currentText().toStdString() == "���ڻ����")
		{
			hv_typeTemp = typeStructTemplate.Ncc;
		}
		else
		{
			hv_typeTemp = typeStructTemplate.Scaled;
		}
		runParams.hv_MatchMethod = hv_typeTemp;   //����ģʽ:"none"������,"scaled"ͬ������,"aniso"�첽����,"auto"�Զ�

		if (ui.spb_NumLevels_Train->value() == -1)
		{
			runParams.hv_NumLevels = typeStructTemplate.Auto;  //����������("auto")
		}
		else
		{
			runParams.hv_NumLevels = ui.spb_NumLevels_Train->value();  //����������
		}
		runParams.hv_AngleStart = ui.dspb_AngleStart_Train->value();             //��ʼ�Ƕ�
		runParams.hv_AngleExtent = ui.dspb_AngleExtent_Train->value();            //�Ƕȷ�Χ
		if (ui.dspb_AngleStep_Train->value() == 0)
		{
			runParams.hv_AngleStep = typeStructTemplate.Auto;   //�ǶȲ���("auto")
		}
		else
		{
			runParams.hv_AngleStep = ui.dspb_AngleStep_Train->value();             //�ǶȲ���
		}
		runParams.hv_ScaleRMin = ui.dspb_ScaleRMin_Train->value();              //��С������
		runParams.hv_ScaleRMax = ui.dspb_ScaleRMax_Train->value();              //���������
		if (ui.dspb_ScaleRStep_Train->value() == 0)
		{
			runParams.hv_ScaleRStep = typeStructTemplate.Auto;     //�з������Ų���("auto")
		}
		else
		{
			runParams.hv_ScaleRStep = ui.dspb_ScaleRStep_Train->value();       //�з������Ų���
		}
		runParams.hv_ScaleCMin = ui.dspb_ScaleCMin_Train->value();              //��С������
		runParams.hv_ScaleCMax = ui.dspb_ScaleCMax_Train->value();              //���������
		if (ui.dspb_ScaleCStep_Train->value() == 0)
		{
			runParams.hv_ScaleCStep = typeStructTemplate.Auto;     //�з������Ų���("auto")
		}
		else
		{
			runParams.hv_ScaleCStep = ui.dspb_ScaleCStep_Train->value();       //�з������Ų���
		}
		runParams.hv_Optimization = HTuple(ui.cmb_Optimization_Train->currentText().toStdString().c_str());           //�Ż��㷨
		runParams.hv_Metric = HTuple(ui.cmb_Metric_Train->currentText().toStdString().c_str());                 //����/����
		if (ui.spb_Contrast_Train->value() == 0)
		{
			runParams.hv_Contrast = typeStructTemplate.Auto;               //�Աȶ�("auto")
		}
		else
		{
			runParams.hv_Contrast = ui.spb_Contrast_Train->value();               //�Աȶ�
		}

		if (ui.spb_MinContrast_Train->value() == 0)
		{
			runParams.hv_MinContrast = typeStructTemplate.Auto;       //��С�Աȶ�("auto")
		}
		else
		{
			runParams.hv_MinContrast = ui.spb_MinContrast_Train->value();   //��С�Աȶ�
		}
		runParams.ho_TrainRegion = TrainRoi_Template;				//ģ��ѵ������
		runParams.ho_CropModelImg = ho_CropModelImg;				//ģ��ѵ��Сͼ
		runParams.hv_MinScore = ui.dspb_MinScore_Run->value();               //��С����
		runParams.hv_NumMatches = ui.spb_NumMatches_Run->value();             //ƥ�������0���Զ�ѡ��100�����ƥ��100��
		runParams.hv_MaxOverlap = ui.dspb_MaxOverlap_Run->value();             //Ҫ�ҵ�ģ��ʵ��������ص�
		runParams.hv_SubPixel = HTuple(ui.cmb_SubPixel_Run->currentText().toStdString().c_str());               //�����ؾ���
		runParams.hv_Greediness = ui.dspb_Greediness_Run->value();             //̰��ϵ��
		runParams.hv_TimeOut = ui.spb_Timeout->value();		//��ʱʱ��

		//*******************************************************************************
		//��������ֵ
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
			//������ʾͼ�θ�ֵ
			ConcatObj(ho_ShowObj, SearchRoi_Template, &ho_ShowObj);
			runParams.ho_SearchRegion = SearchRoi_Template;
		}
		else
		{
			//���û�л�������������ȫͼ����
			GenEmptyObj(&SearchRoi_Template);
			SearchRoi_Template.Clear();
			GenEmptyObj(&runParams.ho_SearchRegion);
			runParams.ho_SearchRegion.Clear();
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "ģ���������ʧ��", QMessageBox::Ok);
		return;
	}
}

//��ʼ��ģ����ʾ���
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
			font.setBold(true);//����Ϊ����
			font.setPointSize(12);//���������С
			//hearderItem->setTextColor(Qt::red);//������ɫ
			hearderItem->setForeground(QBrush(Qt::red));//������ɫ
			hearderItem->setFont(font);//��������
			ui.tablewidget_Result_Template->setHorizontalHeaderItem(i, hearderItem);
		}
		//��ģ��ƥ�������ı�ͷ����¼����źŲ�
		connect(ui.tablewidget_Result_Template->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_TemplateData(int)));
	}
	catch (...)
	{

	}
}
//����ģ����ʾ�������
void TemplateWindow::SetTableData_Template(const ResultParamsStruct_Template &resultStruct, qint64 Ct)
{
	try
	{
		//ˢ��ʱ��
		getmessage("CT:" + QString::number(Ct) + "ms");
		//�ж���ģ����
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
		//ˢ��ͼ��
		showImg(ho_Image, "margin", "green");
		//ˢ�½�����
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
//����ģ��ۺ���
int TemplateWindow::slot_FindTemplate()
{
	try
	{
		int ret = 1;
		//XML��ȡ����
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML��ȡʧ�ܣ���Ĭ��ֵ
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
			//����һά�����в���
			ret = dataIOC.WriteParams_Template(ConfigPath, XmlPath, hv_RunParamsTemplate, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//�����㷨
		return TemplateDetect1->FindTemplate(ho_Image, hv_RunParamsTemplate, TemplateDetect1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//ѵ��ģ��ۺ���
int TemplateWindow::slot_CreateTemplate(RunParamsStruct_Template &runParams)
{
	try
	{
		//if (ConfigPath.empty() || XmlPath.empty())
		//{
		//	QMessageBox::information(this, "��ʾ", "���ȱ������", QMessageBox::Ok);
		//	return -1;
		//}
		int ret = 1;
		//����ģ��
		ret = TemplateDetect1->CreateTemplate(ho_Image, runParams);
		if (ret != 0)
		{
			return ret;
		}
		////����ģ��ƥ��ѵ�����������в���
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

//ѡ�񱣴�ģ�͵�·��
void TemplateWindow::on_pb_SelectModelPath_clicked()
{
	try
	{
		ui.lineEdit_ModelPath->clear();
		// ��ȡ�ļ���·��
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
		QMessageBox::information(this, "��ʾ", "ģ��洢·��ѡ��ʧ��", QMessageBox::Ok);
		return;
	}

}
//����ģ�͵�ָ��·��
void TemplateWindow::on_pb_SaveModelFile_clicked()
{
	try
	{
		if (ui.lineEdit_ModelName->text() == "")
		{
			QMessageBox::information(this, "��ʾ", "ģ�������ǿ�", QMessageBox::Ok);
			return;
		}
		if (ui.lineEdit_ModelPath->text() == "")
		{
			QMessageBox::information(this, "��ʾ", "ģ��洢·���ǿ�", QMessageBox::Ok);
			return;
		}
		if (ui.cmb_ModelType->currentText() == "������״")
		{
			string path = ui.lineEdit_ModelPath->text().trimmed().toStdString();
			string name = ui.lineEdit_ModelName->text().trimmed().toStdString();
			HTuple modelIDPath = HTuple(path.c_str()) + HTuple(name.c_str()) + ".shm";
			WriteShapeModel(hv_RunParamsTemplate.hv_ModelID, modelIDPath);
		}
		else if(ui.cmb_ModelType->currentText() == "���ڻ����")
		{
			string path = ui.lineEdit_ModelPath->text().trimmed().toStdString();
			string name = ui.lineEdit_ModelName->text().trimmed().toStdString();
			HTuple modelIDPath = HTuple(path.c_str()) + HTuple(name.c_str()) + ".ncm";
			WriteNccModel(hv_RunParamsTemplate.hv_ModelID, modelIDPath);
		}
		else
		{
			QMessageBox::information(this, "��ʾ", "ģ��洢���Ͳ�����", QMessageBox::Ok);
			return;
		}
		QMessageBox::information(this, "��ʾ", "ģ��洢�ɹ�", QMessageBox::Ok);
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "ģ��洢��ָ��·��ʧ��", QMessageBox::Ok);
		return;
	}
}
//�Ƿ����ø߼�����
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
//exe·��
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
//����һ������strOld
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
//ģ��ƥ�������ÿ��Cell����¼�
void TemplateWindow::on_tablewidget_Result_Template_cellClicked(int row, int column)
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
			//����ʾѡ������
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//ģ��ƥ��������ͷ�ĵ���¼��ۺ���
void TemplateWindow::slot_VerticalHeader_TemplateData(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Result_Template->currentRow();
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
			//����ʾѡ������
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//��ʾģ��ͼ
void TemplateWindow::DisplyModelImg(const HObject &image)
{
	try
	{
		if (image.Key() == nullptr)
		{
			return;
		}
		SetSystem("flush_graphic", "false");
		//�򿪴���
		OpenWindow(0, 0, (Hlong)ui.lbl_Window_ModelImg->width(), (Hlong)ui.lbl_Window_ModelImg->height(), (Hlong)ui.lbl_Window_ModelImg->winId(), "visible", "", &hv_WindowHandle_ModelImg);
		//��ȡͼ���С
		GetImageSize(image, &m_imgWidth_Crop, &m_imgHeight_Crop);
		//��ȡ����ϵ��
		TupleMin2(1.0 * ui.lbl_Window_ModelImg->width() / m_imgWidth_Crop, 1.0 * ui.lbl_Window_ModelImg->height() / m_imgHeight_Crop, &m_hvScaledRate_Crop);
		//����ͼ��
		ZoomImageFactor(image, &m_hResizedImg_Crop, m_hvScaledRate_Crop, m_hvScaledRate_Crop, "constant");
		//��ȡ���ź�Ĵ�С
		GetImageSize(m_hResizedImg_Crop, &m_scaledWidth_Crop, &m_scaledHeight_Crop);
		//�򿪴���
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
