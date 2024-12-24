#include <ui/user_interface.h>

#include <gb.h>

#include <filesystem>
#include <fstream>

namespace jmpr
{
	UI::UI(PPU *ppu, APU *apu, BootRom *boot)
		: _opened(true),
		  _audio_manager(apu),
		  _boot(boot),
		  _last_input(SDL_KeyboardEvent())
	{
		_settings = std::make_unique<UserSettings>();
		_game_window = GameWindow(ppu, _settings.get());
		initImGui();
		initControls();
	}

	void UI::cleanup()
	{
		_settings->save();

		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyRenderer(_imgui_renderer);
		SDL_DestroyWindow(_imgui_window);
		_game_window.cleanup();

		SDL_Quit();
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		_game_window.render();
		renderImGui();
	}
}
