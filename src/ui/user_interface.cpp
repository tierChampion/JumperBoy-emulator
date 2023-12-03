#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{

	UI::UI(PPU *ppu, APU *apu, InputHandler *inpHandler)
	{

		// Initialize SDL Video
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		_opened = initVisuals();
		initAudio(apu);
		initImGui();

		_ppu = ppu;
		_inp_handler = inpHandler;
	}

	UI::~UI()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyTexture(_vc._texture);
		SDL_DestroyRenderer(_vc._renderer);
		SDL_DestroyWindow(_vc._window);

		SDL_DestroyTexture(_debug._texture);
		SDL_DestroyRenderer(_debug._renderer);
		SDL_DestroyWindow(_debug._window);

		SDL_PauseAudioDevice(_audio_id, 1);

		SDL_Quit();
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
		ImGui_ImplSDL2_InitForSDLRenderer(_vc._window, _vc._renderer);
		ImGui_ImplSDLRenderer2_Init(_vc._renderer);

		_file_browser = ImGui::FileBrowser();
		_file_browser.SetTitle("Search for ROM");
		_file_browser.SetTypeFilters({".gb", ".gbc"});

		_file_browser.Open();
	}

	void UI::loop(bool gamePlaying)
	{
		handleEvents(gamePlaying);

		renderImGui();

		if (gamePlaying)
		{
			render();
		}
		else 
			renderDefault();	

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(_vc._renderer);
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