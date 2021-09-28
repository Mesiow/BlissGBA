#pragma once
#include <SFML\Graphics.hpp>
#include "../Utils.h"

class MemoryBus;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define BM_MODE3_SIZE 0x12BFF

//LCD Registers
#define DISPCNT 0x04000000
#define DISPSTAT 0x04000004
#define VCOUNT 0x04000006

#define HBLANK_START 960
#define HBLANK_CYCLES 272
#define VBLANK_START 197120
#define VBLANK_CYCLES 83776

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

struct BitmapMode4 {
	static constexpr u8 bpp = 1;
	sf::Image pixels;
	sf::Texture framebuffer;
	sf::Sprite frame;
};

class Ppu {
public:
	Ppu(MemoryBus *mbus);
	void update(s32 cycles);
	void render(sf::RenderTarget& target);
	void reset();
	void renderBitmapModes();
	void renderBitmapMode3();
	void renderBitmapMode4();
	void bufferPixels();

	void updateLCDStatus();
	void updateCurrentScanline();

	void setBGMode(u16 lcdControl);
	void setHBlankFlag(bool value);
	void setVBlankFlag(bool value);
	void setScaleFactor(float scaleFactor);

	void writeU8(u32 address, u8 value);
	void writeU16(u32 address, u16 value);

	//Extend 5 bit color val into 8 bits
	u8 getU8Color(u8 color);

	u8 readU8(u32 address);
	u16 readU16(u32 address);

	BGMode mode;
	BitmapMode3 mode3;
	BitmapMode4 mode4;

	u32 cycleCounter = 0;
	//u32 hblankCounter = 0;
	u32 vblankCounter = 0;
	u16 vcount = 0;
	u16 currentScanline = 0;
	MemoryBus* mbus;
};