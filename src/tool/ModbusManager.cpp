#include "ModbusManager.h"
#include <stdexcept>
#include <iostream>
#include "qc_log.h"

ModbusManager& ModbusManager::instance() {
	static ModbusManager instance;
	return instance;
}

bool ModbusManager::open(const std::string& ip, int port, int slaveID) {
	std::lock_guard<std::mutex> lock(mutex);

	try {
		// 如果已连接，先关闭
		if (ctx) {
			modbus_close(ctx);
			modbus_free(ctx);
			ctx = nullptr;
		}

		// 创建新的modbus上下文
		ctx = modbus_new_tcp(ip.c_str(), port);
		if (!ctx) {
			machineLog->write("Failed to create modbus context！！", Normal);
			return false;

			//throw std::runtime_error("Failed to create modbus context");
		}

		// 设置从机地址
		if (modbus_set_slave(ctx, slaveID) != 0) {
			modbus_free(ctx);
			ctx = nullptr;
			machineLog->write("Failed to set slave ID！！", Normal);
			return false;

			//throw std::runtime_error("Failed to set slave ID");
		}

		// 设置响应超时（1秒）
		modbus_set_response_timeout(ctx, 1, 0);

		// 建立连接
		if (modbus_connect(ctx) != 0) {
			modbus_free(ctx);
			ctx = nullptr;
			machineLog->write("Modbus connection failed！！", Normal);
			return false;
		}

		// 保存连接信息
		currentIp = ip;
		currentPort = port;
		currentSlaveId = slaveID;
		return true;
	}
	catch (const std::runtime_error& e)
	{
		return false;
	}
}

void ModbusManager::close() {
	std::lock_guard<std::mutex> lock(mutex);
	if (ctx) {
		modbus_close(ctx);
		modbus_free(ctx);
		ctx = nullptr;
	}
	currentIp = "";
	currentPort = -1;
	currentSlaveId = -1;
}

bool ModbusManager::isConnected() const {
	std::lock_guard<std::mutex> lock(mutex);
	return ctx != nullptr;
}

// ==== 写入操作 ====

void ModbusManager::writeCoil(int addr, int data) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	if (modbus_write_bit(ctx, addr, data) < 0) {
		machineLog->write("Write coil failed: " + QString(modbus_strerror(errno)), Normal);
	}
}

void ModbusManager::writeCoils(int addr, const std::vector<int>& data) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	std::vector<uint8_t> bits(data.size());
	for (size_t i = 0; i < data.size(); i++) {
		bits[i] = static_cast<uint8_t>(data[i] != 0);
	}

	if (modbus_write_bits(ctx, addr, data.size(), bits.data()) < 0) {
		machineLog->write("Write multiple coils failed: " + QString(modbus_strerror(errno)), Normal);
	}
}

void ModbusManager::writeRegisterInt(int addr, int data) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx)	machineLog->write("Not connected to modbus", Normal);

	if (modbus_write_register(ctx, addr, data) < 0) {
		machineLog->write("Write register failed: " + QString(modbus_strerror(errno)), Normal);
	}
}

void ModbusManager::writeRegisterInts(int addr, const std::vector<int>& datas) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	std::vector<uint16_t> regs(datas.size());
	for (size_t i = 0; i < datas.size(); i++) {
		regs[i] = static_cast<uint16_t>(datas[i]);
	}

	if (modbus_write_registers(ctx, addr, regs.size(), regs.data()) < 0) {
		machineLog->write("Write multiple registers failed: " + QString(modbus_strerror(errno)), Normal);
	}
}

void ModbusManager::writeRegisterFloat(int addr, float data) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	uint16_t* regs = reinterpret_cast<uint16_t*>(&data);
	if (modbus_write_registers(ctx, addr, 2, regs) < 0) {
		machineLog->write("Write float register failed: " + QString(modbus_strerror(errno)), Normal);
	}
}

void ModbusManager::writeRegisterFloats(int addr, const std::vector<float>& data) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	int numRegs = data.size() * 2;
	std::vector<uint16_t> regs(numRegs);

	for (size_t i = 0; i < data.size(); ++i) {
		uint16_t* p = reinterpret_cast<uint16_t*>(const_cast<float*>(&data[i]));
		regs[i * 2] = p[0]; // 默认高位在前（大端）
		regs[i * 2 + 1] = p[1];
	}

	if (modbus_write_registers(ctx, addr, numRegs, regs.data()) < 0) {
		machineLog->write("Write multiple float registers failed: " + QString(modbus_strerror(errno)), Normal);
	}
}

