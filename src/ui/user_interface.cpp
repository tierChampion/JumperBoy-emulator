#include <ui/user_interface.h>

#include <gb.h>

#include <filesystem>
#include <fstream>

namespace jmpr
{
	UI::UI(PPU *ppu, APU *apu)
		: _opened(true), _game_window(GameWindow(ppu, apu)),
		  _controls({false, false, 0.05f, true, true, true, true, 0}),
		  _input_preset(0)
	{
		initImGui();
		loadSettings();
	}

	void UI::cleanup()
	{
		saveSettings();

		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyRenderer(_imgui_renderer);
		SDL_DestroyWindow(_imgui_window);
		_game_window.cleanup();

		SDL_Quit();
	}

	void UI::saveSettings()
	{
		// create folder
		std::filesystem::path folderPath = std::string(DIRECTORY_PATH) + std::string("/settings/");

		if (!std::filesystem::exists(folderPath))
		{
			if (!std::filesystem::create_directories(folderPath))
			{
				std::cerr << "Error creating the folder!" << std::endl;
			}
		}

		// recents
		std::ofstream recentFile(std::string(DIRECTORY_PATH) + std::string("/settings/recents.csv"));

		if (recentFile.is_open())
		{

			for (std::string recent : _recents)
			{
				recentFile << recent << "\n";
			}

			recentFile.close();
		}
		else
		{
			std::cerr << "Error: could not open the recent file." << std::endl;
		}

		// controls
		std::ofstream controlsFile(std::string(DIRECTORY_PATH) + std::string("/settings/controls.csv"));

		if (controlsFile.is_open())
		{
			for (u8 i = 1; i < _input_maps.size(); i++)
			{
				std::map<std::string, JumperInput> map = _input_maps[i];

				controlsFile << "{\n";
				for (auto &pair : map)
				{
					if (pair.second != JumperInput::NONE)
						controlsFile << pair.first << ":" << inputToName(pair.second) << "\n";
				}
				controlsFile << "}\n";
			}

			controlsFile.close();
		}
		else
		{
			std::cerr << "Error: could not open the controls file." << std::endl;
		}
	}

	void UI::loadSettings()
	{
		// recents
		std::ifstream recentFile(std::string(DIRECTORY_PATH) + std::string("/settings/recents.csv"));

		if (recentFile.is_open())
		{
			std::string line;
			while (std::getline(recentFile, line))
			{
				_recents.push_back(line);
			}

			recentFile.close();
		}

		// controls
		_input_maps.push_back(std::map<std::string, JumperInput>());

		_input_maps[0]["W"] = JumperInput::UP;
		_input_maps[0]["A"] = JumperInput::LEFT;
		_input_maps[0]["S"] = JumperInput::DOWN;
		_input_maps[0]["D"] = JumperInput::RIGHT;
		_input_maps[0]["H"] = JumperInput::A;
		_input_maps[0]["J"] = JumperInput::B;
		_input_maps[0]["N"] = JumperInput::SELECT;
		_input_maps[0]["M"] = JumperInput::START;

		std::ifstream controlsFile(std::string(DIRECTORY_PATH) + std::string("/settings/controls.csv"));

		if (controlsFile.is_open())
		{
			std::string line;
			while (std::getline(controlsFile, line))
			{
				if (line[0] == '{')
				{
					_input_maps.push_back(std::map<std::string, JumperInput>());
				}
				else if (line[0] != '}')
				{
					size_t pos = line.find(":");

					_input_maps[_input_maps.size() - 1][line.substr(0, pos)] =
						nameToInput(line.substr(pos + 1, line.size()));
				}
			}

			controlsFile.close();
		}

		// pallets
		_game_window.addPallet({0xFF0FBC9B,
								0xFF0FAC8B,
								0xFF306230,
								0xFF0F380F});

		_game_window.addPallet({0xFFDDDDDD,
								0xFFAAAAAA,
								0xFF555555,
								0xFF000000});
		// std::ifstream palletsFile(std::string(DIRECTORY_PATH) + std::string("/settings/pallets.csv"));

		// if (palletsFile.is_open())
		// {
		// 	std::string line;
		// 	while (std::getline(palletsFile, line))
		// 	{
		// 		if (line[0] == '{')
		// 		{
		// 			_input_maps.push_back(std::map<std::string, JumperInput>());
		// 		}
		// 		else if (line[0] != '}')
		// 		{
		// 			size_t pos = line.find(":");

		// 			_input_maps[_input_maps.size() - 1][line.substr(0, pos)] =
		// 				nameToInput(line.substr(pos + 1, line.size()));
		// 		}
		// 	}

		// 	controlsFile.close();
		// }
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		renderImGui();

		_game_window.render();
	}
}