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
	scanlineCounter += cycles;

	u16 lcd_control = readU16(LCD_CONTROL);
	setBGMode(lcd_control);

	//Bitmap mode 3
	if (mode == BGMode::THREE) {
		for (s32 y = 0; y < SCREEN_HEIGHT; y++) {
			for (s32 x = 0; x < SCREEN_WIDTH; x++) {
				//2 bytes associated to each pixel(defining one of the 32768 colors)
				u16 pixel = readU16(VRAM_START + ((y * SCREEN_WIDTH + x) * mode3.bpp));

				u8 red = getU8Color((pixel & 0x1F));
				u8 green = getU8Color((pixel >> 5) & 0x1F);
				u8 blue = getU8Color((pixel >> 10) & 0x1F);

				const sf::Color color = sf::Color(red, green, blue, 255);
				mode3.pixels.setPixel(x, y, color);
			}
		}
	}
}

void Ppu::render(sf::RenderTarget& target)
{
	target.draw(mode3.frame);
}

void Ppu::bufferPixels()
{
	mode3.framebuffer.loadFromImage(mode3.pixels);
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
