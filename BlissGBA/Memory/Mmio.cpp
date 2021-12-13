#include "Mmio.h"
#include "GeneralMemory.h"

Mmio::Mmio(GeneralMemory* gm)
{
	this->gm = gm;
}

void Mmio::setIF(u16 value)
{
	u8 hi, lo;
	lo = value & 0xFF;
	hi = (value >> 8) & 0xFF;

	u32 addr = IF - IO_START_ADDR;
	gm->io[addr] = lo;
	gm->io[addr + 1] = hi;
}
