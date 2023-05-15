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

	// FIFO
	struct FifoElem {
		u32 _col;
		u8 _pallet;
		u8 _spr_prio;
		u8 _bg_prio;
	};

	struct Sprite;

	class PixelTransferHandler {

		LCD* _lcd;
		VRAM* _vram;

		FifoPhase _phase;
		std::queue<FifoElem> _bg_fifo;
		std::queue<FifoElem> _spr_fifo;
		std::queue<Sprite> _visible_sprites;
		u8 _lx; // x on the scanline
		u8 _pushed_x; // x of the last pixel pushed
		u8 _fetcher_x;
		u8 _fifo_x;
		u8 _map_y;
		u8 _map_x;
		u8 _tile_y;
		u8 _bg_fetch[3];

		friend class PPU;

	public:

		PixelTransferHandler() {}
		PixelTransferHandler(LCD* lcd, VRAM* vram);

		void prepareForTransfer();
		void resetFifo();

		void renderingProcess();
		void fetchProcess();
		bool addProcess();
		void pushProcess();

	private:

		void tileIdFetch();
		void tileHalfDataFetch(u8 id);
	};
}