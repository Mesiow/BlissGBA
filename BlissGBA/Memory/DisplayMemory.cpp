#include "DisplayMemory.h"

void DisplayMemory::zero()
{
	std::fill(pram, pram + BG_OBJ_PALETTE_SIZE, 0x00);
	std::fill(vram, vram + VRAM_SIZE, 0x00);
	std::fill(oam, oam + OAM_SIZE, 0x00);
}

void DisplayMemory::writeU8(u32 address, u8 value)
{
	if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
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
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address - PRAM_START_ADDR;
		return pram[addr];
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
		return vram[addr];
	}
}

u16 DisplayMemory::readU16(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address - PRAM_START_ADDR;

		u8 lo, hi;
		lo = pram[addr];
		hi = pram[addr + 1];

		u16 value = (hi << 8) | lo;

		return value;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;

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
