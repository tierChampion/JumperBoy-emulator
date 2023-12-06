#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{

	UI::UI(PPU *ppu, APU *apu, InputHandler *inpHandler)
		: _opened(true), _game_window(GameWindow(ppu, apu))
	{
		_game_window.open();
		// initImGui();

		_inp_handler = inpHandler;
	}

	UI::~UI()
	{
		// ImGui_ImplSDLRenderer2_Shutdown();
		// ImGui_ImplSDL2_Shutdown();
		// ImGui::DestroyContext();
	}

	void UI::initImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		// ImGui_ImplSDL2_InitForSDLRenderer(_window, _renderer);
		// ImGui_ImplSDLRenderer2_Init(_renderer);

		_file_browser = ImGui::FileBrowser();
		_file_browser.SetTitle("Search for ROM");
		_file_browser.SetTypeFilters({".gb", ".gbc"});

		_file_browser.Open();
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		// renderImGui();

		_game_window.render();

		// ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		// SDL_RenderPresent(_renderer);
	}

	void UI::renderImGui()
	{
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		bool show_demo_window = true;
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Render();
	}
}