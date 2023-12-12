#include <ui/game_window.h>

#include <ppu/ppu.h>
#include <apu/apu.h>

#include <gb.h>

namespace jmpr
{
    const u8 DEFAULT_SCALE = 4;

    const std::array<u32, 8> PALLET = {
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

    GameWindow::GameWindow(PPU *ppu) : _ppu(ppu), _opened(false),
                                                 _debug(false),
                                                 _pallet_id(0),
                                                 _scale(DEFAULT_SCALE)
    {
        // Initialize SDL Video
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        {
            std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
            exit(-10);
        }
    }

    void GameWindow::initWindow()
    {
        u16 width = _scale * X_RESOLUTION;

        if (_debug)
            width += (16 * 8) * _scale + (_scale * 15) + _scale * 4;

        u16 height = _scale * Y_RESOLUTION;

        if (_window == nullptr)
            _window = SDL_CreateWindow("JumperBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       width, height, 0);
        else
        {
            destroyGraphics();
            SDL_SetWindowSize(_window, width, height);
        }

        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ABGR8888,
                                     SDL_TEXTUREACCESS_STREAMING, width, height);
        _surface = SDL_CreateRGBSurfaceWithFormat(0,
                                                  width, height, 32, SDL_PIXELFORMAT_ABGR8888);
    }

    void GameWindow::open()
    {
        if (_opened)
            return;
        initWindow();
        SDL_ShowWindow(_window);
        _opened = true;
    }

    void GameWindow::close()
    {
        GameBoy::pause();
        _opened = false;
        SDL_HideWindow(_window);
    }

    void GameWindow::toggleDebug()
    {
        _debug = !_debug;

        if (_opened)
        {
            initWindow();
        }
    }

    void GameWindow::addPallet(const std::array<u32, 4> &pallet)
    {
        _pallets.push_back(pallet);
    }

    std::vector<std::array<u32, 4>> GameWindow::getPallets() const
    {
        return _pallets;
    }

    void GameWindow::setUsedPallet(u8 palletId)
    {
        _pallet_id = palletId;
    }

    void GameWindow::cleanup()
    {
        destroyGraphics();
        SDL_DestroyWindow(_window);
    }

    void GameWindow::destroyGraphics()
    {
        SDL_FreeSurface(_surface);
        SDL_DestroyTexture(_texture);
        SDL_DestroyRenderer(_renderer);
    }

    void GameWindow::render()
    {
        if (_opened)
        {
            renderGame();
            if (_debug)
                renderTiles();

            SDL_UpdateTexture(_texture, nullptr, _surface->pixels, _surface->pitch);
            SDL_RenderClear(_renderer);
            SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
            SDL_RenderPresent(_renderer);
        }
    }

    void GameWindow::renderGame()
    {
        if (!_ppu->pendingRender())
            return;

        _ppu->dispatchRender();

        SDL_Rect rect;
        rect.w = _scale;
        rect.h = _scale;
        rect.x = 0;
        rect.y = 0;

        for (u32 y = 0; y < Y_RESOLUTION; y++)
        {
            for (u32 x = 0; x < X_RESOLUTION; x++)
            {
                u8 color = _ppu->readBuffer(y * X_RESOLUTION + x);

                SDL_FillRect(_surface, &rect, _pallets[_pallet_id][color]);
                rect.x += _scale;
            }

            rect.x = 0;
            rect.y += _scale;
        }
    }

    void GameWindow::renderTiles()
    {
        u16 tileDim = (8 + 1) * _scale;

        SDL_Rect rect;
        rect.x = X_RESOLUTION * _scale;
        rect.y = 0;
        rect.w = tileDim / 2;
        rect.h = Y_RESOLUTION * _scale;

        SDL_FillRect(_surface, &rect, _pallets[_pallet_id][3]);

        // display the 384 tiles
        for (u16 y = 0; y < 24; y++)
        {
            for (u16 x = 0; x < 16; x++)
            {
                renderSingleTile((y * 16) + x, (x)*tileDim + tileDim / 2, y * tileDim);
            }
        }
    }

    void GameWindow::renderSingleTile(u16 tileId, u16 xPos, u16 yPos)
    {
        u8 data[16];

        // read tile data
        for (u8 i = 0; i < 16; i++)
        {
            data[i] = _ppu->getVRAM()->ppuRead(0x8000 + (16 * tileId) + i);
        }

        SDL_Rect rect;
        rect.w = _scale;
        rect.h = _scale;

        for (u8 y = 0; y < 8; y++)
        {

            u8 pal1 = data[2 * y];
            u8 pal2 = data[2 * y + 1];

            rect.y = yPos + _scale * y;

            for (u8 x = 0; x < 8; x++)
            {

                u8 color = (bit(pal2, 7 - x) << 1) | (bit(pal1, 7 - x));
                rect.x = xPos + _scale * x + X_RESOLUTION * _scale;

                SDL_FillRect(_surface, &rect, _pallets[_pallet_id][color]);
            }
        }
    }
}