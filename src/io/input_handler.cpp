#include <io/input_handler.h>

#include <io/joypad.h>
#include <unordered_map>
#include <fstream>

namespace jmpr {

	static const std::unordered_map<std::string, JoypadInput> JOYPAD_INPUT_LOOKUP = {
		{"RIGHT", JoypadInput::RIGHT},
		{"LEFT", JoypadInput::LEFT},
		{"UP", JoypadInput::UP},
		{"DOWN", JoypadInput::DOWN},
		{"A", JoypadInput::A},
		{"B", JoypadInput::B},
		{"SELECT", JoypadInput::SELECT},
		{"START", JoypadInput::START}
	};

	JoypadInput fromName(std::string inputName) {

		return JOYPAD_INPUT_LOOKUP.at(inputName);
	}

	InputHandler::InputHandler(Joypad* joypad) {

		_joypad = joypad;

		std::ifstream stream(std::string(DIRECTORY_PATH) + std::string(CONTROLS_FILE));
		_input_map = nlohmann::json::parse(stream);

		stream.close();
	}

	void InputHandler::onKeyDown(const SDL_KeyboardEvent event) {

		if (event.repeat != 0) return;

		if (_input_map.contains(SDL_GetKeyName(event.keysym.sym))) {

			_joypad->pressInput(fromName(_input_map[SDL_GetKeyName(event.keysym.sym)]));
		}
	}

	void InputHandler::onKeyUp(const SDL_KeyboardEvent event) {

		if (event.repeat != 0) return;

		if (_input_map.contains(SDL_GetKeyName(event.keysym.sym))) {

			_joypad->releaseInput(fromName(_input_map[SDL_GetKeyName(event.keysym.sym)]));
		}
	}
}