#include <ui/user_interface.h>

#include <gb.h>

namespace jmpr
{

	void UI::handleEvents(bool gamePlaying)
	{
		while (SDL_PollEvent(&_curr_event))
		{
			if (_curr_event.window.windowID == SDL_GetWindowID(_imgui_window))
				ImGui_ImplSDL2_ProcessEvent(&_curr_event);

			switch (_curr_event.type)
			{

			case SDL_QUIT:
				_opened = false;
				break;

			case SDL_WINDOWEVENT:
				if (_curr_event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					if (_curr_event.window.windowID == SDL_GetWindowID(_imgui_window))
						_opened = false;
					else
						_game_window.close();
				}
				break;

			case SDL_KEYDOWN:
			{
				_last_input = _curr_event.key;
				JumperInput input = _input_maps[_input_preset][SDL_GetKeyName(_curr_event.key.keysym.sym)];
				if (gamePlaying && isJoypadInput(input))
					GameBoy::getInstance()->pressButton(static_cast<u8>(input) - 1);

				break;
			}

			case SDL_KEYUP:
			{
				JumperInput input = _input_maps[_input_preset][SDL_GetKeyName(_curr_event.key.keysym.sym)];
				if (gamePlaying && isJoypadInput(input))
					GameBoy::getInstance()->releaseButton(static_cast<u8>(input) - 1);

				break;
			}
			}
		}
	}
}