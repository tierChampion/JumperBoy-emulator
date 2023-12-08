#pragma once

#include <common.h>

namespace jmpr {

	enum class JoypadInput {
		RIGHT,
		LEFT,
		UP,
		DOWN,
		A,
		B,
		SELECT,
		START
	};

	static u8 selectionBit(u8 input);
	static u8 inputBit(u8 input);

	class InterruptHandler;

	class Joypad {

		u8 _p1_selection;
		u8 _p1_input;

		InterruptHandler* _int_handler;

	public:

		Joypad() {}
		Joypad(InterruptHandler* handler);

		void reboot();

		u8 readP1Register() const;
		void writeP1Register(u8 data);

		void pressInput(u8 input);
		void releaseInput(u8 input);
	};
}