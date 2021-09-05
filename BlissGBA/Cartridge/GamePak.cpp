#include "GamePak.h"

GamePak::GamePak()
	:gamepakWS0(nullptr),
	gamepakWS1(nullptr),
	gamepakWS2(nullptr),
	gamepakSRAM(nullptr)
{ 
	
}	

GamePak::~GamePak()
{
	zeroMemory();
}

void GamePak::load(const std::string& fileName)
{
	zeroMemory();

	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		u32 size = file.tellg();
		file.seekg(0, file.beg);
		
		if (size > GAMEPAK_WS_SIZE * 3) {
			std::cerr << "GamePak file too large!" << std::endl;
			return;
		}

		bool loaded = false;
		if (size <= GAMEPAK_WS_SIZE) {
			loaded = true;
			//wait state 0
			gamepakWS0 = new u8[GAMEPAK_WS_SIZE];
			file.read((char*)gamepakWS0, GAMEPAK_WS_SIZE);
		}
		else if (size > GAMEPAK_WS_SIZE && size < GAMEPAK_WS_SIZE * 2) {
			loaded = true;
			//wait state 1
		}
		else if (size > GAMEPAK_WS_SIZE * 2 && size < GAMEPAK_WS_SIZE * 3) {
			loaded = true;
			//wait state 3
		}

		if (loaded)
			parseHeader(size);

		file.close();
	}
}

void GamePak::writeU8(u32 address, u8 value)
{
}

void GamePak::writeU16(u32 address, u16 value)
{
}

void GamePak::writeU32(u32 address, u32 value)
{
}

u8 GamePak::readU8(u32 address)
{
	if (address >= GAMEPAK_WS0_START_ADDR && address <= GAMEPAK_WS0_END_ADDR) {
		u32 addr = address - GAMEPAK_WS0_START_ADDR;
		return gamepakWS0[addr];
	}
}

u16 GamePak::readU16(u32 address)
{
	if (address >= GAMEPAK_WS0_START_ADDR && address <= GAMEPAK_WS0_END_ADDR) {
		u32 addr = address - GAMEPAK_WS0_START_ADDR;

		u8 lo, hi;
		lo = gamepakWS0[addr];
		hi = gamepakWS0[addr + 1];

		u16 value = (hi << 8) | lo;

		return value;
	}
}

void GamePak::parseHeader(u32 size)
{
	std::string sizeStr = std::to_string(size);
	if (size == GAMEPAK_WS_SIZE) romSize = "32MB";

	//byte
	if (size < 1000) {
		romSize = "0." + sizeStr + "bytes";
	}
	else{
		//kb
		if (sizeStr.length() == 4) {
			sizeStr.erase(2, 3);
			sizeStr.insert(1, ".");
			romSize = sizeStr + "KB";
		}
		else if (sizeStr.length() == 5) {
			sizeStr.erase(3, 4);
			sizeStr.insert(2, ".");
			romSize = sizeStr + "KB";
		}
		//mb
		else if (sizeStr.length() == 7 || sizeStr.length() == 8) {
			//convert to mebibytes
			size /= 1048576;
			sizeStr = std::to_string(size);
			romSize = sizeStr + "MB";
		}
	}

	for (s32 i = 0; i < 12; i++) {
		title.push_back((char)gamepakWS0[i + 0xA0]);
	}
}

void GamePak::zeroMemory()
{
	if (gamepakWS0) delete gamepakWS0;
	if (gamepakWS1) delete gamepakWS1;
	if (gamepakWS2) delete gamepakWS2;
	if (gamepakSRAM) delete gamepakSRAM;

	gamepakWS0 = nullptr;
	gamepakWS1 = nullptr;
	gamepakWS2 = nullptr;
	gamepakSRAM = nullptr;
}
