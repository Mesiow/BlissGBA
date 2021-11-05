#pragma once

class Arm;

struct AddressingMode {
	AddressingMode(Arm& cpu);
	Arm& cpu;
};

//(Data processing)
struct AddressingMode1 : public AddressingMode{
	AddressingMode1(Arm& cpu);
	u32 shift(ArmInstruction& ins, u8& shiftedBit); //handles immediate shifts and register shifts
	u32 imm(ArmInstruction& ins, u8& shiftedBit); //handles 32-bit immediate rotate 
	u8 isRegisterShift(ArmInstruction& ins);
};

enum class AddrModeLoadStoreType : u8 {
	PREINDEXED = 0,
	OFFSET,
	POSTINDEX
};

//Struct used for Addr Mode 2 and 3
struct AddrModeLoadStoreResult {
	AddrModeLoadStoreType type;
	u32 address;
	u32 rn;
	u8 shifterCarryOut;
};

struct AddressingMode2 : public AddressingMode{
	AddressingMode2(Arm& cpu);
	AddrModeLoadStoreResult immOffsetIndex(ArmInstruction& ins);
	AddrModeLoadStoreResult scaledRegisterOffsetIndex(ArmInstruction& ins);
};

//(Misc Loads and Stores)
struct AddressingMode3 : public AddressingMode{
	AddressingMode3(Arm& cpu);
	AddrModeLoadStoreResult immOffsetIndex(ArmInstruction& ins);
	AddrModeLoadStoreResult registerOffsetIndex(ArmInstruction& ins);
};

struct AddrMode4Result {
	bool writeback;
	u32 startAddress;
	u32 endAddress;
	u32 rn;
};

//(Load and Store Multiple)
struct AddressingMode4 : public AddressingMode {
	AddressingMode4(Arm& cpu);
	AddrMode4Result incrementAfter(ArmInstruction& ins);
	AddrMode4Result incrementBefore(ArmInstruction& ins);
	AddrMode4Result decrementAfter(ArmInstruction& ins);
	AddrMode4Result decrementBefore(ArmInstruction& ins);
};