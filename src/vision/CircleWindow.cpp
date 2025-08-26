#include "CircleWindow.h"

CircleWindow::CircleWindow(QWidget* parent)
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
	CircleDetect1 = new CircleDetect();
	//Բ���
	IniTableData_Circle();
	////�����ļ���XML·��
	//ConfigPath = FolderPath() + "/config/";
	//XmlPath = FolderPath() + "/config/Config.xml";
}

CircleWindow::CircleWindow(HObject image, int _processID, int modubleID, std::string _modubleName)
{
	CircleDetect1 = new CircleDetect();
	nodeName = _modubleName;
	processModbuleID = modubleID;
	processId = _processID;
	if (image.Key() != nullptr)
		ho_Image = image;
}

CircleWindow::~CircleWindow()
{
	//�ͷ�Բnew���ڴ�
	delete CircleDetect1;
}
void CircleWindow::on_toolButton_clicked() {
	CircleWindow::showMinimized(); //��С��
}

void CircleWindow::on_toolButton_2_clicked() {
	if (CircleWindow::isMaximized()) {
		CircleWindow::showNormal();//��ԭ�¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
	else {
		CircleWindow::showMaximized();//����¼�
		//ui.toolButton_2->setIcon(QIcon(":/img/max.png"));
	}
}

void CircleWindow::on_toolButton_3_clicked() {
	this->close();
}

void CircleWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		whereismouse = event->pos();
	}
}

