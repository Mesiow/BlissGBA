#include "DisplayMemory.h"

void DisplayMemory::zero()
{
	std::fill(pram, pram + BG_OBJ_PALETTE_SIZE, 0x00);
	std::fill(vram, vram + VRAM_SIZE, 0x00);
	std::fill(oam, oam + OAM_SIZE, 0x00);
}

void DisplayMemory::writeU8(u32 address, u8 value)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address - PRAM_START_ADDR;
		pram[addr] = value;
	}
	//Pram memory mirror
	else if (address >= PRAM_MIRROR_START_ADDR && address <= PRAM_MIRROR_END_ADDR) {
		u32 addr = (address - BG_OBJ_PALETTE_SIZE) - PRAM_START_ADDR;
		pram[addr] = value;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
		vram[addr] = value;
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address - OAM_START_ADDR;
		oam[addr] = value;
	}
	//Oam memory mirror
	else if (address >= OAM_MIRROR_START_ADDR && address <= OAM_MIRROR_END_ADDR) {
		u32 addr = (address - OAM_SIZE) - OAM_START_ADDR;
		oam[addr] = value;
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

void DisplayMemory::writeU32(u32 address, u32 value)
{
	u8 upper2, upper1;
	u8 lower2, lower1;

	lower1 = value & 0xFF;
	lower2 = (value >> 8) & 0xFF;
	upper1 = (value >> 16) & 0xFF;
	upper2 = (value >> 24) & 0xFF;

	writeU8(address, lower1);
	writeU8(address + 1, lower2);
	writeU8(address + 2, upper1);
	writeU8(address + 3, upper2);
}

u8 DisplayMemory::readU8(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address - PRAM_START_ADDR;
		return pram[addr];
	}
	//Pram memory mirror
	else if (address >= PRAM_MIRROR_START_ADDR && address <= PRAM_MIRROR_END_ADDR) {
		u32 addr = (address - BG_OBJ_PALETTE_SIZE) - PRAM_START_ADDR;
		return pram[addr];
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
		return vram[addr];
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address - OAM_START_ADDR;
		return oam[addr];
	}
	//Oam memory mirror
	else if (address >= OAM_MIRROR_START_ADDR && address <= OAM_MIRROR_END_ADDR) {
		u32 addr = (address - OAM_SIZE) - OAM_START_ADDR;
		return oam[addr];
	}
}

u16 DisplayMemory::readU16(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address - PRAM_START_ADDR;
		u16 value = readPramU16(addr);

		return value;
	}
	//Pram memory mirror
	else if (address >= PRAM_MIRROR_START_ADDR && address <= PRAM_MIRROR_END_ADDR) {
		u32 addr = (address - BG_OBJ_PALETTE_SIZE) - PRAM_START_ADDR;
		u16 value = readPramU16(addr);

		return value;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
		u16 value = readVramU16(addr);

		return value;
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address - OAM_START_ADDR;
		u16 value = readOamU16(addr);
		
		return value;
	}
	//Oam memory mirror
	else if (address >= OAM_MIRROR_START_ADDR && address <= OAM_MIRROR_END_ADDR) {
		u32 addr = (address - OAM_SIZE) - OAM_START_ADDR;
		u16 value = readOamU16(addr);

		return value;
	}
}

u32 DisplayMemory::readU32(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address - PRAM_START_ADDR;
		u32 value = readPramU32(addr);

		return value;
	}
	//Pram memory mirror
	else if (address >= PRAM_MIRROR_START_ADDR && address <= PRAM_MIRROR_END_ADDR) {
		u32 addr = (address - BG_OBJ_PALETTE_SIZE) - PRAM_START_ADDR;
		u32 value = readPramU32(addr);

		return value;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
		u32 value = readVramU32(addr);

		return value;
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address - OAM_START_ADDR;
		u32 value = readOamU32(addr);
		
		return value;
	}
	//Oam memory mirror
	else if (address >= OAM_MIRROR_START_ADDR && address <= OAM_MIRROR_END_ADDR) {
		u32 addr = (address - OAM_SIZE) - OAM_START_ADDR;
		u32 value = readOamU32(addr);

		return value;
	}
}

u16 DisplayMemory::readPramU16(u32 address)
{
	u8 lo, hi;
	lo = pram[address];
	hi = pram[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u16 DisplayMemory::readVramU16(u32 address)
{
	u8 lo, hi;
	lo = vram[address];
	hi = vram[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u16 DisplayMemory::readOamU16(u32 address)
{
	u8 lo, hi;
	lo = oam[address];
	hi = oam[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u32 DisplayMemory::readPramU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = pram[address];
	byte2 = pram[address + 1];
	byte3 = pram[address + 2];
	byte4 = pram[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}

u32 DisplayMemory::readVramU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = vram[address];
	byte2 = vram[address + 1];
	byte3 = vram[address + 2];
	byte4 = vram[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}

u32 DisplayMemory::readOamU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = oam[address];
	byte2 = oam[address + 1];
	byte3 = oam[address + 2];
	byte4 = oam[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}
