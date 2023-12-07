#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{
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

    void UI::initFileBrowser()
    {
        _file_browser = ImGui::FileBrowser();
        _file_browser.SetTitle("Search for ROM");
        _file_browser.SetTypeFilters({".gb", ".gbc"});

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
            if (ImGui::MenuItem("Open"))
                initFileBrowser();
            if (ImGui::MenuItem("Quit"))
                _opened = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::Checkbox("Show tiles", &_controls.tiles))
            {
                _game_window.toggleDebug();
            }
            if (ImGui::SliderFloat("Volume", &_controls.vol, 0.0f, 1.0f))
            {
                GameBoy::setVolume(_controls.vol);
            }
            if (ImGui::Checkbox("Channel 1", &_controls.channel1))
            {
                GameBoy::toggleAudioChannel(0);
            }
            if (ImGui::Checkbox("Channel 2", &_controls.channel2))
            {
                GameBoy::toggleAudioChannel(1);
            }
            if (ImGui::Checkbox("Channel 3", &_controls.channel3))
            {
                GameBoy::toggleAudioChannel(2);
            }
            if (ImGui::Checkbox("Channel 4", &_controls.channel4))
            {
                GameBoy::toggleAudioChannel(3);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        if (_controls.browser)
        {
            _file_browser.Display();

            if (_file_browser.HasSelected())
            {
                GameBoy::insertCartridge(_file_browser.GetSelected().string());
                _game_window.open();
                _file_browser.ClearSelected();
                _controls.browser = false;
            }
        }

        // 3. Show another simple window.
        bool show_another_window = true;
        if (show_another_window)
        {
            ImGui::Begin("Settings window");
            ImGui::Text("Hello from another window!");
            // if (ImGui::Button("Debug ?"))
            // 	_game_window.toggleDebug();
            ImGui::End();
        }

        ImGui::Render();
        SDL_RenderClear(_imgui_renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(_imgui_renderer);
    }
}