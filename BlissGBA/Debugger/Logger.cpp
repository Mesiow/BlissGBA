#include "Logger.h"
#include "../Cpu/Arm.h"

Logger::Logger(Arm& cpu)
	:cpu(cpu)
{

}

void Logger::createAndOpenFile(const std::string& fileName)
{
	if (!file.is_open()) {
		std::string abs_path = "Debugger/logs/" + fileName + ".txt";
		file.open(abs_path, std::ios::out | std::ios::app);
		if (!file.is_open()) {
			std::cerr << "File <" << fileName << "> failed to open\n";
			return;
		}
		else {
			std::cout << "File <" << fileName << "> successfully created and opened for writing\n";
		}
	}
}

void Logger::writeFile()
{
	if (file.is_open()) {
		if (cpu.getState() == State::ARM) {
			u32 opcode = cpu.currentExecutingArmOpcode;
			file << "Opcode: " + intToHexString(opcode) + " ";
		}
		else {
			u16 opcode = cpu.currentExecutingThumbOpcode;
			file << "Opcode: " + intToHexString(opcode) + " ";
		}

		for (int i = 0; i < NUM_REGISTERS; i++) {
			file << "R" + std::to_string(i) + ": "
				+ intToHexString(cpu.registers[i].value) + " ";
		}
		file << "R13: " + intToHexString(cpu.SP) + " ";
		file << "R14: " + intToHexString(cpu.LR) + " ";
		file << "R15: " + intToHexString(cpu.R15) + " ";
		file << "CPSR: " + intToHexString(cpu.CPSR) + " ";
		file << "SPSR: " + intToHexString(cpu.SPSR) + "\n";
	}
}

void Logger::closeFile()
{
	if (file.is_open()) {
		file.close();
	}
}

std::string Logger::intToHexString(u32 value)
{
	std::stringstream ss;
	ss << std::hex << value;
	std::string lower = ss.str();
	for (int i = 0; i < lower.length(); i++) {
		lower[i] = toupper(lower[i]);
	}
	std::string res = "0x" + lower;

	return res;
}
