#pragma once

#include <common.h>
#include <SDL/SDL.h>

namespace jmpr {

	class InputHandler;

	class UI {

		bool _opened;

		SDL_Window* _window;
		SDL_Event _curr_event;

		InputHandler* _inp_handler;

		const char* _curr_input;

		inline static const char* _ROM_FILTER[2] = { "*.gb", "*.gbc" };

	public:

		UI(InputHandler* inpHandler);
		~UI();

		bool isOpened() { return _opened; }
		void handleEvents(bool gamePlaying);
	};
}