#include "Arm.h"
#include "../Memory/MemoryBus.h"

Arm::Arm(MemoryBus *mbus)
	:addrMode1(*this), addrMode2(*this),
	addrMode3(*this), addrMode4(*this)
{
	mapArmOpcodes();
	this->mbus = mbus;
}

u8 Arm::clock()
{
	state = getState();
	if (state == State::ARM) {
		u32 encoding = armpipeline[0];
		armpipeline[0] = armpipeline[1];
		
		ArmInstruction ins;
		ins.encoding = encoding;
		executeArmIns(ins);

		armpipeline[1] = fetchU32();
	}
	else if (state == State::THUMB) {
		u16 encoding = thumbpipeline[0];
		thumbpipeline[0] = thumbpipeline[1];
		thumbpipeline[1] = fetchU16();

		ThumbInstruction ins;
		ins.encoding = encoding;
		executeThumbIns(ins);
	}
	return cycles;
}

void Arm::reset()
{
	cycles = 0;
	for (s32 i = 0; i < 0xD; i++) {
		registers[i].value = 0x00000000;
	}

	LR = 0x08000000; //R14
	R15 = 0x08000000;
	SP = 0x03007F00; //R13
	CPSR = 0x0000001F;
	SPSR = 0x00000000;

	armpipeline[0] = fetchU32();
	armpipeline[1] = fetchU32();
}

void Arm::setFlag(u32 flagBits, bool condition)
{
	if (condition) {
		setFlag(flagBits);
	}
}

void Arm::setFlag(u32 flagBits)
{
	//M0 - M4
	for (s32 i = 0; i < 5; i++) {
		u8 mi = (1 << i);
		if (flagBits & mi) {
			CPSR |= mi;
		}
	}

	if (flagBits & T) {
		CPSR |= T;
	}
	if (flagBits & F) {
		CPSR |= F;
	}
	if (flagBits & I) {
		CPSR |= I;
	}
	if (flagBits & V) {
		CPSR |= V;
	}
	if (flagBits & C) {
		CPSR |= C;
	}
	if (flagBits & Z) {
		CPSR |= Z;
	}
	if (flagBits & N) {
		CPSR |= N;
	}
}

void Arm::clearFlag(u32 flagBits)
{
	//M0 - M4
	for (s32 i = 0; i < 5; i++) {
		u8 mi = (1 << i);
		if (flagBits & mi) {
			CPSR &= ~(mi);
		}
	}

	if (flagBits & T) {
		CPSR &= ~(T);
	}
	if (flagBits & F) {
		CPSR &= ~(F);
	}
	if (flagBits & I) {
		CPSR &= ~(I);
	}
	if (flagBits & V) {
		CPSR &= ~(V);
	}
	if (flagBits & C) {
		CPSR &= ~(C);
	}
	if (flagBits & Z) {
		CPSR &= ~(Z);
	}
	if (flagBits & N) {
		CPSR &= ~(N);
	}
}

void Arm::fillPipeline()
{
	u32 first_instr = (mbus->readU8(R15)) | (mbus->readU8(R15 + 1) << 8) |
		(mbus->readU8(R15 + 2) << 16) | (mbus->readU8(R15 + 3) << 24);

	u32 second_instr = (mbus->readU8(R15 + 4)) | (mbus->readU8(R15 + 5) << 8) |
		(mbus->readU8(R15 + 6) << 16) | (mbus->readU8(R15 + 7) << 24);

	//Fill pipeline at boot with first 2 instructions
	armpipeline[0] = first_instr;
	armpipeline[1] = second_instr;
}

void Arm::flushPipeline() 
{
	armpipeline[0] = fetchU32();
	armpipeline[1] = readU32();
}

u8 Arm::getFlag(u32 flag)
{
	return (CPSR & flag) ? 1 : 0;
}

u8 Arm::carryFrom(u32 op1, u32 op2)
{
	return (((u64)op1 + (u64)op2) > 0xFFFFFFFF);
}

u8 Arm::borrowFrom(u32 op1, u32 op2)
{
	return (((s64)op1 - (s64)op2) < 0);
}

