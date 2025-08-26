#include "CMyLabel.h"

//���嵥���Ŵ���
#define ZOOMRATIO 2.0

CMyLabel::CMyLabel(QWidget *parent)
	: QLabel(parent)
{
	m_bIsMove = false;
	createMenu();
	createAction();
	createContextMenu();
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuSlot(QPoint)));
}

CMyLabel::~CMyLabel()
{
}

//����Halconͼ���Halcon���ھ�����û���Ӧ����¼���ʵʱ����ͼ��
void CMyLabel::setHalconWnd(HObject img, HObject obj,bool isShowObj, HTuple hHalconID, QLabel *label)
{
	m_hHalconID = hHalconID;
	m_currentImg = img;
	m_currentObj = obj;
	m_label = label;
	m_isShowObj = isShowObj;
}

//�����������¼�����������ͼ��
void CMyLabel::wheelEvent(QWheelEvent *ev)
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	double Zoom;			//�Ŵ����С����
	HTuple  mouseRow, mouseCol, Button;
	HTuple startRowBf, startColBf, endRowBf, endColBf, Ht, Wt, startRowAft, startColAft, endRowAft, endColAft;

	//����ǰ�����Ŵ�
	if (ev->angleDelta().y() > 0)
	{
		Zoom = ZOOMRATIO;
	}
	else
	{
		Zoom = 1 / ZOOMRATIO;
	}

	//��ȡ�����ԭͼ�ϵ�λ�ã�ע����ԭͼ���꣬����Label�µ�����
	SetCheck("give_error");
	try
	{
		GetMposition(m_hHalconID, &mouseRow, &mouseCol, &Button);

	}
	catch (...)
	{
		return;
	}
	SetCheck("~give_error");

	//��ȡԭͼ��ʾ�Ĳ��֣�ע��Ҳ��ԭͼ����
	GetPart(m_hHalconID, &startRowBf, &startColBf, &endRowBf, &endColBf);

	//����ǰ��ʾ��ͼ����
	Ht = endRowBf - startRowBf;
	Wt = endColBf - startColBf;

	//��ͨ��halcon�ܴ����ͼ�����ߴ���32K*32K�����������Сԭͼ�񣬵�����ʾ��ͼ�񳬳����ƣ������ɳ������
	if ((Ht * Wt < 20000 * 20000 || Zoom == ZOOMRATIO))
	{
		//�������ź��ͼ������
		startRowAft = mouseRow - ((mouseRow - startRowBf) / Zoom);
		startColAft = mouseCol - ((mouseCol - startColBf) / Zoom);

		endRowAft = startRowAft + (Ht / Zoom);
		endColAft = startColAft + (Wt / Zoom);

		//����Ŵ�����򷵻�
		if (endRowAft - startRowAft < 2)
		{
			return;
		}

		if (m_hHalconID != NULL)
		{
			//�����ͼ���������ͼ��
			DetachBackgroundFromWindow(m_hHalconID);
		}
		SetPart(m_hHalconID, startRowAft, startColAft, endRowAft, endColAft);
		//AttachBackgroundToWindow(m_currentImg, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(m_currentImg, m_hHalconID);
		//��ʾͼ��
		if (m_isShowObj)
		{
			SetCheck("give_error");
			try
			{
				DispObj(m_currentObj, m_hHalconID);
			}
			catch (...)
			{
				return;
			}
			SetCheck("~give_error");
		}
	}
}

//��갴���¼�
void CMyLabel::mousePressEvent(QMouseEvent *ev)
{
	HTuple mouseRow, mouseCol, Button;
	SetCheck("give_error");
	try
	{
		GetMposition(m_hHalconID, &mouseRow, &mouseCol, &Button);
	}
	catch (...)
	{
		return;
	}
	SetCheck("~give_error");

	//��갴��ʱ����������
	m_tMouseDownRow = mouseRow;
	m_tMouseDownCol = mouseCol;

	m_bIsMove = true;
}

//����ͷ��¼�
void CMyLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	m_bIsMove = false;
}

