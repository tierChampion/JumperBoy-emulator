#pragma once

#include <common.h>

#include <SDL/SDL.h>

namespace jmpr {

	class Joypad;

	class InputHandler {

		Joypad* _joypad;

	public:

		InputHandler() {}
		InputHandler(Joypad* joypad);

		// keyboard events
		void onKeyDown(const SDL_KeyboardEvent event);
		void onKeyUp(const SDL_KeyboardEvent event);

		// support other events latter...
	};

}