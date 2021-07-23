#pragma once

/*

General Internal Memory
  00000000-00003FFF   BIOS - System ROM         (16 KBytes)
  00004000-01FFFFFF   Not used
  02000000-0203FFFF   WRAM - On-board Work RAM  (256 KBytes) 2 Wait
  02040000-02FFFFFF   Not used
  03000000-03007FFF   WRAM - On-chip Work RAM   (32 KBytes)
  03008000-03FFFFFF   Not used
  04000000-040003FE   I/O Registers
  04000400-04FFFFFF   Not used
Internal Display Memory
  05000000-050003FF   BG/OBJ Palette RAM        (1 Kbyte)
  05000400-05FFFFFF   Not used
  06000000-06017FFF   VRAM - Video RAM          (96 KBytes)
  06018000-06FFFFFF   Not used
  07000000-070003FF   OAM - OBJ Attributes      (1 Kbyte)
  07000400-07FFFFFF   Not used
External Memory (Game Pak)
  08000000-09FFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 0
  0A000000-0BFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 1
  0C000000-0DFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 2
  0E000000-0E00FFFF   Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
  0E010000-0FFFFFFF   Not used
Unused Memory Area
  10000000-FFFFFFFF   Not used (upper 4bits of address bus unused)

*/
#include "GeneralMemory.h"
#include "DisplayMemory.h"
#include "../Cartridge/GamePak.h"

#define GENERAL_MEM_END 0x5000000
#define DISPLAY_MEM_END 0x70003FF
#define EXTERNAL_MEM_START 0x8000000
#define EXTERNAL_MEM_END 0xE00FFFF

class MemoryBus {
public:
	MemoryBus();
	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u16 value);

	u8 readU8(u32 address);
	u16 readU16(u32 address);
	u32 readU32(u32 address);

	bool isAlignedU16(u32 address);
	bool isAlignedU32(u32 address);

	u8* getBiosMemory() { return genMem.getBios(); }
	u8* getGamePakMemory() { return pak.getGamePakWS0(); }

private:
	GeneralMemory genMem;
	DisplayMemory displayMem;
	GamePak pak;

};