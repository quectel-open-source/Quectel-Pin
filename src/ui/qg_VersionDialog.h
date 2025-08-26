#pragma once

#include <QDialog>
#include "ui_qg_VersionDialog.h"

class qg_VersionDialog : public QDialog
{
	Q_OBJECT

public:
	qg_VersionDialog(int version,QWidget *parent = nullptr);
	~qg_VersionDialog();
	int getVersion()
	{
		return m_version;
	};

private:
	Ui::qg_VersionDialogClass ui;
	int m_version = 0;


private slots:
	void on_pushButton_OK_clicked();
};
