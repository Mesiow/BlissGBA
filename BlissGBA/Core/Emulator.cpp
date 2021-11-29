#include "Emulator.h"

Emulator::Emulator(sf::RenderWindow *window, float displayScaleFactor)
	:mbus(), ppu(&mbus), cpu(&mbus), joypad(&mbus), debug(window, this)
{
	this->displayScaleFactor = displayScaleFactor;
	showDebugger = true;
	debuggerRunning = true;
	running = false;
	debug.running = &running;
	debug.showDebugger = &showDebugger;

	mbus.loadGamePak("test_roms/armwrestler.gba");
	reset();
}

void Emulator::run()
{
	if (running) {
		s32 cycles_this_frame = 0;
		while (cycles_this_frame < maxCycles) {
			if (debuggerRunning)
				debug.update();

			u8 cycle = cpu.clock();
			cycle *= 1;
			cycles_this_frame += cycle;

			//emulate timers
			ppu.update(cycles_this_frame);
			//emulate interrupts		
		}

		joypad.update();
		ppu.bufferPixels();
	}
}

void Emulator::render(sf::RenderTarget &target)
{
	if(showDebugger) debug.render();
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
	if(debuggerRunning)
		debug.handleEvents(ev);

	if (ev.type == sf::Event::KeyPressed) {
		if (ev.key.code == (Key)Button::Button_A) {
			printf("a pressed\n");
			joypad.buttonPressed(Button::Button_A, true);
		}
		if (ev.key.code == (Key)Button::Button_B) {
			joypad.buttonPressed(Button::Button_B, true);
		}
		if (ev.key.code == (Key)Button::Select) {
			joypad.buttonPressed(Button::Select, true);
		}
		if (ev.key.code == (Key)Button::Start) {
			printf("start pressed\n");
			joypad.buttonPressed(Button::Start, true);
		}
		if (ev.key.code == (Key)Button::Right) {
			joypad.buttonPressed(Button::Right, true);
		}
		if (ev.key.code == (Key)Button::Left) {
			joypad.buttonPressed(Button::Left, true);
		}
		if (ev.key.code == (Key)Button::Up) {
			printf("up pressed\n");
			joypad.buttonPressed(Button::Up, true);
		}
		if (ev.key.code == (Key)Button::Down) {
			printf("down pressed\n");
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
			printf("a released\n");
			joypad.buttonPressed(Button::Button_A, false);
		}
		if (ev.key.code == (Key)Button::Button_B){
			joypad.buttonPressed(Button::Button_B, false);
		}
		if (ev.key.code == (Key)Button::Select) {
			joypad.buttonPressed(Button::Select, false);
		}
		if (ev.key.code == (Key)Button::Start) {
			printf("start released\n");
			joypad.buttonPressed(Button::Start, false);
		}
		if (ev.key.code == (Key)Button::Right) {
			joypad.buttonPressed(Button::Right, false);
		}
		if (ev.key.code == (Key)Button::Left) {
			joypad.buttonPressed(Button::Left, false);
		}
		if (ev.key.code == (Key)Button::Up) {
			printf("up released\n");
			joypad.buttonPressed(Button::Up, false);
		}
		if (ev.key.code == (Key)Button::Down) {
			printf("down released\n");
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