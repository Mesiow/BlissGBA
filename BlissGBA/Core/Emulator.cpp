#include "Emulator.h"

Emulator::Emulator(sf::RenderWindow *window, float displayScaleFactor)
	:mbus(), ppu(&mbus), cpu(&mbus), debug(window, this)
{
	this->displayScaleFactor = displayScaleFactor;
	showDebugger = true;
	running = false;
	debug.running = &running;
	debug.showDebugger = &showDebugger;
}

void Emulator::run()
{
	if (running) {
		s32 cycles_this_frame = 0;
		while (cycles_this_frame < maxCycles) {
			u8 cycle = cpu.clock();
			cycle *= 4;
			cycles_this_frame += cycle;

			//emulate timers
			ppu.update(cycles_this_frame);
			//emulate interrupts
		}
		ppu.bufferPixels();
	}
}

void Emulator::render(sf::RenderTarget &target)
{
	if(showDebugger) debug.render();
	else {
		ppu.render(target);
	}
}

void Emulator::reset()
{

}

void Emulator::handleEvents(sf::Event& ev)
{
	debug.handleEvents(ev);
}
