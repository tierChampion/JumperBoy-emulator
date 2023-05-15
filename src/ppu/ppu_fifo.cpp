#include <ppu/ppu.h>

namespace jmpr {

	void PPU::fetchProcess() {

		// 2 dots for each step
		switch (_pt_handler._phase) {
		case FifoPhase::GET_TILE:

			_pt_handler.tileIdFetch();
			break;

		case FifoPhase::GET_TILE_LO:

			_pt_handler.tileHalfDataFetch(0);
			break;

		case FifoPhase::GET_TILE_HI:

			_pt_handler.tileHalfDataFetch(1);
			break;

		case FifoPhase::IDLE:

			// skip to the next without doing anything
			_pt_handler._phase = FifoPhase::PUSH;
			break;

		case FifoPhase::PUSH:

			if (addProcess()) {

				_pt_handler._phase = FifoPhase::GET_TILE;
			}
			break;
		}
	}

	bool PPU::addProcess() {

		// fifo is full
		if (_pt_handler._bg_fifo.size() > 8) return false;

		// adjust the x to the closest multiple of 8
		int x = _pt_handler._fetcher_x - (8 - (_lcd._scx & 0b111));

		for (u8 i = 0; i < 8; i++) {

			u8 colorIndex = (bit(_pt_handler._bg_fetch[2], 7 - i) << 1) | (bit(_pt_handler._bg_fetch[1], 7 - i));

			FifoElem pixel = FifoElem();
			pixel._col = _lcd.getBGWindowColor(colorIndex);
			// to see with the rest

			if (x >= 0) {
				_pt_handler._bg_fifo.push(pixel);
				_pt_handler._fifo_x++;
			}
		}

		return true;
	}

	void PPU::pushProcess() {

		if (_pt_handler._bg_fifo.size() > 8) {

			// push a pixel to the raster
			u32 color = _pt_handler._bg_fifo.front()._col;
			_pt_handler._bg_fifo.pop();

			// Only render if pixel is visible with the scroll
			if (_pt_handler._lx >= (_lcd._scx & 0b111)) {

				_vbuffer.get()[_lcd._ly * X_RESOLUTION + _pt_handler._pushed_x] = color;

				_pt_handler._pushed_x++;
			}

			_pt_handler._lx++;
		}
	}

	void PPU::renderingProcess() {

		_pt_handler._map_y = _lcd._ly + _lcd._scy;
		_pt_handler._map_x = _pt_handler._fetcher_x + _lcd._scx;
		_pt_handler._tile_y = ((_lcd._ly + _lcd._scy) & 0b111) * 2;

		// One fetch for every two pushes
		if ((_line_dots & 1) == 0) {

			fetchProcess();
		}

		pushProcess();
	}

	void PPU::resetFifo() {

		while (_pt_handler._bg_fifo.size() > 0) {
			_pt_handler._bg_fifo.pop();
		}
	}
}