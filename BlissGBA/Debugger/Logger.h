#pragma once
#include <sstream>
#include "../Utils.h"

struct ExpectedState {
	u32 regs[13];
	u32 sp;
	u32 lr;
	u32 r15;
	u32 cpsr;
	u32 spsr;
};

class Arm;

struct Comparer {
	Comparer(Arm& cpu);
	void openExistingFile(const std::string& fileName);
	void compareAgainstFile();
	void closeFile();

	std::ifstream file;
	u32* fileBuffer = nullptr;
	ExpectedState* state = nullptr;
	Arm& cpu;
};

struct Logger {
	Logger(Arm& cpu);
	void createAndOpenFile(const std::string &fileName);
	void writeFile();
	void closeFile();

	inline bool isActive() { return file.is_open(); }
	std::string intToHexString(u32 value);

	std::ofstream file;
	Arm& cpu;
};