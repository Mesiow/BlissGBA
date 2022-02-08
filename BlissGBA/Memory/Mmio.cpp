#include "Mmio.h"
#include "GeneralMemory.h"
#include "../Core/Dma.h"
#include "../Cpu/Arm.h"
#include "../Core/Timer.h"

Mmio::Mmio(GeneralMemory* gm)
{
	this->gm = gm;
}

void Mmio::connect(DmaController* dmac)
{
	this->dmac = dmac;
}

void Mmio::connect(TimerController* tmc)
{
	this->tmc = tmc;
}

void Mmio::connect(Arm* cpu)
{
	this->cpu = cpu;
}

void Mmio::writeU8(u32 address, u8 value)
{
	switch (address) {
		case IF: {
			//Cpu writes to IF clears the bit
			u16 irq_flag = readIF();
			irq_flag &= ~(value);
			gm->io[IF - IO_START_ADDR] = irq_flag & 0xFF;
		}
		break;
		case HALTCNT:{
			u8 halt = (value >> 7) & 0x1;
			if (halt == 0x0)
				cpu->halt();

			writeHALTCNT(value);
		}
		break;

		default:
			gm->io[address - IO_START_ADDR] = value;
			break;
	}
}

void Mmio::writeU16(u32 address, u16 value)
{
	switch (address) {
		//LCD
		case DISPCNT: writeDISPCNT(value); break;
		case DISPSTAT: writeDISPSTAT(value); break;
		case VCOUNT: writeVCOUNT(value); break;
		case BG0CNT: writeBG0CNT(value); break;
		case BG0HOFS: writeBG0HOFS(value); break;
		case BG0VOFS: writeBG0VOFS(value); break;
		case BG1CNT: writeBG1CNT(value); break;
		case BG1HOFS: writeBG1HOFS(value); break;
		case BG1VOFS: writeBG1VOFS(value); break;
		case BG2CNT: writeBG2CNT(value); break;
		case BG2HOFS: writeBG2HOFS(value); break;
		case BG2VOFS: writeBG2VOFS(value); break;
		case BG3CNT: writeBG3CNT(value); break;
		case BG3HOFS: writeBG3HOFS(value); break;
		case BG3VOFS: writeBG3VOFS(value); break;

		//DMA
		case DMA1CNT_H: writeDMACNT(DMA1CNT_H, value); break;
		case DMA1CNT_L: writeDMACNT(DMA1CNT_L, value); break;
		case DMA2CNT_H: writeDMACNT(DMA2CNT_H, value); break;
		case DMA2CNT_L: writeDMACNT(DMA2CNT_L, value); break;
		case DMA3CNT_H: writeDMACNT(DMA3CNT_H, value); break;
		case DMA3CNT_L: writeDMACNT(DMA3CNT_L, value); break;

		//Timers
		case TM0CNT_L: printf("u16 write to tm0 cntl: 0x%04X\n", value); writeTMCNTL(TM0CNT_L, value); break;
		case TM1CNT_L: printf("u16 write to tm1 cntl: 0x%04X\n", value); writeTMCNTL(TM1CNT_L, value); break;
		case TM2CNT_L: printf("u16 write to tm2 cntl: 0x%04X\n", value); writeTMCNTL(TM2CNT_L, value); break;
		case TM3CNT_L: printf("u16 write to tm3 cntl: 0x%04X\n", value); writeTMCNTL(TM3CNT_L, value); break;

		case TM0CNT_H: printf("u16 write to tm0 cnth: 0x%04X\n", value); writeTMCNTH(TM0CNT_H, value); break;
		case TM1CNT_H: printf("u16 write to tm1 cnth: 0x%04X\n", value); writeTMCNTH(TM1CNT_H, value); break;
		case TM2CNT_H: printf("u16 write to tm2 cnth: 0x%04X\n", value); writeTMCNTH(TM2CNT_H, value); break;
		case TM3CNT_H: printf("u16 write to tm3 cnth: 0x%04X\n", value); writeTMCNTH(TM3CNT_H, value); break;

		case SOUNDBIAS: writeSOUNDBIAS(value); break;


		//Interrupt/Control
		case IE: writeIE(value); break;
		case IF: {
			//Cpu writes to IF clears the bit
			u16 irq_flag = readIF();
			irq_flag &= ~(value);
			writeIF(irq_flag);
		}
		break;
		case IME: writeIME(value); break;

		default: {
			//If address case not implemented yet just write freely to io
			u32 addr = address - IO_START_ADDR;

			u8 hi, lo;
			lo = value & 0xFF;
			hi = (value >> 8) & 0xFF;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
	}
}

void Mmio::writeU32(u32 address, u32 value)
{
	switch (address) {
		//LCD
		case DISPCNT: writeDISPCNT(value); break;
		case DISPSTAT: writeDISPSTAT(value); break;
		case VCOUNT: writeVCOUNT(value); break;
		case BG0CNT: writeBG0CNT(value); break;
		case BG0HOFS: writeBG0HOFS(value); break;
		case BG0VOFS: writeBG0VOFS(value); break;
		case BG1CNT: writeBG0CNT(value); break;
		case BG1HOFS: writeBG1HOFS(value); break;
		case BG1VOFS: writeBG1VOFS(value); break;
		case BG2CNT: writeBG2CNT(value); break;
		case BG2HOFS: writeBG2HOFS(value); break;
		case BG2VOFS: writeBG2VOFS(value); break;
		case BG3CNT: writeBG3CNT(value); break;
		case BG3HOFS: writeBG3HOFS(value); break;
		case BG3VOFS: writeBG3VOFS(value); break;

		//DMA
		case DMA1SAD: writeDMASource(address, value); break;
		case DMA1DAD: writeDMADest(address, value); break;
		case DMA2SAD: writeDMASource(address, value); break;
		case DMA2DAD: writeDMADest(address, value); break;
		case DMA3SAD: writeDMASource(address, value); break;
		case DMA3DAD: writeDMADest(address, value); break;

		case DMA1CNT_H: writeDMACNT(DMA1CNT_H, value); break;
		case DMA1CNT_L: writeDMACNT(DMA1CNT_L, value); break;
		case DMA2CNT_H: writeDMACNT(DMA2CNT_H, value); break;
		case DMA2CNT_L: writeDMACNT(DMA2CNT_L, value); break;
		case DMA3CNT_H: writeDMACNT(DMA3CNT_H, value); break;
		case DMA3CNT_L: writeDMACNT(DMA3CNT_L, value); break;

		//Timers
		case TM0CNT_L: 
			printf("u32 write to tm0 cntl\n");
			writeTMCNTL(TM0CNT_L, value);
			writeTMCNTH(TM0CNT_H, value >> 16);
			break;
		case TM1CNT_L:
			printf("u32 write to tm1 cntl\n");
			writeTMCNTL(TM1CNT_L, value);
			writeTMCNTH(TM1CNT_H, value >> 16);
			break;
		case TM2CNT_L: 
			printf("u32 write to tm2 cntl\n");
			writeTMCNTL(TM2CNT_L, value);
			writeTMCNTH(TM2CNT_H, value >> 16);
			break;
		case TM3CNT_L: 	
			printf("u32 write to tm3 cntl\n");
			writeTMCNTL(TM3CNT_L, value);
			writeTMCNTH(TM3CNT_H, value >> 16);
			break;
		
		//Audio
		case SOUNDBIAS: writeSOUNDBIAS(value); break;

		//Interrupt/Control
		case IE: { 
			u16 lower = value & 0xFFFF;
			u16 upper = value >> 16;
			writeIE(lower);

			////Cpu writes to IF clears the bit
			u16 irq_flag = readIF();
			irq_flag &= ~(upper);
			writeIF(irq_flag);
		} 
		break;
		case IF: {
			//Cpu writes to IF clears the bit
			u16 irq_flag = readIF();
			irq_flag &= ~(value);
			writeIF(irq_flag);
		}
		break;
		case IME: writeIME(value); break;

		default: {
			//If address case not implemented yet just write freely to io
			u32 addr = address - IO_START_ADDR;

			u8 lower1 = value & 0xFF;
			u8 lower2 = (value >> 8) & 0xFF;
			u8 upper1 = (value >> 16) & 0xFF;
			u8 upper2 = (value >> 24) & 0xFF;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;
		}
	}
}

u8 Mmio::readU8(u32 absoluteAddress)
{
	u32 address = absoluteAddress - IO_START_ADDR;
	return gm->io[address];
}

u16 Mmio::readU16(u32 absoluteAddress)
{
	//Reading from timer counter/reload mmio returns the current counter value
	//(or the recent/frozen counter value if the timer has stopped)
	if (absoluteAddress == TM0CNT_L || absoluteAddress == TM1CNT_L
		|| absoluteAddress == TM2CNT_L || absoluteAddress == TM3CNT_L) {
		switch (absoluteAddress) {
			case TM0CNT_L: return tmc->getTimerCounter(eTimer::TM0); break;
			case TM1CNT_L: return tmc->getTimerCounter(eTimer::TM1); break;
			case TM2CNT_L: return tmc->getTimerCounter(eTimer::TM2); break;
			case TM3CNT_L: return tmc->getTimerCounter(eTimer::TM3); break;
		}
	}

	u32 address = absoluteAddress - IO_START_ADDR;

	u8 lo, hi;
	lo = gm->io[address];
	hi = gm->io[address + 1];

	u16 value = (hi << 8) | lo;

	return value;
}

u32 Mmio::readU32(u32 absoluteAddress)
{
	u32 address = absoluteAddress - IO_START_ADDR;

	u8 byte1, byte2, byte3, byte4;
	byte1 = gm->io[address];
	byte2 = gm->io[address + 1];
	byte3 = gm->io[address + 2];
	byte4 = gm->io[address + 3];

	u32 value = ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1);

	return value;
}

