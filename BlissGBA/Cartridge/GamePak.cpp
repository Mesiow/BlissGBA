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
			gamepakWS0 = new u8[GAMEPAK_WS_SIZE];
			gamepakSRAM = new u8[GAMEPAK_SRAM_SIZE];
			file.read((char*)gamepakWS0, GAMEPAK_WS_SIZE);
		}

		if (loaded)
			parseHeader(size);

		file.close();
	}
	else {
		std::cerr << "Rom/File <" << fileName << " failed to open\n";
	}
}

void GamePak::writeU8(u32 address, u8 value)
{
	if (address >= GAMEPAK_SRAM_START_ADDR && address <= GAMEPAK_SRAM_END_ADDR) {
		u32 addr = address & (GAMEPAK_SRAM_SIZE - 1);
		gamepakSRAM[addr] = value;
	}
}

void GamePak::writeU16(u32 address, u16 value)
{

}

void GamePak::writeU32(u32 address, u32 value)
{

}

u8 GamePak::readU8(u32 address)
{
	if (address >= GAMEPAK_WS0_START_ADDR && address <= GAMEPAK_WS2_END_ADDR) {
		u32 addr = address & (GAMEPAK_WS_SIZE - 1);
		return gamepakWS0[addr];
	}
	else if (address >= GAMEPAK_SRAM_START_ADDR && address <= GAMEPAK_SRAM_END_ADDR) {
		u32 addr = address & (GAMEPAK_SRAM_SIZE - 1);
		//stub flash for now
		if (address == 0x0E000000)
			return 0x62;
		else if (address == 0x0E000001)
			return 0x13;
	
		return gamepakSRAM[addr];
	}
}

u16 GamePak::readU16(u32 address)
{
	if (address >= GAMEPAK_WS0_START_ADDR && address <= GAMEPAK_WS2_END_ADDR) {
		u32 addr = address & (GAMEPAK_WS_SIZE - 1);

		u8 lo = gamepakWS0[addr];
		u8 hi = gamepakWS0[addr + 1];
		u16 value = (hi << 8) | lo;

		return value;
	}
}

u32 GamePak::readU32(u32 address)
{
	if (address >= GAMEPAK_WS0_START_ADDR && address <= GAMEPAK_WS2_END_ADDR) {
		u32 addr = address & (GAMEPAK_WS_SIZE - 1);

		u8 byte1 = gamepakWS0[addr];
		u8 byte2 = gamepakWS0[addr + 1];
		u8 byte3 = gamepakWS0[addr + 2];
		u8 byte4 = gamepakWS0[addr + 3];

		u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

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

void writeSRAMToDisk(u8* sram, u8 length)
{
	std::ofstream file;
	file.open("game.sav", std::ios::binary);
	if (file.is_open()) {
		file.write((const char*)sram, length);
	}
	else {
		std::cerr << "--Writing SRAM to Disk failed--" << std::endl;;
	}
}
