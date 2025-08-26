#pragma once

#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QWheelEvent>
#include <Halcon.h>
#include <halconCpp/HalconCpp.h>
#include <QDebug>
#include <QtGui>
#include <QMenu>
#include <QFileDialog>
#endif

using namespace HalconCpp;

class CMyLabel : public QLabel
{
	Q_OBJECT

public:
	CMyLabel(QWidget *parent = Q_NULLPTR);
	~CMyLabel();

	//设置Halcon图像和Halcon窗口句柄，用户响应鼠标事件后实时更新图像
	void setHalconWnd(HObject img, HObject obj, bool isShowObj, HTuple hHalconID, QLabel *label);

	//鼠标滚轮缩放事件
	void wheelEvent(QWheelEvent *ev);

	//鼠标按下事件
	void mousePressEvent(QMouseEvent *ev);

	//鼠标释放事件
	void mouseReleaseEvent(QMouseEvent *ev);

	//鼠标移动事件
	void mouseMoveEvent(QMouseEvent *ev);

	//鼠标双击事件
	void mouseDoubleClickEvent(QMouseEvent *ev);

	//设置右键菜单是否启用
	void SetActionEnable(bool flag);

public:
	HTuple m_labelID;			//Qt标签句柄
	HTuple m_hHalconID;			//Halcon窗口句柄

	HObject m_currentImg;		//当前的图像
	HObject m_currentObj;      //当前窗口需要显示的图形(例如直线，圆，区域等)
	bool m_isShowObj = false;    //是否在窗口显示图形

	//主界面显示坐标的标签
	QLabel *m_label;

	//鼠标按下的位置 
	HTuple m_tMouseDownRow;
	HTuple m_tMouseDownCol;

	bool m_bIsMove;				 //是否移动图像标识
	bool m_bIsAction = true;	 //是否启用右键菜单
	bool m_bMoveEnable = false;	 //是否启用鼠标移动图像

private:
	//创建右键弹出菜单
	QMenu* m_menu;
	QAction* m_fitImgAction;
	QAction* m_saveImgAction;
	QAction* m_rotateImgAction;
	QAction* m_rotateImgReverseAction;
	QAction* m_EnaleMove;
	QAction* m_UnEnaleMove;

private:
	void createAction();
	void createMenu();
	void createContextMenu();
private slots:
	void contextMenuSlot(QPoint p);
	//图像自适应
	void slot_ShowImgFit();
	//保存图像
	void slot_SaveImg();
	//顺时针旋转
	void slot_RotateImg();
	//逆时针旋转
	void slot_RotateReverseImg();
	//启用拖图
	void slot_EnableMove();
	//禁用拖图
	void slot_UnEnableMove();
};
