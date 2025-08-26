#pragma once

#include <QWidget>
#include "ui_qg_AxisState.h"

class qg_AxisState : public QWidget
{
	Q_OBJECT

public:
	qg_AxisState(QWidget *parent = nullptr);
	~qg_AxisState();

	//设置控件状态
	void setUiState(bool enable);

public slots:
	//更新伺服状态
	void updateAxisState();
	void on_pushButton_pressed();
	void on_pushButton_released();
	void on_pushButton_2_pressed();
	void on_pushButton_2_released();
	void on_checkBox_clicked(bool checked);
	void on_checkBox_2_clicked(bool checked);
	void on_checkBox_noDoor_clicked(bool checked);
	void on_checkBox_3_clicked(bool checked);
	
	//测试SN
	void onlinesnChanged();
	//测试页面
	void on_pushButton_3_clicked();
	

private:
	Ui::qg_AxisStateClass ui;
};
