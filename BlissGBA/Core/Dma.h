#pragma once
#include "../Utils/Utils.h"
#include <assert.h>

//DMA source address registers
#define DMA0SAD 0x40000B0
#define DMA1SAD 0x40000BC
#define DMA2SAD 0x40000C8
#define DMA3SAD 0x40000D4

//DMA destination address registers
#define DMA0DAD 0x40000B4
#define DMA1DAD 0x40000C0
#define DMA2DAD 0x40000CC
#define DMA3DAD 0x40000D8

//DMA control count registers 
//(specify the number of data units to be transfered,
//each unit is 16 or 32bit depending on transfer type)
#define DMA0CNT_L 0x40000B8
#define DMA1CNT_L 0x40000C4
#define DMA2CNT_L 0x40000D0
#define DMA3CNT_L 0x40000DC

//DMA contol bits registers
#define	DMA0CNT_H 0x40000BA
#define DMA1CNT_H 0x40000C6
#define DMA2CNT_H 0x40000D2
#define DMA3CNT_H 0x40000DE


enum class DmaChannel : u8 {
	CH0 = 0,
	CH1,
	CH2,
	CH3,
	None
};

enum class AddrControl : u8 {
	INCREMENT = 0,
	DECREMENT,
	FIXED,
	INCREMENT_RELOAD
};

class MemoryBus;

struct DmaController {
	DmaController(MemoryBus* mbus);
	void handleDMA();
	void handleChannelTransfer(DmaChannel channel);
	void makeTransfer(DmaChannel channel, AddrControl destControl, AddrControl sourceControl, u32 sourceAddr, u32 destAddr, u16 length,
		u8 transferType);
	void enableTransfer(bool enable, DmaChannel channel);

	MemoryBus* mbus;
	DmaChannel channelToTransfer;
	bool transfer;
};