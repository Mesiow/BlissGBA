#include "GamePak.h"

GamePak::GamePak()
{
}

GamePak::~GamePak()
{
	if (gamepakWS0) delete gamepakWS0;
	if (gamepakWS1) delete gamepakWS1;
	if (gamepakWS2) delete gamepakWS2;
}

void GamePak::load(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		u32 size = file.tellg();
		file.seekg(0, file.beg);

		if (size > GAMEPAK_WS_SIZE * 3) {
			std::cerr << "GamePak file too large!" << std::endl;
			return;
		}

		if (size < GAMEPAK_WS_SIZE) {
			//wait state 0
			gamepakWS0 = new u8[GAMEPAK_WS_SIZE];
			file.read((char*)gamepakWS0, GAMEPAK_WS_SIZE);
		}
		else if (size > GAMEPAK_WS_SIZE && size < GAMEPAK_WS_SIZE * 2) {
			//wait state 1
		}
		else if (size > GAMEPAK_WS_SIZE * 2 && size < GAMEPAK_WS_SIZE * 3) {
			//wait state 3
		}
		file.close();
	}
}
