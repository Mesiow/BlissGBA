#pragma once
#include "../Utils/Utils.h"
#include "../Core/io.h"
#include <SFML/Graphics.hpp>

class MemoryBus;
using Key = sf::Keyboard::Key;

#define A_ (1)
#define B_ (1 << 1)
#define Select_ (1 << 2)
#define Start_ (1 << 3)
#define Right_ (1 << 4)
#define Left_ (1 << 5)
#define Up_ (1 << 6)
#define Down_ (1 << 7)
#define R_ (1 << 8)
#define L_ (1 << 9)

enum class Button : u8 {
	Button_A = sf::Keyboard::Key::A,
	Button_B = sf::Keyboard::Key::S,
	Select = sf::Keyboard::Key::D,
	Start = sf::Keyboard::Key::Enter,
	Right = sf::Keyboard::Key::Right,
	Left = sf::Keyboard::Key::Left,
	Up = sf::Keyboard::Key::Up,
	Down = sf::Keyboard::Key::Down,
	Button_R = sf::Keyboard::Key::R,
	Button_L = sf::Keyboard::Key::E
};

struct Joypad {
	Joypad(MemoryBus* mbus);
	void reset();
	void update();
	void buttonPressed(Button button, bool pressed);

	u16 currentInput;
	MemoryBus* mbus;
};