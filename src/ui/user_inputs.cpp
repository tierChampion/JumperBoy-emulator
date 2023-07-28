#include <ui/user_interface.h>

#include <gb.h>
#include <tinyfiledialogs.h>

namespace jmpr {

	void UI::handleEvents(bool gamePlaying) {

		_curr_input = "";

		while (SDL_PollEvent(&_curr_event)) {

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

			case SDL_MOUSEBUTTONDOWN: {

				if (_curr_event.button.button == SDL_BUTTON_LEFT) {
					// Input a command to process afterwards.
					_curr_input = tinyfd_inputBox("Input command",
						"Enter a command: open_rom, set_controls", "");

					if (_curr_input) {
						_curr_input = tinyfd_openFileDialog(
							"Select ROM",
							DIRECTORY_PATH,
							1,
							_ROM_FILTER,
							NULL,
							0
						);

						GameBoy::insertCartridge(_curr_input);
					}
				}

				break;
			}
			}
		}
	}
}