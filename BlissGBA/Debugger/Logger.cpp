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

Comparer::Comparer(Arm& cpu)
	:cpu(cpu)
{

}

void Comparer::openExistingFile(const std::string& fileName)
{
	if (!file.is_open()) {
		file.open(fileName, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "File <" << fileName << "> failed to open\n";
			return;
		}
		else {
			std::cout << "File <" << fileName << "> successfully opened\n";

			file.seekg(0, std::ios::end);
			u32 file_length = file.tellg();
			file.seekg(0, std::ios::beg);

			fileBuffer = new u32[file_length];
			file.read((char*)fileBuffer, file_length);

			state = reinterpret_cast<ExpectedState*>(fileBuffer);
			////Skip first 2 instructions in log
			state += 2;
			for (int i = 0; i < 13; i++) {
				printf("0x%08X\n", state->regs[i]);
			}
			printf("0x%08X\n", state->sp);
			printf("0x%08X\n", state->lr);
			printf("0x%08X\n", state->r15);
			printf("0x%08X\n", state->cpsr);
			printf("0x%08X\n", state->spsr);
		}
	}
}

void Comparer::compareAgainstFile()
{
	u32 state_address;
	if (cpu.getState() == State::ARM)
		state_address = cpu.R15 - 8;
	else
		state_address = cpu.R15 - 4;

	for (s32 i = 0; i < 13; i++) {
		if (cpu.registers[i].value != state->regs[i]) {
			if (cpu.getState() == State::ARM) {
				printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
				printf("Register %d does not match\n", i);
				printf("Our value: 0x%08X\n", cpu.getRegister(RegisterID{ i }));
				printf("Logs value: 0x%08X\n", state->regs[i]);
				assert(false);
			}
			else {
				printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
				printf("Register %d does not match\n", i);
				printf("Our value: 0x%08X\n", cpu.getRegister(RegisterID{ i }));
				printf("Logs value: 0x%08X\n", state->regs[i]);
				assert(false);
			}
			
		}
	}

	u32 r13 = cpu.getRegister(RegisterID{ R13_ID });
	if (r13 != state->sp) {
		printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
		printf("Register 13(SP) does not match\n");
		printf("Our value: 0x%08X\n", r13);
		printf("Logs value: 0x%08X\n", state->sp);
		assert(false);
	}
	u32 r14 = cpu.getRegister(RegisterID{ R14_ID });
	if (r14 != state->lr) {
		printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
		printf("Register 14(LR) does not match\n");
		printf("Our value: 0x%08X\n", r14);
		printf("Logs value: 0x%08X\n", state->lr);
		assert(false);
	}
	if (cpu.R15 != state->r15) {
		printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
		printf("Register 15(R15) does not match\n");
		printf("Our value: 0x%08X\n", cpu.R15);
		printf("Logs value: 0x%08X\n", state->r15);
		assert(false);
	}
	if (cpu.CPSR != state->cpsr) {
		printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
		printf("CPSR) does not match\n");
		printf("Our value: 0x%08X\n", cpu.CPSR);
		printf("Logs value: 0x%08X\n", state->cpsr);
		assert(false);
	}
	u32 spsr = cpu.getSPSR();
	if (spsr != state->spsr) {
		printf("!!!CPU State Fails To Match Log!!! at address 0x%08X\n", state_address);
		printf("SPSR does not match\n");
		printf("Our value: 0x%08X\n", spsr);
		printf("Logs value: 0x%08X\n", state->spsr);
		assert(false);
	}

	state += 1;
}

void Comparer::closeFile()
{
	if (file.is_open()) {
		file.close();
	}

	if (fileBuffer != nullptr) delete fileBuffer;
}