void Mmio::writeDMASource(u32 address, u32 value)
{
	u8 lower1 = value & 0xFF;
	u8 lower2 = (value >> 8) & 0xFF;
	u8 upper1 = (value >> 16) & 0xFF;
	u8 upper2 = (value >> 24) & 0xFF;

	if (address == DMA0SAD || address == DMA1SAD
		|| address == DMA2SAD || address == DMA3SAD) {
		u32 addr = address - IO_START_ADDR;

		gm->io[addr] = lower1;
		gm->io[addr + 1] = lower2;
		gm->io[addr + 2] = upper1;
		gm->io[addr + 3] = upper2;
	}
}

void Mmio::writeDMADest(u32 address, u32 value)
{
	u8 lower1 = value & 0xFF;
	u8 lower2 = (value >> 8) & 0xFF;
	u8 upper1 = (value >> 16) & 0xFF;
	u8 upper2 = (value >> 24) & 0xFF;

	if (address == DMA0DAD || address == DMA1DAD
		|| address == DMA2DAD || address == DMA3DAD) {
		u32 addr = address - IO_START_ADDR;

		gm->io[addr] = lower1;
		gm->io[addr + 1] = lower2;
		gm->io[addr + 2] = upper1;
		gm->io[addr + 3] = upper2;
	}
}

