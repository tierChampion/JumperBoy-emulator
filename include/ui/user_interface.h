#pragma once

#include <common.h>
#include <ui/game_window.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imfilebrowser.h>

#include <SDL2/SDL.h>

namespace jmpr
{
	class InputHandler;
	class VRAM;
	class PPU;
	class APU;

	class UI
	{

		bool _opened;

		GameWindow _game_window;

		SDL_Event _curr_event;
		InputHandler *_inp_handler;
		const char *_curr_input;

		SDL_Window *_imgui_window;
		SDL_Renderer *_imgui_renderer;

		ImGuiIO _imgui_io;
		ImGui::FileBrowser _file_browser;
		bool _display_browser;

	public:
		UI(PPU *ppu, APU *apu, InputHandler *inpHandler);
		void cleanup();

		bool isOpened() { return _opened; }

		void loop(bool gamePlaying);

	private:
		void initImGui();
		void initFileBrowser();

		void handleEvents(bool gamePlaying);

		void render();
		void renderDefault();
		void renderImGui();
	};
}