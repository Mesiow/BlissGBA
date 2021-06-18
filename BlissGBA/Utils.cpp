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
