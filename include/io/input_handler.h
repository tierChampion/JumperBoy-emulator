#pragma once

#include <common.h>

#include <SDL2/SDL.h>
#include <json.hpp>

namespace jmpr {

#define CONTROLS_FILE "/settings/controls.json"

	class Joypad;
	enum class JoypadInput;

	JoypadInput fromName(const char* inputName);

	class InputHandler {

		Joypad* _joypad;
		nlohmann::json _input_map;

	public:

		InputHandler() {}
		InputHandler(Joypad* joypad);

		// keyboard events
		void onKeyDown(const SDL_KeyboardEvent event);
		void onKeyUp(const SDL_KeyboardEvent event);

		void updateInputMap();
	};
}