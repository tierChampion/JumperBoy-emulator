#pragma once

#include <common.h>
#include <ui/inputs.h>
#include <ui/game_window.h>
#include <ui/audio_manager.h>
#include <ui/user_settings.h>

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
	class BootRom;

	enum class PathType
	{
		ROM,
		BOOT,
		SAVE
	};

	struct Controls
	{
		bool fileBrowser;
		bool folderBrowser;
		bool boot;
		bool tiles;
		u8 tileBank;
		float vol;
		bool channel1;
		bool channel2;
		bool channel3;
		bool channel4;
		u8 pallet;
		bool palletCreation;
		char palletName[32];
		float colors[12] = {0};
		bool controls;
		char inputName[32];
		std::string inputs[9] = {""};
		PathType pathType;
		char path[256];
		bool capped;
		float fps;
	};

	class UI
	{
		bool _opened;

		GameWindow _game_window;
		AudioManager _audio_manager;

		SDL_Event _curr_event;
		SDL_KeyboardEvent _last_input;

		SDL_Window *_imgui_window;
		SDL_Renderer *_imgui_renderer;
		ImGuiIO _imgui_io;
		ImGui::FileBrowser _file_browser;
		ImGui::FileBrowser _folder_browser;

		Controls _controls;

		std::unique_ptr<UserSettings> _settings;

		BootRom *_boot;

	public:
		UI() {}
		UI(PPU *ppu, APU *apu, BootRom *boot);

		void cleanup();
		bool isOpened() { return _opened; }
		void loop(bool gamePlaying);
		std::string &getSaveFolder() const { return _settings->save_folder; }

	private:
		void initImGui();
		void initControls();
		void initFileBrowser();
		void initFolderBrowser();

		void handleEvents(bool gamePlaying);

		void render();

		void renderImGui();
		void openROM(const std::string &romPath);
		void openBoot(const std::string &bootPath);
		void fileMenu();
		void controlsMenu();
		void visualsMenu();
		void audioMenu();
		void miscellaneousMenu();
		void pathMenu();
		void palletWindow();
		void browserWindow();
		void pathWindow();
		void controlsWindow();
	};
}