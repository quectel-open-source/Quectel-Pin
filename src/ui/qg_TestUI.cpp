#include "qg_TestUI.h"
#include "rs.h"
#include "rs_motion.h"
#include "rs_FunctionTool.h"
#include "ModbusManager.h"

qg_TestUI::qg_TestUI(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

qg_TestUI::~qg_TestUI()
{}

//����
void qg_TestUI::on_pushButton_5_clicked()
{
	//modbus.connectToDevice(ui.lineEdit->text());
	MODBUS_TCP.open(ui.lineEdit->text().toStdString(), 502);
}

//ֵд��
void qg_TestUI::on_pushButton_clicked()
{
	//if (modbus.writeSingleRegister(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toInt())) {
	//	machineLog->write("д��ɹ�", Normal);
	//}
	MODBUS_TCP.writeRegisterInt(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toInt());
}

//ֵ��ȡ
void qg_TestUI::on_pushButton_3_clicked()
{
	//auto values = modbus.readHoldingRegisters(ui.lineEdit_3->text().toInt(), 1);
	//if (!values.isEmpty())
	//{
	//	machineLog->write("��ȡ���: " + QString::number(values[0]), Normal);
	//	ui.lineEdit_4->setText(QString::number(values[0]));
	//}
	auto values = MODBUS_TCP.readRegisterInt(ui.lineEdit_3->text().toInt());
	ui.lineEdit_4->setText(QString::number(values));
	machineLog->write("��ȡ���: " + QString::number(values), Normal);
}

//λд��
void qg_TestUI::on_pushButton_2_clicked()
{
	//if (modbus.writeBit(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), ui.lineEdit_5->text().toInt())) {
	//	machineLog->write("д��ɹ�", Normal);
	//}
	MODBUS_TCP.writeIntCoil(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), ui.lineEdit_5->text().toInt());
	//MODBUS_TCP.writeCoil(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toInt());
}

//λ��ȡ
void qg_TestUI::on_pushButton_4_clicked()
{
	bool bit3State = false;
	//bool rtn = modbus.readBit(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), bit3State);
	//machineLog->write("��ȡ���: " + QString::number(bit3State), Normal);
	//ui.lineEdit_5->setText(QString::number(bit3State));

	MODBUS_TCP.readIntCoil(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), bit3State);
	
	//bit3State = MODBUS_TCP.readCoilInput(ui.lineEdit_3->text().toInt());
	machineLog->write("��ȡ���: " + QString::number(bit3State), Normal);
	ui.lineEdit_5->setText(QString::number(bit3State));

}

//����д��
void qg_TestUI::on_pushButton_6_clicked()
{
	//if (modbus.writeFloatRegister(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toFloat())) {
	//	machineLog->write("д��ɹ�", Normal);
	//}

	MODBUS_TCP.writeRegisterFloat(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toFloat());
}

//�����ȡ
void qg_TestUI::on_pushButton_7_clicked()
{
	float values = 0;
	//auto res = modbus.readFloatRegister(ui.lineEdit_3->text().toInt(), values);
	//if (res)
	//{
	//	machineLog->write("��ȡ���: " + QString::number(values), Normal);
	//	ui.lineEdit_4->setText(QString::number(values));
	//}

	values = MODBUS_TCP.readRegisterFloat(ui.lineEdit_3->text().toInt());
	machineLog->write("��ȡ���: " + QString::number(values), Normal);
	ui.lineEdit_4->setText(QString::number(values));
}

//mes��վ����
void qg_TestUI::on_pushButton_8_clicked()
{
	if (LSM->m_version == EdPin)
	{
		// ׼����������
		MesCheckInRequest_ED request;
		request.sn = ui.lineEdit_7->text();
		request.workPlaceCode = "SPI";
		request.productSn = ui.lineEdit_8->text();
		QString url = ui.lineEdit_9->text();
		//request.IP = "192.168.1.100";
		//request.Slot = "A3";

		// ��������
		rs_FunctionTool tool;
		//url = "http://47.92.30.209:8146";
		MesCheckInResponse_ED response = tool.mes_check_in_ED(url, request);

		// ������Ӧ
		if (response.state) {
			ui.textEdit->append("У��ͨ�������Կ�ʼ����");
		}
		else {
			ui.textEdit->append("У��ʧ�ܣ�" + response.description);
		}
	}
}

//mes��վ����
void qg_TestUI::on_pushButton_9_clicked()
{
	if (LSM->m_version == EdPin)
	{
		// ׼����������
		MesCheckInFullRequest_ED request;

		request.WorkPlaceCode= "��Զ�Ӿ����";
		request.WorkOrderCode= ui.lineEdit_10->text();
		request.User= "admin";
		request.Sn= ui.lineEdit_7->text();
		request.Result= "NG";
		request.ProductCode= "ProductCode";
		request.OtherCode1= "1";
		request.OtherCode2= "2";
		request.OtherCode3= "3";
		request.OtherCode4= "4";
		request.Result_2D= "NG";
		request.Result_3D= "OK";
		request.Screw1Inspection= "1";
		request.Screw2Inspection= "2";
		request.Screw3Inspection= "3";
		request.Screw4Inspection= "4";
		request.Result_LabelCharacters= "NG";
		request.Result_SnapFit= "OK";
		request.WaterproofBreathableInspection= "A";
		request.SealantInspection= "B";
		request.Result_CircularDistance1= "NG";
		request.Result_CircularDistance2= "NG";
		request.Result_CircularDistance3= "OK";
		request.Result_CircularDistance4= "OK";
		request.PictureUrl= "E:\\NG\\2025-08-13\\ID202412231301AFMC_20240322164614.jpg";

		QString url = ui.lineEdit_9->text();
		//request.IP = "192.168.1.100";
		//request.Slot = "A3";

		// ��������
		rs_FunctionTool tool;
		//url = "http://47.92.30.209:8146";
		MesCheckInResponse_ED response = tool.mes_full_check_in(url, request);

		// ������Ӧ
		if (response.state) {
			ui.textEdit->append("��վУ��ͨ�������Կ�ʼ����");
		}
		else {
			ui.textEdit->append("��վУ��ʧ�ܣ�" + response.description);
		}
	}
}