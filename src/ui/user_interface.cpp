#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{

	UI::UI(PPU *ppu, APU *apu, InputHandler *inpHandler)
		: _opened(true), _game_window(GameWindow(ppu, apu))
	{
		_game_window.open();
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

		_file_browser = ImGui::FileBrowser();
		_file_browser.SetTitle("Search for ROM");
		_file_browser.SetTypeFilters({".gb", ".gbc"});

		_file_browser.Open();
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
		if (ImGui::BeginMenu("Wot"))
			ImGui::EndMenu();

		ImGui::EndMainMenuBar();

		// bool show_demo_window = true;
		// if (show_demo_window)
		// 	ImGui::ShowDemoWindow(&show_demo_window);

		// 3. Show another simple window.
		bool show_another_window = true;
		if (show_another_window)
		{
			ImGui::Begin("Settings window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Open game"))
				_game_window.open();

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