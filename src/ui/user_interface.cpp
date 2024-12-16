#include <ui/user_interface.h>

#include <gb.h>

#include <filesystem>
#include <fstream>

namespace jmpr
{
	UI::UI(std::shared_ptr<PPU> ppu, APU *apu)
		: _opened(true),
		  _game_window(ppu),
		  _audio_manager(apu),
		  _input_preset(0),
		  _last_input(SDL_KeyboardEvent())
	{
		initImGui();
		initControls();
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

		// pallets
		std::ofstream palletsFile(std::string(DIRECTORY_PATH) + std::string("/settings/pallets.csv"));

		if (palletsFile.is_open())
		{
			std::vector<std::array<u16, 4>> pallets = _game_window.getPallets();
			for (u8 i = 2; i < pallets.size(); i++)
			{
				palletsFile << "{\n";
				std::array<u16, 4> pallet = pallets[i];
				for (u8 j = 0; j < pallet.size(); j++)
				{
					palletsFile << std::hex << pallet[j] << "\n";
				}
				palletsFile << "}\n";
			}

			palletsFile.close();
		}
		else
		{
			std::cerr << "Error: could not open the pallets file." << std::endl;
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
		_game_window.addPallet({0xFFFF,
								0x3DEF,
								0x1CE7,
								0x0C63});
		std::ifstream palletsFile(std::string(DIRECTORY_PATH) + std::string("/settings/pallets.csv"));

		if (palletsFile.is_open())
		{
			std::string line;
			std::array<u16, 4> pallet;
			u8 i = 0;
			while (std::getline(palletsFile, line))
			{
				if (line[0] == '}')
				{
					_game_window.addPallet(pallet);
					i = 0;
				}
				else if (line[0] != '{')
				{
					pallet[i++] = static_cast<u32>(std::stoul(line, nullptr, 16));
					u32 test = 0;
				}
			}

			controlsFile.close();
		}
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		_game_window.render();
		renderImGui();
	}
}
