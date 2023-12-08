#pragma once

#include <common.h>
#include <ui/inputs.h>
#include <ui/game_window.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imfilebrowser.h>

#include <SDL2/SDL.h>

#include <map>

namespace jmpr
{
	class InputHandler;
	class VRAM;
	class PPU;
	class APU;

	struct Controls
	{
		bool browser;
		bool tiles;
		float vol;
		bool channel1;
		bool channel2;
		bool channel3;
		bool channel4;
	};

	class UI
	{
		bool _opened;

		GameWindow _game_window;

		SDL_Event _curr_event;
		const char *_curr_input;

		SDL_Window *_imgui_window;
		SDL_Renderer *_imgui_renderer;
		ImGuiIO _imgui_io;
		ImGui::FileBrowser _file_browser;

		Controls _controls;
		std::vector<std::string> _recents;
		std::vector<std::map<std::string, JumperInput>> _input_maps;
		u8 _input_preset;

	public:
		UI(PPU *ppu, APU *apu);
		void cleanup();

		bool isOpened() { return _opened; }

		void loop(bool gamePlaying);

	private:
		void initImGui();
		void initFileBrowser();

		void handleEvents(bool gamePlaying);

		void render();
		void renderImGui();
		void openROM(std::string romPath);

		void loadSettings();
		void saveSettings();
	};
}