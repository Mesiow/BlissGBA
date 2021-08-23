#include "Emulator.h"

Emulator::Emulator(sf::RenderWindow *window)
	:mbus(), ppu(), cpu(&mbus), debug(window, this)
{
	showDebugger = true;
	running = false;
	debug.running = &running;
}

void Emulator::run()
{
	if (running) {
		s32 cycles_this_frame = 0;
		while (cycles_this_frame < maxCycles) {
			u8 cycle = cpu.clock();
			cycle *= 4;
			//emulate timers
			ppu.update(cycles_this_frame);
			//emulate interrupts
		}
		ppu.bufferPixels();
	}
}

void Emulator::render()
{
	if(showDebugger) debug.render();
}

void Emulator::reset()
{

}
