#pragma once

#pragma execution_character_set("utf-8")
#include <QDialog>
#include "ui_qg_userChange.h"
#include <QKeyEvent>

class qg_userChange : public QDialog
{
	Q_OBJECT

public:
	qg_userChange(QWidget *parent = nullptr);
	~qg_userChange();

private slots:
	void on_pushButton_OK_clicked();
	void on_pushButton_Cancel_clicked();
	void comboBox_User_ChangedSlot(int idx);
signals:
	void comboBox_User_ChangedSignal(int idx);
	//发送用户信息
	void setUser_signal(int mes, int time = 300);

protected:
	void keyPressEvent(QKeyEvent* event) override;


private:
	Ui::qg_userChangeClass ui;
};
