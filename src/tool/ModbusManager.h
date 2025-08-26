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
	// 删除拷贝构造函数和赋值运算符
	ModbusManager(const ModbusManager&) = delete;
	ModbusManager& operator=(const ModbusManager&) = delete;

	// 获取单例实例
	static ModbusManager& instance();

	// 连接管理
	bool open(const std::string& ip, int port, int slaveID = 1);
	void close();
	bool isConnected() const;

	// 写入操作
	void writeCoil(int addr, int data);
	void writeCoils(int addr, const std::vector<int>& data);
	void writeRegisterInt(int addr, int data);
	void writeRegisterInts(int addr, const std::vector<int>& datas);
	void writeRegisterFloat(int addr, float data);
	void writeRegisterFloats(int addr, const std::vector<float>& data);
	// 写入整数里的位
	bool writeIntCoil(int addr, int bitPos, bool value);


	// 读取操作
	int readCoilInput(int addr);
	int readCoilOutput(int addr);
	int readRegisterInt(int addr);
	std::vector<int> readCoilInputs(int addr, int size);
	std::vector<int> readCoilOutputs(int addr, int size);
	std::vector<int> readRegisterInts(int addr, int size);
	float readRegisterFloat(int addr);
	std::vector<float> readRegisterFloats(int addr, int size);
	bool readIntCoil(int addr, int bitPos, bool& result);


	// 获取当前连接信息
	std::string getIp() const;
	int getPort() const;
	int getSlaveId() const;


private:
	// 私有构造函数
	ModbusManager() : ctx(nullptr) {}
	~ModbusManager();

	// libmodbus 实例
	modbus_t *ctx;

	// 连接信息
	std::string currentIp;
	int currentPort = -1;
	int currentSlaveId = -1;

	// 互斥锁
	mutable std::mutex mutex;

};