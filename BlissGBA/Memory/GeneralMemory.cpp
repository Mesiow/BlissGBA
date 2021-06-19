#include "GeneralMemory.h"

void GeneralMemory::zero()
{
	std::fill(bios, bios + BIOS_SIZE, 0x00);
	std::fill(obwram, obwram + OB_WRAM_SIZE, 0x00);
	std::fill(ocwram, ocwram + OC_WRAM_SIZE, 0x00);
	std::fill(io, io + IO_SIZE, 0x00);
}

void GeneralMemory::writeU8(u32 address, u8 value)
{
	//Can't write to bios rom
	if (address < BIOS_SIZE)
		return;

	if (address >= OB_WRAM_START_ADDR && address <= OB_WRAM_END_ADDR) {

	}
	else if (address >= OC_WRAM_START_ADDR && address <= OC_WRAM_END_ADDR) {

	}
	else if (address >= IO_START_ADDR && address <= IO_END_ADDR) {

	}
}

void GeneralMemory::writeU16(u32 address, u16 value)
{
}

void GeneralMemory::writeU32(u32 address, u16 value)
{
}

u8 GeneralMemory::readU8(u32 address)
{
	return u8();
}

u16 GeneralMemory::readU16(u32 address)
{
	return u16();
}

u32 GeneralMemory::readU32(u32 address)
{
	return u32();
}
