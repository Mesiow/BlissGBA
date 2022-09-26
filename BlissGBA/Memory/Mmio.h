#pragma once
#include "../Utils/Utils.h"
#include "../Ppu/Lcd.h"
#include "../Apu/Audio.h"
#include "../Core/Keypad.h"

class GeneralMemory;
struct DmaController;
struct TimerController;
class Arm;

struct Mmio {
	Mmio(GeneralMemory *gm);
	//Connect components to allow mmio to interact/tell other components
	//about events
	void connect(DmaController* dmac);
	void connect(TimerController* tmc);
	void connect(Arm* cpu);

	void writeU8(u32 address, u8 value); //used internally
	void writeU16(u32 address, u16 value);
	void writeU32(u32 address, u32 value);

	u8 readU8(u32 absoluteAddress);
	u16 readU16(u32 absoluteAddress);
	u32 readU32(u32 absoluteAddress);

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
	void writeBG0HOFS(u16 value);
	void writeBG0VOFS(u16 value);
	void writeBG1CNT(u16 value);
	void writeBG1HOFS(u16 value);
	void writeBG1VOFS(u16 value);
	void writeBG2CNT(u16 value);
	void writeBG2HOFS(u16 value);
	void writeBG2VOFS(u16 value);
	void writeBG3CNT(u16 value);
	void writeBG3HOFS(u16 value);
	void writeBG3VOFS(u16 value);
	u16 readDISPCNT();
	u16 readDISPSTAT();
	u16 readBG0CNT();
	u16 readBG0HOFS();
	u16 readBG0VOFS();
	u16 readBG1CNT();
	u16 readBG1HOFS();
	u16 readBG1VOFS();
	u16 readBG2CNT();
	u16 readBG2HOFS();
	u16 readBG2VOFS();
	u16 readBG3CNT();
	u16 readBG3HOFS();
	u16 readBG3VOFS();

	//Timers
	void writeTMCNTL(u32 address, u16 value);
	void writeTMCNTH(u32 address, u16 value);
	u16 readTMCNTH(u32 address);

	//Apu
	void writeSOUNDBIAS(u32 value);
	u32 readSOUNDBIAS();

	GeneralMemory* gm;
	DmaController* dmac = nullptr;
	TimerController* tmc = nullptr;
	Arm* cpu = nullptr;
};