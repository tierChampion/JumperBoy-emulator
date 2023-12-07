#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{
	UI::UI(PPU *ppu, APU *apu, InputHandler *inpHandler)
		: _opened(true), _game_window(GameWindow(ppu, apu)),
		  _controls({false, false, 0.05f, true, true, true, true})
	{
		initImGui();
		_inp_handler = inpHandler;
	}

	void UI::cleanup()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyWindow(_imgui_window);
		SDL_DestroyRenderer(_imgui_renderer);
		_game_window.cleanup();

		SDL_Quit();
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		renderImGui();

		_game_window.render();
	}
}