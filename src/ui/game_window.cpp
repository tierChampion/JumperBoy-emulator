#include <ui/game_window.h>

#include <ppu/ppu.h>
#include <apu/apu.h>

#include <gb.h>

namespace jmpr
{
    const u8 DEFAULT_SCALE = 4;

    GameWindow::GameWindow(PPU* ppu) : _ppu(ppu), _opened(false),
                                       _debug(false),
                                       _bank(0),
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

        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB555,
                                     SDL_TEXTUREACCESS_STREAMING, width, height);
        _surface = SDL_CreateRGBSurfaceWithFormat(0,
                                                  width, height, 32, SDL_PIXELFORMAT_RGB555);
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
        GameBoy::getInstance()->pause();
        _opened = false;
        SDL_HideWindow(_window);
    }

    void GameWindow::toggleDebug(u8 bank)
    {
        _debug = !_debug;
        _bank = bank;

        if (_opened)
        {
            initWindow();
        }
    }

    void GameWindow::addPallet(const std::array<u16, 4> &pallet)
    {
        // todo, tell the ui!
        if (_pallets.size() < 256)
        {
            _pallets.push_back(pallet);
        }
        else
        {
            _pallets[255] = pallet;
        }
    }

    std::vector<std::array<u16, 4>> GameWindow::getPallets() const
    {
        return _pallets;
    }

    u8 GameWindow::getPalletsSize() const
    {
        return _pallets.size();
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
                u16 color = _ppu->readBuffer(y * X_RESOLUTION + x);

                if (!GameBoy::getInstance()->isCGB())
                {
                    SDL_FillRect(_surface, &rect, _pallets[_pallet_id][color]);
                }
                else
                {
                    u16 red = color & 0b11111;
                    u16 green = (color & 0b1111100000) >> 5;
                    u16 blue = (color & 0b111110000000000) >> 10;
                    SDL_FillRect(_surface, &rect, blue | (green << 5) | (red << 10));
                }
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
            data[i] = _ppu->getVRAM()->ppuRead(_bank, 0x8000 + (16 * tileId) + i);
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

    void GameWindow::renderPallets()
    {
        u16 tileDim = (8 + 1) * _scale;

        SDL_Rect rect;
        rect.x = X_RESOLUTION * _scale;
        rect.y = 0;
        rect.w = 8 * _scale;
        rect.h = 8 * _scale;

        for (u16 z = 0; z < 2; z++)
        {
            for (u16 y = 0; y < 8; y++)
            {
                rect.y = y * tileDim + z * 8 * tileDim;
                for (u16 x = 0; x < 4; x++)
                {
                    rect.x = x * tileDim + X_RESOLUTION * _scale;
                    u16 color = _ppu->getCRAM(z)->ppuRead(y, x, 0) | (_ppu->getCRAM(z)->ppuRead(y, x, 1) << 8);

                    u16 red = color & 0b11111;
                    u16 green = (color & 0b1111100000) >> 5;
                    u16 blue = (color & 0b111110000000000) >> 10;
                    SDL_FillRect(_surface, &rect, blue | (green << 5) | (red << 10));
                }
            }
        }
    }
}