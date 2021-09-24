#pragma once
#include <sstream>
#include "../Utils.h"

class Arm;

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