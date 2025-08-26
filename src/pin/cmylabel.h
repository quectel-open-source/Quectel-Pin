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

	//����Halconͼ���Halcon���ھ�����û���Ӧ����¼���ʵʱ����ͼ��
	void setHalconWnd(HObject img, HObject obj, bool isShowObj, HTuple hHalconID, QLabel *label);

	//�����������¼�
	void wheelEvent(QWheelEvent *ev);

	//��갴���¼�
	void mousePressEvent(QMouseEvent *ev);

	//����ͷ��¼�
	void mouseReleaseEvent(QMouseEvent *ev);

	//����ƶ��¼�
	void mouseMoveEvent(QMouseEvent *ev);

	//���˫���¼�
	void mouseDoubleClickEvent(QMouseEvent *ev);

	//�����Ҽ��˵��Ƿ�����
	void SetActionEnable(bool flag);

public:
	HTuple m_labelID;			//Qt��ǩ���
	HTuple m_hHalconID;			//Halcon���ھ��

	HObject m_currentImg;		//��ǰ��ͼ��
	HObject m_currentObj;      //��ǰ������Ҫ��ʾ��ͼ��(����ֱ�ߣ�Բ�������)
	bool m_isShowObj = false;    //�Ƿ��ڴ�����ʾͼ��

	//��������ʾ����ı�ǩ
	QLabel *m_label;

	//��갴�µ�λ�� 
	HTuple m_tMouseDownRow;
	HTuple m_tMouseDownCol;

	bool m_bIsMove;				 //�Ƿ��ƶ�ͼ���ʶ
	bool m_bIsAction = true;	 //�Ƿ������Ҽ��˵�
	bool m_bMoveEnable = false;	 //�Ƿ���������ƶ�ͼ��

private:
	//�����Ҽ������˵�
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
	//ͼ������Ӧ
	void slot_ShowImgFit();
	//����ͼ��
	void slot_SaveImg();
	//˳ʱ����ת
	void slot_RotateImg();
	//��ʱ����ת
	void slot_RotateReverseImg();
	//������ͼ
	void slot_EnableMove();
	//������ͼ
	void slot_UnEnableMove();
};
