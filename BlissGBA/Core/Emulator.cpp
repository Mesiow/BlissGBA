#include "Emulator.h"

Emulator::Emulator(sf::RenderWindow *window, float displayScaleFactor)
	:mbus(), ppu(&mbus, displayScaleFactor), cpu(&mbus), dmac(&mbus),
	tmc(&mbus), joypad(&mbus), debug(window, this)
{
	this->displayScaleFactor = displayScaleFactor;
	showDebugger = false;
	debuggerRunning = false;
	running = true;
	debug.running = &running;
	debug.showDebugger = &showDebugger;

	//Pass dma/timer controller pointer to mmio so that mmio can
	//tell dma/timer when a specific event happens
	mbus.mmio.connect(&cpu);
	mbus.mmio.connect(&dmac);
	mbus.mmio.connect(&tmc);

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
	//mbus.loadGamePak("test_roms/CPUTest.gba"); //passing 
	//mbus.loadGamePak("test_roms/gba fuzzarm tests/main.gba"); //pass
	//mbus.loadGamePak("test_roms/gba-tests-master/memory/memory.gba"); //pass
	//mbus.loadGamePak("test_roms/irqs/retAddr.gba"); //passing
	//mbus.loadGamePak("test_roms/irqs/irqDemo.gba"); //passing
	//mbus.loadGamePak("test_roms/irqs/irqDemo_2.gba");
	//mbus.loadGamePak("test_roms/yoshi_dma.gba"); //passing
	//mbus.loadGamePak("test_roms/gang-ldmstm.gba");
	//mbus.loadGamePak("test_roms/gba-tests-master/bios/bios.gba");
	//mbus.loadGamePak("test_roms/tonc tests/txt_obj.gba");
	//mbus.loadGamePak("test_roms/openbus_bios_misaligned.gba");

	//Mode 0 test roms
	//mbus.loadGamePak("test_roms/tonc tests/brin_demo.gba");
	//mbus.loadGamePak("test_roms/tonc tests/irq_demo.gba");
	//mbus.loadGamePak("test_roms/suite.gba");
	//mbus.loadGamePak("test_roms/gba-tests-destoer/if_ack/if_ack.gba");
	//mbus.loadGamePak("test_roms/gba-tests-destoer/isr/isr.gba");
	//mbus.loadGamePak("test_roms/tonc tests/prio_demo.gba");

	//mbus.loadGamePak("test_roms/tonc tests/pageflip.gba"); //passing
	//mbus.loadGamePak("test_roms/tonc tests/tmr_demo.gba");
	//mbus.loadGamePak("test_roms/AGB_CHECKER_TCHK10.gba");
	//mbus.loadGamePak("test_roms/biosOpenBus.gba");

	//Boots and Gets in game (seems playable, gameplay a bit slow though, not sure why yet)
	mbus.loadGamePak("roms/AGBDOOM.gba");
	//mbus.loadGamePak("roms/DOOM2.gba");

	//Boots to menu
    //mbus.loadGamePak("roms/Motoracer Advance (USA) (En,Fr,De,Es,It).gba");
	
	//mbus.loadGamePak("roms/Grand Theft Auto Advance (USA).gba");
	//mbus.loadGamePak("roms/Need for Speed - Porsche Unleashed (U).gba");
	//mbus.loadGamePak("roms/OpenLara.gba");
	//mbus.loadGamePak("roms/Wolfenstein 3D (USA, Europe).gba");

	//Boots
	//mbus.loadGamePak("roms/Kirby - Nightmare in Dream Land (USA).gba");
	//mbus.loadGamePak("roms/Pokemon - Emerald Version (USA, Europe).gba");
	//mbus.loadGamePak("roms/Super_Mario_Advance_3_-_Yoshis_Island_U_.gba");
	//mbus.loadGamePak("roms/MARIOLUI.gba");
	//mbus.loadGamePak("roms/Final Fight One (USA).gba");
	//mbus.loadGamePak("roms/FF6ADVAN.gba");
	//mbus.loadGamePak("roms/DRAGONBL.gba");
	//mbus.loadGamePak("roms/STARWARS.gba");
	//mbus.loadGamePak("roms/POKEMONR.gba");
	//mbus.loadGamePak("roms/POKEMONS.gba");
	//mbus.loadGamePak("roms/Mario Kart - Super Circuit (U)(Inferno).gba");

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

			tmc.handleTimers(cycles);
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