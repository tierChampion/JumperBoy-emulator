#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr {

	void UI::handleEvents(bool gamePlaying) {

		_curr_input = "";

		while (SDL_PollEvent(&_curr_event)) {

			ImGui_ImplSDL2_ProcessEvent(&_curr_event);

			switch (_curr_event.type) {

			case SDL_QUIT:
				_opened = false; break;

			case SDL_KEYDOWN: {

				if (gamePlaying)
					_inp_handler->onKeyDown(_curr_event.key);
				break;
			}

			case SDL_KEYUP: {

				if (gamePlaying)
					_inp_handler->onKeyUp(_curr_event.key);
				break;
			}
			}
		}
	}
}