#include "GeneralMemory.h"

void GeneralMemory::loadBios(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		u32 size = file.tellg();
		file.seekg(0, file.beg);

		if (size > BIOS_SIZE) {
			std::cerr << "Bios file too large!" << std::endl;
			return;
		}

		file.read((char*)bios, BIOS_SIZE);
		file.close();
	}
}

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
		u32 addr = address & (OB_WRAM_SIZE - 1);
		obwram[addr] = value;
	}
	else if (address >= OC_WRAM_START_ADDR && address <= OC_WRAM_END_ADDR) {
		u32 addr = address & (OC_WRAM_SIZE - 1);
		ocwram[addr] = value;
	}
	else if (address >= IO_START_ADDR && address <= IO_END_ADDR) {
		u32 addr = address & (IO_SIZE - 1);
		io[addr] = value;
	}
}

void GeneralMemory::writeU16(u32 address, u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	writeU8(address, lo);
	writeU8(address + 1, hi);
}

void GeneralMemory::writeU32(u32 address, u32 value)
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

u8 GeneralMemory::readU8(u32 address)
{
	if (address < BIOS_SIZE)
		return bios[address];

	if (address >= OB_WRAM_START_ADDR && address <= OB_WRAM_END_ADDR) {
		u32 addr = address & (OB_WRAM_SIZE - 1);
		return obwram[addr];
	}
	else if (address >= OC_WRAM_START_ADDR && address <= OC_WRAM_END_ADDR) {
		u32 addr = address & (OC_WRAM_SIZE - 1);
		return ocwram[addr];
	}
	else if (address >= IO_START_ADDR && address <= IO_END_ADDR) {
		u32 addr = address & (IO_SIZE - 1);
		return io[addr];
	}
}

u16 GeneralMemory::readU16(u32 address)
{
	if (address >= OB_WRAM_START_ADDR && address <= OB_WRAM_END_ADDR) {
		u32 addr = address & (OB_WRAM_SIZE - 1);
		u16 value = readOBWramU16(addr);

		return value;
	}
	else if (address >= OC_WRAM_START_ADDR && address <= OC_WRAM_END_ADDR) {
		u32 addr = address & (OC_WRAM_SIZE - 1);
		u16 value = readOCWramU16(addr);

		return value;
	}
	else if (address >= IO_START_ADDR && address <= IO_END_ADDR) {
		u32 addr = address & (IO_SIZE - 1);
		u16 value = readIOU16(addr);

		return value;
	}
}

u32 GeneralMemory::readU32(u32 address)
{
	if (address >= OB_WRAM_START_ADDR && address <= OB_WRAM_END_ADDR) {
		u32 addr = address & (OB_WRAM_SIZE - 1);
		u32 value = readOBWramU32(addr);

		return value;
	}
	else if (address >= OC_WRAM_START_ADDR && address <= OC_WRAM_END_ADDR) {
		u32 addr = address & (OC_WRAM_SIZE - 1);
		u32 value = readOCWramU32(addr);

		return value;
	}
	else if (address >= IO_START_ADDR && address <= IO_END_ADDR) {
		u32 addr = address & (IO_SIZE - 1);
		u32 value = readIOU32(addr);

		return value;
	}
}

u16 GeneralMemory::readOBWramU16(u32 address)
{
	u8 lo, hi;
	lo = obwram[address];
	hi = obwram[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u16 GeneralMemory::readOCWramU16(u32 address)
{
	u8 lo, hi;
	lo = ocwram[address];
	hi = ocwram[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u16 GeneralMemory::readIOU16(u32 address)
{
	u8 lo, hi;
	lo = io[address];
	hi = io[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u32 GeneralMemory::readOBWramU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = obwram[address];
	byte2 = obwram[address + 1];
	byte3 = obwram[address + 2];
	byte4 = obwram[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}

u32 GeneralMemory::readOCWramU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = ocwram[address];
	byte2 = ocwram[address + 1];
	byte3 = ocwram[address + 2];
	byte4 = ocwram[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}

u32 GeneralMemory::readIOU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = io[address];
	byte2 = io[address + 1];
	byte3 = io[address + 2];
	byte4 = io[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}
