#include <ppu/pixel_transfer.h>

#include <ppu/lcd.h>
#include <ppu/vmem.h>

#include <functional>

namespace jmpr {

	PixelTransferHandler::PixelTransferHandler(LCD* lcd, VRAM* vram) {

		_lcd = lcd;
		_vram = vram;

		_pixel_fifo = std::queue<u16>();
		_visible_sprites = std::vector<Sprite>();

		_bgw_fetch = FetcherElem();
		_spr_fetch = std::vector<SpriteFetch>();

		prepareForTransfer();
	}

	void PixelTransferHandler::reboot() {

		_phase = FifoPhase::GET_TILE;
		_lx = 0;
		_pushed_x = 0;
		_fifo_x = 0;
		_fetcher_x = 0;
		_map_y = 0;
		_map_x = 0;
		_tile_y = 0;

		_pixel_fifo = {};

		_spr_fetch.clear();
		_visible_sprites.clear();

		_bgw_fetch.id = 0x0000;
		_bgw_fetch.lo = 0x0000;
		_bgw_fetch.hi = 0x0000;
	}

	bool PixelTransferHandler::transferComplete() {
		return (_pushed_x >= X_RESOLUTION);
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

		//_pixel_fifo = {};
		//_visible_sprites.clear();

		_bgw_fetch.id = 0x0000;
		_bgw_fetch.lo = 0x0000;
		_bgw_fetch.hi = 0x0000;
	}

	void PixelTransferHandler::resetFifo() {

		_pixel_fifo = {};
		_visible_sprites.clear();
	}

	void PixelTransferHandler::resetWindow() {

		_window_y = 0;
	}

	// Main transfer function

	void PixelTransferHandler::pixelTransferProcedure(std::shared_ptr<u16> vbuffer, u16 lineDots) {

		_map_y = _lcd->getScanline() + _lcd->getBGScrollY();
		_map_x = _fetcher_x + _lcd->getBGScrollX();
		// (Height of the scanline) mod 8 * 2 Bytes/Line.
		_tile_y = ((_lcd->getScanline() + _lcd->getBGScrollY()) % 8) * 2;

		// One fetch for every two pushes
		if ((lineDots & 1) == 0) {

			fetcherProcedure();
		}

		pushToVBufferProcedure(vbuffer);
	}

	// Fetching

	void PixelTransferHandler::fetcherProcedure() {

		bool doSpriteOperations = _lcd->objEnabled() && _visible_sprites.size() > 0;
		// todo sprite ops if enabled
		switch (_phase) {
		case FifoPhase::GET_TILE:

			tileIdFetch();
			windowTileIdFetch();

			// fetch sprite id
			if (doSpriteOperations)
				spriteIdFetch();

			_phase = FifoPhase::GET_TILE_LO;

			break;

		case FifoPhase::GET_TILE_LO:

			tileHalfDataFetch(0);
			// fetch sprite 0
			if (doSpriteOperations)
				spriteHalfDataFetch(0);

			_phase = FifoPhase::GET_TILE_HI;

			break;

		case FifoPhase::GET_TILE_HI:

			tileHalfDataFetch(1);
			// fetch sprite 1
			if (doSpriteOperations)
				spriteHalfDataFetch(1);

			_fetcher_x += 8;
			_phase = FifoPhase::IDLE;
			break;

		case FifoPhase::IDLE:

			// skip to the next without doing anything
			_phase = FifoPhase::PUSH;
			break;

		case FifoPhase::PUSH:

			// Push to single fifo
			if (pushToFifoProcedure()) {

				_phase = FifoPhase::GET_TILE;
			}
			break;
		}
	}

	void PixelTransferHandler::tileIdFetch() {

		if (_lcd->bgWindowPriority()) {

			// index of the tile to read
			_bgw_fetch.id = _vram->ppuRead(_lcd->bgTileMapAreaBegin() +
				(_map_x / 8) + ((_map_y / 8) * 32));

			if (_lcd->tileDataAreaBegin() == 0x8800) {
				_bgw_fetch.id += 0x80;
			}
		}
	}

	void PixelTransferHandler::windowTileIdFetch() {

		if (insideWindow()) {

			_bgw_fetch.id = _vram->ppuRead(_lcd->windowTileMapAreaBegin() +
				// X on the window
				((_fetcher_x + 7 - _lcd->getWindowX()) / 8) +
				// Y on the window
				(_window_y / 8) * 32);

			if (_lcd->tileDataAreaBegin() == 0x8800) {
				_bgw_fetch.id += 0x80;
			}
		}
	}

	void PixelTransferHandler::tileHalfDataFetch(u8 id) {

		u8 tileLine = insideWindow() ? ((_window_y % 8) * 2) : _tile_y;

		if (id == 0) {
			_bgw_fetch.lo = _vram->ppuRead(_lcd->tileDataAreaBegin() + (_bgw_fetch.id * 16)
				+ tileLine);
		}
		else if (id == 1) {
			_bgw_fetch.hi = _vram->ppuRead(_lcd->tileDataAreaBegin() + (_bgw_fetch.id * 16)
				+ tileLine + 1);
		}
	}

