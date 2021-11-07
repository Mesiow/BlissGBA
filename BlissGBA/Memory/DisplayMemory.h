#pragma once
#include "../Utils.h"

#define BG_OBJ_PALETTE_SIZE 0x400
#define VRAM_SIZE 0x18000
#define OAM_SIZE 0x400

#define VRAM_START_ADDR 0x06000000
#define VRAM_END_ADDR 0x06017FFF
#define PRAM_START_ADDR 0x05000000
#define PRAM_END_ADDR 0x050003FF
#define OAM_START_ADDR 0x07000000
#define OAM_END_ADDR 0x070003FF

class DisplayMemory {
public:
	void zero();
	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u32 value);

	u8 readU8(u32 address);
	u16 readU16(u32 address);
	u32 readU32(u32 address);

	u8 pram[BG_OBJ_PALETTE_SIZE];
	u8 vram[VRAM_SIZE];
	u8 oam[OAM_SIZE];
};