u8 Arm::overflowFromAdd(u32 op1, u32 op2)
{
	u32 signBit = 0x80000000;
	bool negative = ((op1 & signBit) && (op2 & signBit));
	bool positive = (((op1 & signBit) == 0) && ((op2 & signBit) == 0));

	if (positive) {
		s64 result = (s64)op1 + (s64)op2;
		if (result & signBit) return true;
	}
	if (negative) {
		s64 result = (s64)op1 + (s64)op2;
		if ((result & signBit) == 0) return true;
	}
	return false;
}

u8 Arm::overflowFromSub(u32 op1, u32 op2) 
{
	u32 signBit = 0x80000000;
	//subtracting a negative (5 - (-2))
	bool negative = (((op1 & signBit) == 0) && (op2 & signBit));
	//subtracting a positive (-5 - 2)
	bool positive = ((op1 & signBit) && ((op2 & signBit) == 0));

	if (positive) {
		s64 result = op1 - op2;
		if (result & signBit) return true;
	}
	if (negative) {
		s64 result = op1 - op2;
		if ((result & signBit) == 0) return true;
	}
	return false;
}

u8 Arm::fetchOp(u32 encoding)
{
	u8 opcode = ((encoding >> 21) & 0xF);
	return opcode;
}

PSR Arm::getPSR()
{
	return PSR{
		CPSR,
		SPSR
	};
}

u32 Arm::getRegister(RegisterID reg)
{
	if (reg.id >= 0xD) {
		if (reg.id == 0xD) return SP;
		if (reg.id == 0xE) return LR;
		if (reg.id == 0xF) return R15;
	}
	return registers[reg.id].value;
}

void Arm::writeRegister(RegisterID reg, u32 value)
{
	if (reg.id >= 0xD) {
		if (reg.id == 0xD) SP = value;
		if (reg.id == 0xE) LR = value;
		if (reg.id == 0xF) R15 = value;

		return;
	}
	registers[reg.id].value = value;
}

void Arm::writePC(u32 pc)
{
	R15 |= ((pc << 2) & 0xFFFFFFFF);
}

