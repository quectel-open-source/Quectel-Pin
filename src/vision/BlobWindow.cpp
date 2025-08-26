#include "BlobWindow.h"

BlobWindow::BlobWindow(QWidget *parent)
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
	BlobDetect1 = new BlobDetect();
	IniTableData_Blob();
	IniBlobTools();
	GenEmptyObj(&SearchRoi_Blob);
	GenRectangle1(&SearchRoi_Blob, 10, 10, 500, 500);

	//ע���ź��Զ������ͣ�����ʹ���źŲ۴����Զ�������
	qRegisterMetaType<ResultParamsStruct_Blob>("ResultParamsStruct_Blob");
	connect(this, SIGNAL(sig_BlobResults(ResultParamsStruct_Blob, qint64)), this, SLOT(slot_BlobResults(ResultParamsStruct_Blob, qint64)));

	////�����ļ���XML·��
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
	BlobWindow::showMinimized(); //��С��
}

void BlobWindow::on_toolButton_2_clicked() {
	if (BlobWindow::isMaximized()) {
		BlobWindow::showNormal();//��ԭ�¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		BlobWindow::showMaximized();//����¼�
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
		//��������󻯻���С��ʱҲ�����д���
		if (BlobWindow::isMaximized() || BlobWindow::isMinimized()) {
			return;
		}
		else {
			//���ڰ�ť֮����Ҫ�������ĵط������д���(����)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() || ui.cmb_Type_Binarization_Blob->underMouse() ||
				ui.lbl_Window->underMouse() || ui.pb_DrawRoi_Blob->underMouse() || ui.cmb_IsFollow->underMouse() || ui.cmb_OperatorType_Morphology_Blob->underMouse() || 
				ui.cmb_Polarity_Blob->underMouse() || ui.cmb_DestRegion->underMouse() || ui.cmb_FeaturesFilter_Blob->underMouse() ||
				ui.cmb_AndOr_Blob->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//BlobWindow::move(BlobWindow::mapToGlobal(event->pos() - whereismouse));//�ƶ�
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
void BlobWindow::InitWindow()
{
	//����halcon���ļ�·��Ϊutf8�������������
	SetSystem("filename_encoding", "utf8");
	//�������д��룬����������ƶ�ʱ������mouseMove����ʱ���Կ������ꡣ����ֻ������갴��ʱ�ſ��Կ�������
	ui.lbl_Window->setMouseTracking(true);
	//���ɿ�ͼ��
	GenEmptyObj(&ho_Image);
	GenEmptyObj(&ho_ShowObj);
	ho_Image.Clear();
	m_hHalconID = NULL;
	m_hLabelID = (Hlong)ui.lbl_Window->winId();
	ui.frame->hide();
}

//��ʾͼ��
void BlobWindow::showImg(const HObject &ho_Img, HTuple hv_FillType, HTuple hv_Color)
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
void BlobWindow::GiveParameterToWindow()
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
		QMessageBox::information(this, "��ʾ", "ͼ���ʧ��!", QMessageBox::Ok);
		return;
	}

}

//ͼ������Ӧ����
void BlobWindow::on_pBtn_FitImage_clicked()
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
	//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
	GiveParameterToWindow();
}
//��մ���
void BlobWindow::on_btn_ClearWindow_clicked()
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

//Blob�����̬ѧ��������
void BlobWindow::on_pb_AddOperatorType_Morphology_Blob_clicked()
{
	try
	{
		TypeStruct_RegionOperator_Blob mType;
		if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��������" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���θ�ʴ" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���ο�����" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���α�����" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ������νṹ��" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ������νṹ��")
		{
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��������")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Dilation_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���θ�ʴ")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Erosion_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���ο�����")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Opening_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���α�����")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Closing_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ������νṹ��")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_BottomHat_Rectangle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ������νṹ��")
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
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ������" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�θ�ʴ" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�ο�����" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�α�����" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ����Բ�νṹ��" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ����Բ�νṹ��")
		{
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ������")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Dilation_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�θ�ʴ")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Erosion_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�ο�����")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Opening_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�α�����")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Closing_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ����Բ�νṹ��")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_BottomHat_Circle);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ����Բ�νṹ��")
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
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "�׶�������")
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
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "�׶�ȫ�����" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "������ͨ��" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��������ϲ�" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "����")
		{
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "�׶�ȫ�����")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Fillup);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "������ͨ��")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Connection);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��������ϲ�")
			{
				hv_RunParamsBlob.hv_RegionOperator_Type.Append(mType.hv_Union1);
			}
			if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "����")
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
			QMessageBox::information(this, "��ʾ", "ѡ��ɸѡ��������Ϊ��!", QMessageBox::Ok);
			return;
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "�����̬ѧʧ��!", QMessageBox::Ok);
		return;
	}

}

