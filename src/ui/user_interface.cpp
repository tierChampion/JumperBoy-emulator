#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr {

	UI::UI(PPU* ppu, APU* apu, InputHandler* inpHandler) {

		// Initialize SDL Video
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		_opened = initVisuals();
		initAudio(apu);

		_ppu = ppu;
		_inp_handler = inpHandler;
	}

	UI::~UI() {

		SDL_DestroyTexture(_vc._texture);
		SDL_DestroyRenderer(_vc._renderer);
		SDL_DestroyWindow(_vc._window);

		SDL_DestroyTexture(_debug._texture);
		SDL_DestroyRenderer(_debug._renderer);
		SDL_DestroyWindow(_debug._window);

		SDL_Quit();
	}
}