void Arm::setCC(u32 rd, bool borrow, bool overflow,
	bool shiftOut, u8 shifterCarryOut)
{
	if (rd != R15) {
		(rd >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(rd == 0) ? setFlag(Z) : clearFlag(Z);
		if (shiftOut) {
			//overflow not affected
			(shifterCarryOut == 1) ? setFlag(C) : clearFlag(C);
		}
		else {
			(borrow == false) ? setFlag(C) : clearFlag(C);
			(overflow == true) ? setFlag(V) : clearFlag(V);
		}
	}
	else {
		CPSR = SPSR;
	}
}

State Arm::getState()
{
	u8 st = ((CPSR & T) >> T_BIT);
	return State(st);
}

u8 Arm::getConditionCode(u8 cond)
{
	switch (cond) {
		case 0b0000: return getFlag(Z); break;
		case 0b0001: return (getFlag(Z) == 0); break;
		case 0b0010: return getFlag(C); break;
		case 0b0011: return (getFlag(C) == 0); break;
		case 0b0100: return getFlag(N); break;
		case 0b0101: return (getFlag(N) == 0); break;
		case 0b0110: return getFlag(V); break;
		case 0b0111: return (getFlag(V) == 0); break;
		case 0b1000: return (getFlag(C) && (getFlag(Z) == 0)); break;
		case 0b1001: return ((getFlag(C) == 0) || getFlag(Z)); break;
		case 0b1010: return (getFlag(N) == getFlag(V)); break;
		case 0b1011: return (getFlag(N) != getFlag(V)); break;
		case 0b1100: return ((getFlag(Z) == 0) && (getFlag(N) == getFlag(V))); break;
		case 0b1101: return (getFlag(Z) || (getFlag(N) != getFlag(V))); break;
		case 0b1110: return 1; break;//always
		default: return 1; break; //always
	}
}

u16 Arm::readU16()
{
	return u16();
}

u16 Arm::fetchU16()
{
	return u16();
}

u32 Arm::readU32()
{
	u32 word = (mbus->readU8(R15)) |
		(mbus->readU8(R15 + 1) << 8) |
		(mbus->readU8(R15 + 2) << 16) |
		(mbus->readU8(R15 + 3) << 24);

	return word;
}

u32 Arm::fetchU32()
{
	u32 word = (mbus->readU8(R15++)) |
		(mbus->readU8(R15++) << 8) |
		(mbus->readU8(R15++) << 16) |
		(mbus->readU8(R15++) << 24);

	return word;
}

u32 Arm::shift(u32 value, u8 amount, u8 type, u8 &shiftedBit)
{
	switch (type) {
		case 0b00:
			value = lsl(value, amount, shiftedBit);
			break;

		case 0b01:
			value = lsr(value, amount, shiftedBit);
			break;

		case 0b10:
			value = asr(value, amount, shiftedBit);
			break;

		case 0b11: {
			if (amount == 0) {
				value = rrx(value, shiftedBit);
				return value;
			}
			value = ror(value, amount);
			//Save last carried out bit
			shiftedBit = (value >> 31) & 0x1;
		}
			break;
	}
	return value;
}

u32 Arm::lsl(u32 value, u8 shift, u8& shiftedBit)
{
	//Save last carried out bit
	shiftedBit = (31 - (shift - 1));
	shiftedBit = ((value >> shiftedBit) & 0x1);

	value <<= shift;
	return value;
}

u32 Arm::lsr(u32 value, u8 shift, u8& shiftedBit)
{
	shiftedBit = (shift - 1);
	shiftedBit = ((value >> shiftedBit) & 0x1);

	value >>= shift;
	return value;
}

u32 Arm::asr(u32 value, u8 shift, u8& shiftedBit)
{
	shiftedBit = (shift - 1);
	shiftedBit = ((value >> shiftedBit) & 0x1);

	u8 msb = ((value >> 31) & 0x1);
	value >>= shift;
	value |= (msb << 31);

	return value;
}

u32 Arm::ror(u32 value, u8 shift)
{
	u32 rotatedOut = getNthBits(value, 0, shift);
	u32 rotatedIn = getNthBits(value, shift, 31);

	u32 result = ((rotatedIn) | (rotatedOut << (32 - shift)));
	return result;
}

u32 Arm::rrx(u32 value, u8 &shiftedBit)
{
	//shifter_carry_out
	shiftedBit = value & 0x1;
	value = (((u32)getFlag(C) << 31) | (value >> 1));
	
	return value;
}

u8 Arm::executeArmIns(ArmInstruction& ins)
{
	u8 cond = getConditionCode(ins.cond());
	if (!cond) {
		return 1;
	}
	
	u16 instruction = ins.instruction();
	cycles = armlut[instruction](ins);

	return 1;
}

u8 Arm::executeThumbIns(ThumbInstruction& ins)
{
	return 1;
}

u8 Arm::executeDataProcessing(ArmInstruction& ins, bool flags, bool immediate)
{
	RegisterID rd = ins.rd();
	RegisterID rn = ins.rn();

	switch (ins.opcode()) {
		case 0b0000: return opAND(ins, rd, rn, flags, immediate); break;
		case 0b0001: return opEOR(ins, rd, rn, flags, immediate); break;
		case 0b0010: return opSUB(ins, rd, rn, flags, immediate); break;
		case 0b0011: return opRSB(ins, rd, rn, flags, immediate); break;
		case 0b0100: return opADD(ins, rd, rn, flags, immediate); break;
		case 0b0101: return opADC(ins, rd, rn, flags, immediate); break;
		case 0b0110: return opSBC(ins, rd, rn, flags, immediate); break;
		case 0b0111: return opRSC(ins, rd, rn, flags, immediate); break;
		case 0b1000: return opTST(ins, rd, rn, flags, immediate); break;
		case 0b1001: return opTEQ(ins, rd, rn, flags, immediate); break;
		case 0b1010: return opCMP(ins, rd, rn, flags, immediate); break;
		case 0b1011: return opCMN(ins, rd, rn, flags, immediate); break;
		case 0b1100: return opORR(ins, rd, rn, flags, immediate); break;
		case 0b1101: return opMOV(ins, rd, rn, flags, immediate); break;
		case 0b1110: return opBIC(ins, rd, rn, flags, immediate); break;
		case 0b1111: return opMVN(ins, rd, rn, flags, immediate); break;
	}

	return 1;
}

u8 Arm::executeDataProcessingImmFlags(ArmInstruction& ins)
{
	executeDataProcessing(ins, true, true);
	return 1;
}

u8 Arm::executeDataProcessingImm(ArmInstruction& ins)
{
	executeDataProcessing(ins, false, true);
	return 1;
}

u8 Arm::executeDataProcessingImmShiftFlags(ArmInstruction& ins)
{
	executeDataProcessing(ins, true, false);
	return 1;
}

u8 Arm::executeDataProcessingImmShift(ArmInstruction& ins)
{
	executeDataProcessing(ins, false, false);
	return 1;
}

u8 Arm::executeDataProcessingRegShiftFlags(ArmInstruction& ins)
{
	executeDataProcessing(ins, true, false);
	return 1;
}

u8 Arm::executeDataProcessingRegShift(ArmInstruction& ins)
{
	executeDataProcessing(ins, false, false);
	return 1;
}

u8 Arm::handleUndefinedIns(ArmInstruction& ins)
{
	u16 lutIndex = ins.instruction();
	printf("LUT Index: %d ", lutIndex);
	printf("ARM undefined or unimplemented instruction 0x%08X at PC: 0x%08X\n", ins.encoding, R15 - 8);

	return 0;
}

u8 Arm::executeLoadStore(ArmInstruction& ins, AddrModeLoadStoreResult& result)
{
	RegisterID rn = ins.rn();
	RegisterID rd = ins.rd();

	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	bool byte = (ins.B() == 0x1); //true == byte, false == word (transfer quantity)
	bool load = (ins.L() == 0x1); //load or store

	switch (result.type) {
		case AddrModeLoadStoreType::PREINDEXED:
			reg_rn = result.address;
			writeRegister(rn, reg_rn);
			break;

		case AddrModeLoadStoreType::POSTINDEX:
			reg_rn = result.rn;
			writeRegister(rn, reg_rn);
			break;
	}

	bool useImmediateOffset = (result.type == AddrModeLoadStoreType::OFFSET);
	u32 address = result.address; //imm offset (no writeback to register rn)

	if (load) {
		//LDR, LDRB
		if (byte) {
			(useImmediateOffset == true) ? opLDRB(ins, rd, address) : opLDRB(ins, rd, reg_rn);
		}
		else {
			(useImmediateOffset == true) ? opLDR(ins, rd, address) : opLDR(ins, rd, reg_rn);
		}
	}
	else {
		//STR, STRB
		if (byte) {
			(useImmediateOffset == true) ? opSTRB(ins, rd, address) : opSTRB(ins, rd, reg_rn);
		}
		else {
			(useImmediateOffset == true) ? opSTR(ins, rd, address) : opSTR(ins, rd, reg_rn);
		}
	}


	return 1;
}

u8 Arm::executeLoadStoreImm(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode2.immOffsetIndex(ins);
	executeLoadStore(ins, result);

	return 1;
}

u8 Arm::executeLoadStoreShift(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode2.scaledRegisterOffsetIndex(ins);
	executeLoadStore(ins, result);

	return 1;
}

u8 Arm::executeMiscLoadAndStore(ArmInstruction& ins, AddrModeLoadStoreResult &result)
{
	RegisterID rn = ins.rn();
	RegisterID rd = ins.rd();

	u32 reg_rn = getRegister(rn);
	u32 reg_rd = getRegister(rd);

	bool load = (ins.L() == 0x1);
	u8 SH = (ins.S() << 1) | ins.H();

	switch (result.type) {
		case AddrModeLoadStoreType::PREINDEXED:
			reg_rn = result.address;
			writeRegister(rn, reg_rn);
			break;
		
		case AddrModeLoadStoreType::POSTINDEX:
			reg_rn = result.rn;
			writeRegister(rn, reg_rn);
			break;
	}

	bool useImmediateOffset = (result.type == AddrModeLoadStoreType::OFFSET);
	u32 address = result.address; //imm offset (no writeback to register rn)
	
	switch (SH) {
		case 0b00: /*SWP*/ break;
		case 0b01: //Unsigned halfwords
		{
			if (!load) {
				(useImmediateOffset == true) ? opSTRH(ins, rd, address) 
					: opSTRH(ins, rd, reg_rn); 
			}
			else {
				(useImmediateOffset == true) ? opLDRH(ins, rd, address) 
					: opLDRH(ins, rd, reg_rn); 
			}
		}
		break;

		case 0b10: //Signed byte
		{
			if (!load) {
				(useImmediateOffset == true) ? opSTRH(ins, rd, address)
					: opSTRH(ins, rd, reg_rn);
			}
			else { 
				(useImmediateOffset == true) ? opLDRSB(ins, rd, address) 
					: opLDRSB(ins, rd, reg_rn); 
			}
		}
		break;

		case 0b11: //Signed halfword
		{
			if (!load) {
				(useImmediateOffset == true) ? opSTRH(ins, rd, address)
					: opSTRH(ins, rd, reg_rn);
			}
			else {
				(useImmediateOffset == true) ? opLDRSH(ins, rd, address) 
					: opLDRSH(ins, rd, reg_rn); 
			}
		}
		break;
	}

	return 1;
}

u8 Arm::executeMiscLoadStoreImm(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode3.immOffsetIndex(ins);
	executeMiscLoadAndStore(ins, result);

	return 1;
}

u8 Arm::executeMiscLoadStoreReg(ArmInstruction& ins)
{
	AddrModeLoadStoreResult result = addrMode3.registerOffsetIndex(ins);
	executeMiscLoadAndStore(ins, result);

	return 1;
}

u8 Arm::executeLDM(ArmInstruction& ins)
{
	u8 PU = (ins.P() << 1) | ins.U();
	AddrMode4Result result;
	switch (PU) {
		case 0b01: result = addrMode4.incrementAfter(ins); break;
		case 0b11: result = addrMode4.incrementBefore(ins); break;
		case 0b00: result = addrMode4.decrementAfter(ins); break;
		case 0b10: result = addrMode4.decrementBefore(ins); break;
	}

	return 1;
}

u8 Arm::executeSTM(ArmInstruction& ins)
{
	return 1;
}

u8 Arm::opMOV(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);
	
	reg_rd = shifter_op;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opADD(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	//Shifter carry out is not needed here
	u8 shifter_carry_out = 0;
	bool carry = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn + shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	carry = carryFrom(reg_rn, shifter_op);
	overflow = overflowFromAdd(reg_rn, shifter_op);
	
	if (flags) {
		setCC(reg_rd, !carry, overflow);
	}

	return 1;
}

u8 Arm::opAND(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn & shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, false, false, true, shifter_carry_out);
	}

	return 1;
}


