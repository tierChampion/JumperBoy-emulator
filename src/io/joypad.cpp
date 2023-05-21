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

		u8 dirNibble = loNibble(_p1_input) | (0b1111 * bit(_p1_selection, 0));
		u8 butNibble = hiNibble(_p1_input) | (0b1111 * bit(_p1_selection, 1));

		// hinibble & lonibble

		return merge8(loNibble(_p1_selection), (dirNibble & butNibble));
	}

	/**
	* Write to the joypad register.
	*/
	void Joypad::writeP1Register(u8 data) {

		// Only the selection bits can be changed.
		_p1_selection = 0xFC | (hiNibble(data) & 0x3);
	}

	/**
	* Enter an input. Interrupt requested if the input type is allowed.
	*/
	void Joypad::pressInput(JoypadInput input) {

		_p1_input = reset(_p1_input, u8(input));

		if (bit(_p1_selection, selectionBit(input)) == 0) {
			// Request an interrupt
			_int_handler->requestInterrupt(InterruptType::JOYPAD);
		}
	}

	/**
	* Release an input. Only registered if the corresponding button type
	* is activated.
	*/
	void Joypad::releaseInput(JoypadInput input) {

		_p1_input = set(_p1_input, u8(input));
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