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
	//样式初始化
	void initAxisStyle();
	void initDIStyle();
	void initDOStyle();
	void updateDIConfigUI();
	void updateDOConfigUI();
	void initParamerStyle();
	void initUI();
	//总线IO
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
	void onTimerDITimeout();  // DI监控
	void on_pushButton_ParaSave_clicked();
	void onTimerDOTimeout();  // DO监控
	

private:
	Ui::qg_AxisParameterClass ui;

	//是否处于DI监控状态
	bool m_isDiReading{ false };
	bool m_isDoReading{ false };
	// DI监控定时器
	QTimer* m_timerDI;     
	// DO监控定时器
	QTimer* m_timerDO;
};
