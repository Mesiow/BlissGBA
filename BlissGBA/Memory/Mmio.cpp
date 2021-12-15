#include "Mmio.h"
#include "GeneralMemory.h"
#include "../Core/Dma.h"

Mmio::Mmio(GeneralMemory* gm)
{
	this->gm = gm;
}

void Mmio::connect(DmaController* dmac)
{
	this->dmac = dmac;
}

void Mmio::writeU8(u32 address, u8 value)
{
	gm->io[address] = value;
}

void Mmio::writeU16(u32 address, u16 value)
{
	if (address == IE) {
		writeIE(value);
	}
	else if (address == IF) {
		//Cpu writes to IF clears the bit
		u16 irq_flag = readIF();
		irq_flag &= ~(value);
		writeIF(irq_flag);
	}
	else if (address == IME) {
		writeIME(value);
	}
	else if (address == DMA3CNT_H) {
		printf("write to dma3");
		writeDMACNT(DMA3CNT_H, value);
	}
	else {
		//If adress case not implemented yet just write freely to io with mbus
		u32 addr = address - IO_START_ADDR;
	 
		u8 hi, lo;
		lo = value & 0xFF;
		hi = (value >> 8) & 0xFF;

		gm->io[addr] = lo;
		gm->io[addr + 1] = hi;
	}
}

void Mmio::writeU32(u32 address, u32 value)
{
	if (address == IF) {
		//Cpu writes to IF, clears the bit
		u16 irq_flag = readIF();
		irq_flag &= ~(value);
		writeIF(irq_flag);
	}
	else if (address == IE) {
		u16 lower = value & 0xFFFF;
		u16 upper = value >> 16;
		writeIE(lower);
		writeIF(upper);
	}
	else if (address == IME) {
		writeIME(value);
	}
}

u16 Mmio::readU16(u32 address)
{
	u8 lo, hi;
	lo = gm->io[address];
	hi = gm->io[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u32 Mmio::readU32(u32 address)
{
	u8 byte1, byte2, byte3, byte4;
	byte1 = gm->io[address];
	byte2 = gm->io[address + 1];
	byte3 = gm->io[address + 2];
	byte4 = gm->io[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}

void Mmio::writeDMACNT(u32 address, u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	switch (address) {
		case DMA3CNT_L: {
			u32 addr = DMA3CNT_L - IO_START_ADDR;
			
			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
		break;
		case DMA3CNT_H: {
			u32 addr = DMA3CNT_H - IO_START_ADDR;


			//Dma enable check
			u16 dma3_cnt_h = readU16(addr);
			u8 enable_bit = (dma3_cnt_h >> 15) & 0x1;

			//write new value
			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;

			if (enable_bit == 0x0) { //off
				if ((value >> 15) & 0x1) { //write back to dma_cnt_h to start 
					dmac->enableTransfer(true, DmaChannel::CH3);
				}
			}
		}
		break;

		default:
			break;
	}
}

u16 Mmio::readDMACNT(u32 address)
{
	switch (address) {
		case DMA3CNT_L: {
			u32 addr = DMA3CNT_L - IO_START_ADDR;
			u16 dma3_cnt_l = readU16(addr);

			return dma3_cnt_l;
		}
		break;
		case DMA3CNT_H: {
			u32 addr = DMA3CNT_H - IO_START_ADDR;
			u16 dma3_cnt_h = readU16(addr);
			
			return dma3_cnt_h;
		}
		break;

		default: 
			break;
	}

	return 0;
}

u32 Mmio::readDMASource(u32 address)
{
	switch (address) {
		case DMA3SAD: {
			u32 addr = DMA3SAD - IO_START_ADDR;
			u32 dma3_source_addr = readU32(addr);

			return dma3_source_addr;
		}
		break;
	}
}

u32 Mmio::readDMADest(u32 address)
{
	switch (address) {
		case DMA3DAD: {
			u32 addr = DMA3DAD - IO_START_ADDR;
			u32 dma3_dest_addr = readU32(addr);

			return dma3_dest_addr;
		}
		break;
	}
}

void Mmio::writeIF(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = IF - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeIE(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = IE - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeIME(u32 value)
{
	u8 upper2, upper1;
	u8 lower2, lower1;

	lower1 = value & 0xFF;
	lower2 = (value >> 8) & 0xFF;
	upper1 = (value >> 16) & 0xFF;
	upper2 = (value >> 24) & 0xFF;

	u32 addr = IME - IO_START_ADDR;
	writeU8(addr, lower1);
	writeU8(addr + 1, lower2);
	writeU8(addr + 2, upper1);
	writeU8(addr + 3, upper2);
}

u16 Mmio::readIF()
{
	u32 addr = IF - IO_START_ADDR;
	u16 irq_flag = readU16(addr);

	return irq_flag;
}

u16 Mmio::readIE()
{
	u32 addr = IE - IO_START_ADDR;
	u16 ie = readU16(addr);

	return ie;
}

u32 Mmio::readIME()
{
	u32 addr = IME - IO_START_ADDR;
	u32 ime = readU32(addr);

	return ime;
}


