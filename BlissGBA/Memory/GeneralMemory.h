#pragma once
#include "../Utils.h"

#define BIOS_SIZE 0x4000 //16KB
#define OB_WRAM_SIZE 0x40000 //256KB
#define OC_WRAM_SIZE 0x8000 //32KB
#define IO_SIZE 0x3FF

class GeneralMemory {
private:
	u8 bios[BIOS_SIZE];
	u8 obwram[OB_WRAM_SIZE];
	u8 ocwram[OC_WRAM_SIZE];
	u8 io[IO_SIZE];
};
