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
	PC = 0x80000000;
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

u8 Arm::isFlagSet(u32 flag)
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
	u32 pc = ((PC >> 2) & 0xFFFFFF);
	return pc;
}

State Arm::getState()
{
	u8 st = ((CPSR & T) >> T_BIT);
	return State(st);
}

u32 Arm::readU32()
{
	u32 word = (mbus->readU8(PC)) |
		(mbus->readU8(PC + 1) << 8) |
		(mbus->readU8(PC + 2) << 16) |
		(mbus->readU8(PC + 3) << 24);

	return word;
}

u32 Arm::fetchU32()
{
	u32 word = (mbus->readU8(PC++)) |
		(mbus->readU8(PC++) << 8) |
		(mbus->readU8(PC++) << 16) |
		(mbus->readU8(PC++) << 24);

	return word;
}

u8 Arm::opMOV(ArmInstruction& ins)
{
	u8 cond = ins.cond();
	u8 rd = ins.rd();
	u8 i = ins.i();
	u8 s = ins.s();

	bool set = (s == 0x0) ? false : true;
	bool immediate = (i == 0x0) ? false : true;

	if (set) {

	}

	return 1;
}