//Blob�����̬ѧ��������
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
		QMessageBox::information(this, "��ʾ", "�����̬ѧʧ��", QMessageBox::Ok);
		return;
	}
}
//**********************************************************************************************************
//����	roi����
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
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 1)
		{
			HTuple RecRow, RecCol, RecPhi, RecLength1, RecLength2;
			DrawRectangle2(m_hHalconID, &RecRow, &RecCol, &RecPhi, &RecLength1, &RecLength2);
			GenRectangle2(&OutRegion, RecRow, RecCol, RecPhi, RecLength1, RecLength2);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 2)
		{
			HTuple CircleRow, CircleCol, CircleRadius;
			DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
			GenCircle(&OutRegion, CircleRow, CircleCol, CircleRadius);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 3)
		{
			HTuple EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12;
			DrawEllipse(m_hHalconID, &EllipseRow, &EllipseCol, &EllipsePhi, &EllipseRadius1, &EllipseRadius12);
			GenEllipse(&OutRegion, EllipseRow, EllipseCol, EllipsePhi, EllipseRadius1, EllipseRadius12);
			ConcatObj(ho_ShowObj, OutRegion, &ho_ShowObj);
			//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
			GiveParameterToWindow();
			DispObj(OutRegion, m_hHalconID);
		}
		else if (RoiShape == 4)
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
//����ROI
void BlobWindow::on_pb_DrawRoi_Blob_clicked()
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
		DrawRoiFunc(0, TempRoi);
		SearchRoi_Blob = TempRoi;
	}
	catch (...)
	{

	}

}
//���ROI
void BlobWindow::on_pb_ClearRoi_Blob_clicked()
{
	GenEmptyObj(&SearchRoi_Blob);
	SearchRoi_Blob.Clear();
}
//����Blob����
void BlobWindow::on_pb_Run_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}
	int ret = 1;
	//����
	QDateTime startTime = QDateTime::currentDateTime();
	ret = slot_BlobDetect();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	
	//���߳�ˢ�½�����
	QtConcurrent::run([=]()
	{
		emit sig_BlobResults(BlobDetect1->mResultParams, intervalTimeMS);
	});

}
//ˢ��Blob����ۺ���
void BlobWindow::slot_BlobResults(ResultParamsStruct_Blob ResultParams_Blob, qint64 Ct)
{
	try
	{
		getmessage("CT:" + QString::number(Ct) + "ms");
		//ˢ��ͼƬ
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
		//ˢ�½�����
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
//���ɸѡ����
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
		if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "���")
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
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "�Ƕ�")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Angle_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "Բ��")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Circularity_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "���ܶ�")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Compactness_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "͹��")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Convexity_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "���ζ�")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rectangularity_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "�߶�")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Height_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "���")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Width_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "��С���Բ�뾶")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_OuterRadius_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "��С��Ӿذ볤")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rect2Len1_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "��С��Ӿذ��")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rect2Len2_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else if (ui.cmb_FeaturesFilter_Blob->currentText().toStdString() == "��С��ӾؽǶ�")
		{
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Append(mType.hv_Rect2Angle_Filter);
			hv_RunParamsBlob.hv_MinValue_Filter.Append(ui.dspb_MinValue_FeaturesFilter_Blob->value());
			hv_RunParamsBlob.hv_MaxValue_Filter.Append(ui.dspb_MaxValue_FeaturesFilter_Blob->value());
		}
		else
		{
			QMessageBox::information(this, "��ʾ", "ѡ��ɸѡ��������Ϊ��!", QMessageBox::Ok);
			return;
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "���ɸѡ����ʧ��!", QMessageBox::Ok);
		return;
	}

}

