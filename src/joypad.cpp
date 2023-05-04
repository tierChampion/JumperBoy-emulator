#include <joypad.h>

#include <interrupt.h>

namespace jmpr {

	Joypad::Joypad(InterruptHandler* handler) {

		_p1 = 0xCF;
		_inp_handler = InputHandler(this);

		_int_handler = handler;
	}

	u8 Joypad::readP1Register() const {
		return _p1;
	}

	/**
	* Write to the joypad register.
	*/
	void Joypad::writeP1Register(u8 data) {

		// Only the selection bits can be changed.
		_p1 = 0xC0 | (data & 0x30) | (loNibble(_p1));
	}

	/**
	* Enter an input. Only registered if the corresponding button type
	* is activated.
	*/
	void Joypad::pressInput(JoypadInput input) {

		u8 inputType = selectionBit(input);

		if (bit(_p1, inputType) == 0) {

			reset(_p1, inputBit(input));

			// Request an interrupt
			_int_handler->requestInterrupt(InterruptType::JOYPAD);
			// todo make sure this only runs for the first time the input is entered
		}
	}

	/**
	* Release an input. Only registered if the corresponding button type
	* is activated.
	*/
	void Joypad::releaseInput(JoypadInput input) {

		u8 inputType = selectionBit(input);

		if (bit(_p1, inputType) == 0) {

			set(_p1, inputBit(input));
		}
	}

	/**
	* Get the index of the selection bit needed for the input to register.
	*/
	static u8 selectionBit(JoypadInput input) {
		return (((u8)input) / 4) + 4;
	}

	/**
	* Get the index of the input bit to modify.
	*/
	static u8 inputBit(JoypadInput input) {

		return ((u8)input) % 4;
	}
}