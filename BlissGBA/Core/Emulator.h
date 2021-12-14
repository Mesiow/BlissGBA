#pragma once
#include "../Debugger/DebugUI.h"
#include "../Ppu/Ppu.h"
#include "../Joypad/Joypad.h"


class Emulator {
public:
	Emulator(sf::RenderWindow *window, float displayScaleFactor);
	void run();
	void render(sf::RenderTarget& target);
	void reset();
	void handleEvents(sf::Event& ev);

	MemoryBus mbus;
	Ppu ppu;
	Arm cpu;
	DmaController dmac;
	Joypad joypad;
	DebugUI debug;

	bool debuggerRunning;
	bool showDebugger; //if false, emulator will render full screen
	bool running;
	const int scanlinesPerFrame = 228;
	const int maxCycles = (1232 * scanlinesPerFrame); //1232 cycles per scanline (308 dots * 4 cpu cycles)
	float displayScaleFactor;
};
