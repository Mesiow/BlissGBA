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
	void renderGeneralState();
	void renderBankedRegisters();
	void renderMenuBar();
	void renderInterrupts();
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
	static MemoryEditor palRamEditor;
	static MemoryEditor vramEditor;
	static MemoryEditor oamEditor;
	static MemoryEditor ioEditor;
	static MemoryEditor obwramEditor;
	static MemoryEditor ocwramEditor;
	static MemoryEditor gamepakMemory;

	Logger logger;
	Comparer cmper;
	char logFileName[50];
	bool compareAgainstFile;

	bool* running = nullptr;
	bool *showDebugger = nullptr;
	bool runToAddr;
	bool runToOpcode;
	bool showRegisterWindow;
	bool showBankedRegisters;
	bool showBiosMemory;
	bool showPALRAM;
	bool showVRAM;
	bool showOAM;
	bool showIO;
	bool showOBWRAM;
	bool showOCWRAM;
	bool showGamePakMemory;
	bool showCartWindow;
	bool showInterruptsWindow;
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
	std::string stepCountStr;
	u32 stepCount;

	sf::RenderWindow* window;

	Emulator* emu;
	MemoryBus* mbus;
	Arm* cpu;
	Ppu* ppu;
};
