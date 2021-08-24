#include "DisplayMemory.h"

void DisplayMemory::zero()
{
	std::fill(bgpalette, bgpalette + BG_OBJ_PALETTE_SIZE, 0x00);
	std::fill(vram, vram + VRAM_SIZE, 0x00);
	std::fill(oam, oam + OAM_SIZE, 0x00);
}

void DisplayMemory::writeU8(u32 address, u8 value)
{
	if (address >= VRAM_START && address <= VRAM_END) {
		u32 addr = address - VRAM_START;
		vram[addr] = value;
	}
}

void DisplayMemory::writeU16(u32 address, u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	writeU8(address, lo);
	writeU8(address + 1, hi);
}

void DisplayMemory::writeU32(u32 address, u16 value)
{
}

u8 DisplayMemory::readU8(u32 address)
{
	if (address >= VRAM_START && address <= VRAM_END) {
		u32 addr = address - VRAM_START;
		return vram[addr];
	}
}

u16 DisplayMemory::readU16(u32 address)
{
	if (address >= VRAM_START && address <= VRAM_END) {
		u32 addr = address - VRAM_START;

		u8 lo, hi;
		lo = vram[addr];
		hi = vram[addr + 1];

		u16 value = (hi << 8) | lo;

		return value;
	}
}

u32 DisplayMemory::readU32(u32 address)
{
	return u32();
}
