#pragma once
#include "imgui.h"
#include "imgui_memory_editor.h"
#include <SFML/Graphics.hpp>
#include <string>
#include "../Cpu/Arm.h"
#include "../Memory/MemoryBus.h"

struct DebugUI {
	DebugUI(sf::RenderWindow *window, MemoryBus *mbus, Arm *cpu);
	void render();
	void renderRegisters();
	void renderMenuBar();
	void update();
	void handleButtonPresses();
	void handleEvents(sf::Event& ev);

	static MemoryEditor mainMemory;
	static MemoryEditor biosMemory;

	bool showRegisterWindow;
	bool showBiosMemory;
	bool vsync;

	bool sign;
	bool zero;
	bool carry;
	bool overflow;

	sf::RenderWindow* window;
	MemoryBus* mbus;
	Arm* cpu;
};
