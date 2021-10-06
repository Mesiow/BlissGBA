#pragma once
#include "../Utils.h"

/*
	Instruction encodings for arm and thumb instruction sets
*/

struct RegisterID {
	u8 id;
};

struct ArmInstruction {
	u8 cond();
	u8 i(); //immediate operand (0 = operand 2 is a register, 1 = operand 2 is an immediate value)
	u8 opcode(); //opcode field in a data processing instruction
	u8 s(); //set condition codes (0 = do not alter cond codes, 1 = set condition codes)
	RegisterID rn(); //1st operand register
	RegisterID rd(); //destination register
	u16 operand2();
	u8 shiftAmount(); //shift amount applied to Rm
	u8 shiftType(); 
	RegisterID rs();
	RegisterID rm(); //2nd operand register
	u8 rotate(); //shift applied to imm
	u8 imm(); //u8 immediate value

	u8 branch(); //3 bit number to determine if branch instruction
	u8 l(); //link bit for branching instructions
	s32 offset(); //sign extended offset in branch instruction
	u16 instruction(); //12bit number (bits 20-27 and 4-7 combined) 
	                   //that allows us to decode which instruction we are executing
	u8 bits8to11();
	u16 offset12();

	//Load/Store/Misc
	u8 P(); u8 U(); u8 W(); u8 L();
	u8 S(); u8 H(); u8 B();
	u8 immedH(); u8 immedL();
	u8 S_LSM(); //S bit for load/store multiple
	u16 registerList();

	//Status reg access decoding
	u8 R();
	u8 fieldMask();

	u32 encoding;
};

struct ThumbInstruction {
	u8 cond();
	u8 instruction(); //8 bit number representing certain thumb instruction
	s8 signedImm8();
	s16 signedImm11();
	u8 offset5(); //immediate value
	u8 rs(); //source register
	u8 rd(); //destination register

	u16 encoding;
};