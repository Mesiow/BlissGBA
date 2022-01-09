#include "Mmio.h"
#include "GeneralMemory.h"
#include "../Core/Dma.h"
#include "../Cpu/Arm.h"

Mmio::Mmio(GeneralMemory* gm)
{
	this->gm = gm;
}

void Mmio::connect(DmaController* dmac)
{
	this->dmac = dmac;
}

void Mmio::connect(Arm* cpu)
{
	this->cpu = cpu;
}

void Mmio::writeU8(u32 address, u8 value)
{
	switch (address) {
		case HALTCNT:{
			u8 halt = (value >> 7) & 0x1;
			if (halt == 0x0)
				cpu->halt();

			writeHALTCNT(value);
		}
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

		//DMA
		case DMA1CNT_H: writeDMACNT(DMA1CNT_H, value); break;
		case DMA1CNT_L: writeDMACNT(DMA1CNT_L, value); break;
		case DMA2CNT_H: writeDMACNT(DMA2CNT_H, value); break;
		case DMA2CNT_L: writeDMACNT(DMA2CNT_L, value); break;
		case DMA3CNT_H: writeDMACNT(DMA3CNT_H, value); break;
		case DMA3CNT_L: writeDMACNT(DMA3CNT_L, value); break;

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

			u8 hi, lo;
			lo = value & 0xFF;
			hi = (value >> 8) & 0xFF;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
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

void Mmio::writeDMASource(u32 address, u32 value)
{
	u8 lower1 = value & 0xFF;
	u8 lower2 = (value >> 8) & 0xFF;
	u8 upper1 = (value >> 16) & 0xFF;
	u8 upper2 = (value >> 24) & 0xFF;

	u32 addr = 0;
	switch (address) {
		case DMA0SAD: {
			printf("write to dma 0 SAD\n");
			addr = DMA0SAD - IO_START_ADDR;
		}
		break;
		case DMA1SAD: {
			printf("write to dma 1 SAD\n");
			addr = DMA1SAD - IO_START_ADDR;
		}
		break;
		case DMA2SAD: {
			printf("write to dma 2 SAD\n");
			addr = DMA2SAD - IO_START_ADDR;
		}
		break;
		case DMA3SAD: {
			printf("write to dma 3 SAD\n");
			addr = DMA3SAD - IO_START_ADDR;
		}
		break;

		default:
			break;
	}

	gm->io[addr] = lower1;
	gm->io[addr + 1] = lower2;
	gm->io[addr + 2] = upper1;
	gm->io[addr + 3] = upper2;
}

void Mmio::writeDMADest(u32 address, u32 value)
{
	u8 lower1 = value & 0xFF;
	u8 lower2 = (value >> 8) & 0xFF;
	u8 upper1 = (value >> 16) & 0xFF;
	u8 upper2 = (value >> 24) & 0xFF;

	u32 addr = 0;
	switch (address) {
		case DMA0DAD: {
			printf("write to dma 0 DAD\n");
			addr = DMA0DAD - IO_START_ADDR;
		}
		break;
		case DMA1DAD: {
			printf("write to dma 1 DAD\n");
			addr = DMA1DAD - IO_START_ADDR;
		}
		break;
		case DMA2DAD: {
			printf("write to dma 2 DAD\n");
			addr = DMA2DAD - IO_START_ADDR;
		}
		break;
		case DMA3DAD: {
			printf("write to dma 3 DAD\n");
			addr = DMA3DAD - IO_START_ADDR;
		}
		break;

		default:
			break;
	}

	gm->io[addr] = lower1;
	gm->io[addr + 1] = lower2;
	gm->io[addr + 2] = upper1;
	gm->io[addr + 3] = upper2;
}

void Mmio::writeDMACNT(u32 address, u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	switch (address) {
		//DMA 0
		case DMA0CNT_L: {
			printf("write to dma 0 cnt L");
			u32 addr = DMA0CNT_L - IO_START_ADDR;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
	    break;
		case DMA0CNT_H: {
			printf("write to dma 0 cnt H\n");
			u32 addr = DMA0CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma0_cnt_h = readU16(addr);
			u8 enable_bit = (dma0_cnt_h >> 15) & 0x1;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;

			if (enable_bit == 0x0) {
				if ((value >> 15) & 0x1) {
					dmac->enableTransfer(true, DmaChannel::CH0);
				}
			}
		}
		break;

		//DMA 1
		case DMA1CNT_L: {
			printf("write to dma 1 cnt L");
			u32 addr = DMA1CNT_L - IO_START_ADDR;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
		break;
		case DMA1CNT_H: {
			printf("write to dma 1 cnt H\n");
			u32 addr = DMA1CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma1_cnt_h = readU16(addr);
			u8 enable_bit = (dma1_cnt_h >> 15) & 0x1;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;

			if (enable_bit == 0x0) {
				if ((value >> 15) & 0x1) {
					dmac->enableTransfer(true, DmaChannel::CH1);
				}
			}
		}
		break;

		//DMA 2
		case DMA2CNT_L: {
			printf("write to dma 2 cnt L");
			u32 addr = DMA2CNT_L - IO_START_ADDR;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
		break;
		case DMA2CNT_H: {
			printf("write to dma 2 cnt H\n");
			u32 addr = DMA2CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma2_cnt_h = readU16(addr);
			u8 enable_bit = (dma2_cnt_h >> 15) & 0x1;

			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;

			if (enable_bit == 0x0) {
				if ((value >> 15) & 0x1) {
					dmac->enableTransfer(true, DmaChannel::CH2);
				}
			}
		}
		break;

		//DMA 3
		case DMA3CNT_L: {
			printf("write to dma 3 cnt L\n");
			u32 addr = DMA3CNT_L - IO_START_ADDR;
			
			gm->io[addr] = lo;
			gm->io[addr + 1] = hi;
		}
		break;
		case DMA3CNT_H: {
			printf("write to dma 3 cnt H\n");
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

void Mmio::writeDMACNT(u32 address, u32 value)
{
	u8 lower1 = value & 0xFF;
	u8 lower2 = (value >> 8) & 0xFF;
	u8 upper1 = (value >> 16) & 0xFF;
	u8 upper2 = (value >> 24) & 0xFF;

	switch (address) {
		//DMA 0
		case DMA0CNT_L: {
			printf("write(32bit) to dma 0 cnt L");
			u32 addr = DMA0CNT_L - IO_START_ADDR;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;
		}
		break;
		case DMA0CNT_H: {
			printf("write(32bit) to dma 0 cnt H\n");
			u32 addr = DMA0CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma1_cnt_h = readU16(addr);
			u8 enable_bit = (dma1_cnt_h >> 15) & 0x1;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;

			if (enable_bit == 0x0) {
				if ((value >> 15) & 0x1) {
					dmac->enableTransfer(true, DmaChannel::CH1);
				}
			}
		}
		break;
		
		//DMA 1
		case DMA1CNT_L: {
			printf("write to dma 1 cnt L");
			u32 addr = DMA1CNT_L - IO_START_ADDR;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;
		}
		break;
		case DMA1CNT_H: {
			printf("write to dma 1 cnt H\n");
			u32 addr = DMA1CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma1_cnt_h = readU16(addr);
			u8 enable_bit = (dma1_cnt_h >> 15) & 0x1;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;

			if (enable_bit == 0x0) {
				if ((value >> 15) & 0x1) {
					dmac->enableTransfer(true, DmaChannel::CH1);
				}
			}
		}
		break;

		//DMA 2
		case DMA2CNT_L: {
			printf("write(32bit) to dma 2 cnt L");
			u32 addr = DMA2CNT_L - IO_START_ADDR;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;
		}
		break;
		case DMA2CNT_H: {
			printf("write(32bit) to dma 2 cnt H\n");
			u32 addr = DMA2CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma2_cnt_h = readU16(addr);
			u8 enable_bit = (dma2_cnt_h >> 15) & 0x1;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;

			if (enable_bit == 0x0) {
				if ((value >> 15) & 0x1) {
					dmac->enableTransfer(true, DmaChannel::CH2);
				}
			}
		}
		break;

		//DMA 3
		case DMA3CNT_L: {
			printf("write(32bit) to dma 3 cnt L\n");
			u32 addr = DMA3CNT_L - IO_START_ADDR;

			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;
		}
					  break;
		case DMA3CNT_H: {
			printf("write(32bit) to dma 3 cnt H\n");
			u32 addr = DMA3CNT_H - IO_START_ADDR;

			//Dma enable check
			u16 dma3_cnt_h = readU16(addr);
			u8 enable_bit = (dma3_cnt_h >> 15) & 0x1;

			//write new value
			gm->io[addr] = lower1;
			gm->io[addr + 1] = lower2;
			gm->io[addr + 2] = upper1;
			gm->io[addr + 3] = upper2;

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
	u32 ime = readU32(addr) & 0x1;

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


