#include <ui/user_interface.h>

#include <gb.h>

#include <fstream>

namespace jmpr
{
	UI::UI(PPU *ppu, APU *apu, InputHandler *inpHandler)
		: _opened(true), _game_window(GameWindow(ppu, apu)),
		  _controls({false, false, 0.05f, true, true, true, true})
	{
		initImGui();
		_inp_handler = inpHandler;

		std::ifstream recentFile(std::string(DIRECTORY_PATH) + std::string("/settings/recents.csv"));

		if (recentFile.is_open()) {
			std::string line;
			if (recentFile.good()) {
				while (std::getline(recentFile, line))
				{
					_recents.push_back(line);
				}
			}
		}
	}

	UI::~UI()
	{
		// save recents to file
		std::ofstream recentFile(std::string(DIRECTORY_PATH) + std::string("/settings/recents.csv"));

		for (std::string recent : _recents)
		{
			recentFile << recent << "\n";
		}

		recentFile.close();
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