//���ɸѡ����
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
		QMessageBox::information(this, "��ʾ", "���ɸѡ����ʧ��!", QMessageBox::Ok);
		return;
	}

}
//��ʼ��Blob������
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
			font.setBold(true);//����Ϊ����
			font.setPointSize(12);//���������С
			//hearderItem->setTextColor(Qt::red);//������ɫ
			hearderItem->setForeground(QBrush(Qt::red));//������ɫ
			hearderItem->setFont(font);//��������
			ui.tablewidget_Results_Blob->setHorizontalHeaderItem(i, hearderItem);
		}
		//��Blob������ı�ͷ����¼����źŲ�
		connect(ui.tablewidget_Results_Blob->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_VerticalHeader_Blob(int)));

		//��̬ѧ����Ҽ��˵��¼�
		ui.listWidget_OperatorTypes_Morphology_Blob->setContextMenuPolicy(Qt::CustomContextMenu);
		Action1_Morphology = new QAction("ɾ��", this);
		Menu_Morphology = new QMenu(this);
		Menu_Morphology->addAction(Action1_Morphology);
		//�����������źŲ�
		connect(Action1_Morphology, &QAction::triggered, [=]()
		{
			try
			{
				//��ȡ���Ƴ�������ֵ
				int row = ui.listWidget_OperatorTypes_Morphology_Blob->currentRow();
				if (row < 0)
				{
					QMessageBox::information(this, "��ʾ", "��ѡ���Ƴ���ѡ��", QMessageBox::Ok);
					return;
				}
				//��ȡ��Ҫ���Ƴ��������������
				HTuple strType = hv_RunParamsBlob.hv_RegionOperator_Type[row];
				//�Ƴ��ؼ��е�ָ��ѡ��
				QListWidgetItem *aItem = ui.listWidget_OperatorTypes_Morphology_Blob->takeItem(row);
				delete aItem;
				//�Ƴ����Ӧ������
				if (hv_RunParamsBlob.hv_RegionOperator_Type.TupleLength() > 0)
				{
					TupleRemove(hv_RunParamsBlob.hv_RegionOperator_Type, row, &hv_RunParamsBlob.hv_RegionOperator_Type);
				}
				//�Ƴ����Ӧ�����͵Ĳ���
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
				QMessageBox::information(this, "��ʾ", "ɾ����̬ѧʧ��", QMessageBox::Ok);
				return;
			}
		});
		connect(ui.listWidget_OperatorTypes_Morphology_Blob, &QListWidget::customContextMenuRequested, [=](const QPoint &pos)
		{
			Menu_Morphology->exec(QCursor::pos());
		});

		//ɸѡ����Ҽ��˵��¼�
		ui.listWidget_FeaturesFilter_Blob->setContextMenuPolicy(Qt::CustomContextMenu);
		Action1_Filter = new QAction("ɾ��", this);
		Menu_Filter = new QMenu(this);
		Menu_Filter->addAction(Action1_Filter);
		//�����������źŲ�
		connect(Action1_Filter, &QAction::triggered, [=]()
		{
			try
			{
				int row = ui.listWidget_FeaturesFilter_Blob->currentRow();
				if (row < 0)
				{
					QMessageBox::information(this, "��ʾ", "��ѡ���Ƴ���ѡ��", QMessageBox::Ok);
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
				QMessageBox::information(this, "��ʾ", "ɾ��ɸѡ����ʧ��!", QMessageBox::Ok);
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
//��ʼ��Blob���ڿؼ�
void BlobWindow::IniBlobTools()
{
	try
	{
		//��ֵ��
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
		//��̬ѧ
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
		//����ɸѡ
		ui.cmb_FeaturesFilter_Blob->setCurrentIndex(0);
	}
	catch (...)
	{
	}
}
//����Blob����
void BlobWindow::on_pb_SaveData_clicked()
{
	//�ѱ��������Ϊ�Զ����ļ�����
// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/Blob_formulation";

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
	//����Blob����
	SetRunParams_Blob(hv_RunParamsBlob);
	//�������в���
	ret = dataIOC.WriteParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��", QMessageBox::Ok);
		return;
	}
	BlobDetect1->configPath = ConfigPath;
	BlobDetect1->XMLPath = XmlPath;
	BlobDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//��ȡBlob����
void BlobWindow::on_pb_LoadData_clicked()
{
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/Blob_formulation";

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
	//��ȡֱ�߲���
	errorCode = dataIOC.ReadParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "��ʾ", "������ȡʧ��", QMessageBox::Ok);
		return;
	}
	//�������µ�����
	UpdateBlobDataWindow(hv_RunParamsBlob);

}
//Blob��ֵ������ѡ���¼�
void BlobWindow::on_cmb_Type_Binarization_Blob_currentTextChanged(const QString &arg1)
{
	try
	{
		TypeStruct_BinarizationDetect mType;
		if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "�̶���ֵ")
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
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "����Ӧ��ֵ")
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
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "��̬��ֵ")
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
//Blob��̬ѧ�����������ѡ���¼�
void BlobWindow::on_cmb_OperatorType_Morphology_Blob_activated(const QString &arg1)
{
	try
	{
		if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��������" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���θ�ʴ" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���ο�����" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "���α�����" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ������νṹ��" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ������νṹ��")
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
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ������" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�θ�ʴ" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�ο�����" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "Բ�α�����" ||
			ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ����Բ�νṹ��" || ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "��ñ����Բ�νṹ��")
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
		else if (ui.cmb_OperatorType_Morphology_Blob->currentText().toStdString() == "�׶�������")
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
//Blob������ÿ��Cell����¼�
void BlobWindow::on_tablewidget_Results_Blob_cellClicked(int row, int column)
{
	try
	{
		int SelectRow = row;
		if (SelectRow < 0)
		{
			return;
		}
		//ˢ�½������
		if (BlobDetect1->mResultParams.hv_RetNum > 0)
		{
			HObject ho_ObjTemp;
			SelectObj(BlobDetect1->mResultParams.ho_DestRegions, &ho_ObjTemp, SelectRow + 1);
			//����ʾ��������
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, BlobDetect1->mResultParams.ho_DestRegions, &ho_ShowObj);
			showImg(ho_Image, "fill", "green");//margin
			//����ʾѡ������
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//Blob�������ͷ�ĵ���¼��ۺ���
void BlobWindow::slot_VerticalHeader_Blob(int)
{
	try
	{
		int SelectRow = ui.tablewidget_Results_Blob->currentRow();
		if (SelectRow < 0)
		{
			return;
		}
		//ˢ�½������
		if (BlobDetect1->mResultParams.hv_RetNum > 0)
		{
			HObject ho_ObjTemp;
			SelectObj(BlobDetect1->mResultParams.ho_DestRegions, &ho_ObjTemp, SelectRow + 1);
			//����ʾ��������
			GenEmptyObj(&ho_ShowObj);
			ConcatObj(ho_ShowObj, BlobDetect1->mResultParams.ho_DestRegions, &ho_ShowObj);
			showImg(ho_Image, "fill", "green");//margin
			//����ʾѡ������
			SetColor(m_hHalconID, "red");
			DispObj(ho_ObjTemp, m_hHalconID);
		}
	}
	catch (...)
	{

	}

}
//����Blob������ֵ����
void BlobWindow::SetRunParams_Blob(RunParamsStruct_Blob &runParams)
{
	try
	{
		//�������򱣴�
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
		//********************************��ֵ��������ֵ*******************************************
		//��ֵ������
		TypeStruct_Binarization_Blob typeBinarization;
		HTuple hv_typeTemp;
		if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "�̶���ֵ")
		{
			hv_typeTemp = typeBinarization.Fixed_Threshold;
		}
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "����Ӧ��ֵ")
		{
			hv_typeTemp = typeBinarization.Auto_Threshold;
		}
		else if (ui.cmb_Type_Binarization_Blob->currentText().toStdString() == "��̬��ֵ")
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
		//���ֵ
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
		QMessageBox::information(this, "��ʾ", "��������ʧ��", QMessageBox::Ok);
		return;
	}

}
//���в������µ�����
void BlobWindow::UpdateBlobDataWindow(const RunParamsStruct_Blob &runParams)
{
	try
	{
		//**********************Blob��ֵ���������µ�����*************************************************************
		ui.cmb_Type_Binarization_Blob->setCurrentText(QString::fromUtf8(runParams.hv_ThresholdType.S().Text()));
		ui.spb_LowThreshold_Blob->setValue(runParams.hv_LowThreshold.TupleInt().I());
		ui.spb_HighThreshold_Blob->setValue(runParams.hv_HighThreshold.TupleInt().I());
		ui.dspb_Sigma_Blob->setValue(runParams.hv_Sigma.TupleReal().D());
		ui.dspb_DynThresholdWidth_Blob->setValue(runParams.hv_CoreWidth.TupleReal().D());
		ui.dspb_DynThresholdHeight_Blob->setValue(runParams.hv_CoreHeight.TupleReal().D());
		ui.spb_DynThresholdContrast_Blob->setValue(runParams.hv_DynThresholdContrast.TupleInt().I());
		ui.cmb_Polarity_Blob->setCurrentText(QString::fromUtf8(runParams.hv_DynThresholdPolarity.S().Text()));

		//******************************Blob��̬ѧ�������µ�����**********************************************************************
		//���������������
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

		//����ɸѡ��������
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
		//���
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
		QMessageBox::information(this, "��ʾ", "����ˢ��ʧ��", QMessageBox::Ok);
		return;
	}

}
//Blob���ۺ���
int BlobWindow::slot_BlobDetect()
{
	try
	{
		int ret = 1;
		//XML��ȡ����
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML��ȡʧ�ܣ���Ĭ��ֵ(ֻ����ֵ����ֵ)
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

			//�������������գ�ɸѡ�������
			hv_RunParamsBlob.hv_FeaturesFilter_Type.Clear();
			hv_RunParamsBlob.hv_RegionOperator_Type.Clear();

			//����Blob������в���
			ret = dataIOC.WriteParams_Blob(ConfigPath, XmlPath, hv_RunParamsBlob, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//�����㷨
		return BlobDetect1->BlobFunc(ho_Image, hv_RunParamsBlob, BlobDetect1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//exe·��
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
//����һ������strOld
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