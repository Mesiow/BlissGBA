#pragma once
#include "../Utils/Utils.h"
#include "../Ppu/Lcd.h"
#include "../Core/io.h"

class GeneralMemory;
struct DmaController;
class Arm;

struct Mmio {
	Mmio(GeneralMemory *gm);
	//Connect components to allow mmio to interact/tell other components
	//about events
	void connect(DmaController* dmac);
	void connect(Arm* cpu);

	void writeU8(u32 address, u8 value); //used internally
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u32 value);
	u16 readU16(u32 address);
	u32 readU32(u32 address);

	//Dma
	void writeDMASource(u32 address, u32 value);
	void writeDMADest(u32 address, u32 value);
	void writeDMACNT(u32 address, u16 value);
	void writeDMACNT(u32 address, u32 value);
	u16 readDMACNT(u32 address);
	u32 readDMASource(u32 address);
	u32 readDMADest(u32 address);

	//Keypad
	void writeKEYINPUT(u16 value);
	void writeKEYCNT(u16 value);

	//Interrupts/System Control
	void writeIF(u16 value);
	void writeIE(u16 value);
	void writeIME(u32 value);
	u16 readIF();
	u16 readIE();
	u32 readIME();

	void writeHALTCNT(u8 value);

	//Lcd
	void writeDISPCNT(u16 value);
	void writeDISPSTAT(u16 value);
	void writeVCOUNT(u16 value);
	void writeBG0CNT(u16 value);
	u16 readDISPCNT();
	u16 readBG0CNT();

	GeneralMemory* gm;
	DmaController* dmac = nullptr;
	Arm* cpu = nullptr;
};