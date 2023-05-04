#include <user_interface.h>

#include <gb.h>
#include <tinyfiledialogs.h>

namespace jmpr {

	UI::UI(InputHandler* inpHandler) {

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		// Creation of the window
		_window = SDL_CreateWindow("JumperBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			256, 256, 0);

		_opened = (_window == NULL ? false : true);

		_inp_handler = inpHandler;
	}

	UI::~UI() {

		SDL_DestroyWindow(_window);
		SDL_Quit();
	}

	void UI::handleEvents() {

		_curr_input = "";

		while (SDL_PollEvent(&_curr_event)) {

			switch (_curr_event.type) {

			case SDL_QUIT: _opened = false; break;

			case SDL_KEYDOWN:
				_inp_handler->onKeyDown(_curr_event.key); break;

			case SDL_KEYUP:
				_inp_handler->onKeyUp(_curr_event.key); break;

			case SDL_MOUSEBUTTONDOWN: {

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

				break;
			}
			}
		}
	}
}