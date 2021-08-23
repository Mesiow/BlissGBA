#pragma once
#include "../Debugger/DebugUI.h"
#include "../Ppu/Ppu.h"

class Emulator {
public:
	Emulator(sf::RenderWindow *window);
	void run();
	void render();
	void reset();

	MemoryBus mbus;
	Ppu ppu;
	Arm cpu;
	DebugUI debug;

	bool showDebugger; //if false, emulator will render full screen
	bool running;
	const int scanlinesPerFrame = 228;
	const int maxCycles = (1232 * scanlinesPerFrame); //1232 cycles per scanline (308 dots * 4 cpu cycles)
};
