#pragma once
#include "../Utils.h"

#define BG_OBJ_PALETTE_SIZE 0x400
#define VRAM_SIZE 0x18000
#define OAM_SIZE 0x400

class DisplayMemory {
public:
	void zero();
	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u16 value);

	u8 readU8(u32 address);
	u16 readU16(u32 address);
	u32 readU32(u32 address);

private:
	u8 bgpalette[BG_OBJ_PALETTE_SIZE];
	u8 vram[VRAM_SIZE];
	u8 oam[OAM_SIZE];
};