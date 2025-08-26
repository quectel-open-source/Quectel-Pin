﻿#include "QGraphicsViews.h"
#include <QScrollBar>
#include <QMenu>
#include "ControlItem.h"
#include "BaseItem.h"
#include <QFileDialog>
#include <QAction>
#include <QLabel>
#include "QGraphicsScenes.h"
#include <QHBoxLayout>
#include <QThread>
#include <QPushButton>
#include <QWidgetAction>
#include "ImageItem.h"
#include "qmutex.h"

QGraphicsViews::QGraphicsViews(QWidget* parent) : QGraphicsView(parent)
{
	this->installEventFilter(this);//安装事件过滤器
	this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//解决拖动是背景图片残影
	setDragMode(QGraphicsView::ScrollHandDrag);
	drawBg();
	// 隐藏水平/竖直滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//this->setBackgroundBrush(Qt::gray);
	// 设置场景范围
	setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
	// 反锯齿
	//setRenderHints(QPainter::Antialiasing);
	item = new ImageItem;
	scene = new QGraphicsScenes;
	scene->addItem(item);
	this->setScene(scene);
	drawItem = new DrawItem;
	AddItems(drawItem);
	//GrayValue = new QLabel(); //显示灰度值
	//GrayValue->setObjectName("GrayValue_X");
	//GrayValue->setStyleSheet("color:rgb(200,255,200); background-color:rgba(50,50,50,160); font: 微软雅黑;font-size: 15px;");
	//GrayValue->setVisible(false);
	//GrayValue->setFixedWidth(700);
	////显示区域
	//fWidget = new QWidget(this);
	//fWidget->setFixedHeight(25);
	////fWidget->setGeometry(2, 538, 702, 25);
	//fWidget->setStyleSheet("background-color:rgba(0,0,0,0);");
	//auto fLayout = new QHBoxLayout(fWidget);
	//fLayout->setSpacing(0);
	//fLayout->setMargin(0);
	//fLayout->addWidget(GrayValue);
	//fLayout->addStretch();
	
	//connect(item, SIGNAL(RGBValue(QString)), this, SLOT(slot_RGBValue(QString)));
	//SetType(false, false);
}

QGraphicsViews::~QGraphicsViews()
{
	// 1. 释放场景对象及其管理的所有items
	if (scene) {
		// 先移除场景中的所有项目，避免双重删除
		scene->clear();
		delete scene;
		scene = nullptr;
	}

	//// 2. 释放自定义绘图项
	//if (drawItem) {
	//	// 如果drawItem是场景的子项，scene->clear()已经删除它
	//	// 所以这里只处理非场景子项的情况
	//	if (!drawItem->parentItem() && !drawItem->scene()) {
	//		delete drawItem;
	//	}
	//	drawItem = nullptr;
	//}

	//// 3. 释放图像项目
	//if (item) {
	//	// 确保不会双重删除
	//	if (!item->scene()) {
	//		delete item;
	//	}
	//	item = nullptr;
	//}

	//// 4. 清理父窗口引用（不删除，因为所有权在父窗口）
	//fWidget = nullptr;

	//// 5. 删除灰度值标签（如果创建）
	//if (GrayValue) {
	//	delete GrayValue;
	//	GrayValue = nullptr;
	//}

	// 6. 其他资源清理
	image = QPixmap(); // 清除QPixmap资源
	Image = QImage();  // 清除QImage资源
	bgPix = QPixmap(); // 清除背景图片资源
}

void QGraphicsViews::slot_RGBValue(QString InfoVal)
{
	GrayValue->setText(InfoVal);
}
bool QGraphicsViews::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == this)//当事件发生在u1（为Qlabel型）控件上
	{
		if (event->type() == QEvent::MouseButtonDblClick)//当为双击事件时
		{
			clickcount++;
			if (clickcount % 2 == 0) //此处为双击一次全屏，再双击一次退出
			{
				this->setWindowFlags(Qt::Dialog);
				this->showFullScreen();//全屏显示
			}
			else
			{
				this->setWindowFlags(Qt::SubWindow);
				this->showNormal();//退出全屏
			};
		}
		return QObject::eventFilter(obj, event);
	}
	return true;
}

