#include <ui/user_interface.h>

#include <bus.h>

#include <array>

namespace jmpr {

#define SCALE 2

	bool UI::initVisuals() {

		// Creation of the window
		_window = SDL_CreateWindow("JumperBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			16 * 8 * SCALE, 32 * 8 * SCALE, 0);
		_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
		_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_STREAMING, 16 * 8 * SCALE, 32 * 8 * SCALE);
		_surface = SDL_CreateRGBSurfaceWithFormat(0, 16 * 8 * SCALE, 32 * 8 * SCALE, 32, SDL_PIXELFORMAT_ABGR8888);

		return _window != NULL;
	}

	void UI::render() {

		SDL_UpdateTexture(_texture, NULL, _surface->pixels, _surface->pitch);
		SDL_RenderClear(_renderer);
		SDL_RenderCopy(_renderer, _texture, NULL, NULL);
		SDL_RenderPresent(_renderer);
	}

	void UI::displayTileData(Bus* bus) {

		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = _surface->w;
		rect.h = _surface->h;
		SDL_FillRect(_surface, &rect, 0xFF002200);

		// display the 384 tiles
		for (u16 y = 0; y < 24; y++) {
			for (u16 x = 0; x < 16; x++) {

				displaySingleTile(bus, (y * 16) + x, 8 * x * SCALE, 8 * y * SCALE);
			}

		}

		render();
	}

	inline static const std::array<u32, 4> PALLET = {
		0xFF002200,
		0xFF224422,
		0xFF448844,
		0xFF88FF88,
	};

	void UI::displaySingleTile(Bus* bus, u8 tileId, u8 xPos, u8 yPos) {

		u8 data[16];

		// read tile data
		for (u8 i = 0; i < 16; i++) {
			data[i] = bus->read(0x8000 + (16 * tileId) + i);
		}

		SDL_Rect line;
		line.w = SCALE;
		line.h = SCALE;

		for (u8 y = 0; y < 8; y++) {

			u8 pal1 = data[2 * y];
			u8 pal2 = data[2 * y + 1];

			line.y = yPos + SCALE * y;

			for (u8 x = 0; x < 8; x++) {

				u8 color = (bit(pal2, 7 - x) << 1) | (bit(pal1, 7 - x));
				line.x = xPos + SCALE * x;

				SDL_FillRect(_surface, &line, PALLET[color]);
			}
		}
	}
}
