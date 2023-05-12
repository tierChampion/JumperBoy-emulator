#include <ui/user_interface.h>

#include <gb.h>
#include <tinyfiledialogs.h>

namespace jmpr {

	UI::UI(InputHandler* inpHandler) {

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		_opened = initVisuals();

		_inp_handler = inpHandler;
	}

	UI::~UI() {

		SDL_DestroyTexture(_texture);
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
	}
}