#pragma once
#include "../Utils.h"

/*
	Instruction encodings for arm and thumb instruction sets
*/

struct ArmInstruction {
	u8 cond();
	u8 i(); //immediate operand (0 = operand 2 is a register, 1 = operand 2 is an immediate value)
	u8 opcode(); //opcode field in a data processing instruction
	u8 s(); //set condition codes (0 = do not alter cond codes, 1 = set condition codes)
	u8 rn(); //1st operand register
	u8 rd(); //destination register
	u16 operand2();
	u8 shiftAmount(); //shift amount applied to Rm
	u8 shiftType(); 
	u8 rs();
	u8 rm(); //2nd operand register
	u8 rotate(); //shift applied to imm
	u8 imm(); //u8 immediate value

	u32 encoding;
};

struct ThumbInstruction {
	u8 offset5(); //immediate value
	u8 rs(); //source register
	u8 rd(); //destination register

	u16 encoding;
};