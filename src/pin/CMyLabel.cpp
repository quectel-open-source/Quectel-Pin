#include "CMyLabel.h"

//定义单步放大倍率
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

//设置Halcon图像和Halcon窗口句柄，用户响应鼠标事件后实时更新图像
void CMyLabel::setHalconWnd(HObject img, HObject obj,bool isShowObj, HTuple hHalconID, QLabel *label)
{
	m_hHalconID = hHalconID;
	m_currentImg = img;
	m_currentObj = obj;
	m_label = label;
	m_isShowObj = isShowObj;
}

//鼠标滚轮缩放事件，用于缩放图像
void CMyLabel::wheelEvent(QWheelEvent *ev)
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	double Zoom;			//放大或缩小倍率
	HTuple  mouseRow, mouseCol, Button;
	HTuple startRowBf, startColBf, endRowBf, endColBf, Ht, Wt, startRowAft, startColAft, endRowAft, endColAft;

	//滚轮前滑，放大
	if (ev->angleDelta().y() > 0)
	{
		Zoom = ZOOMRATIO;
	}
	else
	{
		Zoom = 1 / ZOOMRATIO;
	}

	//获取光标在原图上的位置，注意是原图坐标，不是Label下的坐标
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

	//获取原图显示的部分，注意也是原图坐标
	GetPart(m_hHalconID, &startRowBf, &startColBf, &endRowBf, &endColBf);

	//缩放前显示的图像宽高
	Ht = endRowBf - startRowBf;
	Wt = endColBf - startColBf;

	//普通版halcon能处理的图像最大尺寸是32K*32K。如果无限缩小原图像，导致显示的图像超出限制，则会造成程序崩溃
	if ((Ht * Wt < 20000 * 20000 || Zoom == ZOOMRATIO))
	{
		//计算缩放后的图像区域
		startRowAft = mouseRow - ((mouseRow - startRowBf) / Zoom);
		startColAft = mouseCol - ((mouseCol - startColBf) / Zoom);

		endRowAft = startRowAft + (Ht / Zoom);
		endColAft = startColAft + (Wt / Zoom);

		//如果放大过大，则返回
		if (endRowAft - startRowAft < 2)
		{
			return;
		}

		if (m_hHalconID != NULL)
		{
			//如果有图像，则先清空图像
			DetachBackgroundFromWindow(m_hHalconID);
		}
		SetPart(m_hHalconID, startRowAft, startColAft, endRowAft, endColAft);
		//AttachBackgroundToWindow(m_currentImg, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(m_currentImg, m_hHalconID);
		//显示图形
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

//鼠标按下事件
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

	//鼠标按下时的行列坐标
	m_tMouseDownRow = mouseRow;
	m_tMouseDownCol = mouseCol;

	m_bIsMove = true;
}

//鼠标释放事件
void CMyLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	m_bIsMove = false;
}

