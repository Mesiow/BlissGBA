#pragma once

enum class Addressing : u8{
	MODE1 = 0,
	MODE2,
	MODE3,
	MODE4,
	MODE5
};

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
	u32 imm(ArmInstruction& ins, u8& shiftedBit); //returns result
	u8 isRegisterShift(ArmInstruction& ins);
};

struct AddressingMode2 : public AddressingMode{

};

struct AddressingMode3 : public AddressingMode{

};