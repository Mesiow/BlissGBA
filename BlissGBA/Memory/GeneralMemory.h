#pragma once
#include "../Utils.h"

#define BIOS_SIZE 0x4000 //16KB
#define OB_WRAM_SIZE 0x40000 //256KB
#define OC_WRAM_SIZE 0x8000 //32KB
#define IO_SIZE 0x3FF

#define OB_WRAM_START_ADDR 0x2000000
#define OB_WRAM_END_ADDR 0x203FFFF

#define OC_WRAM_START_ADDR 0x3000000
#define OC_WRAM_END_ADDR 0x3007FFF
#define IO_START_ADDR 0x4000000
#define IO_END_ADDR 0x40003FE

class GeneralMemory {
public:
	void loadBios(const std::string& fileName);

	void zero();
	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u16 value);

	u8 readU8(u32 address);
	u16 readU16(u32 address);
	u32 readU32(u32 address);


	u8 bios[BIOS_SIZE];
	u8 obwram[OB_WRAM_SIZE];
	u8 ocwram[OC_WRAM_SIZE];
	u8 io[IO_SIZE];
};
