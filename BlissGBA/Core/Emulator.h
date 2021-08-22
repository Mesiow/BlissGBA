#pragma once
#include "../Utils.h"


class Emulator {
	void run();

	bool running;
	const int scanlinesPerFrame = 228;
	const int maxCycles = (1232 * scanlinesPerFrame); //1232 cycles per scanline (308 dots * 4 cpu cycles)
};
