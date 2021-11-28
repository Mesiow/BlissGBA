#include "Joypad.h"
#include "../Memory/MemoryBus.h"

Joypad::Joypad(MemoryBus* mbus)
	:mbus(mbus)
{
	reset();
}

void Joypad::reset()
{
	//All inputs not pressed
	mbus->writeU16(KEYINPUT, 0x3FF);
	currentInput = 0x3FF;
}

void Joypad::update()
{
	keyinput = mbus->readU16(KEYINPUT);
	if (keyinput != currentInput) {
		keyinput &= currentInput;
		mbus->writeU16(KEYINPUT, keyinput);
	}
}

void Joypad::buttonPressed(Button button)
{
	//0 = pressed
	switch (button) {
		case Button::Start: currentInput &= ~(Start_); break;
	}
}

void Joypad::buttonReleased(Button button)
{
	//1 = released
	switch (button) {
		case Button::Start: currentInput |= (Start_); break;
	}
}