u8 Arm::opEOR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn ^ shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opSUB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn - shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	borrow = borrowFrom(reg_rn, shifter_op);
	overflow = overflowFromSub(reg_rn, shifter_op);
	
	if (flags) {
		setCC(reg_rd, borrow, overflow);
	}

	return 1;
}

u8 Arm::opRSB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

 	u32 shifter_op = (immediate == true) ? 
		addrMode1.imm(ins, shifter_carry_out) :  addrMode1.shift(ins, shifter_carry_out);
	
	u32 result = shifter_op - reg_rn;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	borrow = borrowFrom(shifter_op, reg_rn);
	overflow = overflowFromSub(shifter_op, reg_rn);

	if (flags) {
		setCC(reg_rd, !borrow, overflow);
	}

	return 1;
}

u8 Arm::opADC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool carry = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn + (shifter_op + getFlag(C));
	reg_rd = result;
	writeRegister(rd, reg_rd);

	carry = carryFrom(reg_rn, shifter_op + getFlag(C));
	overflow = overflowFromAdd(reg_rn, shifter_op + getFlag(C));

	if (flags) {
		setCC(reg_rd, !carry, overflow);
	}

	return 1;
}

u8 Arm::opSBC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn - (shifter_op - !getFlag(C));
	reg_rd = result;
	writeRegister(rd, reg_rd);

	borrow = borrowFrom(reg_rn, shifter_op - !getFlag(C));
	overflow = overflowFromSub(reg_rn, shifter_op - !getFlag(C));

	if (flags) {
		setCC(reg_rd, !borrow, overflow);
	}

	return 1;
}

