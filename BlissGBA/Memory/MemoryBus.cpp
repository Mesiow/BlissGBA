#include "MemoryBus.h"

MemoryBus::MemoryBus()
{
	genMem.zero();
	displayMem.zero();

	genMem.loadBios("roms/gba_bios.bin");
}

void MemoryBus::writeU8(u32 address, u8 value)
{
	if (address < GENERAL_MEM_END) {
		genMem.writeU8(address, value);
	}
	else if (address >= GENERAL_MEM_END && address <= DISPLAY_MEM_END) {
		displayMem.writeU8(address, value);
	}
	else if (address >= EXTERNAL_MEM_START && address <= EXTERNAL_MEM_END) {
		//cartridge.writeU8(address, value);
	}
}

void MemoryBus::writeU16(u32 address, u16 value)
{
	if (!isAlignedU16(address)) {
		std::cerr << "--Unaligned memory write U16 attempted--" << std::endl;
		return;
	}

	if (address < GENERAL_MEM_END) {
		genMem.writeU16(address, value);
	}
	else if (address >= GENERAL_MEM_END && address <= DISPLAY_MEM_END) {
		displayMem.writeU16(address, value);
	}
	else if (address >= EXTERNAL_MEM_START && address <= EXTERNAL_MEM_END) {
		//cartridge.writeU16(address, value);
	}
}

void MemoryBus::writeU32(u32 address, u16 value)
{
	if (!isAlignedU32(address)) {
		std::cerr << "--Unaligned memory write U32 attempted--" << std::endl;
		return;
	}

	if (address < GENERAL_MEM_END) {
		genMem.writeU32(address, value);
	}
	else if (address >= GENERAL_MEM_END && address <= DISPLAY_MEM_END) {
		displayMem.writeU32(address, value);
	}
	else if (address >= EXTERNAL_MEM_START && address <= EXTERNAL_MEM_END) {
		//cartridge.writeU32(address, value);
	}
}

u8 MemoryBus::readU8(u32 address)
{
	return u8();
}

u16 MemoryBus::readU16(u32 address)
{
	return u16();
}

u32 MemoryBus::readU32(u32 address)
{
	return u32();
}

bool MemoryBus::isAlignedU16(u32 address)
{
	return ((address % 2) == 0);
}

bool MemoryBus::isAlignedU32(u32 address)
{
	return ((address % 4) == 0);
}