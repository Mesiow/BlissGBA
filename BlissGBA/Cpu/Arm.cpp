#include "Arm.h"

Arm::Arm()
{
	reset();
}

void Arm::reset()
{
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

}

void Arm::setFlag(u32 flagBits)
{

}

void Arm::clearFlag(u32 flagBits)
{

}

u32 Arm::getPC()
{
	u32 pc = ((PC >> 2) & 0xFFFFFF);
	return pc;
}
