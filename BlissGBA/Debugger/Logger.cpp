#include "Logger.h"

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
		//write data(registers, cpu state) to file
	}
}

void Logger::closeFile()
{
	if (file.is_open()) {
		file.close();
	}
}
