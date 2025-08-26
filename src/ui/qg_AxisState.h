#pragma once

#include <QWidget>
#include "ui_qg_AxisState.h"

class qg_AxisState : public QWidget
{
	Q_OBJECT

public:
	qg_AxisState(QWidget *parent = nullptr);
	~qg_AxisState();

	//���ÿؼ�״̬
	void setUiState(bool enable);

public slots:
	//�����ŷ�״̬
	void updateAxisState();
	void on_pushButton_pressed();
	void on_pushButton_released();
	void on_pushButton_2_pressed();
	void on_pushButton_2_released();
	void on_checkBox_clicked(bool checked);
	void on_checkBox_2_clicked(bool checked);
	void on_checkBox_noDoor_clicked(bool checked);
	void on_checkBox_3_clicked(bool checked);
	
	//����SN
	void onlinesnChanged();
	//����ҳ��
	void on_pushButton_3_clicked();
	

private:
	Ui::qg_AxisStateClass ui;
};
