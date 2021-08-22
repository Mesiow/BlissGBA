#pragma once
#include "../Utils.h"

#define PPU_WIDTH 240
#define PPU_HEIGHT 160

struct BitmapMode3 {
	static constexpr u8 bpp = 2; //2 bytes per pixel
	u8 framebuffer[PPU_WIDTH * PPU_HEIGHT * bpp];
};

class Ppu {
	void update(s32 cycles);
	void bufferPixels();

	BitmapMode3 mode3;
};