//鼠标移动事件
void CMyLabel::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	HTuple startRowBf, startColBf, endRowBf, endColBf, mouseRow, mouseCol, Button;

	//获取当前鼠标在原图的位置
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


	//鼠标按下并移动时，移动图像，否则只显示坐标
	//暂时屏蔽鼠标拖动图像，为了方便在Hwindow上绘制区域
	if (m_bIsMove)
	{
		if (m_bMoveEnable)
		{
			//计算移动值
			double RowMove = mouseRow.TupleReal().D() - m_tMouseDownRow.TupleReal().D();
			double ColMove = mouseCol.TupleReal().D() - m_tMouseDownCol.TupleReal().D();

			//得到当前的窗口坐标
			GetPart(m_hHalconID, &startRowBf, &startColBf, &endRowBf, &endColBf);

			//移动图像
			if (m_hHalconID != NULL)
			{
				//如果有图像，则先清空图像
				DetachBackgroundFromWindow(m_hHalconID);
			}
			SetPart(m_hHalconID, startRowBf - RowMove, startColBf - ColMove, endRowBf - RowMove, endColBf - ColMove);
			//AttachBackgroundToWindow(m_currentImg, m_hHalconID);
			ClearWindow(m_hHalconID);
			SetSystem("flush_graphic", "true");
			DispObj(m_currentImg, m_hHalconID);
			//显示图形
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

	//获取灰度值
	HTuple pointGray;
	SetCheck("give_error");
	try
	{
		GetGrayval(m_currentImg, mouseRow, mouseCol, &pointGray);
	}
	catch (...)
	{
		m_label->setText(QString::fromLocal8Bit("X坐标：-    Y坐标：-    灰度值：-"));

		return;
	}
	SetCheck("~give_error");
	if (pointGray.TupleLength().I() == 1)
	{
		//设置坐标
		m_label->setText(QString::fromLocal8Bit("X坐标：%1    Y坐标：%2    灰度值：%3").arg(mouseCol[0].D()).arg(mouseRow[0].D()).arg(pointGray[0].D()));
	}
	if (pointGray.TupleLength().I() == 3)
	{
		//设置坐标
		m_label->setText(QString::fromLocal8Bit("X坐标：%1    Y坐标：%2    灰度值：%3 %4 %5").arg(mouseCol[0].D()).arg(mouseRow[0].D()).arg(pointGray[0].D()).arg(pointGray[1].D()).arg(pointGray[2].D()));
	}

}

//鼠标双击事件
void CMyLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
	SetActionEnable(true);
}
//设置右键菜单是否启用
void CMyLabel::SetActionEnable(bool flag)
{
	m_bIsAction = flag;
}
//图像自适应
void CMyLabel::slot_ShowImgFit()
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	SetCheck("give_error");
	try
	{
		//判断窗口句柄
		if (m_hHalconID != NULL)
		{
			//如果有图像，则先清空图像
			DetachBackgroundFromWindow(m_hHalconID);
		}

		//设置图形显示属性
		SetDraw(m_hHalconID, "margin");
		SetColor(m_hHalconID, "green");
		SetLineWidth(m_hHalconID, 1);


		//获取图像大小
		HTuple m_imgWidth, m_imgHeight;
		GetImageSize(m_currentImg, &m_imgWidth, &m_imgHeight);
		//获取缩放系数
		HTuple m_hvScaledRate;
		TupleMin2(1.0 * this->width() / m_imgWidth, 1.0 * this->height() / m_imgHeight, &m_hvScaledRate);
		//缩放图像
		HObject m_hResizedImg;
		ZoomImageFactor(m_currentImg, &m_hResizedImg, m_hvScaledRate, m_hvScaledRate, "constant");
		//获取缩放后的大小
		HTuple m_scaledWidth, m_scaledHeight;
		GetImageSize(m_hResizedImg, &m_scaledWidth, &m_scaledHeight);
		//打开窗口
		if (1.0 * this->width() / m_imgWidth < 1.0 * this->height() / m_imgHeight)
		{
			SetWindowExtents(m_hHalconID, this->height() / 2.0 - m_scaledHeight / 2.0, 0, this->width(), m_scaledHeight);
		}
		else
		{
			SetWindowExtents(m_hHalconID, 0, this->width() / 2.0 - m_scaledWidth / 2.0, m_scaledWidth, this->height());

		}
		SetPart(m_hHalconID, 0, 0, m_imgHeight - 1, m_imgWidth - 1);
		//这句显示图像，不会闪烁(把图像设置成窗口的背景图)
		//AttachBackgroundToWindow(m_currentImg, m_hHalconID);
		ClearWindow(m_hHalconID);
		SetSystem("flush_graphic", "true");
		DispObj(m_currentImg, m_hHalconID);

		//显示图形
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
//保存图像
void CMyLabel::slot_SaveImg()
{
	if (m_currentImg.Key() == nullptr)
	{
		return;
	}
	//获取图片保存路径
	QString file_path = QFileDialog::getSaveFileName(this, "save file", "C:\\", "bmp files (*.bmp);;jpeg files(*.jpeg);;png files(*.png);;tiff files(*.tiff)");
	if (file_path.size() <= 0)
	{
		return;
	}
	//根据类型存图
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
//顺时针旋转
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
//逆时针旋转
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
//启用拖图
void CMyLabel::slot_EnableMove()
{
	m_bMoveEnable = true;
}
//禁用拖图
void CMyLabel::slot_UnEnableMove()
{
	m_bMoveEnable = false;
}
void CMyLabel::createAction()
{
	//创建触发事件
	m_fitImgAction = new QAction(QStringLiteral("自适应"), this);
	connect(m_fitImgAction, SIGNAL(triggered()), this, SLOT(slot_ShowImgFit()));

	m_saveImgAction = new QAction(QStringLiteral("保存图片"), this);
	connect(m_saveImgAction, SIGNAL(triggered()), this, SLOT(slot_SaveImg()));

	m_rotateImgAction = new QAction(QStringLiteral("顺时针旋转"), this);
	connect(m_rotateImgAction, SIGNAL(triggered()), this, SLOT(slot_RotateImg()));

	m_rotateImgReverseAction = new QAction(QStringLiteral("逆时针旋转"), this);
	connect(m_rotateImgReverseAction, SIGNAL(triggered()), this, SLOT(slot_RotateReverseImg()));

	m_EnaleMove = new QAction(QStringLiteral("启用拖图"), this);
	connect(m_EnaleMove, SIGNAL(triggered()), this, SLOT(slot_EnableMove()));

	m_UnEnaleMove = new QAction(QStringLiteral("禁用拖图"), this);
	connect(m_UnEnaleMove, SIGNAL(triggered()), this, SLOT(slot_UnEnableMove()));


	//事件添加到菜单
	m_menu->addAction(m_fitImgAction);
	m_menu->addSeparator();//添加分隔符
	m_menu->addAction(m_saveImgAction);
	m_menu->addSeparator();//添加分隔符
	m_menu->addAction(m_rotateImgAction);
	m_menu->addSeparator();//添加分隔符
	m_menu->addAction(m_rotateImgReverseAction);
	m_menu->addSeparator();//添加分隔符
	m_menu->addAction(m_EnaleMove);
	m_menu->addSeparator();//添加分隔符
	m_menu->addAction(m_UnEnaleMove);
}
void CMyLabel::createMenu()
{
	//创建右键菜单
	m_menu = new QMenu(this);
	m_menu->setStyleSheet("QMenu{background-color:rgb(255,255,255);color:rgb(0, 0, 0);font:11pt ""微软雅黑"";}");


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
