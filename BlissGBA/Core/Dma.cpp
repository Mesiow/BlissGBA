#include "Dma.h"
#include "../Memory/MemoryBus.h"

DmaController::DmaController(MemoryBus* mbus)
	:mbus(mbus)
{
	channelToTransfer = DmaChannel::None;
	transfer = false;
}

void DmaController::handleDMA()
{
	if (transfer) {
		switch (channelToTransfer) {
			case DmaChannel::CH0: {
				printf("Handling DMA Channel 0...\n");
			}
			break;
			case DmaChannel::CH1: {
				printf("Handling DMA Channel 1...\n");
			}
			break;
			case DmaChannel::CH2: {
				printf("Handling DMA Channel 2...\n");
			}
			break;
			case DmaChannel::CH3: {
				//printf("Handling DMA Channel 3...\n");
				u16 dma3_cnt_h = mbus->mmio.readDMACNT(DMA3CNT_H);
				//if = 0, clear the enable bit after the transfer is done.
				//if = 1, the enable bit remains set after the transfer and 
				//the transfer will be restart each time when the start condition 
				//(eg. HBlank, Fifo) becomes true. The specified number of data units 
				//is transferred <each> time when the transfer is (re-)started. 
				//The transfer will be repeated forever, until it gets stopped by software.
				u8 repeat = (dma3_cnt_h >> 9) & 0x1;
				if (repeat == 0x0) {
					//printf("\nNo repeat (DMA3)\n");
					handleChannelTransfer(DmaChannel::CH3);

					//clear enable bit after transfer
					dma3_cnt_h = resetBit(dma3_cnt_h, 15);
					mbus->mmio.writeDMACNT(DMA3CNT_H, dma3_cnt_h);

					transfer = false;
				}
				else { //repeat transfer
					printf("Repeat(DMA3)\n");
					assert(false);
				}
			}
			break;

			default:
				break;
		}
	}
}


void DmaController::handleChannelTransfer(DmaChannel channel)
{
	switch (channel) {
		case DmaChannel::CH3: {
			u16 dma3_cnt_h = mbus->mmio.readDMACNT(DMA3CNT_H);
			u8 transfer_type = (dma3_cnt_h >> 10) & 0x1;
			u8 dest_addr_control = (dma3_cnt_h >> 5) & 0x3;
			u8 source_addr_control = (dma3_cnt_h >> 7) & 0x3;

			u32 source = mbus->mmio.readDMASource(DMA3SAD);
			u32 dest = mbus->mmio.readDMADest(DMA3DAD);
			
			u16 length = mbus->mmio.readDMACNT(DMA3CNT_L);
			makeTransfer(channel, (AddrControl)dest_addr_control, (AddrControl)source_addr_control,
				source, dest, length, transfer_type);
		}
		break;

		default:
			break;
	}
}


void DmaController::makeTransfer(DmaChannel channel, AddrControl destControl, AddrControl sourceControl, u32 sourceAddr, u32 destAddr, u16 length,
	u8 transferType)
{
	if (length == 0) {
		if (channel == DmaChannel::CH3) {
			if (transferType == 0x0) length = 0x4000;
			else length = 0x10000;
		}
	}

	if (transferType == 0x0) { //16 bit transfer
		if (sourceControl == AddrControl::FIXED) {
			for (u32 i = 0; i <= (length * 2); i++) {
				u16 value = mbus->readU16(sourceAddr);

				mbus->writeU16(destAddr, value);
				if (destControl == AddrControl::INCREMENT)
					destAddr += 2;
			}
		}
		else {
			for (u32 address = sourceAddr; address <= (sourceAddr + (length * 2)); address += 2) {
				u16 value = mbus->readU16(address);

				mbus->writeU16(destAddr, value);
				if (destControl == AddrControl::INCREMENT)
					destAddr += 2;
			}
		}
	}
	else { //32 bit transfer
		if (sourceControl == AddrControl::FIXED) {
			for (u32 i = 0; i <= (length * 4); i++) {
				u32 value = mbus->readU32(sourceAddr);

				mbus->writeU32(destAddr, value);
				if (destControl == AddrControl::INCREMENT)
					destAddr += 4;
			}
		}
		else {
			for (u32 address = sourceAddr; address <= (sourceAddr + (length * 4)); address += 4) {
				u32 value = mbus->readU32(address);

				mbus->writeU32(destAddr, value);
				if (destControl == AddrControl::INCREMENT)
					destAddr += 4;
			}
		}
	}
}

void DmaController::enableTransfer(bool enable, DmaChannel channel)
{
	if (channel == DmaChannel::CH2 || channel == DmaChannel::CH1) return;

	transfer = enable;
	channelToTransfer = channel;
}

