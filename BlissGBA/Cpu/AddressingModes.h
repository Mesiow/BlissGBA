#pragma once

class Arm;

//(Data processing)
struct AddressingMode1 {
	AddressingMode1(Arm& cpu);

	u32 regShift(ArmInstruction& ins, u8& shiftedBit);
	u8 imm(ArmInstruction& ins, u8& shiftedBit); //returns result

	Arm& cpu;
};

struct AddressingMode2 {

};

struct AddressingMode3 {

};