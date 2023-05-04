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

	static u8 selectionBit(JoypadInput input);
	static u8 inputBit(JoypadInput input);

	class Joypad {

		u8 _p1;

	public:

		Joypad();

		u8 readP1Register() const;
		void writeP1Register(u8 data);

		void pressInput(JoypadInput input);
		void releaseInput(JoypadInput input);
	};
}