u8 Arm::opRSC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	bool borrow = false;
	bool overflow = false;

	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = shifter_op - (reg_rn - !getFlag(C));
	reg_rd = result;
	writeRegister(rd, reg_rd);

	borrow = borrowFrom(shifter_op, (reg_rn - !getFlag(C)));
	overflow = overflowFromSub(shifter_op, (reg_rn - !getFlag(C)));

	if (flags) {
		setCC(reg_rd, !borrow, overflow);
	}

	return 1;
}

u8 Arm::opTST(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn & shifter_op;

	(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(result == 0) ? setFlag(Z) : clearFlag(Z);
	(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);
	

	return 1;
}

u8 Arm::opTEQ(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn ^ shifter_op;

	(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
	(result == 0) ? setFlag(Z) : clearFlag(Z);
	(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);


	return 1;
}

u8 Arm::opCMP(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn - shifter_op;

	bool borrow = borrowFrom(reg_rn, shifter_op);
	bool overflow = overflowFromSub(reg_rn, shifter_op);

	setCC(result, borrow, overflow);

	return 1;
}

u8 Arm::opCMN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn + shifter_op;

	bool carry = carryFrom(reg_rn, shifter_op);
	bool overflow = overflowFromAdd(reg_rn, shifter_op);

	setCC(result, !carry, overflow);

	return 1;
}

u8 Arm::opORR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn | shifter_op;
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opBIC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	u32 result = reg_rn & ~(shifter_op);
	reg_rd = result;
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opMVN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
	bool flags, bool immediate)
{
	u32 reg_rd = getRegister(rd);

	u8 shifter_carry_out = 0;
	u32 shifter_op = (immediate == true) ?
		addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

	reg_rd = ~(shifter_op);
	writeRegister(rd, reg_rd);

	if (flags) {
		setCC(reg_rd, false, false, true, shifter_carry_out);
	}

	return 1;
}

