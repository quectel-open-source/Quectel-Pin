#include "qg_userChange.h"
#include <QtWidgets/qmessagebox.h>
#include "rs_motion.h"
#include "rs.h"

qg_userChange::qg_userChange(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//隐藏边框
	setWindowFlags(Qt::FramelessWindowHint);

	connect(ui.comboBox_User, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_User_ChangedSlot(int)));
	ui.lineEdit_Password->setEchoMode(QLineEdit::Password);
	switch (LSM->m_version)
	{
	case ENUM_VERSION::EdPin:
	{
#if LOCAL_TEST == 0
		QPixmap pix(":/icons/易鼎丰.png");
#else
		QPixmap pix(":/icons/jiangzhun.png");
#endif
		ui.label_4->setPixmap(pix);
		break;
	}
	default:
		break;
	}
	//先显示普通用户
	comboBox_User_ChangedSlot(0);
}

qg_userChange::~qg_userChange()
{}


void qg_userChange::on_pushButton_OK_clicked()
{
	if (ui.comboBox_User->currentIndex() == 1)
	{
		//管理员
		// 验证密码（暂用密码"123456"）
		if (ui.lineEdit_Password->text() != "123456") {
			QMessageBox::critical(nullptr, "错误", "密码错误，请重试！");
			return;
		}
		emit setUser_signal(1,ui.lineEdit_AutoBreak->text().toInt());
	}
	else
	{
		//普通用户
		emit setUser_signal(0);
	}
	accept();
}

//切换用户
void qg_userChange::comboBox_User_ChangedSlot(int idx)
{
	switch (idx)
	{
	case 0:
		ui.lineEdit_Password->setEnabled(false);
		ui.lineEdit_AutoBreak->setEnabled(false);
		ui.label_3->setEnabled(false);
		ui.label_2->setEnabled(false);
		//ui.lineEdit_Password->hide();
		//ui.lineEdit_AutoBreak->hide();
		//ui.label_3->hide();
		//ui.label_2->hide();
		break;
	case 1:
		ui.lineEdit_Password->setEnabled(true);
		ui.lineEdit_AutoBreak->setEnabled(true);
		ui.label_3->setEnabled(true);
		ui.label_2->setEnabled(true);
		//ui.lineEdit_Password->show();
		//ui.lineEdit_AutoBreak->show();
		//ui.label_3->show();
		//ui.label_2->show();
		break;
	default:
		break;
	}
}

void qg_userChange::on_pushButton_Cancel_clicked()
{
	close();
}

void qg_userChange::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
		on_pushButton_OK_clicked();
		event->accept();
	}
	else {
		QDialog::keyPressEvent(event);
	}
}