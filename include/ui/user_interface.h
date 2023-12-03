#pragma once

#include <common.h>

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

		SDL_AudioDeviceID _audio_id;

		SDL_Event _curr_event;
		InputHandler* _inp_handler;
		const char* _curr_input;

		ImGuiIO _imgui_io;
		ImGui::FileBrowser _file_browser;

		inline static const char* _ROM_FILTER[2] = { "*.gb", "*.gbc" };

	public:

		UI(PPU* ppu, APU* apu, InputHandler* inpHandler);
		~UI();

		bool isOpened() { return _opened; }
		void handleEvents(bool gamePlaying);

		void render();

	private:

		bool initVisuals();
		void initAudio(APU* apu);

		void renderVideoBuffer();
		void displayTileData();
		void displaySingleTile(u16 tileId, u16 xPos, u16 yPos);

		void renderInContext(VisualContext context);
	};
}