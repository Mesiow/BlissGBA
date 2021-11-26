#include "Emulator.h"

Emulator::Emulator(sf::RenderWindow *window, float displayScaleFactor)
	:mbus(), ppu(&mbus), cpu(&mbus), debug(window, this)
{
	this->displayScaleFactor = displayScaleFactor;
	showDebugger = true;
	debuggerRunning = true;
	running = false;
	debug.running = &running;
	debug.showDebugger = &showDebugger;

	mbus.loadGamePak("test_roms/gba-tests-master/arm/arm.gba");
	reset();
}

void Emulator::run()
{
	if (running) {
		s32 cycles_this_frame = 0;
		while (cycles_this_frame < maxCycles) {
			if (debuggerRunning)
				debug.update();

			u8 cycle = cpu.clock();
			cycle *= 1;
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
	cpu.reset();
	ppu.reset();
}

void Emulator::handleEvents(sf::Event& ev)
{
	if(debuggerRunning)
		debug.handleEvents(ev);
}
