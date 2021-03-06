#include "Arm.h"
#include "../Memory/MemoryBus.h"

void Arm::mapArmOpcodes()
{
	for (s32 i = 0; i < 4096; i++) {
		//SWI
		if ((i & 0xF00) == 0xF00) {
			armlut[i] = b(&Arm::opSWI);
		}
		//Multiply long
		else if ((i & 0xF8F) == 0x89) {
			armlut[i] = b(&Arm::executeMultiplyLong);
		}
		//Multiply
		else if ((i & 0xFCF) == 0x9) {
			armlut[i] = b(&Arm::executeMultiply);
		}
		//Swap
		else if ((i & 0xFBF) == 0x109) {
			armlut[i] = b(&Arm::executeSwap);
		}
		//Branch
		else if ((i >> 8) == 0b1010) {
			armlut[i] = b(&Arm::opB);
		}
		//Branch with link
		else if ((i >> 8) == 0b1011) {
			armlut[i] = b(&Arm::opBL);
		}
		else if (i == 0b000100100001) {
			armlut[i] = b(&Arm::opBX);
		}
		//MRS
		else if (((i >> 7) == 0b00010) && (((i >> 4) & 0x3) == 0b00)) {
			armlut[i] = b(&Arm::opMRS);
		}
		////MSR Immediate
		else if (((i >> 7) == 0b00110) && (((i >> 4) & 0x3) == 0b10)) {
			armlut[i] = b(&Arm::executeMSRImm);
		}
		//MSR Register
		else if (((i >> 7) == 0b00010) && ((i & 0xF) == 0b0000) && !testBit(i, 4)) {
			armlut[i] = b(&Arm::executeMSRReg);
		}
		//Addressing Mode 2 Load and Store Word or Unsigned Byte
		else if ((i >> 9) == 0b010) {
			armlut[i] = b(&Arm::executeLoadStoreImm);
		}
		else if ((i >> 9) == 0b011) {
			armlut[i] = b(&Arm::executeLoadStoreShift);
		}
		//Addressing Mode 3 Misc Loads and Stores
		else if (((i & 0b1001) == 0b1001) && ((i >> 9) == 0) && (((i >> 6) & 0x1) == 0x1)) {
			armlut[i] = b(&Arm::executeMiscLoadStoreImm);
		}
		else if (((i & 0b1001) == 0b1001) && ((i >> 9) == 0) && (((i >> 6) & 0x1) == 0)) {
			armlut[i] = b(&Arm::executeMiscLoadStoreReg);
		}
		//LDM
		else if (((i >> 9) == 0b100) && (((i >> 4) & 0x1) == 0x1)) {
			armlut[i] = b(&Arm::executeLDM);
		}
		//STM
		else if(((i >> 9) == 0b100) && (((i >> 4) & 0x1) == 0x0)){
			armlut[i] = b(&Arm::executeSTM);
		}
		else if ((((i >> 7) & 0x1F) == 0b00110) && ((i >> 4) & 0x3) == 0b00){
			armlut[i] = b(&Arm::handleUndefinedIns);
		}
		//Addressing Mode 1 immediate with flags(S bit)
		else if (((i >> 9) == 0b001) && ((i >> 4) & 0x1) == 0x1) {
			armlut[i] = b(&Arm::executeDataProcessingImmFlags);
		}
		//Addressing Mode 1 immediate
		else if ((i >> 9) == 0b001) {
			armlut[i] = b(&Arm::executeDataProcessingImm);
		}
		//Addressing Mode 1 immediate shift with flags(S bit)
		else if (((i >> 9) == 0) && ((i & 0x1) == 0) && ((i >> 4) & 0x1) == 0x1) {
			armlut[i] = b(&Arm::executeDataProcessingImmShiftFlags);
		}
		//Addressing Mode 1 immediate shift
		else if (((i >> 9) == 0) && ((i & 0x1) == 0)) {
			armlut[i] = b(&Arm::executeDataProcessingImmShift);
		}
		//Addressing Mode 1 register shift with flags(S bit)
		else if (((i >> 9) == 0) && ((i >> 4) & 0x1) == 0x1) {
			armlut[i] = b(&Arm::executeDataProcessingRegShiftFlags);
		}
		else if ((i >> 9) == 0) {
			armlut[i] = b(&Arm::executeDataProcessingRegShift);
		}
		else {
			armlut[i] = b(&Arm::handleUndefinedIns);
		}
	}
}

