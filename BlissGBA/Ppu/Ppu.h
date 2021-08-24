#pragma once
#include <SFML\Graphics.hpp>
#include "../Utils.h"

class MemoryBus;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

//#define VRAM_START 0x06000000
#define BM_MODE3_SIZE 0x12BFF

//LCD Registers
#define LCD_CONTROL 0x04000000

enum class BGMode {
	//Tile/Map modes
	ZERO = 0,
	ONE,
	TWO,

	//Bitmap modes
	THREE,
	FOUR,
	FIVE
};

struct BitmapMode3 {
	static constexpr u8 bpp = 2; //2 bytes per pixel
	sf::Image pixels;
	sf::Texture framebuffer;
	sf::Sprite frame;
};

class Ppu {
public:
	Ppu(MemoryBus *mbus);
	void update(s32 cycles);
	void render(sf::RenderTarget& target);
	void bufferPixels();

	void setBGMode(u16 lcdControl);

	//Extend 5 bit color val into 8 bits
	u8 getU8Color(u8 color);

	u8 readU8(u32 address);
	u16 readU16(u32 address);

	BGMode mode;
	BitmapMode3 mode3;

	u32 scanlineCounter = 0;
	u16 liney = 0;
	MemoryBus* mbus;
};