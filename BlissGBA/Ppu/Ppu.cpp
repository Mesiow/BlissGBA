#include "Ppu.h"
#include "../Memory/MemoryBus.h"

Ppu::Ppu(MemoryBus *mbus)
	:mbus(mbus)
{
	reset();
}

void Ppu::update(s32 cycles)
{
	cycleCounter += cycles;

	switch (displayMode) {
		case DisplayMode::Visible: {
			//counter >= 1006 (hblank is off until this point)
			if (cycleCounter >= (HBLANK_START + 46)) {
				if (currentScanline < SCREEN_HEIGHT)
					render();

				displayMode = DisplayMode::HBlank;
				setHBlankFlag(1);
				requestInterrupt(HBLANK_INT);
			}
		}
		break; 

		case DisplayMode::HBlank: {
			//is HBlank done?
			if (cycleCounter >= (HBLANK_START + HBLANK_CYCLES)) {
				updateScanline();

				//Enable VBlank
				if (currentScanline == SCREEN_HEIGHT) {
					displayMode = DisplayMode::VBlank;
					setVBlankFlag(1);
					requestInterrupt(VBLANK_INT);
				}
			}
			else {
				displayMode = DisplayMode::Visible;
			}
		}
		break;

		case DisplayMode::VBlank: {
			if (cycleCounter >= (HBLANK_START + HBLANK_CYCLES)){
				updateScanline();
				if (currentScanline == 228) {
					displayMode = DisplayMode::Visible;
					setVBlankFlag(0);
					currentScanline = 0;
				}
			}
		}
		break;
	}
}

void Ppu::render(sf::RenderTarget& target)
{
	if (mode == BGMode::THREE)
		target.draw(mode3.frame);
	else if (mode == BGMode::FOUR)
		target.draw(mode4.frame);
}

void Ppu::reset()
{
	mode3.pixels.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
	mode3.framebuffer.loadFromImage(mode3.pixels);
	mode3.frame = sf::Sprite(mode3.framebuffer);

	mode4.pixels.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
	mode4.framebuffer.loadFromImage(mode4.pixels);
	mode4.frame = sf::Sprite(mode4.framebuffer);

	float scaleFactor = 2.5;
	mode3.frame.setScale(scaleFactor, scaleFactor);
	mode4.frame.setScale(scaleFactor, scaleFactor);

	displayMode = DisplayMode::Visible;
	currentScanline = 0;
}

void Ppu::render()
{
	u16 display_ctrl = readU16(DISPCNT);
	setBGMode(display_ctrl);

	if (mode == BGMode::THREE) {
		renderBitmapMode3();
	}
	else if (mode == BGMode::FOUR) {
		renderBitmapMode4();
	}
}

void Ppu::renderBitmapMode3()
{
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
}

void Ppu::renderBitmapMode4()
{
	for (u32 x = 0; x < SCREEN_WIDTH; x++) {
		u32 index = ((currentScanline * SCREEN_WIDTH + x) * mode4.bpp);
		u8 paletteIndex = readU8(VRAM_START_ADDR + index);

		u32 pramAddr = PRAM_START_ADDR + (paletteIndex * 2);
		u16 palette = readU16(pramAddr);

		u8 red = getU8Color((palette & 0x1F));
		u8 green = getU8Color((palette >> 5) & 0x1F);
		u8 blue = getU8Color((palette >> 10) & 0x1F);

		const sf::Color color = sf::Color(red, green, blue, 255);
		mode4.pixels.setPixel(x, currentScanline, color);
	}
}

void Ppu::bufferPixels()
{
	if (mode == BGMode::THREE)
		mode3.framebuffer.update(mode3.pixels);
	else if (mode == BGMode::FOUR)
		mode4.framebuffer.update(mode4.pixels);
}

void Ppu::updateScanline()
{
	currentScanline++;
	writeU16(VCOUNT, currentScanline);

	//exit hblank
	setHBlankFlag(0);
	cycleCounter = 0;
}

void Ppu::setBGMode(u16 lcdstatus)
{
	u8 bg_mode = lcdstatus & 0x7;
	switch (bg_mode) {
		case 0x0: mode = BGMode::ZERO; break;
		case 0x1: mode = BGMode::ONE; break;
		case 0x2: mode = BGMode::TWO; break;

		case 0x3: mode = BGMode::THREE; break;
		case 0x4: mode = BGMode::FOUR; break;
		case 0x5: mode = BGMode::FIVE; break;
	}
}

void Ppu::setHBlankFlag(bool value)
{
	u16 lcd_stat = readU16(DISPSTAT);
	(value == true) ? lcd_stat = setBit(lcd_stat, 1) : lcd_stat = resetBit(lcd_stat, 1);

	writeU16(DISPSTAT, lcd_stat);
}

void Ppu::setVBlankFlag(bool value)
{
	u16 lcd_stat = readU16(DISPSTAT);
	(value == true) ? lcd_stat = setBit(lcd_stat, 0) : lcd_stat = resetBit(lcd_stat, 0);

	writeU16(DISPSTAT, lcd_stat);
}

void Ppu::setScaleFactor(float scaleFactor)
{
	if (mode == BGMode::THREE)
		mode3.frame.setScale(scaleFactor, scaleFactor);
	else if (mode == BGMode::FOUR)
		mode4.frame.setScale(scaleFactor, scaleFactor);
}

void Ppu::writeU8(u32 address, u8 value)
{
	mbus->writeU8(address, value);
}

void Ppu::writeU16(u32 address, u16 value)
{
	mbus->writeU16(address, value);
}

void Ppu::requestInterrupt(u16 interrupt)
{
	u16 irq_flag = mbus->readU16(IF);
	irq_flag = setBit(irq_flag, interrupt);
	mbus->mmio.writeIF(irq_flag);
}

u8 Ppu::getU8Color(u8 color)
{
	u8 new_color = color << 3;
	new_color |= (color >> 2);

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
