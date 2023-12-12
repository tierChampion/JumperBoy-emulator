#pragma once

#include <common.h>

#include <vector>

#include <SDL2/SDL.h>

namespace jmpr
{
    class PPU;
    class APU;

    class GameWindow
    {
        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        SDL_Surface *_surface;

        PPU *_ppu;

        bool _opened;
        bool _debug;

        std::vector<std::array<u32, 4>> _pallets;
        u8 _pallet_id;

        u8 _scale;

    public:
        GameWindow(PPU *ppu);
        void cleanup();

        void render();
        void toggleDebug();
        void open();
        void close();

        void addPallet(const std::array<u32, 4> &pallet);
        std::vector<std::array<u32, 4>> getPallets() const;

        void setUsedPallet(u8 palletId);

    private:
        void initWindow();
        void destroyGraphics();

        void renderGame();
        void renderTiles();
        void renderSingleTile(u16 tileId, u16 xPos, u16 yPos);
    };
}