void CircleWindow::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		//��������󻯻���С��ʱҲ�����д���
		if (CircleWindow::isMaximized() || CircleWindow::isMinimized()) {
			return;
		}
		else {
			//���ڰ�ť֮����Ҫ�������ĵط������д���(����)
			if (ui.toolButton->underMouse() || ui.toolButton_2->underMouse() || ui.toolButton_3->underMouse() ||
				ui.lbl_Window->underMouse() || ui.btn_DrawCircle_Circle->underMouse() || ui.cmb_IsFollow_Circle->underMouse() ||
				ui.cmb_MeasureInterpolation_Circle->underMouse() || ui.cmb_MeasureSelect_Circle->underMouse() || ui.cmb_MeasureTransition_Circle->underMouse()) {

			}
			else if (keypoint == 1) {

			}
			else {
				//CircleWindow::move(CircleWindow::mapToGlobal(event->pos() - whereismouse));//�ƶ�
			}
		}
	}
}
void CircleWindow::getmessage(QString value) {
	QString currenttime = (QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss");
	ui.plainTextEdit->appendPlainText(currenttime + ": " + value);
}

void CircleWindow::resizeEvent(QResizeEvent* event)
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
void CircleWindow::InitWindow()
{
	try
	{
		/*OpenWindow(0, 0, ui.lbl_Window->width(), ui.lbl_Window->height(), (Hlong)ui.lbl_Window->winId(), "", "", &hv_WindowHandle);
		ui.lbl_Window->installEventFilter(this);
		ui.lbl_Window->setMouseTracking(true);
		ROIControl = new ROIController(hv_WindowHandle, "MSA");
		ROIControl->View_MSA = this;
		widthImage = 0, heightImage = 0;*/

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
	catch (...)
	{

	}
}

//��ʾͼ��
void CircleWindow::showImg(const HObject& ho_Img, HTuple hv_FillType, HTuple hv_Color)
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
void CircleWindow::GiveParameterToWindow()
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
void CircleWindow::on_pBtn_ReadImage_clicked()
{
	try
	{
		QFileDialog dlg;
		dlg.setAcceptMode(QFileDialog::AcceptOpen);
		dlg.setFileMode(QFileDialog::ExistingFile);
		QString filePath = dlg.getOpenFileName();
		std::string str1 = filePath.toStdString();
		const char* str = str1.c_str();
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
void CircleWindow::on_pBtn_FitImage_clicked()
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
void CircleWindow::on_ckb_ShowObj_stateChanged(int arg1)
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
void CircleWindow::on_btn_ClearWindow_clicked()
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
//*****************Բ�����ش���**********************************************************************
//����Բ
void CircleWindow::on_btn_DrawCircle_Circle_clicked()
{
	try
	{
		if (ho_Image.Key() == nullptr)
		{
			//ͼ��Ϊ�գ�����ͼ��clear
			QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
			return;
		}
		ui.lbl_Window->SetActionEnable(false);
		SetColor(m_hHalconID, "red");
		SetLineWidth(m_hHalconID, 1);

		HObject ho_XldCircle;
		HTuple CircleRow, CircleCol, CircleRadius;
		DrawCircle(m_hHalconID, &CircleRow, &CircleCol, &CircleRadius);
		GenCircle(&ho_XldCircle, CircleRow, CircleCol, CircleRadius);
		ConcatObj(ho_ShowObj, ho_XldCircle, &ho_ShowObj);
		//������ˢ��ͼ�񴫵ݲ���(��Ҫ������Ϊ�˹�������ʱ��ʾͼ��)
		GiveParameterToWindow();
		DispObj(ho_XldCircle, m_hHalconID);

		//����������
		ShowArrow_Circle(m_hHalconID, CircleRow, CircleCol, CircleRadius);
		//����Բ������ֵ
		ui.dspb_CenterRow_Circle->setValue(CircleRow.TupleReal().D());
		ui.dspb_CenterCol_Circle->setValue(CircleCol.TupleReal().D());
		ui.dspb_CenterRadius_Circle->setValue(CircleRadius.TupleReal().D());
	}
	catch (...)
	{

	}
}
//���ԲROI
void CircleWindow::on_btn_ClearRoi_Circle_clicked()
{
	//��մ�����ʾ��ͼ�α���
	GenEmptyObj(&ho_ShowObj);
	//��ʾԭͼ
	showImg(ho_Image, "margin", "green");
}
//���Ƽ�ͷ
void CircleWindow::ShowArrow_Circle(HTuple windowId, HTuple CircleRow, HTuple CircleCol, HTuple CircleRadius)
{
	try
	{
		//��ʾ������������
		//��ͷ����ʼ������(���㷽ʽ����Բ��Ϊ��㣬�յ��������Բ��������ϰ뾶�������)
		HTuple hv_ArrowRow1, hv_ArrowCol1, hv_ArrowRow2, hv_ArrowCol2;
		hv_ArrowRow1 = CircleRow;
		hv_ArrowCol1 = CircleCol;
		hv_ArrowRow2 = CircleRow - CircleRadius;
		hv_ArrowCol2 = CircleCol;

		//��ʾ��������
		SetColor(m_hHalconID, "green");
		DispArrow(windowId, hv_ArrowRow1, hv_ArrowCol1, hv_ArrowRow2, hv_ArrowCol2, 5);
	}
	catch (const std::exception&)
	{

	}
}
//���в���Բ�ṹ�������ֵ����
void CircleWindow::SetRunParams_Circle(RunParamsStruct_Circle& runParams)
{
	try
	{
		//ץԲroi��ʼ��
		runParams.hv_Row = ui.dspb_CenterRow_Circle->value();
		runParams.hv_Column = ui.dspb_CenterCol_Circle->value();
		runParams.hv_Radius = ui.dspb_CenterRadius_Circle->value();

		//ץԲģ����Ҫ��ץԲ����
		runParams.hv_MeasureLength1 = ui.spb_MeasureLength1_Circle->value();		//���߰볤
		runParams.hv_MeasureLength2 = ui.spb_MeasureLength2_Circle->value();		//���߰��
		runParams.hv_MeasureSigma = ui.dspb_MeasureSigma_Circle->value();		//ƽ��ֵ 
		runParams.hv_MeasureThreshold = ui.spb_MeasureThreshold_Circle->value();	//��Ե��ֵ
		runParams.hv_MeasureTransition = HTuple(ui.cmb_MeasureTransition_Circle->currentText().toStdString().c_str());	//���߼���
		runParams.hv_MeasureSelect = HTuple(ui.cmb_MeasureSelect_Circle->currentText().toStdString().c_str());		//��Եѡ��
		runParams.hv_MeasureNum = ui.spb_MeasureNum_Circle->value();		    //���߸���
		runParams.hv_InstancesNum = 1;		//ץԲ����
		runParams.hv_MeasureMinScore = ui.dspb_MinScore_Circle->value();     //��С�÷�
		runParams.hv_DistanceThreshold = ui.dspb_DistanceThreshold_Circle->value();   //������ֵ
		runParams.hv_MeasureInterpolation = HTuple(ui.cmb_MeasureInterpolation_Circle->currentText().toStdString().c_str());//��ֵ�㷨
		runParams.hv_MmPixel = ui.dspb_MmPixel->value();
		runParams.hv_DectType = ui.ckb_DectType->isChecked() ? 1 : 0;	//Բ�������㷨ѡ��

		if (ui.cmb_IsFollow_Circle->currentText() == "Yes")
		{
			runParams.isFollow = true;
		}
		else
		{
			runParams.isFollow = false;
		}
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��!", QMessageBox::Ok);
		return;
	}
}
//���Բ
void CircleWindow::on_pb_Run_clicked()
{
	if (ho_Image.Key() == nullptr)
	{
		//ͼ��Ϊ�գ�����ͼ��clear
		QMessageBox::information(this, "��ʾ", "ͼ���ǿգ����ȼ���ͼ��!", QMessageBox::Ok);
		return;
	}

	//��Բ����
	QDateTime startTime = QDateTime::currentDateTime();
	slot_FindCircle();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 intervalTimeMS = startTime.msecsTo(endTime);
	//ˢ�½��
	SetTableData_Circle(CircleDetect1->mResultParams, intervalTimeMS);
}
//��ʼ��Բ�����ʾ���
void CircleWindow::IniTableData_Circle()
{
	try
	{
		std::vector<std::string> vec_Name = CircleDetect1->getModubleResultTitleList();
		QTableWidgetItem* hearderItem;
		ui.tablewidget_PointsResult_Circle->setColumnCount(vec_Name.size());
		for (int i = 0; i < ui.tablewidget_PointsResult_Circle->columnCount(); i++)
		{
			hearderItem = new QTableWidgetItem(QString::fromStdString(vec_Name[i]));
			QFont font = hearderItem->font();
			font.setBold(true);//����Ϊ����
			font.setPointSize(12);//���������С
			//hearderItem->setTextColor(Qt::red);//������ɫ
			hearderItem->setForeground(QBrush(Qt::red));//������ɫ
			hearderItem->setFont(font);//��������
			ui.tablewidget_PointsResult_Circle->setHorizontalHeaderItem(i, hearderItem);
		}
	}
	catch (...)
	{

	}
}
//����Բ�����ʾ�������
void CircleWindow::SetTableData_Circle(const ResultParamsStruct_Circle& resultParams, qint64 Ct)
{
	try
	{
		//ˢ��ʱ��
		getmessage("CT:" + QString::number(Ct) + "ms");
		ui.txt_Row1_Circle->clear();
		ui.txt_Col1_Circle->clear();
		ui.txt_Radius_Circle->clear();
		ui.txt_Roundness->clear();
		//ˢ��ͼ��
		GenEmptyObj(&ho_ShowObj);
		if (resultParams.hv_RetNum > 0)
		{
			ConcatObj(ho_ShowObj, resultParams.ho_Contour_Circle, &ho_ShowObj);
			ConcatObj(ho_ShowObj, resultParams.ho_Contour_Calipers, &ho_ShowObj);
			ConcatObj(ho_ShowObj, resultParams.ho_Contour_CrossPoints, &ho_ShowObj);
		}
		else
		{
			getmessage("NG");
		}
		showImg(ho_Image, "margin", "green");
		//ˢ�½�����
		std::vector<std::map<int, std::string>> resultList = CircleDetect1->getModubleResultList();
		int count = resultList.size();
		ui.tablewidget_PointsResult_Circle->clearContents();
		ui.tablewidget_PointsResult_Circle->setRowCount(count);
		for (int i = 0; i < count; i++)
		{
			QTableWidgetItem* item;
			for (std::map<int, std::string>::iterator iter = resultList[i].begin(); iter != resultList[i].end(); iter++)
			{
				item = new QTableWidgetItem(QString::fromStdString(iter->second), 1000);
				item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tablewidget_PointsResult_Circle->setItem(i, iter->first, item);
			}
		}
		if (resultParams.hv_RetNum > 0)
		{
			//ˢ��Բ������Ͱ뾶
			ui.txt_Row1_Circle->setText(QString::number(resultParams.hv_CircleRowCenter.D(), 'f', 2));
			ui.txt_Col1_Circle->setText(QString::number(resultParams.hv_CircleColumnCenter.D(), 'f', 2));
			ui.txt_Radius_Circle->setText(QString::number(resultParams.hv_CircleRadius.D(), 'f', 2));
			ui.txt_Roundness->setText(QString::number(resultParams.hv_Roundness.D(), 'f', 2));
		}
	}
	catch (const std::exception&)
	{

	}
}
//Բ��������
void CircleWindow::on_pb_SaveData_clicked()
{
	//�ѱ��������Ϊ�Զ����ļ�����
// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/Circle_formulation";

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
	//������Բ����
	SetRunParams_Circle(hv_RunParamsCircle);
	//����Բ���в���
	ret = dataIOC.WriteParams_Circle(ConfigPath, XmlPath, hv_RunParamsCircle, processId, nodeName, processModbuleID);
	if (ret != 0)
	{
		QMessageBox::information(this, "��ʾ", "��������ʧ��", QMessageBox::Ok);
		return;
	}
	CircleDetect1->configPath = ConfigPath;
	CircleDetect1->XMLPath = XmlPath;
	CircleDetect1->getRefreshConfig(processId, processModbuleID, nodeName);
}
//Բ������ȡ
void CircleWindow::on_pb_LoadData_clicked()
{
	// ��ȡ��ִ���ļ���·��
	QString exePath = QCoreApplication::applicationDirPath();

	// �ڿ�ִ���ļ�·���´����ļ���·��
	QString folderPath = exePath + "/Vision_formulation/Circle_formulation";

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
	//��ȡԲ����
	errorCode = dataIOC.ReadParams_Circle(ConfigPath, XmlPath, hv_RunParamsCircle, processId, nodeName, processModbuleID);
	if (errorCode != Ok_Xml)
	{
		QMessageBox::information(this, "��ʾ", "������ȡʧ��", QMessageBox::Ok);
		return;
	}
	//�������µ�����
	UpdateCircleDataWindow(hv_RunParamsCircle);
}
//����Բ������в���������
void CircleWindow::UpdateCircleDataWindow(const RunParamsStruct_Circle& RunParams)
{
	try
	{
		//ץԲroi��ʼ��
		ui.dspb_CenterRow_Circle->setValue(RunParams.hv_Row.TupleReal().D());
		ui.dspb_CenterCol_Circle->setValue(RunParams.hv_Column.TupleReal().D());
		ui.dspb_CenterRadius_Circle->setValue(RunParams.hv_Radius.TupleReal().D());
		//ץԲģ����Ҫ��ץ�߲���
		ui.spb_MeasureLength1_Circle->setValue(RunParams.hv_MeasureLength1.TupleInt().I());		//���߰볤
		ui.spb_MeasureLength2_Circle->setValue(RunParams.hv_MeasureLength2.TupleInt().I());		//���߰��
		ui.dspb_MeasureSigma_Circle->setValue(RunParams.hv_MeasureSigma.TupleReal().D());		//ƽ��ֵ 
		ui.spb_MeasureThreshold_Circle->setValue(RunParams.hv_MeasureThreshold.TupleInt().I());	//��Ե��ֵ
		ui.cmb_MeasureTransition_Circle->setCurrentText(QString::fromUtf8(RunParams.hv_MeasureTransition.S().Text()));	//���߼���
		ui.cmb_MeasureSelect_Circle->setCurrentText(QString::fromUtf8(RunParams.hv_MeasureSelect.S().Text()));		//��Եѡ��
		ui.spb_MeasureNum_Circle->setValue(RunParams.hv_MeasureNum.TupleInt().I());		    //���߸���
		ui.dspb_MinScore_Circle->setValue(RunParams.hv_MeasureMinScore.TupleReal().D());     //��С�÷�
		ui.dspb_DistanceThreshold_Circle->setValue(RunParams.hv_DistanceThreshold.TupleReal().D());   //������ֵ
		ui.dspb_MmPixel->setValue(RunParams.hv_MmPixel.TupleReal().D());
		ui.cmb_MeasureInterpolation_Circle->setCurrentText(QString::fromUtf8(RunParams.hv_MeasureInterpolation.S().Text()));//��ֵ�㷨
		if (RunParams.isFollow)
		{
			ui.cmb_IsFollow_Circle->setCurrentText("Yes");
		}
		else
		{
			ui.cmb_IsFollow_Circle->setCurrentText("No");
		}
		ui.ckb_DectType->setChecked(RunParams.hv_DectType.I() == 1 ? true : false);
	}
	catch (...)
	{
		QMessageBox::information(this, "��ʾ", "����ˢ��ʧ��", QMessageBox::Ok);
		return;
	}
}
//���Բ�ۺ���
int CircleWindow::slot_FindCircle()
{
	try
	{
		int ret = 1;
		//XML��ȡ����
		ErrorCode_Xml errorCode = dataIOC.ReadParams_Circle(ConfigPath, XmlPath, hv_RunParamsCircle, processId, nodeName, processModbuleID);
		if (errorCode != Ok_Xml)
		{
			//XML��ȡʧ�ܣ���Ĭ��ֵ
			hv_RunParamsCircle.hv_Row = 50;
			hv_RunParamsCircle.hv_Column = 50;
			hv_RunParamsCircle.hv_Radius = 50;
			//ץ��ģ����Ҫ��ץ�߲���
			hv_RunParamsCircle.hv_MeasureLength1 = 50;
			hv_RunParamsCircle.hv_MeasureLength2 = 5;
			hv_RunParamsCircle.hv_MeasureSigma = 1.0;
			hv_RunParamsCircle.hv_MeasureThreshold = 20;
			hv_RunParamsCircle.hv_MeasureTransition = "all";
			hv_RunParamsCircle.hv_MeasureSelect = "first";
			hv_RunParamsCircle.hv_MeasureNum = 10;
			hv_RunParamsCircle.hv_InstancesNum = 1;
			hv_RunParamsCircle.hv_MeasureMinScore = 0.5;
			hv_RunParamsCircle.hv_DistanceThreshold = 3.5;
			hv_RunParamsCircle.hv_MmPixel = 0.1;
			hv_RunParamsCircle.hv_MeasureInterpolation = "nearest_neighbor";
			hv_RunParamsCircle.isFollow = false;
			//����ֱ�߼�����в���
			ret = dataIOC.WriteParams_Circle(ConfigPath, XmlPath, hv_RunParamsCircle, processId, nodeName, processModbuleID);
			if (ret != 0)
			{
				return ret;
			}
		}
		//�����㷨
		return CircleDetect1->FindCircle(ho_Image, hv_RunParamsCircle, CircleDetect1->mResultParams);
	}
	catch (...)
	{
		return -1;
	}
}
//exe·��
string CircleWindow::FolderPath()
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
string CircleWindow::strReplaceAll(const string& strResource, const string& strOld, const string& strNew)
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
//****************************************************************************************************