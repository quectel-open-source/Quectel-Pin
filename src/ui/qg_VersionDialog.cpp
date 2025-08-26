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
	if (text == "˫��Pin���")
		m_version = TwoPin;
	else if (text == "�׶���Pinר��")
		m_version = EdPin;
	else if (text == "���NPinר��")
		m_version = LsPin;
	else if (text == "��̩Pinר��")
		m_version = BtPin;
	else if (text == "����Pinר��")
		m_version = JmPin;
	else if (text == "�����������")
		m_version = JmHan;
	accept();
}

