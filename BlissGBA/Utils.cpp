#include "Utils.h"

u32 setBit(u32 val, u8 bitToSet)
{
	u32 mask = (1 << bitToSet);
	val |= mask;
	return val;
}

u32 resetBit(u32 val, u8 bitToReset)
{
	u32 mask = (1 << bitToReset);
	val &= ~(mask);
	return val;
}

u8 testBit(u32 val, u8 bitToTest)
{
	u32 mask = (1 << bitToTest);
	return ((val & mask) > 0 ? 1 : 0);
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

u32 signExtend32(s32 value, u8 start)
{
	u32 res = ((value) << (32 - start)) >> (32 - start);
	return res;
}

u16 signExtend16(s16 value, u8 start)
{
	u16 res = ((value << (16 - start)) >> (16 - start));
	return res;
}
