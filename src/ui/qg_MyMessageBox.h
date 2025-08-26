#pragma once

#include <QDialog>
#include "ui_qg_MyMessageBox.h"

class qg_MyMessageBox : public QDialog
{
	Q_OBJECT

public:
	qg_MyMessageBox(QWidget *parent = nullptr,QString title = "ÌבÊ¾", QString text = "");
	~qg_MyMessageBox();



private:
	Ui::qg_MyMessageBoxClass ui;
};

