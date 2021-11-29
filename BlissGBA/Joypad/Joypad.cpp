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
	mbus->writeU16(KEYINPUT, currentInput);
}

void Joypad::buttonPressed(Button button, bool pressed)
{
	//0 = pressed
	if (pressed) {
		switch (button) {
			case Button::Button_A: currentInput &= ~(A_); break;
			case Button::Button_B: currentInput &= ~(B_); break;
			case Button::Select: currentInput &= ~(Select_); break;
			case Button::Start: currentInput &= ~(Start_); break;
			case Button::Right: currentInput &= ~(Right_); break;
			case Button::Left: currentInput &= ~(Left_); break;
			case Button::Up: currentInput &= ~(Up_); break;
			case Button::Down: currentInput &= ~(Down_);  break;
			case Button::Button_R: currentInput &= ~(R_); break;
			case Button::Button_L: currentInput &= ~(L_); break;
		}
	}
	//1 = released
	else {
		switch (button) {
			case Button::Button_A: currentInput ^= (A_); break;
			case Button::Button_B: currentInput ^= (B_); break;
			case Button::Select: currentInput ^= (Select_); break;
			case Button::Start: currentInput ^= (Start_); break;
			case Button::Right: currentInput ^= (Right_); break;
			case Button::Left: currentInput ^= (Left_); break;
			case Button::Up: currentInput ^= (Up_); break;
			case Button::Down: currentInput ^= (Down_);  break;
			case Button::Button_R: currentInput ^= (R_); break;
			case Button::Button_L: currentInput ^= (L_); break;
		}
	}
}
