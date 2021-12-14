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
			case DmaChannel::CH3: {
				u16 dma3_cnt_h = mbus->mmio.readDMACNT(DMA3CNT_H);
				//if = 0, clear the enable bit after the transfer is done.
				//if = 1, the enable bit remains set after the transfer and 
				//the transfer will be restart each time when the start condition 
				//(eg. HBlank, Fifo) becomes true. The specified number of data units 
				//is transferred <each> time when the transfer is (re-)started. 
				//The transfer will be repeated forever, until it gets stopped by software.
				u8 repeat = (dma3_cnt_h >> 9) & 0x1;
				if (repeat == 0x0) {
					printf("\nno repeat\n");
					handleChannelTransfer(DmaChannel::CH3);

					//clear enable bit after transfer
					dma3_cnt_h = resetBit(dma3_cnt_h, 15);
					mbus->mmio.writeDMACNT(DMA3CNT_H, dma3_cnt_h);

					transfer = false;
				}
				else { //repeat transfer

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

			u32 source = mbus->mmio.readDMASource(DMA3SAD);
			u32 dest = mbus->mmio.readDMADest(DMA3DAD);

			
			u16 length = mbus->mmio.readDMACNT(DMA3CNT_L);
			if (transfer_type == 0x0) { //16 bit transfer
				for (u32 address = source; address <= (source + (length * 2)); address += 2) {
					u16 value = mbus->readU16(address);

					mbus->writeU16(dest, value);
					dest += 2;
				}
			}
			else { //32 bit transfer
				for (u32 address = source; address <= (source + (length * 4)); address += 4) {
					u32 value = mbus->readU32(address);

					mbus->writeU32(dest, value);
					dest += 4;
				}
			}
		}
		break;

		default:
			break;
	}
}

void DmaController::enableTransfer(bool enable, DmaChannel channel)
{
	if (channel == DmaChannel::CH3) printf("\ntransfer enabled for ch3");
	transfer = enable;
	channelToTransfer = channel;
}