void Arm::mapThumbOpcodes() 
{
	for (s32 i = 0; i < 256; i++) {
		//SWI
		if (i == 0b11011111) {
			thumblut[i] = b(&Arm::thumbOpSWI);
		}
		//Add/subtract register
		else if (((i >> 2) & 0x3F) == 0b000110) {
			thumblut[i] = b(&Arm::executeThumbAddSubReg);
		}
		//Add/subtract immediate
		else if (((i >> 2) & 0x3F) == 0b000111) {
			thumblut[i] = b(&Arm::executeThumbAddSubImm);
		}
		//Shift by immediate
		else if (((i >> 5) & 0x7) == 0b000) {
			thumblut[i] = b(&Arm::executeThumbShiftByImm);
		}
		//Add/sub/cmp/mov immediate (Data processing immediate)
		else if (((i >> 5) & 0x7) == 0b001) {
			thumblut[i] = b(&Arm::executeThumbDataProcessingImm);
		}
		//Data processing register
		else if (((i >> 2) & 0x3F) == 0b010000) {
			thumblut[i] = b(&Arm::executeThumbDataProcessingReg);
		}
		//Branch/Exchange
		else if ((((i >> 2) & 0x3F) == 0b010001) && ((i & 0x3) == 0b11)) {
			thumblut[i] = b(&Arm::executeThumbBranchExchange);
		}
		//Special Data processing
		else if (((i >> 2) & 0x3F) == 0b010001) {
			thumblut[i] = b(&Arm::executeThumbSpecialDataProcessing);
		}
		//Load from literal pool
		else if (((i >> 3) & 0x1F) == 0b01001) {
			thumblut[i] = b(&Arm::executeThumbLoadFromPool);
		}
		//Load/Store register offset
		else if (((i >> 4) & 0xF) == 0b0101) {
			thumblut[i] = b(&Arm::executeThumbLoadStoreRegisterOffset);
		}
		//Load/Store Word/Byte imm offset
		else if (((i >> 5) & 0x7) == 0b011) {
			thumblut[i] = b(&Arm::executeThumbLoadStoreWordByteImmOffset);
		}
		//Load/Store halfword immediate offset
		else if (((i >> 4) & 0xF) == 0b1000) {
			thumblut[i] = b(&Arm::executeThumbLoadStoreHalfwordImmOffset);
		}
		//Load/Store to/from stack
		else if (((i >> 4) & 0xF) == 0b1001) {
			thumblut[i] = b(&Arm::executeThumbLoadStoreStack);
		}
		//Add to SP or PC
		else if (((i >> 4) & 0xF) == 0b1010) {
			thumblut[i] = b(&Arm::executeThumbAddSPOrPC);
		}
		//Misc
		else if (((i >> 4) & 0xF) == 0b1011) {
			thumblut[i] = b(&Arm::executeThumbMisc);
		}
		//Load/Store multiple
		else if (((i >> 4) & 0xF) == 0b1100) {
			thumblut[i] = b(&Arm::executeThumbLoadStoreMultiple);
		}
		//Conditional branch
		else if (((i >> 4) & 0xF) == 0b1101) {
			thumblut[i] = b(&Arm::thumbOpBCond);
		}
		//Undefined instruction
		else if ((i & 0xFF) == 0b11011110) {
			thumblut[i] = b(&Arm::handleUndefinedThumbIns);
		}
		//Unconditional branch
		else if ((((i >> 5) & 0x1F) == 0b111) && 
			((((i >> 3) & 0x3) == 0b00) || 
			(((i >> 3) & 0x3) == 0b10) || 
			(((i >> 3) & 0x3) == 0b11) ||
			(((i >> 3) & 0x3) == 0b01))) {
			thumblut[i] = b(&Arm::executeThumbUnconditionalBranch);
		}
		//Undefined instruction
		else if ((((i >> 5) & 0x1F) == 0b11101) && ((i & 0x1) == 0x1)) {
			thumblut[i] = b(&Arm::handleUndefinedThumbIns);
		}
		//Undefined
		else {
			thumblut[i] = b(&Arm::handleUndefinedThumbIns);
		}
	}
}
