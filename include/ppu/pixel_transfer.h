#pragma once

#include <common.h>
#include <ppu/vmem.h>

#include <queue>

namespace jmpr {

	enum class FifoPhase {
		GET_TILE,
		GET_TILE_LO,
		GET_TILE_HI,
		IDLE,
		PUSH
	};

	struct FetcherElem {
		u8 id;
		u8 attributes;
		u8 lo;
		u8 hi;
	};

	struct Sprite;

	struct SpriteFetch {
		u8 id;
		u8 lo;
		u8 hi;
		Sprite* spr;
	};

	class PixelTransferHandler {

		LCD* _lcd;
		VRAM* _vram;
		CRAM* _crams;

		FifoPhase _phase;
		std::queue<u16> _pixel_fifo;
		std::vector<Sprite> _visible_sprites;

		u8 _lx; // x on the scanline
		u8 _pushed_x; // x of the last pixel pushed
		u8 _fetcher_x; // x of the fetcher
		u8 _fifo_x; // x of the fifo

		u8 _map_y; // y of the current map
		u8 _map_x; // x of the current map

		u8 _window_y; // y of the window

		u8 _tile_y; // y in the current tile

		FetcherElem _bgw_fetch;
		std::vector<SpriteFetch> _spr_fetch;

		friend class PPU;

	public:

		PixelTransferHandler() {}
		PixelTransferHandler(LCD* lcd, VRAM* vram, CRAM* crams);

		void reboot();

		bool transferComplete();

		void prepareForTransfer();
		void resetFifo();
		void resetWindow();

		void pixelTransferProcedure(std::shared_ptr<u16> vbuffer, u16 lineDots);

		void jumpWindowScanline();

	private:

		// Fetching

		void fetcherProcedure();
		void tileIdFetch();
		void windowTileIdFetch();
		void tileHalfDataFetch(u8 id);
		void spriteIdFetch();
		void spriteHalfDataFetch(u8 id);
		u16 spriteColorFetch(u8 colorId);


		// Pushing

		bool pushToFifoProcedure();
		void pushToVBufferProcedure(std::shared_ptr<u16> vbuffer);


		// Window status
		u8 getCurrentScroll() const;
		bool isWindowVisible() const;
		bool insideWindow() const;
	};
}