void Mmio::writeDMACNT(u32 address, u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	if (address == DMA0CNT_L || address == DMA1CNT_L
		|| address == DMA2CNT_L || address == DMA3CNT_L) {
		u32 addr = address - IO_START_ADDR;

		gm->io[addr] = lo;
		gm->io[addr + 1] = hi;
	}
	else if (address == DMA0CNT_H || address == DMA1CNT_H
		|| address == DMA2CNT_H || address == DMA3CNT_H) {
		u32 addr = address - IO_START_ADDR;

		//Dma enable check
		u16 dma_cnt_h = readU16(address);
		u8 enable_bit = (dma_cnt_h >> 15) & 0x1;

		gm->io[addr] = lo;
		gm->io[addr + 1] = hi;

		DmaChannel channel = DmaChannel::None;
		switch (address) {
			case DMA0CNT_H: channel = DmaChannel::CH0; break;
			case DMA1CNT_H: channel = DmaChannel::CH1; break;
			case DMA2CNT_H: channel = DmaChannel::CH2; break;
			case DMA3CNT_H: channel = DmaChannel::CH3; break;
		}

		if (enable_bit == 0x0) {
			if ((value >> 15) & 0x1) {
				dmac->enableTransfer(true, channel);
			}
		}
	}
}

void Mmio::writeDMACNT(u32 address, u32 value)
{
	u8 lower1 = value & 0xFF;
	u8 lower2 = (value >> 8) & 0xFF;
	u8 upper1 = (value >> 16) & 0xFF;
	u8 upper2 = (value >> 24) & 0xFF;

	if (address == DMA0CNT_L || address == DMA1CNT_L || address == DMA2CNT_L) {
		u32 addr = address - IO_START_ADDR;

		gm->io[addr] = lower1;
		gm->io[addr + 1] = lower2;
		gm->io[addr + 2] = upper1;
		gm->io[addr + 3] = upper2;
	}
	else if (address == DMA0CNT_H || address == DMA1CNT_H || address == DMA2CNT_H) {
		u32 addr = address - IO_START_ADDR;

		//Dma enable check
		u16 dma_cnt_h = readU16(address);
		u8 enable_bit = (dma_cnt_h >> 15) & 0x1;

		gm->io[addr] = lower1;
		gm->io[addr + 1] = lower2;
		gm->io[addr + 2] = upper1;
		gm->io[addr + 3] = upper2;

		DmaChannel channel = DmaChannel::None;
		switch (address) {
			case DMA0CNT_H: channel = DmaChannel::CH0; break;
			case DMA1CNT_H: channel = DmaChannel::CH1; break;
			case DMA2CNT_H: channel = DmaChannel::CH2; break;
			case DMA3CNT_H: channel = DmaChannel::CH3; break;
		}

		if (enable_bit == 0x0) {
			if ((value >> 15) & 0x1) {
				dmac->enableTransfer(true, channel);
			}
		}
	}
	else if (address == DMA3CNT_L) {
		//32 bit write to dma3 cnt L also writes into cnt H
		u32 addr = DMA3CNT_L - IO_START_ADDR;
		u32 cnt_h_addr = DMA3CNT_H - IO_START_ADDR;

		//Dma enable check
		u16 dma3_cnt_h = readU16(DMA3CNT_H);
		u8 enable_bit = (dma3_cnt_h >> 15) & 0x1;

		gm->io[addr] = lower1;
		gm->io[addr + 1] = lower2;
		gm->io[addr + 2] = upper1;
		gm->io[addr + 3] = upper2;

		if (enable_bit == 0x0) { //off
			if ((value >> 31) & 0x1) { //write back to dma_cnt_h to start 
				dmac->enableTransfer(true, DmaChannel::CH3);
			}
		}
	}
}

