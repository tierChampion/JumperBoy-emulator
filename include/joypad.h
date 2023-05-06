#pragma once

#include <common.h>

#include <input_handler.h>

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

	static u8 selectionBit(JoypadInput input);
	static u8 inputBit(JoypadInput input);

	class InterruptHandler;

	class Joypad {

		u8 _p1_selection;
		u8 _p1_input;

		InputHandler _inp_handler;

		InterruptHandler* _int_handler;

	public:

		Joypad(InterruptHandler* handler);

		InputHandler* getInputHandler() { return &_inp_handler; }

		u8 readP1Register() const;
		void writeP1Register(u8 data);

		void pressInput(JoypadInput input);
		void releaseInput(JoypadInput input);
	};
}