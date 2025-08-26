#pragma once
#pragma execution_character_set("utf-8")
#include <modbus.h>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <cstring> // for memcpy
#include <vector>
#include <string>
#include <iostream>

#define MODBUS_TCP ModbusManager::instance()

class ModbusManager {
public:
	// ɾ���������캯���͸�ֵ�����
	ModbusManager(const ModbusManager&) = delete;
	ModbusManager& operator=(const ModbusManager&) = delete;

	// ��ȡ����ʵ��
	static ModbusManager& instance();

	// ���ӹ���
	bool open(const std::string& ip, int port, int slaveID = 1);
	void close();
	bool isConnected() const;

	// д�����
	void writeCoil(int addr, int data);
	void writeCoils(int addr, const std::vector<int>& data);
	void writeRegisterInt(int addr, int data);
	void writeRegisterInts(int addr, const std::vector<int>& datas);
	void writeRegisterFloat(int addr, float data);
	void writeRegisterFloats(int addr, const std::vector<float>& data);
	// д���������λ
	bool writeIntCoil(int addr, int bitPos, bool value);


	// ��ȡ����
	int readCoilInput(int addr);
	int readCoilOutput(int addr);
	int readRegisterInt(int addr);
	std::vector<int> readCoilInputs(int addr, int size);
	std::vector<int> readCoilOutputs(int addr, int size);
	std::vector<int> readRegisterInts(int addr, int size);
	float readRegisterFloat(int addr);
	std::vector<float> readRegisterFloats(int addr, int size);
	bool readIntCoil(int addr, int bitPos, bool& result);


	// ��ȡ��ǰ������Ϣ
	std::string getIp() const;
	int getPort() const;
	int getSlaveId() const;


private:
	// ˽�й��캯��
	ModbusManager() : ctx(nullptr) {}
	~ModbusManager();

	// libmodbus ʵ��
	modbus_t *ctx;

	// ������Ϣ
	std::string currentIp;
	int currentPort = -1;
	int currentSlaveId = -1;

	// ������
	mutable std::mutex mutex;

};