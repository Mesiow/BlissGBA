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
		u32 addr = address - OB_WRAM_START_ADDR;
		obwram[addr] = value;
	}
	else if (address >= OC_WRAM_START_ADDR && address <= OC_WRAM_END_ADDR) {
		u32 addr = address - OC_WRAM_START_ADDR;
		ocwram[addr] = value;
	}
	else if (address >= IO_START_ADDR && address <= IO_END_ADDR) {
		u32 addr = address - IO_START_ADDR;
		io[addr] = value;
	}
}

void GeneralMemory::writeU16(u32 address, u16 value)
{
}

void GeneralMemory::writeU32(u32 address, u16 value)
{
	//TODO: write u32 value to IO
}

u8 GeneralMemory::readU8(u32 address)
{
	if (address < BIOS_SIZE)
		return bios[address];

	return 0;
}

u16 GeneralMemory::readU16(u32 address)
{
	return u16();
}

u32 GeneralMemory::readU32(u32 address)
{
	return u32();
}
