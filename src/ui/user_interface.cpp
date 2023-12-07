#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{

	UI::UI(PPU *ppu, APU *apu, InputHandler *inpHandler)
		: _opened(true), _game_window(GameWindow(ppu, apu)), _display_browser(false)
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
		_display_browser = true;
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		renderImGui();

		_game_window.render();
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
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		if (_display_browser)
		{
			_file_browser.Display();

			if (_file_browser.HasSelected())
			{
				GameBoy::insertCartridge(_file_browser.GetSelected().string().c_str());
				_game_window.open();
				_file_browser.ClearSelected();
			}
		}

		// bool show_demo_window = true;
		// if (show_demo_window)
		// 	ImGui::ShowDemoWindow(&show_demo_window);

		// 3. Show another simple window.
		bool show_another_window = true;
		if (show_another_window)
		{
			ImGui::Begin("Settings window");
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Debug ?"))
				_game_window.toggleDebug();
			ImGui::End();
		}

		ImGui::Render();
		SDL_RenderClear(_imgui_renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(_imgui_renderer);
	}
}