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

struct AddressingMode2 : public AddressingMode{

};



enum class AddrMode3Type : u8 {
	PREINDEXED = 0,
	OFFSET,
	POSTINDEX
};

struct AddrMode3Result {
	AddrMode3Type type;
	u32 address;
	u32 rn;
};

//(Misc Loads and Stores)
struct AddressingMode3 : public AddressingMode{
	AddressingMode3(Arm& cpu);
	AddrMode3Result immOffsetIndex(ArmInstruction& ins);
	AddrMode3Result registerOffsetIndex(ArmInstruction& ins);
};