u16 Mmio::readDMACNT(u32 address)
{
	if (address == DMA0CNT_L || address == DMA0CNT_H
		|| address == DMA1CNT_L || address == DMA1CNT_H
		|| address == DMA2CNT_L || address == DMA2CNT_H
		|| address == DMA3CNT_L || address == DMA3CNT_H) {
		u16 dmacnt = readU16(address);
		return dmacnt;
	}
	return 0;
}

u32 Mmio::readDMASource(u32 address)
{
	if (address == DMA0SAD || address == DMA1SAD
		|| address == DMA2SAD || address == DMA3SAD) {
		u32 dmasource_addr = readU32(address);
		return dmasource_addr;
	}
	return 0;
}

u32 Mmio::readDMADest(u32 address)
{
	if (address == DMA0DAD || address == DMA1DAD
		|| address == DMA2DAD || address == DMA3DAD) {
		u32 dmadest_addr = readU32(address);
		return dmadest_addr;
	}
	return 0;
}

void Mmio::writeKEYINPUT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = KEYINPUT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeKEYCNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = KEYCNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
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
	gm->io[addr] = lower1;
	gm->io[addr + 1] = lower2;
	gm->io[addr + 2] = upper1;
	gm->io[addr + 3] = upper2;
}

u16 Mmio::readIF()
{
	u16 irq_flag = readU16(IF);
	return irq_flag;
}

u16 Mmio::readIE()
{
	u16 ie = readU16(IE);
	return ie;
}

u32 Mmio::readIME()
{
	u32 ime = readU32(IME) & 0x1;
	return ime;
}

void Mmio::writeHALTCNT(u8 value)
{
	u32 addr = HALTCNT - IO_START_ADDR;
	gm->io[addr] = value;
}

