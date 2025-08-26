#pragma once

#include <QDialog>
#include "ui_qg_TestUI.h"
#include "ModbusTcpClient.h"
#include "qc_log.h"

class qg_TestUI : public QDialog
{
	Q_OBJECT

public:
	qg_TestUI(QWidget *parent = nullptr);
	~qg_TestUI();
public:
	ModbusTcpClient modbus;

public slots:
	void on_pushButton_5_clicked();
	void on_pushButton_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_4_clicked();
	void on_pushButton_6_clicked();
	void on_pushButton_7_clicked();
	void on_pushButton_8_clicked();
	void on_pushButton_9_clicked();
private:
	Ui::qg_TestUIClass ui;
};
