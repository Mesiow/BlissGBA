#include "Emulator.h"

void Emulator::run()
{
	if (running) {
		s32 cycles_this_frame = 0;
		while (cycles_this_frame < maxCycles) {
			//u8 cycle = cpu.clock();
			//cycle *= 4;
			//emulate timers
			//ppu.update(cycles_this_frame);
			//emulate interrupts
		}
		//update ppu framebuffer
	}
}
