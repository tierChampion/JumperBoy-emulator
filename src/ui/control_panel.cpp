#include <ui/user_interface.h>

#include <gb.h>

#include <algorithm>

namespace jmpr
{

    const u8 MAXIMUM_RECENT_COUNT = 10;

    void UI::initImGui()
    {
        _imgui_window = SDL_CreateWindow("JumperBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500,
                                         SDL_WINDOW_RESIZABLE);
        _imgui_renderer = SDL_CreateRenderer(_imgui_window, -1,
                                             SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForSDLRenderer(_imgui_window, _imgui_renderer);
        ImGui_ImplSDLRenderer2_Init(_imgui_renderer);
    }

    void UI::initControls()
    {
        _controls.browser = false;
        _controls.tiles = false;
        _controls.tileBank = 0;
        _controls.vol = 0.05f;
        _controls.channel1 = true;
        _controls.channel2 = true;
        _controls.channel3 = true;
        _controls.channel4 = true;
        _controls.pallet = 0;
        _controls.palletCreation = false;
        _controls.controls = false;
        _controls.capped = true;
        _controls.fps = 59.7;
    }

    void UI::initFileBrowser()
    {
        _file_browser = ImGui::FileBrowser();
        _file_browser.SetTitle("Search for ROM");
        _file_browser.SetTypeFilters({".gb", ".gbc"});
        _file_browser.SetPwd(DIRECTORY_PATH);

        _file_browser.Open();
        _controls.browser = true;
    }

    void UI::renderImGui()
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            fileMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options"))
        {
            optionsMenu();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        browserWindow();
        controlsWindow();
        palletWindow();

        ImGui::Render();
        SDL_RenderClear(_imgui_renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(_imgui_renderer);
    }

    void UI::fileMenu()
    {
        if (ImGui::MenuItem("Open"))
            initFileBrowser();
        if (ImGui::BeginMenu("Open recent"))
        {
            for (s8 i = _recents.size() - 1; i >= 0; i--)
            {
                std::string recent = _recents[i];

                if (ImGui::MenuItem(recent.c_str()))
                {
                    openROM(recent);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Quit"))
            _opened = false;
    }

    void UI::optionsMenu()
    {
        if (ImGui::Button("Reboot"))
        {
            GameBoy::getInstance()->reboot();
        }
        if (ImGui::Checkbox("Show tiles", &_controls.tiles))
        {
            _game_window.toggleDebug(_controls.tileBank & 0x2);
            _controls.tileBank = (_controls.tileBank + 1) & 0x3;
        }
        if (ImGui::SliderFloat("Volume", &_controls.vol, 0.0f, 1.0f, "%.2f"))
        {
            GameBoy::getInstance()->setVolume(_controls.vol);
        }
        if (ImGui::BeginMenu("Audio channels"))
        {
            if (ImGui::Checkbox("Channel 1", &_controls.channel1))
            {
                GameBoy::getInstance()->toggleAudioChannel(0);
            }
            if (ImGui::Checkbox("Channel 2", &_controls.channel2))
            {
                GameBoy::getInstance()->toggleAudioChannel(1);
            }
            if (ImGui::Checkbox("Channel 3", &_controls.channel3))
            {
                GameBoy::getInstance()->toggleAudioChannel(2);
            }
            if (ImGui::Checkbox("Channel 4", &_controls.channel4))
            {
                GameBoy::getInstance()->toggleAudioChannel(3);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginListBox("Pallet"))
        {
            for (u8 i = 0; i < _game_window.getPalletsSize(); i++)
            {
                if (ImGui::Selectable(std::to_string(i).c_str(), _controls.pallet == i))
                {
                    _controls.pallet = i;
                    _game_window.setUsedPallet(_controls.pallet);
                }
            }
            ImGui::EndListBox();
        }
        if (ImGui::BeginListBox("Controls"))
        {
            for (u8 i = 0; i < _input_maps.size(); i++)
            {
                if (ImGui::Selectable(std::to_string(i).c_str(), _input_preset == i))
                {
                    _input_preset = i;
                }
            }
            ImGui::EndListBox();
        }
        if (ImGui::Button("Add pallet..."))
        {
            _controls.palletCreation = true;
        }
        if (ImGui::Button("Add controls..."))
        {
            _controls.controls = true;
        }
        if (ImGui::SliderFloat("FPS", &_controls.fps, 1.0f, 150.0f, "%.1f"))
        {
            GameBoy::getInstance()->setDesiredFPS(_controls.fps);
        }
        if (ImGui::Checkbox("Capped FPS", &_controls.capped))
        {
            GameBoy::getInstance()->setCapped(_controls.capped);
        }
    }

    void UI::controlsWindow()
    {
        if (_controls.controls)
        {
            ImGui::Begin("Select your controls:");
            for (u8 i = 1; i <= static_cast<u8>(JumperInput::MAX_SPEED); i++)
            {
                if (ImGui::Selectable(
                        ("(" + inputToName(static_cast<JumperInput>(i)) + ") " + _controls.inputs[i - 1]).c_str()))
                {
                    _controls.inputs[i - 1] = std::toupper(_last_input.keysym.sym);
                }
            }

            if (ImGui::Button("Confirm"))
            {
                std::map<std::string, JumperInput> inputMap;
                for (u8 i = 1; i <= static_cast<u8>(JumperInput::MAX_SPEED); i++)
                {
                    if (_controls.inputs[i - 1] == "")
                    {
                        std::cerr << "Error: The new control map is incomplete." << std::endl;
                        ImGui::End();
                        return;
                    }
                    inputMap[_controls.inputs[i - 1]] = static_cast<JumperInput>(i);
                }
                _input_maps.push_back(inputMap);
                _controls.controls = false;
            }
            ImGui::End();
        }
    }

    void UI::palletWindow()
    {
        if (_controls.palletCreation)
        {
            ImGui::Begin("Create your new pallet");
            ImGui::ColorPicker3("Color #1:", &_controls.colors[0]);
            ImGui::ColorPicker3("Color #2:", &_controls.colors[3]);
            ImGui::ColorPicker3("Color #3:", &_controls.colors[6]);
            ImGui::ColorPicker3("Color #4:", &_controls.colors[9]);

            if (ImGui::Button("Confirm"))
            {
                std::array<u16, 4> pallet;
                for (u8 i = 0; i < 4; i++)
                {
                    pallet[i] = (static_cast<u16>(_controls.colors[3 * i] * 31.0f)) |
                                ((static_cast<u16>(_controls.colors[3 * i + 1] * 31.0f) << 5)) |
                                ((static_cast<u16>(_controls.colors[3 * i + 2] * 31.0f) << 10));

                    std::cout << (int)pallet[i] << std::endl;
                }

                _game_window.addPallet(pallet);
                _controls.palletCreation = false;
            }
            ImGui::End();
        }
    }

    void UI::browserWindow()
    {
        if (_controls.browser)
        {
            _file_browser.Display();

            if (_file_browser.HasSelected())
            {
                openROM(_file_browser.GetSelected().string());

                _file_browser.ClearSelected();
                _controls.browser = false;
            }
        }
    }

    void UI::openROM(std::string romPath)
    {
        auto it = std::find(_recents.begin(), _recents.end(), romPath);
        if (it != _recents.end())
            _recents.erase(it);
        else if (_recents.size() == MAXIMUM_RECENT_COUNT)
            _recents.erase(_recents.begin());

        _recents.push_back(romPath);

        bool valid = GameBoy::getInstance()->insertCartridge(romPath);
        if (valid)
        {
            _game_window.open();
            GameBoy::getInstance()->resume();
        }
    }
}
