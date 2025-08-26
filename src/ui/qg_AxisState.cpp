#include "qg_AxisState.h"
#include <QTimer>
#include "rs_motion.h"
#include "qg_TestUI.h"

qg_AxisState::qg_AxisState(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// ������ʱ��
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &qg_AxisState::updateAxisState);
	connect(ui.lineEdit, &QLineEdit::textChanged, this, &qg_AxisState::onlinesnChanged);//�û��˺�
	timer->start(1000);

	ui.checkBox_3->hide();
	switch (LSM->m_version)
	{
	case EdPin:
		ui.checkBox_3->show();
		break;
	default:
		break;
	}
}

qg_AxisState::~qg_AxisState()
{}

//�����ŷ�״̬
void qg_AxisState::updateAxisState()
{
	switch (LSM->m_version)
	{
	case ENUM_VERSION::TwoPin:
		//˫��
		break;
	case ENUM_VERSION::EdPin:
	case ENUM_VERSION::JmPin:
		break;
	case ENUM_VERSION::LsPin:
	case ENUM_VERSION::JmHan:
		ui.label_Axis_6->hide();
		ui.label_6->hide();
		ui.label_Axis_7->hide();
		ui.label_7->hide();
		break;
	case ENUM_VERSION::BtPin:
		ui.label_Axis_4->hide();
		ui.label_4->hide();
		ui.label_Axis_5->hide();
		ui.label_5->hide();
		ui.label_Axis_6->hide();
		ui.label_6->hide();
		ui.label_Axis_7->hide();
		ui.label_7->hide();
		break;
	default:
		break;
	}


#ifndef DEBUG_LHC
	if (!LSM->m_cardInitStatus)
		return;
	if (LSM->m_isStart)
		return;
#endif // !DEBUG_LHC

	QString style = "min-width:40px; min-height:40px; max-width:40px; max-height:40px; "
		"border-radius:20px; border:1px solid black;";

	for (const auto& axis : LSM->m_Axis)
	{
		// ��̬���ɱ�ǩ����
		QString labelName = QString("label_Axis_%1").arg(axis.second.axisNum + 1);
		// ���Ҷ�Ӧ�� QLabel �ؼ�
		QLabel* axisLabel = findChild<QLabel*>(labelName);
		if (axisLabel)
		{
			bool err = LSM->isAxisErc(axis.first);
			if (err)
			{
				//����
				QString finalStyle = style + "background:red;";
				axisLabel->setStyleSheet(finalStyle);
			}
			else
			{
				QString finalStyle = style + "background:green;";
				axisLabel->setStyleSheet(finalStyle);
			}
		}
	}
}

void qg_AxisState::on_pushButton_pressed()
{
	LSM->inDI = true;
}

void qg_AxisState::on_pushButton_released()
{
	LSM->inDI = false;
}

void qg_AxisState::on_pushButton_2_pressed()
{
	LSM->outDI = true;
}

void qg_AxisState::on_pushButton_2_released()
{
	LSM->outDI = false;
}

//��ʹ��mes
void qg_AxisState::on_checkBox_clicked(bool checked)
{
	if (checked)
	{
		LSM->notMes = true;
	}
	else
	{
		LSM->notMes = false;
	}
}

//��ʹ��ɨ��
void qg_AxisState::on_checkBox_2_clicked(bool checked)
{
	if (checked)
	{
		LSM->notQR = true;
	}
	else
	{
		LSM->notQR = false;
	}
}

//����sn
void qg_AxisState::onlinesnChanged()
{
	LSM->testSN = ui.lineEdit->text();
}

//����ҳ��
void qg_AxisState::on_pushButton_3_clicked()
{
	qg_TestUI dlg;
	dlg.exec();
}

//��ʹ���Ž�
void qg_AxisState::on_checkBox_noDoor_clicked(bool checked)
{
	if (checked)
	{
		LSM->m_notDoor = true;
	}
	else
	{
		LSM->m_notDoor = false;
	}
}

//��ʹ�û�����
void qg_AxisState::on_checkBox_3_clicked(bool checked)
{
	if (checked)
	{
		LSM->m_notRobot = true;
	}
	else
	{
		LSM->m_notRobot = false;
	}
}

//���ÿؼ�״̬
void qg_AxisState::setUiState(bool enable)
{
	if (enable == false)
	{
		//ui.checkBox_2->setChecked(false);
		//ui.checkBox->setChecked(false);
		ui.checkBox_noDoor->setChecked(false);
		//ui.checkBox_3->setChecked(false);
		//LSM->m_notRobot = false;
		LSM->m_notDoor = false;
		//LSM->notQR = false;
		//LSM->notMes = false;
	}

	ui.checkBox_2->setEnabled(enable);
	ui.checkBox->setEnabled(enable);
	ui.checkBox_noDoor->setEnabled(enable);
	ui.checkBox_3->setEnabled(enable);
	ui.lineEdit->setEnabled(enable);
	ui.pushButton->setEnabled(enable);
	ui.pushButton_2->setEnabled(enable);
	ui.pushButton_3->setEnabled(enable);
	//this->update();
}