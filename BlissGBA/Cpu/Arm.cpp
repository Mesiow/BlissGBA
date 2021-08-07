#include "Arm.h"
#include "../Memory/MemoryBus.h"

Arm::Arm(MemoryBus *mbus)
	:addrMode1(*this)
{
	this->mbus = mbus;
	reset();
}

u8 Arm::clock()
{
	state = getState();
	if (state == State::ARM) {
		u32 encoding = armpipeline[0];
		armpipeline[0] = armpipeline[1];
		armpipeline[1] = fetchU32();

		ArmInstruction ins;
		ins.encoding = encoding;
		executeArmIns(ins);
	}
	else if (state == State::THUMB) {
		u32 encoding = thumbpipeline[0];
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

	LR = 0x00000000;
	R15 = 0x08000000;
	SP = 0x03007F00;
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

	u32 result = rotatedIn | (rotatedOut << (31 - shift));
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
	RegisterID rd = ins.rd();
	RegisterID rn = ins.rn();

	//(B,BL)
	if (ins.branch() == 0b101) {
		return ((ins.l() == 0x0) ? opB(ins, cond) : opBL(ins, cond));
	}
	else {
		switch (ins.instruction()) {
			case 0b000100100001: return opBX(ins, cond, rn); break;
		}
	}
	
	switch (ins.opcode()) {
		case 0b0000: return opAND(ins, cond, rd, rn); break;
		case 0b0001: return opEOR(ins, cond, rd, rn); break;
		case 0b0010: return opSUB(ins, cond, rd, rn); break;
		case 0b0011: return opRSB(ins, cond, rd, rn); break;
		case 0b0100: return opADD(ins, cond, rd, rn); break;
		case 0b0101: return opADC(ins, cond, rd, rn); break;
		case 0b0110: return opSBC(ins, cond, rd, rn); break;
		case 0b0111: return opRSC(ins, cond, rd, rn); break;
		case 0b1000: return opTST(ins, cond, rd, rn); break;
		case 0b1001: return opTEQ(ins, cond, rd, rn); break;
		case 0b1010: return opCMP(ins, cond, rd, rn); break;
		case 0b1011: return opCMN(ins, cond, rd, rn); break;
		case 0b1100: return opORR(ins, cond, rd, rn); break;
		case 0b1101: return opMOV(ins, cond, rd, rn); break;
		case 0b1110: return opBIC(ins, cond, rd, rn); break;
		case 0b1111: return opMVN(ins, cond, rd, rn); break;
	}

}

u8 Arm::executeThumbIns(ThumbInstruction& ins)
{
	return 1;
}

u8 Arm::opMOV(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;
	
	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);
	
		reg_rd = shifter_op;
		writeRegister(rd, reg_rd);

		if (set) {
			setCC(reg_rd, false, false, true, shifter_carry_out);
		}
	}

	return 1;
}

u8 Arm::opADD(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
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
		
		if (set) {
			setCC(reg_rd, !carry, overflow);
		}
	}

	return 1;
}

u8 Arm::opAND(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn & shifter_op;
		reg_rd = result;
		writeRegister(rd, reg_rd);

		if (set) {
			setCC(reg_rd, false, false, true, shifter_carry_out);
		}
	}

	return 1;
}


u8 Arm::opEOR(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn ^ shifter_op;
		reg_rd = result;
		writeRegister(rd, reg_rd);

		if (set) {
			setCC(reg_rd, false, false, true, shifter_carry_out);
		}
	}

	return 1;
}

u8 Arm::opSUB(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
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
		
		if (set) {
			setCC(reg_rd, !borrow, overflow);
		}
	}

	return 1;
}

u8 Arm::opRSB(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
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

		if (set) {
			setCC(reg_rd, !borrow, overflow);
		}
	}

	return 1;
}

u8 Arm::opADC(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
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

		if (set) {
			setCC(reg_rd, !carry, overflow);
		}
	}

	return 1;
}

u8 Arm::opSBC(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
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

		if (set) {
			setCC(reg_rd, !borrow, overflow);
		}
	}

	return 1;
}

u8 Arm::opRSC(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
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

		if (set) {
			setCC(reg_rd, !borrow, overflow);
		}
	}

	return 1;
}

u8 Arm::opTST(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rn = getRegister(rn);
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn & shifter_op;

		(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
		(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);
	}

	return 1;
}

u8 Arm::opTEQ(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rn = getRegister(rn);
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn ^ shifter_op;

		(result >> 31) & 0x1 ? setFlag(N) : clearFlag(N);
		(result == 0) ? setFlag(Z) : clearFlag(Z);
		(shifter_carry_out == 1) ? setFlag(C) : clearFlag(C);
	}

	return 1;
}

u8 Arm::opCMP(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rn = getRegister(rn);
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn - shifter_op;

		bool borrow = borrowFrom(reg_rn, shifter_op);
		bool overflow = overflowFromSub(reg_rn, shifter_op);

		setCC(result, !borrow, overflow);
	}

	return 1;
}

u8 Arm::opCMN(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rn = getRegister(rn);
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn + shifter_op;

		bool carry = carryFrom(reg_rn, shifter_op);
		bool overflow = overflowFromAdd(reg_rn, shifter_op);

		setCC(result, !carry, overflow);
	}

	return 1;
}

u8 Arm::opORR(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn | shifter_op;
		reg_rd = result;
		writeRegister(rd, reg_rd);

		if (set) {
			setCC(reg_rd, false, false, true, shifter_carry_out);
		}
	}

	return 1;
}

u8 Arm::opBIC(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);
	u32 reg_rn = getRegister(rn);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		u32 result = reg_rn & ~(shifter_op);
		reg_rd = result;
		writeRegister(rd, reg_rd);

		if (set) {
			setCC(reg_rd, false, false, true, shifter_carry_out);
		}
	}

	return 1;
}

u8 Arm::opMVN(ArmInstruction& ins, u8 condition, RegisterID rd, RegisterID rn)
{
	u32 reg_rd = getRegister(rd);

	bool set = (ins.s() == 0x0) ? false : true;
	bool immediate = (ins.i() == 0x0) ? false : true;

	if (condition) {
		u8 shifter_carry_out = 0;
		u32 shifter_op = (immediate == true) ?
			addrMode1.imm(ins, shifter_carry_out) : addrMode1.shift(ins, shifter_carry_out);

		reg_rd = ~(shifter_op);
		writeRegister(rd, reg_rd);

		if (set) {
			setCC(reg_rd, false, false, true, shifter_carry_out);
		}
	}

	return 1;
}

u8 Arm::opB(ArmInstruction& ins, u8 condition)
{
	if (condition) {
		R15 = R15 + (ins.offset() << 2);
	}

	return 1;
}

u8 Arm::opBL(ArmInstruction& ins, u8 condition)
{
	if (condition) {
		LR = R15 - 4;
		R15 = R15 + (ins.offset() << 2);
	}

	return 1;
}

u8 Arm::opBX(ArmInstruction& ins, u8 condition, RegisterID rm)
{
	u32 reg_rm = getRegister(rm);
	if (condition) {
		(reg_rm & 0x1) == 1 ? setFlag(T) : clearFlag(T);
		R15 = reg_rm & 0xFFFFFFFE;
	}

	return 1;
}


