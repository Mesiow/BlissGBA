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
#include "Logger.h"

class Emulator;
class MemoryBus;
class Arm;
class Ppu;

struct DebugUI {
	DebugUI(sf::RenderWindow *window, Emulator *emu);
	void render();
	void renderRegisters();
	void renderMenuBar();
	void renderCartInfo();
	void renderPipeline();
	void renderDisplay();
	void renderEmuButtons();
	void renderLogSetup();
	void update();
	void handleButtonPresses();
	void handleEvents(sf::Event& ev);

	void onDebugUIToggle();

	static MemoryEditor biosMemory;
	static MemoryEditor vramEditor;
	static MemoryEditor ioEditor;
	static MemoryEditor gamepakMemory;

	Logger logger;
	char logFileName[50];

	bool* running = nullptr;
	bool *showDebugger = nullptr;
	bool runToAddr;
	bool runToOpcode;
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
	bool showLoggerSetup;
	bool showKeys[2];
	

	u32 addressToRunTo;
	char addressBufferText[11];
	u32 armOpcodeToRunTo;
	u16 thumbOpcodeToRunTo;
	char opcodeBufferText[9];

	sf::RenderWindow* window;

	Emulator* emu;
	MemoryBus* mbus;
	Arm* cpu;
	Ppu* ppu;
};