void QGraphicsViews::DispImage(QImage Image)
{
	//加锁
	static QMutex mutex;
	QMutexLocker locker(&mutex);
	image = QPixmap::fromImage(Image);
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
	//clockwise(); //改为水平显示，图片顺时针旋转
	GetFit();
}

void QGraphicsViews::DispPoint(QVector<QPointF>& list, QColor color)
{
	drawItem->ClearAll();
	drawItem->AddRegion(list);
}

void QGraphicsViews::AddItems(BaseItem* item)
{
	item->scale = &ZoomValue;
	this->scene->addItem(item);
}

//设定信息模块是否显示
void QGraphicsViews::SetType(bool InfoFlg, bool GrayValueFlg)
{
	//item->setAcceptHoverEvents(!InfoFlg);
	//GrayValue->setVisible(!InfoFlg);
}

void QGraphicsViews::SetToFit(qreal val)
{
	ZoomFrame(val / ZoomValue);
	ZoomValue = val;
	QScrollBar* pHbar = this->horizontalScrollBar();
	pHbar->setSliderPosition(PixX);
	QScrollBar* pVbar = this->verticalScrollBar();
	pVbar->setSliderPosition(PixY);
}

void QGraphicsViews::ClearObj()
{
	foreach(auto item, scene->items())
	{
		if (item->type() == 10)
		{
			scene->removeItem(item);
		}
	}
}

void QGraphicsViews::ClearImage()
{
	image.fill(color1);//填充为背景色
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
}

void QGraphicsViews::ZoomFrame(double value)
{
	this->scale(value, value);
}

void QGraphicsViews::GetFit()
{
	if (this->width() < 1 || image.width() < 1)
	{
		return;
	}
	//图片自适应方法
	double winWidth = this->width();
	double winHeight = this->height();
	double ScaleWidth = (image.width() + 1) / winWidth;
	double ScaleHeight = (image.height() + 1) / winHeight;
	double row1, column1;
	double s = 0;
	if (ScaleWidth >= ScaleHeight)
	{
		row1 = -(1) * ((winHeight * ScaleWidth) - image.height()) / 2;
		column1 = 0;
		s = 1 / ScaleWidth;
	}
	else
	{
		row1 = 0;
		column1 = -(1.0) * ((winWidth * ScaleHeight) - image.width()) / 2;
		s = 1 / ScaleHeight;
	}
	if (ZoomFit != s || PixX != column1 * s)
	{
		ZoomFit = s;
		PixX = column1 * s;
		PixY = row1 * s;
		SetToFit(s);
	}
}

void QGraphicsViews::drawBg()
{
	bgPix.fill(color1);
	//bgPix.fill(QColor(255, 255, 255));
	QPainter painter(&bgPix);
	//painter.fillRect(0, 0, 18, 18, color2);
	//painter.fillRect(18, 18, 18, 18, color2);
	painter.end();
}

void QGraphicsViews::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);
}

void QGraphicsViews::resizeEvent(QResizeEvent* event)
{
	GetFit();
	QGraphicsView::resizeEvent(event);
}

