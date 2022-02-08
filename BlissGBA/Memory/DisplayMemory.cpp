#include "DisplayMemory.h"

DisplayMemory::DisplayMemory()
{
	zero();
}

void DisplayMemory::zero()
{
	std::fill(pram, pram + BG_OBJ_PALETTE_SIZE, 0x00);
	std::fill(vram, vram + VRAM_SIZE, 0x00);
	std::fill(oam, oam + OAM_SIZE, 0x00);
}

void DisplayMemory::writeU8(u32 address, u8 value)
{
	//If 8 bit writes to vram 6000000 - 600FFFF or 6000000 - 6013FFF or to
	//palette ram, then write the new 8 bit value to both upper and lower 8 bits of the address
	
	if (address >= PRAM_START_ADDR && address <= (PRAM_START_ADDR + 0x3FF)) {
		u32 addr = address & (BG_OBJ_PALETTE_SIZE - 1);
		
		//write 8 bit value to halfword address
		pram[addr] = value;
		pram[addr + 1] = value;
	}	
	else if (address >= VRAM_START_ADDR && address <= (VRAM_START_ADDR + 0x13FFF)) {
		u32 vram_addr = address & 0x1FFFF;
		if (vram_addr > (VRAM_SIZE - 1))
			vram_addr -= 0x8000;

		vram[vram_addr] = value;
		vram[vram_addr + 1] = value;
	}
	//Ignore writes to 7000000 - 70003FF
	else if (address >= (OAM_START_ADDR + 0x400) && address <= OAM_END_ADDR) {
		u32 addr = address & (OAM_SIZE - 1);

		oam[addr] = value;
		oam[addr + 1] = value;
	}
}

void DisplayMemory::writeU16(u32 address, u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address & (BG_OBJ_PALETTE_SIZE - 1);

		pram[addr] = lo;
		pram[addr + 1] = hi;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR_MIRROR) {
		u32 vram_addr = address & 0x1FFFF;
		if (vram_addr > (VRAM_SIZE - 1))
			vram_addr -= 0x8000;

		vram[vram_addr] = lo;
		vram[vram_addr + 1] = hi;
	}
	else if (address >= address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address & (OAM_SIZE - 1);

		oam[addr] = lo;
		oam[addr + 1] = hi;
	}
}

void DisplayMemory::writeU32(u32 address, u32 value)
{
	u8 upper2, upper1;
	u8 lower2, lower1;

	lower1 = value & 0xFF;
	lower2 = (value >> 8) & 0xFF;
	upper1 = (value >> 16) & 0xFF;
	upper2 = (value >> 24) & 0xFF;

	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address & (BG_OBJ_PALETTE_SIZE - 1);
		pram[addr] = lower1;
		pram[addr + 1] = lower2;
		pram[addr + 2] = upper1;
		pram[addr + 3] = upper2;
	}
    else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR) {
		u32 addr = address - VRAM_START_ADDR;
		vram[addr] = lower1;
		vram[addr + 1] = lower2;
		vram[addr + 2] = upper1;
		vram[addr + 3] = upper2;
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address & (OAM_SIZE - 1);
		oam[addr] = lower1;
		oam[addr + 1] = lower2;
		oam[addr + 2] = upper1;
		oam[addr + 3] = upper2;
	}
}

u8 DisplayMemory::readU8(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address & (BG_OBJ_PALETTE_SIZE - 1);
		return pram[addr];
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR_MIRROR) {
		u32 vram_addr = address & 0x1FFFF;
		if (vram_addr > (VRAM_SIZE - 1))
			vram_addr -= 0x8000;

		return vram[vram_addr];
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address & (OAM_SIZE - 1);
		return oam[addr];
	}
}

u16 DisplayMemory::readU16(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address & (BG_OBJ_PALETTE_SIZE - 1);
		u16 value = readPramU16(addr);

		return value;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR_MIRROR) {
		u32 vram_addr = address & 0x1FFFF;
		if (vram_addr > (VRAM_SIZE - 1))
			vram_addr -= 0x8000;

		u16 value = readVramU16(vram_addr);

		return value;
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address & (OAM_SIZE - 1);
		u16 value = readOamU16(addr);
		
		return value;
	}
}

u32 DisplayMemory::readU32(u32 address)
{
	if (address >= PRAM_START_ADDR && address <= PRAM_END_ADDR) {
		u32 addr = address & (BG_OBJ_PALETTE_SIZE - 1);
		u32 value = readPramU32(addr);

		return value;
	}
	else if (address >= VRAM_START_ADDR && address <= VRAM_END_ADDR_MIRROR) {
		u32 vram_addr = address & 0x1FFFF; 
		if (vram_addr > (VRAM_SIZE - 1)) //if greater than 96k, mirror the previous 32k block
			vram_addr -= 0x8000;

		u32 value = readVramU32(vram_addr);

		return value;
	}
	else if (address >= OAM_START_ADDR && address <= OAM_END_ADDR) {
		u32 addr = address & (OAM_SIZE - 1);
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