//����ƶ��¼�
void CMyLabel::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	HTuple startRowBf, startColBf, endRowBf, endColBf, mouseRow, mouseCol, Button;

	//��ȡ��ǰ�����ԭͼ��λ��
	SetCheck("give_error");
	try
	{
		GetMposition(m_hHalconID, &mouseRow, &mouseCol, &Button);
		//qDebug()<< "row:"<<
	}
	catch (...)
	{
		return;
	}
	SetCheck("~give_error");


	//��갴�²��ƶ�ʱ���ƶ�ͼ�񣬷���ֻ��ʾ����
	//��ʱ��������϶�ͼ��Ϊ�˷�����Hwindow�ϻ�������
	if (m_bIsMove)
	{
		if (m_bMoveEnable)
		{
			//�����ƶ�ֵ
			double RowMove = mouseRow.TupleReal().D() - m_tMouseDownRow.TupleReal().D();
			double ColMove = mouseCol.TupleReal().D() - m_tMouseDownCol.TupleReal().D();

			//�õ���ǰ�Ĵ�������
			GetPart(m_hHalconID, &startRowBf, &startColBf, &endRowBf, &endColBf);

			//�ƶ�ͼ��
			if (m_hHalconID != NULL)
			{
				//�����ͼ���������ͼ��
				DetachBackgroundFromWindow(m_hHalconID);
			}
			SetPart(m_hHalconID, startRowBf - RowMove, startColBf - ColMove, endRowBf - RowMove, endColBf - ColMove);
			//AttachBackgroundToWindow(m_currentImg, m_hHalconID);
			ClearWindow(m_hHalconID);
			SetSystem("flush_graphic", "true");
			DispObj(m_currentImg, m_hHalconID);
			//��ʾͼ��
			if (m_isShowObj)
			{
				SetCheck("give_error");
				try
				{
					DispObj(m_currentObj, m_hHalconID);
				}
				catch (...)
				{
					return;
				}
				SetCheck("~give_error");
			}
		}
	}

	//��ȡ�Ҷ�ֵ
	HTuple pointGray;
	SetCheck("give_error");
	try
	{
		GetGrayval(m_currentImg, mouseRow, mouseCol, &pointGray);
	}
	catch (...)
	{
		m_label->setText(QString::fromLocal8Bit("X���꣺-    Y���꣺-    �Ҷ�ֵ��-"));

		return;
	}
	SetCheck("~give_error");
	if (pointGray.TupleLength().I() == 1)
	{
		//��������
		m_label->setText(QString::fromLocal8Bit("X���꣺%1    Y���꣺%2    �Ҷ�ֵ��%3").arg(mouseCol[0].D()).arg(mouseRow[0].D()).arg(pointGray[0].D()));
	}
	if (pointGray.TupleLength().I() == 3)
	{
		//��������
		m_label->setText(QString::fromLocal8Bit("X���꣺%1    Y���꣺%2    �Ҷ�ֵ��%3 %4 %5").arg(mouseCol[0].D()).arg(mouseRow[0].D()).arg(pointGray[0].D()).arg(pointGray[1].D()).arg(pointGray[2].D()));
	}

}