	void PixelTransferHandler::spriteIdFetch() {

		for (u8 s = 0; s < _visible_sprites.size(); s++) {

			Sprite* spr = &_visible_sprites[s];

			u8 xSpr = (spr->_xpos) + (_lcd->getBGScrollX() % 8);

			// Is the sprite present in the next 8 pixels
			if (xSpr > _fetcher_x && xSpr < _fetcher_x + 16) {

				SpriteFetch fetch = SpriteFetch();
				fetch.id = spr->_tile_id;
				fetch.spr = spr;

				_spr_fetch.push_back(fetch);
			}
		}

		// Sort fetches by sprite x, not done in cgb mode
		if (_spr_fetch.size() > 1) {
			std::sort(_spr_fetch.begin(),
				_spr_fetch.end(),
				[](SpriteFetch a, SpriteFetch b) {return a.spr->_xpos < b.spr->_xpos; });
		}
	}

	void PixelTransferHandler::spriteHalfDataFetch(u8 id) {

		// look only at the sprites that were selected
		for (u8 s = 0; s < _spr_fetch.size(); s++) {

			SpriteFetch* fetch = &_spr_fetch[s];

			// get the y coord of the tile to read (see _tile_y for explanation)
			u8 yLine = ((_lcd->getScanline() + 16) - fetch->spr->_ypos) * 2;

			if (fetch->spr->isFlippedY()) {
				// Maximum y - current y
				yLine = ((_lcd->objSize() - 1) * 2) - yLine;
			}

			// get index of tile and modify it in case the tile is 16 pixels high
			u16 tileId = fetch->id;

			if (_lcd->objSize() == 16) {
				// Round the id
				tileId &= ~(1);
			}

			// use the same formula to get the pixel as the bg
			if (id == 0) {
				fetch->lo = _vram->ppuRead(0x8000 + (tileId * 16)
					+ yLine);
			}
			else if (id == 1) {
				fetch->hi = _vram->ppuRead(0x8000 + (tileId * 16)
					+ yLine + 1);
			}
		}
	}

	u16 PixelTransferHandler::spriteColorFetch(u8 colorId) {

		// find the sprite with the lowest xpos that touches the pixel.
		for (u8 i = 0; i < _spr_fetch.size(); i++) {

			SpriteFetch fetch = _spr_fetch[i];

			// X pos in Sprite space
			u8 xSpr = (fetch.spr->_xpos - 8) + (_lcd->getBGScrollX() % 8);

			// check if the sprite touches the pixel
			if (xSpr + 8 < _fifo_x) continue;

			u8 offset = _fifo_x - xSpr;

			if (offset > 7) continue;

			u8 b = 7 - offset;

			if (fetch.spr->isFlippedX()) {
				b = offset;
			}

			// calculate the color id
			u8 sprColorId = (bit(fetch.hi, b) << 1) | (bit(fetch.lo, b));

			// Transparent color or doesn't have priority
			if (sprColorId == 0 || (fetch.spr->noPriority() && colorId > 0)) {
				continue;
			}

			return _lcd->getOBJcolor(fetch.spr->pallet(), sprColorId);
		}

		return colorId;
	}

	// Pushing

	bool PixelTransferHandler::pushToFifoProcedure() {

		// fifo is full
		if (_pixel_fifo.size() > 8) return false;

		// adjust the x for scrolling 
		int x = _fetcher_x - (8 - getCurrentScroll());

		if (x < 0) return false;

		for (u8 i = 0; i < 8; i++) {

			u8 colorIndex = (bit(_bgw_fetch.hi, 7 - i) << 1) | (bit(_bgw_fetch.lo, 7 - i));

			u16 pixel = _lcd->getBGWindowColor(colorIndex);

			// Adapt the color for sprites, window, etc.

			if (!_lcd->lcdEnabled())
				pixel = _lcd->getBGWindowColor(0);

			if (_lcd->objEnabled() && _spr_fetch.size() > 0)
				pixel = spriteColorFetch(pixel);

			_pixel_fifo.push(pixel);
			_fifo_x++;
		}

		_spr_fetch.clear();

		return true;
	}

	void PixelTransferHandler::pushToVBufferProcedure(std::shared_ptr<u16> vbuffer) {

		// switch to a general pixel fifo

		if (_pixel_fifo.size() > 8) {

			// push a pixel to the raster
			u16 color = _pixel_fifo.front();
			_pixel_fifo.pop();

			// Only render if pixel is visible with the scroll
			if (_lx >= getCurrentScroll()) {

				vbuffer.get()[_lcd->getScanline() * X_RESOLUTION + _pushed_x] = color;

				_pushed_x++;
			}

			_lx++;
		}
	}

	// Window ops

	void PixelTransferHandler::jumpWindowScanline() {

		// Increment window line if currently inside a window
		if (isWindowVisible() &&
			_lcd->getScanline() > _lcd->getWindowY() &&
			_lcd->getScanline() < _lcd->getWindowY() + Y_RESOLUTION) {
			_window_y++;
		}
	}

	u8 PixelTransferHandler::getCurrentScroll() const {

		return insideWindow() ? 0 : (_lcd->getBGScrollX() % 8);
	}

	bool PixelTransferHandler::isWindowVisible() const {

		return _lcd->windowEnabled() && _lcd->bgWindowPriority() &&
			between(_lcd->getWindowX(), 0, 166) &&
			between(_lcd->getWindowY(), 0, 143);
	}

	bool PixelTransferHandler::insideWindow() const {
		return isWindowVisible() &&
			(_fetcher_x + 7 >= _lcd->getWindowX() &&
				_fetcher_x + 7 < _lcd->getWindowX() + Y_RESOLUTION + 14 &&
				_lcd->getScanline() >= _lcd->getWindowY() &&
				_lcd->getScanline() < _lcd->getWindowY() + X_RESOLUTION);
	}
}
