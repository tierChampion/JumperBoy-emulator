#include <ppu/pixel_transfer.h>

#include <ppu/lcd.h>
#include <ppu/vmem.h>

namespace jmpr {

	PixelTransferHandler::PixelTransferHandler(LCD* lcd, VRAM* vram) {

		_lcd = lcd;
		_vram = vram;

		_bg_fifo = std::queue<FifoElem>();
		_spr_fifo = std::queue<FifoElem>();
		_visible_sprites = std::queue<Sprite>();

		prepareForTransfer();
	}

	// Resetting

	void PixelTransferHandler::prepareForTransfer() {

		_phase = FifoPhase::GET_TILE;
		_lx = 0;
		_pushed_x = 0;
		_fifo_x = 0;
		_fetcher_x = 0;
		_map_y = 0;
		_map_x = 0;
		_tile_y = 0;
		_bg_fetch[0] = 0;
		_bg_fetch[1] = 0;
		_bg_fetch[2] = 0;
	}

	void PixelTransferHandler::resetFifo() {
		while (_bg_fifo.size() > 0) {
			_bg_fifo.pop();
		}
	}

	// Fetching

	void PixelTransferHandler::tileIdFetch() {

		if (_lcd->bgWindowPriority()) {

			// index of the tile to read
			_bg_fetch[0] = _vram->ppuRead(_lcd->bgTileMapAreaBegin() +
				(_map_x / 8) + ((_map_y / 8) * 32));

			if (_lcd->tileDataAreaBegin() == 0x8800) {
				_bg_fetch[0] += 0x80;
			}
		}

		_phase = FifoPhase::GET_TILE_LO;
		_fetcher_x += 8;
	}

	void PixelTransferHandler::tileHalfDataFetch(u8 id) {

		_bg_fetch[id + 1] = _vram->ppuRead(_lcd->tileDataAreaBegin() + (_bg_fetch[0] * 16)
			+ _tile_y + id);

		_phase = FifoPhase((u8)_phase + 1);
	}
}