//���˫���¼�
void CMyLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
	SetActionEnable(true);
}
//�����Ҽ��˵��Ƿ�����
void CMyLabel::SetActionEnable(bool flag)
{
	m_bIsAction = flag;
}
//ͼ������Ӧ
void CMyLabel::slot_ShowImgFit()
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	SetCheck("give_error");
	try
	{
		//�жϴ��ھ��
		if (m_hHalconID != NULL)
		{
			//�����ͼ���������ͼ��
			DetachBackgroundFromWindow(m_hHalconID);
		}

		//����ͼ����ʾ����
		SetDraw(m_hHalconID, "margin");
		SetColor(m_hHalconID, "green");
		SetLineWidth(m_hHalconID, 1);


		//��ȡͼ���С
		HTuple m_imgWidth, m_imgHeight;
		GetImageSize(m_currentImg, &m_imgWidth, &m_imgHeight);
		//��ȡ����ϵ��
		HTuple m_hvScaledRate;
		TupleMin2(1.0 * this->width() / m_imgWidth, 1.0 * this->height() / m_imgHeight, &m_hvScaledRate);
		//����ͼ��
		HObject m_hResizedImg;
		ZoomImageFactor(m_currentImg, &m_hResizedImg, m_hvScaledRate, m_hvScaledRate, "constant");
		//��ȡ���ź�Ĵ�С
		HTuple m_scaledWidth, m_scaledHeight;
		GetImageSize(m_hResizedImg, &m_scaledWidth, &m_scaledHeight);
		//�򿪴���
		if (1.0 * this->width() / m_imgWidth < 1.0 * this->height() / m_imgHeight)
		{
			SetWindowExtents(m_hHalconID, this->height() / 2.0 - m_scaledHeight / 2.0, 0, this->width(), m_scaledHeight);
		}
		else
		{
			SetWindowExtents(m_hHalconID, 0, this->width() / 2.0 - m_scaledWidth / 2.0, m_scaledWidth, this->height());

		}
		SetPart(m_hHalconID, 0, 0, m_imgHeight - 1, m_imgWidth - 1);
		//�����ʾͼ�񣬲�����˸(��ͼ�����óɴ��ڵı���ͼ)
		//AttachBackgroundToWindow(m_currentImg, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(m_currentImg, m_hHalconID);

		//��ʾͼ��
		if (m_isShowObj)
		{
			SetCheck("give_error");
			try
			{
				DispObj(m_currentObj, m_hHalconID);
			}
			catch (...)
			{
				return;
			}
			SetCheck("~give_error");
		}
	}
	catch (...)
	{

	}
	SetCheck("~give_error");
}
//����ͼ��
void CMyLabel::slot_SaveImg()
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	//��ȡͼƬ����·��
	QString file_path = QFileDialog::getSaveFileName(this, "save file", "C:\\", "bmp files (*.bmp);;jpeg files(*.jpeg);;png files(*.png);;tiff files(*.tiff)");
	if (file_path.size() <= 0)
	{
		return;
	}
	//�������ʹ�ͼ
	QStringList list = file_path.split(".");
	if (list.size() > 0)
	{
		std::string hv_SaveType = list[list.size() - 1].toStdString();
		std::string FileName = file_path.toStdString();
		HTuple hv_Type;
		GetImageType(m_currentImg, &hv_Type);
		if (hv_Type != "byte")
		{
			ScaleImageMax(m_currentImg, &m_currentImg);
		}
		WriteImage(m_currentImg, HTuple(hv_SaveType.c_str()), 0, HTuple(FileName.c_str()));
	}
}
//˳ʱ����ת
void CMyLabel::slot_RotateImg()
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	HObject ho_RotateImg;
	RotateImage(m_currentImg, &ho_RotateImg, -90, "constant");
	m_currentImg.Clear();
	m_currentImg = ho_RotateImg;
	slot_ShowImgFit();
}
//��ʱ����ת
void CMyLabel::slot_RotateReverseImg()
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	HObject ho_RotateImg;
	RotateImage(m_currentImg, &ho_RotateImg, 90, "constant");
	m_currentImg.Clear();
	m_currentImg = ho_RotateImg;
	slot_ShowImgFit();
}
//������ͼ
void CMyLabel::slot_EnableMove()
{
	m_bMoveEnable = true;
}
//������ͼ
void CMyLabel::slot_UnEnableMove()
{
	m_bMoveEnable = false;
}
void CMyLabel::createAction()
{
	//���������¼�
	m_fitImgAction = new QAction(QStringLiteral("����Ӧ"), this);
	connect(m_fitImgAction, SIGNAL(triggered()), this, SLOT(slot_ShowImgFit()));

	m_saveImgAction = new QAction(QStringLiteral("����ͼƬ"), this);
	connect(m_saveImgAction, SIGNAL(triggered()), this, SLOT(slot_SaveImg()));

	m_rotateImgAction = new QAction(QStringLiteral("˳ʱ����ת"), this);
	connect(m_rotateImgAction, SIGNAL(triggered()), this, SLOT(slot_RotateImg()));

	m_rotateImgReverseAction = new QAction(QStringLiteral("��ʱ����ת"), this);
	connect(m_rotateImgReverseAction, SIGNAL(triggered()), this, SLOT(slot_RotateReverseImg()));

	m_EnaleMove = new QAction(QStringLiteral("������ͼ"), this);
	connect(m_EnaleMove, SIGNAL(triggered()), this, SLOT(slot_EnableMove()));

	m_UnEnaleMove = new QAction(QStringLiteral("������ͼ"), this);
	connect(m_UnEnaleMove, SIGNAL(triggered()), this, SLOT(slot_UnEnableMove()));


	//�¼���ӵ��˵�
	m_menu->addAction(m_fitImgAction);
	m_menu->addSeparator();//��ӷָ���
	m_menu->addAction(m_saveImgAction);
	m_menu->addSeparator();//��ӷָ���
	m_menu->addAction(m_rotateImgAction);
	m_menu->addSeparator();//��ӷָ���
	m_menu->addAction(m_rotateImgReverseAction);
	m_menu->addSeparator();//��ӷָ���
	m_menu->addAction(m_EnaleMove);
	m_menu->addSeparator();//��ӷָ���
	m_menu->addAction(m_UnEnaleMove);
}
void CMyLabel::createMenu()
{
	//�����Ҽ��˵�
	m_menu = new QMenu(this);
	m_menu->setStyleSheet("QMenu{background-color:rgb(255,255,255);color:rgb(0, 0, 0);font:11pt ""΢���ź�"";}");


}
void CMyLabel::createContextMenu()
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
}
void CMyLabel::contextMenuSlot(QPoint p)
{
	if (m_bIsAction)
	{
		m_menu->exec(this->mapToGlobal(p));
	}
}
