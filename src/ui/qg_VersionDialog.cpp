#include "qg_VersionDialog.h"
#include <QListView>
#include "rs.h"

qg_VersionDialog::qg_VersionDialog(int version, QWidget *parent)
	: QDialog(parent),m_version(version)
{
	ui.setupUi(this);
	ui.comboBox_Version->setView(new QListView());
	setStyleSheet("QComboBox QAbstractItemView::item{height:28px;}");
	ui.comboBox_Version->setCurrentIndex(m_version);
}

qg_VersionDialog::~qg_VersionDialog()
{

}


void qg_VersionDialog::on_pushButton_OK_clicked()
{
	//m_version = ui.comboBox_Version->currentIndex();
	auto text = ui.comboBox_Version->currentText();
	if (text == "双轨Pin标机")
		m_version = TwoPin;
	else if (text == "易鼎丰Pin专机")
		m_version = EdPin;
	else if (text == "立昇Pin专机")
		m_version = LsPin;
	else if (text == "博泰Pin专机")
		m_version = BtPin;
	else if (text == "金脉Pin专机")
		m_version = JmPin;
	else if (text == "金脉焊缝检测机")
		m_version = JmHan;
	accept();
}

