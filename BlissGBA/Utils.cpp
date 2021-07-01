#include "Utils.h"

u8 setBit(u8 val, u8 bitToSet)
{
	u8 bit = (1 << bitToSet);
	val |= bit;
	return val;
}

u8 resetBit(u8 val, u8 bitToReset)
{
	u8 bit = (1 << bitToReset);
	val &= ~(bit);
	return val;
}

u8 testBit(u8 val, u8 bitToTest)
{
	u8 bit = (1 << bitToTest);
	return ((val & bit) > 0 ? 1 : 0);
}

u32 getNthBits(u32 value, u8 from, u8 to)
{
	u32 mask = 0;
	for (s32 i = from; i <= to; i++) {
		mask |= 1 << i;
	}
	u32 result = value & mask;
	return result;
}
