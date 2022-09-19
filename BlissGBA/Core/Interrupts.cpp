#include "Interrupts.h"
#include "../Memory/MemoryBus.h"

void requestInterrupt(MemoryBus* mbus, u8 interrupt)
{
	u16 irq_flag = mbus->mmio.readIF();
	irq_flag = setBit(irq_flag, interrupt);
	mbus->mmio.writeIF(irq_flag);
}
