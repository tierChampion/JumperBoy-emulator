#include <input_handler.h>

#include <joypad.h>
#include <unordered_map>

namespace jmpr {

	InputHandler::InputHandler(Joypad* joypad) {

		_joypad = joypad;
	}


	// todo temporary, make it modifiable.
	// add another possible layout with a joypad
	static const std::unordered_map<SDL_Scancode, JoypadInput> INPUT_MAP = {
		{SDL_SCANCODE_W, JoypadInput::UP},
		{SDL_SCANCODE_S, JoypadInput::DOWN},
		{SDL_SCANCODE_A, JoypadInput::LEFT},
		{SDL_SCANCODE_D, JoypadInput::RIGHT},
		{SDL_SCANCODE_H, JoypadInput::A},
		{SDL_SCANCODE_J, JoypadInput::B},
		{SDL_SCANCODE_N, JoypadInput::SELECT},
		{SDL_SCANCODE_M, JoypadInput::START},
	};

	void InputHandler::onKeyDown(const SDL_KeyboardEvent event) {

		if (event.repeat != 0) return;

		// check if the input is mapped to a gameboy input
		auto pair = INPUT_MAP.find(event.keysym.scancode);

		if (pair != INPUT_MAP.end()) {

			std::cout << (int)pair->second << std::endl;
			//_joypad->pressInput(pair->second);
		}
	}

	void InputHandler::onKeyUp(const SDL_KeyboardEvent event) {

		if (event.repeat != 0) return;

		// check if the input is mapped to a gameboy input
		auto pair = INPUT_MAP.find(event.keysym.scancode);

		if (pair != INPUT_MAP.end()) {

			std::cout << (int)pair->second << std::endl;
			//_joypad->releaseInput(pair->second);
		}
	}
}