#include <ui/user_interface.h>

#include <ppu/ppu.h>
#include <ppu/vmem.h>

#include <array>

namespace jmpr
{

#define SCALE 4
// #define DEBUG_TILES

	bool UI::initVisuals()
	{
		u16 debugWidth = 0;
#ifdef DEBUG_TILES
		debugWidth = 16 * 8 * SCALE + (SCALE * 15) + SCALE * 4;
#endif

		u16 width = X_RESOLUTION * SCALE + debugWidth;
		u16 height = Y_RESOLUTION * SCALE;

		// Creation of the main window
		_vc._window = SDL_CreateWindow("JumperBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									   width, height, 0);
		_vc._renderer = SDL_CreateRenderer(_vc._window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		_vc._texture = SDL_CreateTexture(_vc._renderer, SDL_PIXELFORMAT_ABGR8888,
										 SDL_TEXTUREACCESS_STREAMING, width, height);
		_vc._surface = SDL_CreateRGBSurfaceWithFormat(0,
													  width, height, 32, SDL_PIXELFORMAT_ABGR8888);

		return _vc._window != NULL;
	}

	void UI::render()
	{
#ifdef DEBUG_TILES
		displayTileData();
#endif
		renderVideoBuffer();
	}

	void UI::renderInContext(VisualContext context)
	{
		SDL_UpdateTexture(context._texture, NULL, context._surface->pixels, context._surface->pitch);
		SDL_RenderClear(context._renderer);
		SDL_RenderCopy(context._renderer, context._texture, NULL, NULL);
	}

	inline static const std::array<u32, 8> PALLET = {
		///*
		// Green pallet
		0xFF0FBC9B,
		0xFF0FAC8B,
		0xFF306230,
		0xFF0F380F,
		//*/
		///*
		// Gray pallet
		0xFFFFFFFF,
		0xFFAAAAAA,
		0xFF555555,
		0xFF000000,
		//*/
	};

	void UI::displayTileData()
	{
		u16 tileDim = (8 + 1) * SCALE;

		SDL_Rect rect;
		rect.x = X_RESOLUTION * SCALE;
		rect.y = 0;
		rect.w = tileDim / 2;
		rect.h = Y_RESOLUTION * SCALE;

		SDL_FillRect(_vc._surface, &rect, PALLET[3]);

		// display the 384 tiles
		for (u16 y = 0; y < 24; y++)
		{
			for (u16 x = 0; x < 16; x++)
			{
				displaySingleTile((y * 16) + x, (x) * tileDim + tileDim / 2, y * tileDim);
			}
		}
	}

	void UI::displaySingleTile(u16 tileId, u16 xPos, u16 yPos)
	{

		u8 data[16];

		// read tile data
		for (u8 i = 0; i < 16; i++)
		{
			data[i] = _ppu->getVRAM()->ppuRead(0x8000 + (16 * tileId) + i);
		}

		SDL_Rect rect;
		rect.w = SCALE;
		rect.h = SCALE;

		for (u8 y = 0; y < 8; y++)
		{

			u8 pal1 = data[2 * y];
			u8 pal2 = data[2 * y + 1];

			rect.y = yPos + SCALE * y;

			for (u8 x = 0; x < 8; x++)
			{

				u8 color = (bit(pal2, 7 - x) << 1) | (bit(pal1, 7 - x));
				rect.x = xPos + SCALE * x + X_RESOLUTION * SCALE;

				SDL_FillRect(_vc._surface, &rect, PALLET[color]);
			}
		}
	}

	void UI::renderVideoBuffer()
	{

		if (!_ppu->pendingRender())
			return;

		_ppu->dispatchRender();

		SDL_Rect rect;
		rect.w = SCALE;
		rect.h = SCALE;
		rect.x = 0;
		rect.y = 0;

		for (u32 y = 0; y < Y_RESOLUTION; y++)
		{
			for (u32 x = 0; x < X_RESOLUTION; x++)
			{

				u32 color = _ppu->readBuffer(y * X_RESOLUTION + x);

				SDL_FillRect(_vc._surface, &rect, color);
				rect.x += SCALE;
			}

			rect.x = 0;
			rect.y += SCALE;
		}

		renderInContext(_vc);
	}

	void UI::renderDefault()
	{
		SDL_Rect rect;
		rect.w = X_RESOLUTION * SCALE;
		rect.h = Y_RESOLUTION * SCALE;
		rect.x = 0;
		rect.y = 0;

		SDL_FillRect(_vc._surface, &rect, PALLET[3]);

		renderInContext(_vc);
	}
}
