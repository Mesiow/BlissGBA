#include "Ppu.h"
#include "../Memory/MemoryBus.h"

Ppu::Ppu(MemoryBus *mbus)
	:mbus(mbus)
{
	mode3.pixels.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
	mode3.framebuffer.loadFromImage(mode3.pixels);
	mode3.frame = sf::Sprite(mode3.framebuffer);

	float scaleFactor = 2;
	mode3.frame.setScale(scaleFactor, scaleFactor);
}

void Ppu::update(s32 cycles)
{
	cycleCounter += cycles;
	hblankCounter += cycles;
	vblankCounter += cycles;

	updateLCDStatus();

	u16 lcd_control = readU16(DISPCNT);
	setBGMode(lcd_control);

	renderBitmapModes();
}

void Ppu::render(sf::RenderTarget& target)
{
	target.draw(mode3.frame);
}

void Ppu::renderBitmapModes()
{
	//if(cycleCounter)
	u16 lcd_stat = readU16(DISPSTAT);
	bool vblank = testBit(lcd_stat, 0);
	bool hblank = testBit(lcd_stat, 1);
	

	//Bitmap mode 3
	if (mode == BGMode::THREE) {
		//else Draw scanline at the start of HBLANK

		if (hblank) {
			for (u32 x = 0; x < SCREEN_WIDTH; x++) {
				//2 bytes associated to each pixel(defining one of the 32768 colors)
				u32 index = ((currentScanline * SCREEN_WIDTH + x) * mode3.bpp);
				u16 pixel = readU16(VRAM_START_ADDR + index);

				u8 red = getU8Color((pixel & 0x1F));
				u8 green = getU8Color((pixel >> 5) & 0x1F);
				u8 blue = getU8Color((pixel >> 10) & 0x1F);

				const sf::Color color = sf::Color(red, green, blue, 255);
				mode3.pixels.setPixel(x, currentScanline, color);
			}
			updateCurrentScanline();
		}
		
	}
}

void Ppu::bufferPixels()
{
	mode3.framebuffer.update(mode3.pixels);
}

void Ppu::updateLCDStatus()
{
	u16 lcd_stat = readU16(DISPSTAT);
	vcount = readU16(VCOUNT);
	bool vblank = testBit(lcd_stat, 0);
	bool hblank = testBit(lcd_stat, 1);

	if (!hblank) {
		//toggle hblank
		if (hblankCounter >= HBLANK_START && hblankCounter < (HBLANK_START + HBLANK_CYCLES)) {
			lcd_stat = setBit(lcd_stat, 1);
			writeU16(DISPSTAT, lcd_stat);
		}
		else {
			hblankCounter = 0;
			lcd_stat = resetBit(lcd_stat, 1);
			writeU16(DISPSTAT, lcd_stat);
		}
	}

	if (!vblank) {
		//toggle vblank
		if (vblankCounter >= VBLANK_START && vblankCounter < (VBLANK_START + VBLANK_CYCLES)) {
			lcd_stat = setBit(lcd_stat, 0);
			writeU16(DISPSTAT, lcd_stat);
		}
		else {
			vblankCounter = 0;
			lcd_stat = resetBit(lcd_stat, 0);
			writeU16(DISPSTAT, lcd_stat);
		}
	}

	currentScanline = vcount & 0xFF;
}

void Ppu::updateCurrentScanline()
{
	currentScanline++;
	if (currentScanline > SCREEN_HEIGHT) {
		currentScanline = 0;
	}
	vcount = currentScanline & 0xFF;
	writeU16(VCOUNT, vcount);
}

void Ppu::setBGMode(u16 lcdControl)
{
	u8 bg_mode = lcdControl & 0x7;
	switch (bg_mode) {
		case 0x0: mode = BGMode::ZERO; break;
		case 0x1: mode = BGMode::ONE; break;
		case 0x2: mode = BGMode::TWO; break;

		case 0x3: mode = BGMode::THREE; break;
		case 0x4: mode = BGMode::FOUR; break;
		case 0x5: mode = BGMode::FIVE; break;
	}
}

void Ppu::setScaleFactor(float scaleFactor)
{
	mode3.frame.setScale(scaleFactor, scaleFactor);
}

void Ppu::writeU8(u32 address, u8 value)
{
	mbus->writeU8(address, value);
}

void Ppu::writeU16(u32 address, u16 value)
{
	mbus->writeU16(address, value);
}

u8 Ppu::getU8Color(u8 color)
{
	u8 new_color = color << 3;
	new_color |= (new_color >> 2);

	return new_color;
}

u8 Ppu::readU8(u32 address)
{
	return mbus->readU8(address);
}

u16 Ppu::readU16(u32 address)
{
	return mbus->readU16(address);
}
