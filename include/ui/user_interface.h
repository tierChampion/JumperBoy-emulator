#pragma once

#include <common.h>
#include <SDL/SDL.h>

namespace jmpr {

	class InputHandler;
	class VRAM;
	class PPU;

	struct VisualContext {
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		SDL_Texture* _texture;
		SDL_Surface* _surface;
	};

	class UI {

		bool _opened;

		VisualContext _vc;
		VisualContext _debug;
		PPU* _ppu;

		SDL_Event _curr_event;
		InputHandler* _inp_handler;
		const char* _curr_input;

		inline static const char* _ROM_FILTER[2] = { "*.gb", "*.gbc" };

	public:

		UI(PPU* ppu, InputHandler* inpHandler);
		~UI();

		bool isOpened() { return _opened; }
		void handleEvents(bool gamePlaying);

		void renderVideoBuffer();
		void displayTileData(VRAM* vram);

	private:

		bool initVisuals();

		void displaySingleTile(VRAM* vram, u16 tileId, u16 xPos, u16 yPos);

		void render(VisualContext context);
	};
}