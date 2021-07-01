#pragma once

class Arm;

struct AddressingMode {
	AddressingMode(Arm& cpu);
	Arm& cpu;
};

//(Data processing)
struct AddressingMode1 : public AddressingMode{
	AddressingMode1(Arm& cpu);

	//handles immediate shifts and register shifts
	u32 shift(ArmInstruction& ins, u8& shiftedBit);
	//handles 32-bit immediate rotate 
	u8 imm(ArmInstruction& ins, u8& shiftedBit); //returns result

	u8 isRegisterShift(ArmInstruction& ins);
};

struct AddressingMode2 : public AddressingMode{

};

struct AddressingMode3 : public AddressingMode{

};