u8 Arm::opB(ArmInstruction& ins)
{
	R15 = (R15 + (signExtend32(ins.offset(), 24) << 2));
	flushPipeline();

	return 1;
}

u8 Arm::opBL(ArmInstruction& ins)
{
	LR = R15 - 4;
	R15 = (R15 + (signExtend32(ins.offset(), 24) << 2));
	flushPipeline();

	return 1;
}

u8 Arm::opBX(ArmInstruction& ins)
{
	RegisterID rn = ins.rn();
	u32 reg_rn = getRegister(rn);
	
	(reg_rn & 0x1) == 1 ? setFlag(T) : clearFlag(T);
	R15 = reg_rn & 0xFFFFFFFE;
	flushPipeline();

	return 1;
}

u8 Arm::opSWI(ArmInstruction& ins)
{
	
	return 1;
}


u8 Arm::opSTRH(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u32 reg_rd = getRegister(rd);
	mbus->writeU16(address, reg_rd & 0xFFFF);

	return 1;
}


u8 Arm::opLDRH(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u16 value = mbus->readU16(address);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opLDRSB(ArmInstruction& ins, RegisterID rd, u32 address)
{
	s8 value = (s8)mbus->readU8(address);
	value = signExtend32(value, 8);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opLDRSH(ArmInstruction& ins, RegisterID rd, u32 address)
{
	s16 value = (s16)mbus->readU16(address);
	value = signExtend32(value, 16);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opLDRB(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u8 value = mbus->readU8(address);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opLDR(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u32 value = mbus->readU32(address);
	writeRegister(rd, value);

	return 1;
}

u8 Arm::opSTRB(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u32 reg_rd = getRegister(rd);
	mbus->writeU8(address, reg_rd & 0xFF);

	return 1;
}

u8 Arm::opSTR(ArmInstruction& ins, RegisterID rd, u32 address)
{
	u32 reg_rd = getRegister(rd);
	mbus->writeU32(address, reg_rd);

	return 1;
}