void Mmio::writeDISPCNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = DISPCNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeDISPSTAT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = DISPSTAT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeVCOUNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = VCOUNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG0CNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG0CNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG0HOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG0HOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG0VOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG0VOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG1CNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG1CNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG1HOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG1HOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG1VOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG1VOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG2CNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG2CNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG2HOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG2HOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG2VOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG2VOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG3CNT(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG3CNT - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG3HOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG3HOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

void Mmio::writeBG3VOFS(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = BG3VOFS - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}

u16 Mmio::readDISPCNT()
{
	u16 dispcnt = readU16(DISPCNT);
	return dispcnt;
}

u16 Mmio::readBG0CNT()
{
	u16 bg0cnt = readU16(BG0CNT);
	return bg0cnt;
}

u16 Mmio::readBG0HOFS()
{
	u16 bg0hofs = readU16(BG0HOFS);
	return bg0hofs;
}

u16 Mmio::readBG0VOFS()
{
	u16 bg0vofs = readU16(BG0VOFS);
	return bg0vofs;
}

u16 Mmio::readBG1CNT()
{
	u16 bg1cnt = readU16(BG1CNT);
	return bg1cnt;
}

u16 Mmio::readBG1HOFS()
{
	u16 bg1hofs = readU16(BG1HOFS);
	return bg1hofs;
}

u16 Mmio::readBG1VOFS()
{
	u16 bg1vofs = readU16(BG1VOFS);
	return bg1vofs;
}

u16 Mmio::readBG2CNT()
{
	u16 bg2cnt = readU16(BG2CNT);
	return bg2cnt;
}

u16 Mmio::readBG2HOFS()
{
	u16 bg2hofs = readU16(BG2HOFS);
	return bg2hofs;
}

u16 Mmio::readBG2VOFS()
{
	u16 bg2vofs = readU16(BG2VOFS);
	return bg2vofs;
}

u16 Mmio::readBG3CNT()
{
	u16 bg3cnt = readU16(BG3CNT);
	return bg3cnt;
}

u16 Mmio::readBG3HOFS()
{
	u16 bg3hofs = readU16(BG3HOFS);
	return bg3hofs;
}

u16 Mmio::readBG3VOFS()
{
	u16 bg3vofs = readU16(BG3VOFS);
	return bg3vofs;
}

void Mmio::writeTMCNTL(u32 address, u16 value)
{
	//Writing to the timer counter/reload registers sets
	//the reload value (does not actually affect the mmio register/current counter value)
	switch (address) {
		case TM0CNT_L: tmc->setTimerReload(eTimer::TM0, value); break;
		case TM1CNT_L: tmc->setTimerReload(eTimer::TM1, value); break;
		case TM2CNT_L: tmc->setTimerReload(eTimer::TM2, value); break;
		case TM3CNT_L: tmc->setTimerReload(eTimer::TM3, value); break;
	}
}

void Mmio::writeTMCNTH(u32 address, u16 value)
{
	if (address == TM0CNT_H || address == TM1CNT_H
		|| address == TM2CNT_H || address == TM3CNT_H) {
		u32 addr = address - IO_START_ADDR;

		eTimer tm;
		switch (address) {
			case TM0CNT_H: tm = eTimer::TM0; break;
			case TM1CNT_H: tm = eTimer::TM1; break;
			case TM2CNT_H: tm = eTimer::TM2; break;
			case TM3CNT_H: tm = eTimer::TM3; break;
		}

		tmc->setControl(tm, value);
	}
}

u16 Mmio::readTMCNTH(u32 address)
{
	if (address == TM0CNT_H || address == TM1CNT_H
		|| address == TM2CNT_H || address == TM3CNT_H) {
		eTimer tm;
		switch (address) {
			case TM0CNT_H: tm = eTimer::TM0; break;
			case TM1CNT_H: tm = eTimer::TM1; break;
			case TM2CNT_H: tm = eTimer::TM2; break;
			case TM3CNT_H: tm = eTimer::TM3; break;
		}
		return tmc->getTimerControlRegister((u8)tm);
	}
	return 0;
}

void Mmio::writeSOUNDBIAS(u32 value)
{
	u8 upper2, upper1;
	u8 lower2, lower1;

	lower1 = value & 0xFF;
	lower2 = (value >> 8) & 0xFF;
	upper1 = (value >> 16) & 0xFF;
	upper2 = (value >> 24) & 0xFF;

	u32 addr = SOUNDBIAS - IO_START_ADDR;
	gm->io[addr] = lower1;
	gm->io[addr + 1] = lower2;
	gm->io[addr + 2] = upper1;
	gm->io[addr + 3] = upper2;
}

u32 Mmio::readSOUNDBIAS()
{
	u32 soundbias = readU32(SOUNDBIAS);
	return soundbias;
}


