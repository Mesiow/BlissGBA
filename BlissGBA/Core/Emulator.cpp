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
}

void Emulator::handleInput()
{
	if (sf::Keyboard::isKeyPressed((Key)Button::Button_A))
		joypad.buttonPressed(Button::Button_A, true);
	else
		joypad.buttonPressed(Button::Button_A, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Button_B))
		joypad.buttonPressed(Button::Button_B, true);
	else
		joypad.buttonPressed(Button::Button_B, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Select))
		joypad.buttonPressed(Button::Select, true);
	else
		joypad.buttonPressed(Button::Select, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Start))
		joypad.buttonPressed(Button::Start, true);
	else 
		joypad.buttonPressed(Button::Start, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Right))
		joypad.buttonPressed(Button::Right, true);
	else
		joypad.buttonPressed(Button::Right, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Left))
		joypad.buttonPressed(Button::Left, true);
	else
		joypad.buttonPressed(Button::Left, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Up))
		joypad.buttonPressed(Button::Up, true);
	else
		joypad.buttonPressed(Button::Up, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Down))
		joypad.buttonPressed(Button::Down, true);
	else
		joypad.buttonPressed(Button::Down, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Button_R))
		joypad.buttonPressed(Button::Button_R, true);
	else
		joypad.buttonPressed(Button::Button_R, false);

	if (sf::Keyboard::isKeyPressed((Key)Button::Button_L))
		joypad.buttonPressed(Button::Button_L, true);
	else
		joypad.buttonPressed(Button::Button_L, false);
}
