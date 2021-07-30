#pragma once
#include "../Utils.h"

#define GAMEPAK_WS_SIZE 0x2000000 //game pak rom wait state
#define GAMEPAK_WS0_START_ADDR 0x8000000
#define GAMEPAK_WS0_END_ADDR 0x9FFFFFF
#define GAMEPAK_WS1_START_ADDR 0xA000000
#define GAMEPAK_WS1_END_ADDR 0xBFFFFFF
#define GAMEPAK_WS2_START_ADDR 0xC000000
#define GAMEPAK_WS2_END_ADDR 0xDFFFFFF

#define GAMEPAK_SRAM_SIZE 0x10000
#define GAMEPAK_SRAM_START_ADDR 0xE000000
#define GAMEPAK_SRAM_END_ADDR 0xE00FFFF

class GamePak {
public:
	GamePak();
	~GamePak();
	void load(const std::string& fileName);
	u8* getGamePakWS0() { return &gamepakWS0[0]; }

private:
	void parseHeader(u32 size);
	void zeroMemory();

public:
	std::string title;
	std::string romSize;

private:
	u8* gamepakWS0;
	u8* gamepakWS1;
	u8* gamepakWS2;
	u8* gamepakSRAM;
};