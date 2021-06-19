#include "DisplayMemory.h"

void DisplayMemory::zero()
{
	std::fill(bgpalette, bgpalette + BG_OBJ_PALETTE_SIZE, 0x00);
	std::fill(vram, vram + VRAM_SIZE, 0x00);
	std::fill(oam, oam + OAM_SIZE, 0x00);
}

void DisplayMemory::writeU8(u32 address, u8 value)
{
}

void DisplayMemory::writeU16(u32 address, u16 value)
{
}

void DisplayMemory::writeU32(u32 address, u16 value)
{
}

u8 DisplayMemory::readU8(u32 address)
{
	return u8();
}

u16 DisplayMemory::readU16(u32 address)
{
	return u16();
}

u32 DisplayMemory::readU32(u32 address)
{
	return u32();
}
