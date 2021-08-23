#pragma once
#include "imgui.h"
#include "imgui_memory_editor.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <filesystem>
#include "tinyfiledialogs.h"
#include "../Memory/MemoryBus.h"
#include "../Cpu/Arm.h"
#include "../Ppu/Ppu.h"

class Emulator;
class MemoryBus;
class Arm;
class Ppu;

struct DebugUI {
	DebugUI(sf::RenderWindow *window, Emulator *emu);
	void render();
	void renderRegisters();
	void renderButtons();
	void renderMenuBar();
	void renderCartInfo();
	void renderPipeline();
	void renderDisplay();
	void update();
	void handleButtonPresses();
	void handleEvents(sf::Event& ev);

	static MemoryEditor biosMemory;
	static MemoryEditor vramEditor;
	static MemoryEditor ioEditor;
	static MemoryEditor gamepakMemory;

	bool* running = nullptr;
	bool showRegisterWindow;
	bool showBiosMemory;
	bool showVRAM;
	bool showIO;
	bool showGamePakMemory;
	bool showCartWindow;
	bool showPPUWindow;
	bool showPipeline;
	bool showDisplay;
	bool vsync;

	sf::RenderWindow* window;

	Emulator* emu;
	MemoryBus* mbus;
	Arm* cpu;
	Ppu* ppu;
};