void QGraphicsViews::mouseReleaseEvent(QMouseEvent* event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void QGraphicsViews::mouseDoubleClickEvent(QMouseEvent* event)
{
	SetToFit(ZoomFit);
	QGraphicsView::mouseDoubleClickEvent(event);
}

void QGraphicsViews::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void QGraphicsViews::wheelEvent(QWheelEvent* event)
{
	if ((event->angleDelta().y() > 0) && (ZoomValue >= 50)) //最大放大到原始图像的50倍
	{
		return;
	}
	else if ((event->angleDelta().y() < 0) && (ZoomValue <= 0.02))
	{
		return;
	}
	else
	{
		if (event->angleDelta().y() > 0) //鼠标滚轮向前滚动
		{
			ZoomValue *= 1.1; //每次放大10%
			ZoomFrame(1.1);
		}
		else
		{
			ZoomFrame(0.9);
			ZoomValue *= 0.9; //每次缩小10%
		}
	}
}

void QGraphicsViews::drawBackground(QPainter* painter, const QRectF& rect)
{
	Q_UNUSED(rect);
	painter->drawPixmap(QPoint(), image);
}

void QGraphicsViews::paintEvent(QPaintEvent* event)
{
	QPainter paint(this->viewport());
	paint.drawTiledPixmap(QRect(QPoint(0, 0), QPoint(this->width(), this->height())), bgPix);  //绘制背景
	QGraphicsView::paintEvent(event);
}
//选择打开图片
void QGraphicsViews::OnSelectImage()
{
	QString LocalFileName = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Images (*.png *.bmp *.jpg)"));
	QFile file(LocalFileName);
	if (!file.exists())
		return;
	Image.load(LocalFileName);
	QImage imageScale = Image.scaled(QSize(this->width(), this->height()));
	qmutex.lock();
	image = QPixmap::fromImage(imageScale);
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
	GetFit();
	qmutex.unlock();
}
void QGraphicsViews::OnSaveImage()
{
	QString directnary = QFileDialog::getExistingDirectory();
	if (directnary.isNull())
	{
		return;
	}
	QDir dir(directnary);
	if (!dir.exists()) {
		return;
	}
	QDir dir2;
	if (!dir2.exists(directnary + +"//QvisionImage"))//判断需要创建的文件夹是否存在
	{
		dir2.mkdir(directnary + +"//QvisionImage"); //创建文件夹
	}
	//使用QT获取系统时间，并且转换为“2023-03-08 14:35:00” 格式的字符串
	QDateTime curDateTime = QDateTime::currentDateTime();		//获取当前系统时间
	QString curTime = curDateTime.toString("yyyy-MM-dd-hh-mm-ss-zzz");	//转换为string类型
	image.save(directnary + "//" + "QvisionImage" + "//" + curTime + ".bmp");
}
//水平翻转
void QGraphicsViews::horFilp()
{
	qmutex.lock();
	QImage m_Image = image.toImage();
	m_Image = m_Image.mirrored(true, false);
	image = QPixmap::fromImage(m_Image);
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
	GetFit();
	qmutex.unlock();
	
}
//垂直翻转
void QGraphicsViews::verFilp()
{
	qmutex.lock();
	QImage m_Image = image.toImage();
	m_Image = m_Image.mirrored(false, true);
	image = QPixmap::fromImage(m_Image);
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
	GetFit();
	qmutex.unlock();

}
//顺时针旋转
void QGraphicsViews::clockwise()
{
	qmutex.lock();
	//QMatrix matrix;
	//QT版本不兼容 新版 Erik
	QTransform matrix;
	matrix.rotate(90.0);
	image = image.transformed(matrix, Qt::FastTransformation);
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
	GetFit();
	qmutex.unlock();
}
//逆时针旋转
void QGraphicsViews::anticlockwise()
{
	qmutex.lock();
	//QMatrix matrix;
	//QT版本不兼容 新版 Erik
	QTransform matrix;
	matrix.rotate(90.0);
	matrix.rotate(-90.0);
	image = image.transformed(matrix, Qt::FastTransformation);
	item->w = image.width();
	item->h = image.height();
	item->setPixmap(image);
	GetFit();
	qmutex.unlock();
}
//右键菜单
void QGraphicsViews::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint pos = event->pos();
	pos = this->mapToGlobal(pos);
	QMenu* menu = new QMenu(this);
	//菜单样式
	//menu->setStyleSheet("QMenu{font:11pt ""微软雅黑"";}");
	menu->setStyleSheet("color:rgb(200,255,200);QMenu{font:11pt ""微软雅黑"";}");

	menu->addSeparator();//添加分隔符
	QAction *loadImage = new QAction(this);
	loadImage->setText("打开图片");
    loadImage->setIcon(QIcon(":/QVisionTest/Resources/open.png"));    //设置菜单图标
	loadImage->setIconVisibleInMenu(true);       //图标设置为可见
	connect(loadImage, &QAction::triggered, this, &QGraphicsViews::OnSelectImage);
	menu->addAction(loadImage);
	menu->addSeparator();

	QAction* zoomInAction_buf = new QAction(this);
	zoomInAction_buf->setText("图像放大");
    zoomInAction_buf->setIcon(QIcon(":/QVisionTest/Resources/big.png"));
	zoomInAction_buf->setIconVisibleInMenu(true);       //图标设置为可见
	connect(zoomInAction_buf, &QAction::triggered, this, &QGraphicsViews::OnZoomInImage);
	menu->addAction(zoomInAction_buf);
	menu->addSeparator();

	QAction* zoomOutAction_buf = new QAction(this);
	zoomOutAction_buf->setText("图像缩小");
    zoomOutAction_buf->setIcon(QIcon(":/QVisionTest/Resources/big.png"));
	zoomOutAction_buf->setIconVisibleInMenu(true);       //图标设置为可见
	connect(zoomOutAction_buf, &QAction::triggered, this, &QGraphicsViews::OnZoomOutImage);
	menu->addAction(zoomOutAction_buf);
	menu->addSeparator();

	QAction* presetAction_buf = new QAction(this);
	presetAction_buf->setText("图像还原");
    presetAction_buf->setIcon(QIcon(":/QVisionTest/Resources/fit.png"));
	zoomOutAction_buf->setIconVisibleInMenu(true);       //图标设置为可见
	connect(presetAction_buf, &QAction::triggered, this, &QGraphicsViews::OnFitImage);
	menu->addAction(presetAction_buf);
	menu->addSeparator();

	QAction *horFilpAction = new QAction(this);
	horFilpAction->setText("水平翻转");
    horFilpAction->setIcon(QIcon(":/QVisionTest/Resources/morrir2.png"));    //设置菜单图标
	horFilpAction->setIconVisibleInMenu(true);       //图标设置为可见
	connect(horFilpAction, &QAction::triggered, this, &QGraphicsViews::horFilp);
	menu->addAction(horFilpAction);
	menu->addSeparator();//添加分隔符

	QAction *verFilpAction = new QAction(this);
	verFilpAction->setText("垂直翻转");
    verFilpAction->setIcon(QIcon(":/QVisionTest/Resources/morrir1.png"));    //设置菜单图标
	verFilpAction->setIconVisibleInMenu(true);       //图标设置为可见
	connect(verFilpAction, &QAction::triggered, this, &QGraphicsViews::verFilp);
	menu->addAction(verFilpAction);
	menu->addSeparator();//添加分隔符

	QAction *clockwiseAction = new QAction(this);
	clockwiseAction->setText("顺时针旋转");
    clockwiseAction->setIcon(QIcon(":/QVisionTest/Resources/190.png"));    //设置菜单图标
	clockwiseAction->setIconVisibleInMenu(true);       //图标设置为可见
	connect(clockwiseAction, &QAction::triggered, this, &QGraphicsViews::clockwise);
	menu->addAction(clockwiseAction);
	menu->addSeparator();//添加分隔符

	QAction *anticlockwiseAction = new QAction(this);
	anticlockwiseAction->setText("逆时针旋转");
    anticlockwiseAction->setIcon(QIcon(":/QVisionTest/Resources/290.png"));    //设置菜单图标
	anticlockwiseAction->setIconVisibleInMenu(true);       //图标设置为可见
	connect(anticlockwiseAction, &QAction::triggered, this, &QGraphicsViews::anticlockwise);
	menu->addAction(anticlockwiseAction);
	menu->addSeparator();//添加分隔符

	QAction *saveImage = new QAction(this);
	saveImage->setText("保存图片");
    saveImage->setIcon(QIcon(":/QVisionTest/Resources/save.png"));    //设置菜单图标
	saveImage->setIconVisibleInMenu(true);       //图标设置为可见
	connect(saveImage, &QAction::triggered, this, &QGraphicsViews::OnSaveImage);
	menu->addAction(saveImage);
	menu->addSeparator();

	menu->exec(pos);
}

void QGraphicsViews::OnZoomInImage()
{
	double tmp_buf = ZoomValue;
	tmp_buf *= 1.1;
	double tmp = tmp_buf / ZoomValue;
	ZoomValue *= tmp;
	this->scale(tmp, tmp);
}

void QGraphicsViews::OnZoomOutImage()
{
	double tmp_buf = ZoomValue;
	tmp_buf *= 0.9;
	double tmp = tmp_buf / ZoomValue;
	ZoomValue *= tmp;
	this->scale(tmp, tmp);
}

void QGraphicsViews::OnFitImage()
{
	SetToFit(ZoomFit);
}

void QGraphicsViews::DeleteObj(QGraphicsItem* Item)
{
	scene->removeItem(Item);
}

