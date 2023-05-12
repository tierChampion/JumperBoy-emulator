#include <io/joypad.h>

#include <cpu/interrupt.h>

namespace jmpr {

	Joypad::Joypad(InterruptHandler* handler) {

		_p1_selection = 0xFC;
		_p1_input = 0xFF;

		_inp_handler = InputHandler(this);

		_int_handler = handler;
	}

	u8 Joypad::readP1Register() const {
		return merge16(loNibble(_p1_selection), loNibble(_p1_input));
	}

	/**
	* Write to the joypad register.
	*/
	void Joypad::writeP1Register(u8 data) {

		// Only the selection bits can be changed.
		_p1_selection = 0xFC | (data & 0x3);
	}

	/**
	* Enter an input. Only registered if the corresponding button type
	* is activated.
	*/
	void Joypad::pressInput(JoypadInput input) {

		u8 inputType = selectionBit(input);

		if (bit(_p1_selection, inputType) == 0) {

			_p1_input = reset(_p1_input, inputBit(input));

			// Request an interrupt
			_int_handler->requestInterrupt(InterruptType::JOYPAD);
		}
	}

	/**
	* Release an input. Only registered if the corresponding button type
	* is activated.
	*/
	void Joypad::releaseInput(JoypadInput input) {

		u8 inputType = selectionBit(input);

		if (bit(_p1_selection, inputType) == 0) {

			_p1_input = set(_p1_input, inputBit(input));
		}
	}

	/**
	* Get the index of the selection bit needed for the input to register.
	*/
	static u8 selectionBit(JoypadInput input) {
		return (((u8)input) / 4);
	}

	/**
	* Get the index of the input bit to modify.
	*/
	static u8 inputBit(JoypadInput input) {

		return ((u8)input) % 4;
	}
}