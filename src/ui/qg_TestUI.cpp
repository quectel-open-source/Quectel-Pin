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

//连接
void qg_TestUI::on_pushButton_5_clicked()
{
	//modbus.connectToDevice(ui.lineEdit->text());
	MODBUS_TCP.open(ui.lineEdit->text().toStdString(), 502);
}

//值写入
void qg_TestUI::on_pushButton_clicked()
{
	//if (modbus.writeSingleRegister(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toInt())) {
	//	machineLog->write("写入成功", Normal);
	//}
	MODBUS_TCP.writeRegisterInt(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toInt());
}

//值读取
void qg_TestUI::on_pushButton_3_clicked()
{
	//auto values = modbus.readHoldingRegisters(ui.lineEdit_3->text().toInt(), 1);
	//if (!values.isEmpty())
	//{
	//	machineLog->write("读取结果: " + QString::number(values[0]), Normal);
	//	ui.lineEdit_4->setText(QString::number(values[0]));
	//}
	auto values = MODBUS_TCP.readRegisterInt(ui.lineEdit_3->text().toInt());
	ui.lineEdit_4->setText(QString::number(values));
	machineLog->write("读取结果: " + QString::number(values), Normal);
}

//位写入
void qg_TestUI::on_pushButton_2_clicked()
{
	//if (modbus.writeBit(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), ui.lineEdit_5->text().toInt())) {
	//	machineLog->write("写入成功", Normal);
	//}
	MODBUS_TCP.writeIntCoil(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), ui.lineEdit_5->text().toInt());
	//MODBUS_TCP.writeCoil(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toInt());
}

//位读取
void qg_TestUI::on_pushButton_4_clicked()
{
	bool bit3State = false;
	//bool rtn = modbus.readBit(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), bit3State);
	//machineLog->write("读取结果: " + QString::number(bit3State), Normal);
	//ui.lineEdit_5->setText(QString::number(bit3State));

	MODBUS_TCP.readIntCoil(ui.lineEdit_3->text().toInt(), ui.lineEdit_6->text().toInt(), bit3State);
	
	//bit3State = MODBUS_TCP.readCoilInput(ui.lineEdit_3->text().toInt());
	machineLog->write("读取结果: " + QString::number(bit3State), Normal);
	ui.lineEdit_5->setText(QString::number(bit3State));

}

//浮点写入
void qg_TestUI::on_pushButton_6_clicked()
{
	//if (modbus.writeFloatRegister(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toFloat())) {
	//	machineLog->write("写入成功", Normal);
	//}

	MODBUS_TCP.writeRegisterFloat(ui.lineEdit_3->text().toInt(), ui.lineEdit_4->text().toFloat());
}

//浮点读取
void qg_TestUI::on_pushButton_7_clicked()
{
	float values = 0;
	//auto res = modbus.readFloatRegister(ui.lineEdit_3->text().toInt(), values);
	//if (res)
	//{
	//	machineLog->write("读取结果: " + QString::number(values), Normal);
	//	ui.lineEdit_4->setText(QString::number(values));
	//}

	values = MODBUS_TCP.readRegisterFloat(ui.lineEdit_3->text().toInt());
	machineLog->write("读取结果: " + QString::number(values), Normal);
	ui.lineEdit_4->setText(QString::number(values));
}

//mes进站测试
void qg_TestUI::on_pushButton_8_clicked()
{
	if (LSM->m_version == EdPin)
	{
		// 准备请求数据
		MesCheckInRequest_ED request;
		request.sn = ui.lineEdit_7->text();
		request.workPlaceCode = "SPI";
		request.productSn = ui.lineEdit_8->text();
		QString url = ui.lineEdit_9->text();
		//request.IP = "192.168.1.100";
		//request.Slot = "A3";

		// 发送请求
		rs_FunctionTool tool;
		//url = "http://47.92.30.209:8146";
		MesCheckInResponse_ED response = tool.mes_check_in_ED(url, request);

		// 处理响应
		if (response.state) {
			ui.textEdit->append("校验通过，可以开始测试");
		}
		else {
			ui.textEdit->append("校验失败：" + response.description);
		}
	}
}

//mes进站测试
void qg_TestUI::on_pushButton_9_clicked()
{
	if (LSM->m_version == EdPin)
	{
		// 准备请求数据
		MesCheckInFullRequest_ED request;

		request.WorkPlaceCode= "移远视觉检查";
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

		// 发送请求
		rs_FunctionTool tool;
		//url = "http://47.92.30.209:8146";
		MesCheckInResponse_ED response = tool.mes_full_check_in(url, request);

		// 处理响应
		if (response.state) {
			ui.textEdit->append("出站校验通过，可以开始测试");
		}
		else {
			ui.textEdit->append("出站校验失败：" + response.description);
		}
	}
}