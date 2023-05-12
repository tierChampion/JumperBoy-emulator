#pragma once

#include <common.h>
#include <SDL/SDL.h>

namespace jmpr {

	class InputHandler;
	class Bus;

	class UI {

		bool _opened;

		SDL_Window* _window;
		SDL_Renderer* _renderer;
		SDL_Texture* _texture;
		SDL_Surface* _surface;

		SDL_Event _curr_event;
		InputHandler* _inp_handler;
		const char* _curr_input;

		inline static const char* _ROM_FILTER[2] = { "*.gb", "*.gbc" };

	public:

		UI(InputHandler* inpHandler);
		~UI();

		void render();

		bool isOpened() { return _opened; }
		void handleEvents(bool gamePlaying);

		void displayTileData(Bus* bus);// switch to bus

	private:

		bool initVisuals();

		void displaySingleTile(Bus* bus, u8 tileId, u8 xPos, u8 yPos);
	};
}