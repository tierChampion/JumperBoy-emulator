#pragma once

#include <common.h>

#include <vector>

#include <SDL2/SDL.h>

namespace jmpr
{
    class PPU;
    class APU;
    class UserSettings;

    class GameWindow
    {
        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        SDL_Surface *_surface;

        PPU* _ppu;

        bool _opened;
        bool _debug;
        u8 _bank;

        UserSettings* _settings;

    public:
        GameWindow() {}
        GameWindow(PPU* ppu, UserSettings* settings);
        void cleanup();

        void render();
        void toggleDebug(u8 bank);
        void open();
        void close();

    private:
        void initWindow();
        void destroyGraphics();

        void renderGame();
        void renderTiles();
        void renderSingleTile(u16 tileId, u16 xPos, u16 yPos);
        void renderPallets();
    };
}