#include "Emulator.h"

Emulator::Emulator(sf::RenderWindow *window, float displayScaleFactor)
	:mbus(), ppu(&mbus, displayScaleFactor), cpu(&mbus), dmac(&mbus),
	joypad(&mbus), debug(window, this)
{
	this->displayScaleFactor = displayScaleFactor;
	showDebugger = false;
	debuggerRunning = false;
	running = true;
	debug.running = &running;
	debug.showDebugger = &showDebugger;

	//Pass dma controller pointer to mmio so that mmio can
	//tell dma when there is a transfer ready from a received write
	mbus.mmio.connect(&cpu);
	mbus.mmio.connect(&dmac);

	//Cpu/irq Test roms
	//mbus.loadGamePak("test_roms/gba-tests-master/arm/arm.gba"); //pass
	//mbus.loadGamePak("test_roms/gba-tests-master/thumb/thumb.gba"); //pass
	//mbus.loadGamePak("test_roms/armwrestler-gba-fixed.gba"); //pass
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/ARM_DataProcessing.gba"); //pass
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/ARM_Any.gba"); //pass
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/FuzzARM.gba"); //pass
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/THUMB_DataProcessing.gba"); //pass
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/THUMB_Any.gba"); //pass
	//mbus.loadGamePak("test_roms/tonc tests/swi_demo.gba"); 
	//mbus.loadGamePak("test_roms/swi.gba");
	//mbus.loadGamePak("test_roms/CPUTest.gba"); 
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/main.gba"); //pass
	//mbus.loadGamePak("test_roms/gba-tests-master/memory/memory.gba"); //pass
	//mbus.loadGamePak("test_roms/irqs/retAddr.gba"); //passing
	//mbus.loadGamePak("test_roms/irqs/irqDemo.gba"); //passing
	//mbus.loadGamePak("test_roms/irqs/irqDemo_2.gba");
	//mbus.loadGamePak("test_roms/yoshi_dma.gba"); //passing
	//mbus.loadGamePak("test_roms/gang-ldmstm.gba");
	//mbus.loadGamePak("test_roms/gba-tests-master/bios/bios.gba");
	//mbus.loadGamePak("test_roms/tonc tests/txt_obj.gba");

	//Mode 0 test roms
	//mbus.loadGamePak("test_roms/tonc tests/brin_demo.gba");
	//mbus.loadGamePak("test_roms/tonc tests/irq_demo.gba");

	//mbus.loadGamePak("test_roms/tonc tests/pageflip.gba"); //passing

	//Playable
	mbus.loadGamePak("roms/AGBDOOM.gba");

	//Boots to menu
    //mbus.loadGamePak("roms/Motoracer Advance (USA) (En,Fr,De,Es,It).gba");
	
	//mbus.loadGamePak("roms/Grand Theft Auto Advance (USA).gba");
	//mbus.loadGamePak("roms/Need for Speed - Porsche Unleashed (U).gba");
	reset();
}

void Emulator::run()
{
	if (running) {
		s32 cycles_this_frame = 0;
		while (cycles_this_frame < maxCycles) {
			if (debuggerRunning)
				debug.update();
	
			u8 cycles = cpu.clock();
			cycles_this_frame += cycles;

			cpu.handleTimers();
			ppu.update(cycles);
			cpu.handleInterrupts();
			dmac.handleDMA();
		}

		joypad.update();
		ppu.bufferPixels();
	}
}

void Emulator::render(sf::RenderTarget &target)
{
	if(debuggerRunning) debug.render();
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
	debug.handleEvents(ev);

	if (ev.type == sf::Event::KeyPressed) {
		if (ev.key.code == (Key)Button::Button_A) {
			joypad.buttonPressed(Button::Button_A, true);
		}
		if (ev.key.code == (Key)Button::Button_B) {
			joypad.buttonPressed(Button::Button_B, true);
		}
		if (ev.key.code == (Key)Button::Select) {
			joypad.buttonPressed(Button::Select, true);
		}
		if (ev.key.code == (Key)Button::Start) {
			joypad.buttonPressed(Button::Start, true);
		}
		if (ev.key.code == (Key)Button::Right) {
			joypad.buttonPressed(Button::Right, true);
		}
		if (ev.key.code == (Key)Button::Left) {
			joypad.buttonPressed(Button::Left, true);
		}
		if (ev.key.code == (Key)Button::Up) {
			joypad.buttonPressed(Button::Up, true);
		}
		if (ev.key.code == (Key)Button::Down) {
			joypad.buttonPressed(Button::Down, true);
		}
		if (ev.key.code == (Key)Button::Button_R) {
			joypad.buttonPressed(Button::Button_R, true);
		}
		if (ev.key.code == (Key)Button::Button_L) {
			joypad.buttonPressed(Button::Button_L, true);
		}
	}
	else if (ev.type == sf::Event::KeyReleased) {
		if (ev.key.code == (Key)Button::Button_A) {
			joypad.buttonPressed(Button::Button_A, false);
		}
		if (ev.key.code == (Key)Button::Button_B){
			joypad.buttonPressed(Button::Button_B, false);
		}
		if (ev.key.code == (Key)Button::Select) {
			joypad.buttonPressed(Button::Select, false);
		}
		if (ev.key.code == (Key)Button::Start) {
			joypad.buttonPressed(Button::Start, false);
		}
		if (ev.key.code == (Key)Button::Right) {
			joypad.buttonPressed(Button::Right, false);
		}
		if (ev.key.code == (Key)Button::Left) {
			joypad.buttonPressed(Button::Left, false);
		}
		if (ev.key.code == (Key)Button::Up) {
			joypad.buttonPressed(Button::Up, false);
		}
		if (ev.key.code == (Key)Button::Down) {
			joypad.buttonPressed(Button::Down, false);
		}
		if (ev.key.code == (Key)Button::Button_R) {
			joypad.buttonPressed(Button::Button_R, false);
		}
		if (ev.key.code == (Key)Button::Button_L) {
			joypad.buttonPressed(Button::Button_L, false);
		}
	}
}