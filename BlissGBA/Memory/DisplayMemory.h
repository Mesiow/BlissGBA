#pragma once
#include "../Utils.h"

#define BG_OBJ_PALETTE_SIZE 0x400
#define VRAM_SIZE 0x18000
#define OAM_SIZE 0x400

#define PRAM_START_ADDR 0x05000000
#define PRAM_END_ADDR 0x050003FF
#define VRAM_START_ADDR 0x06000000
#define VRAM_END_ADDR 0x06017FFF
#define OAM_START_ADDR 0x07000000
#define OAM_END_ADDR 0x070003FF

#define PRAM_MIRROR_START_ADDR 0x5000400
#define PRAM_MIRROR_END_ADDR 0x50007FF
#define VRAM_MIRROR_START_ADDR 0x6018000
#define VRAM_MIRROR_END_ADDR 0x6FFFFFF
#define OAM_MIRROR_START_ADDR 0x7000400
#define OAM_MIRROR_END_ADDR 0x70007FF

class DisplayMemory {
public:
	void zero();
	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u32 value);

	u8 readU8(u32 address);
	u16 readU16(u32 address);
	u32 readU32(u32 address);

	u16 readPramU16(u32 address);
	u16 readVramU16(u32 address);
	u16 readOamU16(u32 address);

	u32 readPramU32(u32 address);
	u32 readVramU32(u32 address);
	u32 readOamU32(u32 address);

	u8 pram[BG_OBJ_PALETTE_SIZE];
	u8 vram[VRAM_SIZE];
	u8 oam[OAM_SIZE];
};