#pragma once
#include "../Utils.h"

#define BG_OBJ_PALETTE_SIZE 0x400
#define VRAM_SIZE 0x18000
#define OAM_SIZE 0x400

class DisplayMemory {
private:
	u8 bgpalette[BG_OBJ_PALETTE_SIZE];
	u8 vram[VRAM_SIZE];
	u8 oam[OAM_SIZE];
};