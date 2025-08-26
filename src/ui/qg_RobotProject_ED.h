#pragma once
#pragma execution_character_set("utf-8")
#include <QDialog>
#include "ui_qg_RobotProject_ED.h"
#include <QTableWidget>
#include <QTableWidgetItem>

class qg_RobotProject_ED : public QDialog
{
	Q_OBJECT

public:
	qg_RobotProject_ED(QWidget *parent = nullptr);
	~qg_RobotProject_ED();
	//��ʼ�����
	void initTableWidget(QTableWidget* tableWidget);
	//�������������
	void saveRobotProject();
	//�ӱ��ض�ȡ����
	void readRobotProject();
public slots:
	//����
	void on_pushButton_save_clicked();
protected:
	void closeEvent(QCloseEvent* event) override;
private:
	Ui::qg_RobotProject_EDClass ui;
};