bool ModbusManager::writeIntCoil(int addr, int bitPos, bool value) {
	// 参数校验：确保位位置在0-15范围内
	if (bitPos < 0 || bitPos > 15) {
		std::cerr << "Invalid bit position: " << bitPos << " (must be 0-15)\n";
		return false;
	}

	try {
		// 读取当前寄存器值
		uint16_t currentValue = readRegisterInt(addr);  // 假设存在安全的读函数

		// 计算掩码并修改指定位
		uint16_t mask = 1 << bitPos;
		uint16_t newValue = value
			? (currentValue | mask)     // 置位 
			: (currentValue & ~mask);   // 清零

		// 写入新值
		writeRegisterInt(addr, static_cast<int>(newValue));
		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "Write bit failed: " << e.what() << "\n";
		return false;
	}
}

// ==== 读取操作 ====

int ModbusManager::readCoilInput(int addr) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	uint8_t value;
	if (modbus_read_input_bits(ctx, addr, 1, &value) < 0) {
		machineLog->write("Read input coil failed: " + QString(modbus_strerror(errno)), Normal);
	}
	return static_cast<int>(value);
}

int ModbusManager::readCoilOutput(int addr) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	uint8_t value;
	if (modbus_read_bits(ctx, addr, 1, &value) < 0) {
		machineLog->write("Read output coil failed: " + QString(modbus_strerror(errno)), Normal);
	}
	return static_cast<int>(value);
}

int ModbusManager::readRegisterInt(int addr) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	uint16_t value;
	if (modbus_read_registers(ctx, addr, 1, &value) < 0) {
		machineLog->write("Read register failed: " + QString(modbus_strerror(errno)), Normal);
	}
	return static_cast<int>(value);
}

std::vector<int> ModbusManager::readCoilInputs(int addr, int size) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	std::vector<uint8_t> values(size);
	if (modbus_read_input_bits(ctx, addr, size, values.data()) < 0) {
		machineLog->write("Read input coils failed: " + QString(modbus_strerror(errno)), Normal);
	}

	std::vector<int> result;
	result.reserve(size);
	for (auto v : values) {
		result.push_back(static_cast<int>(v));
	}
	return result;
}

std::vector<int> ModbusManager::readCoilOutputs(int addr, int size) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	std::vector<uint8_t> values(size);
	if (modbus_read_bits(ctx, addr, size, values.data()) < 0) {
		machineLog->write("Read output coils failed: " + QString(modbus_strerror(errno)), Normal);
	}

	std::vector<int> result;
	result.reserve(size);
	for (auto v : values) {
		result.push_back(static_cast<int>(v));
	}
	return result;
}

std::vector<int> ModbusManager::readRegisterInts(int addr, int size) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	std::vector<uint16_t> values(size);
	if (modbus_read_registers(ctx, addr, size, values.data()) < 0) {
		machineLog->write("Read registers failed: " + QString(modbus_strerror(errno)), Normal);
	}

	std::vector<int> result;
	result.reserve(size);
	for (auto v : values) {
		result.push_back(static_cast<int>(v));
	}
	return result;
}

float ModbusManager::readRegisterFloat(int addr) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	uint16_t regs[2];
	if (modbus_read_registers(ctx, addr, 2, regs) < 0) {
		machineLog->write("Read float register failed:" + QString(modbus_strerror(errno)), Normal);
	}

	float result;
	uint16_t* p = reinterpret_cast<uint16_t*>(&result);
	p[0] = regs[0];
	p[1] = regs[1];
	return result;
}

std::vector<float> ModbusManager::readRegisterFloats(int addr, int size) {
	std::lock_guard<std::mutex> lock(mutex);
	if (!ctx) machineLog->write("Not connected to modbus", Normal);

	int numRegs = size * 2;
	std::vector<uint16_t> regs(numRegs);

	if (modbus_read_registers(ctx, addr, numRegs, regs.data()) < 0) {
		machineLog->write("Read float registers failed:" + QString(modbus_strerror(errno)), Normal);
	}

	std::vector<float> result(size);
	for (int i = 0; i < size; ++i) {
		uint16_t* p = regs.data() + i * 2;
		float f = *reinterpret_cast<float*>(p);
		result[i] = f;
	}

	return result;
}

bool ModbusManager::readIntCoil(int addr, int bitPos, bool& result) {
	// 参数校验：确保位位置在0-15范围内
	if (bitPos < 0 || bitPos > 15) {
		std::cerr << "Invalid bit position: " << bitPos << " (must be 0-15)\n";
		return false;
	}

	try {
		// 读取整个寄存器值
		int registerValue = readRegisterInt(addr);

		// 计算位掩码并提取指定位
		uint16_t mask = 1 << bitPos;
		result = (static_cast<uint16_t>(registerValue) & mask) != 0;
		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "Read bit failed: " << e.what() << "\n";
		return false;
	}
}

// ==== 获取信息 ====

std::string ModbusManager::getIp() const {
	std::lock_guard<std::mutex> lock(mutex);
	return currentIp;
}

int ModbusManager::getPort() const {
	std::lock_guard<std::mutex> lock(mutex);
	return currentPort;
}

int ModbusManager::getSlaveId() const {
	std::lock_guard<std::mutex> lock(mutex);
	return currentSlaveId;
}

ModbusManager::~ModbusManager() {
	close();
}