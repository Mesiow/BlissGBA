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
	if (mode == BGMode::ZERO)
		target.draw(mode0.frame);
	else if (mode == BGMode::THREE)
		target.draw(mode3.frame);
	else if (mode == BGMode::FOUR)
		target.draw(mode4.frame);
}

void Ppu::reset()
{
	mode0.pixels.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
	mode0.framebuffer.loadFromImage(mode0.pixels);
	mode0.frame = sf::Sprite(mode0.framebuffer);

	mode3.pixels.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
	mode3.framebuffer.loadFromImage(mode3.pixels);
	mode3.frame = sf::Sprite(mode3.framebuffer);

	mode4.pixels.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
	mode4.framebuffer.loadFromImage(mode4.pixels);
	mode4.frame = sf::Sprite(mode4.framebuffer);

	float scaleFactor = 2.5;
	mode0.frame.setScale(scaleFactor, scaleFactor);
	mode3.frame.setScale(scaleFactor, scaleFactor);
	mode4.frame.setScale(scaleFactor, scaleFactor);

	displayMode = DisplayMode::Visible;
	currentScanline = 0;
}

void Ppu::render()
{
	u16 display_ctrl = readU16(DISPCNT);
	setBGMode(display_ctrl);

	if (mode == BGMode::ZERO) {
		renderMode0();
	}
	else if (mode == BGMode::THREE) {
		renderBitmapMode3();
	}
	else if (mode == BGMode::FOUR) {
		renderBitmapMode4();
	}
}

void Ppu::renderMode0()
{
	u16 bg0cnt = readU16(BG0CNT);

	u8 char_base_offset = (bg0cnt >> 2) & 0x3; //offset from where to read tiledata from in vram
	u8 bpp = (bg0cnt >> 7) & 0x1; //0 = 16 (4bpp), 1 = 256 (8bpp)
	bool is8bpp = (bpp == 0x1);

	for (u32 x = 0; x < SCREEN_WIDTH; x++) {
		//tile data is in units of 16k bytes
		u32 offset = char_base_offset * 0x4000;
		u32 tile_address = VRAM_START_ADDR + offset;
		
		u8 bits_pp = (is8bpp) ? 8 : 4;
		//u32 index = calculateTileOffset(x, currentScanline, bits_pp);
		u32 index = 0;
		if (bits_pp == 8) {

		}
		else {

		}
		u16 tile = readU16(tile_address + 96);
		u8 paletteNumber = (tile >> 12) & 0xF;

		//Get color of tile pixel from palette ram
		u32 pramAddr = PRAM_START_ADDR;
		if (is8bpp) {
			pramAddr = PRAM_START_ADDR + (tile);
		}
		else {
			u8 lower_pixel = tile & 0xF;
			u8 offset = lower_pixel;
			offset &= 0xF;
			offset |= (tile << 4);
			pramAddr = PRAM_START_ADDR + ((offset * 2) + (16 * paletteNumber));
		}
		u16 palette = readU16(pramAddr);

		u8 red = getU8Color((palette & 0x1F));
		u8 green = getU8Color((palette >> 5) & 0x1F);
		u8 blue = getU8Color((palette >> 10) & 0x1F);

		const sf::Color color = sf::Color(red, green, blue, 255);
		mode0.pixels.setPixel(x, currentScanline, color);
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
	u16 dispcnt = mbus->mmio.readDISPCNT();
	u8 frame = (dispcnt >> 4) & 0x1;
	
	//Use frame 0 or 1
	u32 page = VRAM_START_ADDR + (0xA000 * frame);

	for (u32 x = 0; x < SCREEN_WIDTH; x++) {
		u32 index = ((currentScanline * SCREEN_WIDTH + x) * mode4.bpp);
		u8 paletteIndex = readU8(page + index);

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
	if (mode == BGMode::ZERO)
		mode0.framebuffer.update(mode0.pixels);
	else if (mode == BGMode::THREE)
		mode3.framebuffer.update(mode3.pixels);
	else if (mode == BGMode::FOUR)
		mode4.framebuffer.update(mode4.pixels);
}

u32 Ppu::calculateTileOffset(u32 x, u32 y, u8 bpp)
{
	u32 tilex = x % 8;
	u32 tiley = y % 8;
	u32 index = (tiley * bpp) * tilex;

	return index;
}

void Ppu::updateScanline()
{
	currentScanline++;
	mbus->mmio.writeVCOUNT(currentScanline);

	u16 lcd_stat = readU16(DISPSTAT);
	u8 lyc = (lcd_stat >> 8) & 0xFF; //VCount setting
	u8 vcount_irq = (lcd_stat >> 5) & 0x1;

	if (currentScanline == lyc) {
		setVCountFlag(1); //ly matches lyc
		if (vcount_irq) {
			requestInterrupt(VCOUNT_INT);
		}
	}
	else
		setVCountFlag(0);

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

	mbus->mmio.writeDISPSTAT(lcd_stat);
}

void Ppu::setVBlankFlag(bool value)
{
	u16 lcd_stat = readU16(DISPSTAT);
	(value == true) ? lcd_stat = setBit(lcd_stat, 0) : lcd_stat = resetBit(lcd_stat, 0);

	mbus->mmio.writeDISPSTAT(lcd_stat);
}

void Ppu::setVCountFlag(bool value)
{
	u16 lcd_stat = readU16(DISPSTAT);
	(value == true) ? lcd_stat = setBit(lcd_stat, 2) : lcd_stat = resetBit(lcd_stat, 2);

	mbus->mmio.writeDISPSTAT(lcd_stat);
}

void Ppu::setScaleFactor(float scaleFactor)
{
	if (mode == BGMode::ZERO)
		mode0.frame.setScale(scaleFactor, scaleFactor);
	else if (mode == BGMode::THREE)
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
	u16 irq_flag = mbus->mmio.readIF();
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
