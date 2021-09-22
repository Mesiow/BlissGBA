#pragma once
#include <iostream>
#include <fstream>

struct Logger {
	void createAndOpenFile(const std::string &fileName);
	void writeFile();
	void closeFile();

	std::ofstream file;
};