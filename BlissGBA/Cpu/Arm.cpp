#include "Arm.h"
#include "../Memory/MemoryBus.h"

Arm::Arm(MemoryBus *mbus)
{
	this->mbus = mbus;
	reset();
}

u8 Arm::clock()
{
	state = getState();
	if (state == State::ARM) {
		u32 encoding = fetchU32();
		u8 opcode = fetchOp(encoding);

		decodeAndExecute(encoding, opcode);
	}
	else if (state == State::THUMB) {

	}
	return cycles;
}

void Arm::decodeAndExecute(u32 encoding, u8 opcode)
{
	if (state == State::ARM) {
		ArmInstruction ins;
		ins.encoding = encoding;

		cycles += armlut[opcode](ins);
	}
	else if (state == State::THUMB) {
		//ThumbInstruction ins;
	}
}

void Arm::reset()
{
	cycles = 0;
	for (s32 i = 0; i < 0xD; i++) {
		registers[i].value = 0x00000000;
	}

	LR = 0x00000000;
	R15 = 0x80000000;
	SP = 0x00000000;
	CPSR = 0x6000001F;
	SPSR = 0x03007F00;
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

u8 Arm::fetchOp(u32 encoding)
{
	u8 opcode = ((encoding >> 21) & 0xF);
	return opcode;
}

u32 Arm::getPC()
{
	u32 pc = ((R15 >> 2) & 0xFFFFFFFF);
	return pc;
}

u32 Arm::getRegister(u8 id)
{
	if (id >= 0xD) {
		if (id == 0xD) return SP;
		if (id == 0xE) return LR;
		if (id == 0xF) return R15;
	}
	return registers[id].value;
}

void Arm::writeRegister(u8 id, u32 value)
{
	if (id >= 0xD) {
		if (id == 0xD) SP = value;
		if (id == 0xE) LR = value;
		if (id == 0xF) R15 = value;

		return;
	}
	registers[id].value = value;
}

void Arm::writePC(u32 pc)
{
	R15 |= ((pc << 2) & 0xFFFFFFFF);
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
		case 0b1110: return 1; //always
	}
}

u32 Arm::readU32()
{
	u32 PC = getPC();

	u32 word = (mbus->readU8(PC)) |
		(mbus->readU8(PC + 1) << 8) |
		(mbus->readU8(PC + 2) << 16) |
		(mbus->readU8(PC + 3) << 24);

	return word;
}

u32 Arm::fetchU32()
{
	u32 PC = getPC();

	u32 word = (mbus->readU8(PC++)) |
		(mbus->readU8(PC++) << 8) |
		(mbus->readU8(PC++) << 16) |
		(mbus->readU8(PC++) << 24);

	writePC(PC);

	return word;
}

u32 Arm::shift(u32 value, u8 amount, u8 type, u8 &shiftedBit)
{
	switch (type) {
		case 0b00: 
			shiftedBit = (31 - (amount - 1));
			shiftedBit = ((value >> shiftedBit) & 0x1);
			value <<= amount; 
			break;
		case 0b01:
			shiftedBit = (amount - 1);
			shiftedBit = ((value >> shiftedBit) & 0x1);
			value >>= amount; 
			break;
		case 0b10: 
			u8 msb = ((value >> 31) & 0x1);
			value >>= amount;
			value |= (msb << 31);
			break;
		case 0b11: 
			u8 lsb = (value & 0x1);
			value >>= amount;
			value |= (lsb << 31);
			break;
	}
	return value;
}

u8 Arm::opMOV(ArmInstruction& ins)
{
	u8 cond = ins.cond();
	u8 rd = ins.rd();
	u8 i = ins.i();
	u8 s = ins.s();

	u32 reg_rd = getRegister(rd);

	u8 condition = getConditionCode(cond);
	bool set = (s == 0x0) ? false : true;
	bool immediate = (i == 0x0) ? false : true;

	
	if (condition) {
		u8 shiftedBit = 0;
		if (immediate) {
			//op2 is an immediate value
			u8 result = addrMode1.imm(ins, shiftedBit);
			reg_rd = result;

			writeRegister(rd, reg_rd);
			
		}
		else {
			//op2 is a register
			u32 result = addrMode1.regShift(ins, shiftedBit);
			reg_rd = result;

			writeRegister(rd, reg_rd);
		}

		if (set) {
			(reg_rd & 0x80000000) ? setFlag(N) : clearFlag(N);
			(reg_rd == 0) ? setFlag(Z) : clearFlag(Z);
			(shiftedBit == 1) ? setFlag(C) : clearFlag(C);
		}
	}

	return 1;
}

