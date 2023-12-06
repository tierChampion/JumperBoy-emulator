#pragma once

#include <common.h>
#include <ui/game_window.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imfilebrowser.h>

#include <SDL2/SDL.h>

namespace jmpr {

	class InputHandler;
	class VRAM;
	class PPU;
	class APU;

	// Rethink of the way the visuals work. 
	// Make a dolphin like interface (ui window creates a sdl window with the game)
	// Create a window and destroy it when not needed anymore.
	// Might need to create multiple windows, which visible flags get turned off when
	// closed.

	// Add a bunch of parameters for the game window
	// When creating the debug tile version, resize window and reinstanciate the renderer and texture and surface.

	// Make a window for controlling those parameters, choosing the rom and other.

	class UI {

		bool _opened;

		GameWindow _game_window;

		SDL_Event _curr_event;
		InputHandler* _inp_handler;
		const char* _curr_input;

		SDL_Window* _imgui_window;
		SDL_Renderer* _imgui_renderer;

		ImGuiIO _imgui_io;
		ImGui::FileBrowser _file_browser;

		inline static const char* _ROM_FILTER[2] = { "*.gb", "*.gbc" };

	public:

		UI(PPU* ppu, APU* apu, InputHandler* inpHandler);
		void cleanup();

		bool isOpened() { return _opened; }

		void loop(bool gamePlaying);

	private:

		void initImGui();

		void handleEvents(bool gamePlaying);

		void render();
		void renderDefault();
		void renderImGui();
	};
}