#pragma once
#include "Rtc.h"

#define GAMEPAK_WS_SIZE 0x2000000 //game pak rom wait state
#define GAMEPAK_WS0_START_ADDR 0x8000000
#define GAMEPAK_WS0_END_ADDR 0x9FFFFFF
#define GAMEPAK_WS1_START_ADDR 0xA000000
#define GAMEPAK_WS1_END_ADDR 0xBFFFFFF
#define GAMEPAK_WS2_START_ADDR 0xC000000
#define GAMEPAK_WS2_END_ADDR 0xDFFFFFF

#define GAMEPAK_SRAM_SIZE 0x8000
#define GAMEPAK_SRAM_START_ADDR 0xE000000
#define GAMEPAK_SRAM_END_ADDR 0xE007FFF


class GamePak {
public:
	GamePak(MemoryBus *mbus);
	~GamePak();
	void load(const std::string& fileName);
	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u32 value);

	u8 readU8(u32 address);
	u16 readU16(u32 address);
	u32 readU32(u32 address);
	u8* getGamePakWS0() { return &gamepakWS0[0]; }

	void parseHeader(u32 size);
	void zeroMemory();

	std::string title;
	std::string romSize;

	u8* gamepakWS0;
	u8* gamepakWS1;
	u8* gamepakWS2;
	u8* gamepakSRAM;

	RtcDevice rtc;
	bool has_rtc_chip = false;
};