#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_qg_AxisParameter.h"

class qg_AxisParameter : public QWidget
{
	Q_OBJECT

public:
	qg_AxisParameter(QWidget *parent = nullptr);
	~qg_AxisParameter();
	//��ʽ��ʼ��
	void initAxisStyle();
	void initDIStyle();
	void initDOStyle();
	void updateDIConfigUI();
	void updateDOConfigUI();
	void initParamerStyle();
	void initUI();
	//����IO
	void initDIStyle_EtherCAT();
	void initDOStyle_EtherCAT();

private slots:
	void onTableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void on_tableView_DIConfig_DataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void on_tableView_DOConfig_DataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void on_pushButton_upDate_clicked();
	void on_pushButton_Download_clicked();
	void on_pushButton_DiReading_clicked();
	void on_pushButton_DOReading_clicked();
	void onTimerDITimeout();  // DI���
	void on_pushButton_ParaSave_clicked();
	void onTimerDOTimeout();  // DO���
	

private:
	Ui::qg_AxisParameterClass ui;

	//�Ƿ���DI���״̬
	bool m_isDiReading{ false };
	bool m_isDoReading{ false };
	// DI��ض�ʱ��
	QTimer* m_timerDI;     
	// DO��ض�ʱ��
	QTimer* m_timerDO;
};
