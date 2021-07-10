#pragma once
#include "imgui.h"
#include "imgui_memory_editor.h"
#include <SFML/Graphics.hpp>
#include <string>
#include "../Cpu/Arm.h"

struct DebugUI {
	DebugUI(Arm *cpu);
	void render();
	void update();
	void handleButtonPresses();
	void handleEvents(sf::Event& ev);

	static MemoryEditor mainMemory;
	static MemoryEditor bootRomMemory;

	Arm* cpu;
};
