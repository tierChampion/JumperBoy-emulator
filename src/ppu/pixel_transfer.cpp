#include <ppu/pixel_transfer.h>

#include <ppu/lcd.h>
#include <ppu/vmem.h>

#include <functional>

namespace jmpr {

	// todo: check sprites lateral positions. Accepted sprites are always OOB

	PixelTransferHandler::PixelTransferHandler(LCD* lcd, VRAM* vram) {

		_lcd = lcd;
		_vram = vram;

		_pixel_fifo = std::queue<u32>();
		_visible_sprites = std::vector<Sprite>();

		prepareForTransfer();
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
		_bg_fetch = FetcherElem();
		_spr_fetch = std::vector<SpriteFetch>();
	}

	void PixelTransferHandler::resetFifo() {

		while (_pixel_fifo.size() > 0) {
			_pixel_fifo.pop();
		}

		_visible_sprites.clear();
	}

	// Main transfer function

	void PixelTransferHandler::pixelTransferProcedure(std::shared_ptr<u32> vbuffer, u16 lineDots) {

		_map_y = _lcd->getScanline() + _lcd->getBGScrollY();
		_map_x = _fetcher_x + _lcd->getBGScrollX();
		// (Height of the scanline) mod 8 * 2 Bytes/Line.
		_tile_y = ((_lcd->getScanline() + _lcd->getBGScrollY()) & 0b111) * 2;

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
			// fetch sprite id
			if (doSpriteOperations)
				spriteIdFetch();

			break;

		case FifoPhase::GET_TILE_LO:

			tileHalfDataFetch(0);
			// fetch sprite 0
			if (doSpriteOperations)
				spriteHalfDataFetch(0);

			break;

		case FifoPhase::GET_TILE_HI:

			tileHalfDataFetch(1);
			// fetch sprite 1
			if (doSpriteOperations)
				spriteHalfDataFetch(1);

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
			_bg_fetch.id = _vram->ppuRead(_lcd->bgTileMapAreaBegin() +
				(_map_x / 8) + ((_map_y / 8) * 32));

			if (_lcd->tileDataAreaBegin() == 0x8800) {
				_bg_fetch.id += 0x80;
			}
		}

		_phase = FifoPhase::GET_TILE_LO;
		_fetcher_x += 8;
	}

	void PixelTransferHandler::tileHalfDataFetch(u8 id) {

		if (id == 0) {
			_bg_fetch.lo = _vram->ppuRead(_lcd->tileDataAreaBegin() + (_bg_fetch.id * 16)
				+ _tile_y);
		}
		else if (id == 1) {
			_bg_fetch.hi = _vram->ppuRead(_lcd->tileDataAreaBegin() + (_bg_fetch.id * 16)
				+ _tile_y + 1);
		}

		_phase = FifoPhase((u8)_phase + 1);
	}

	void PixelTransferHandler::spriteIdFetch() {

		for (u8 s = 0; s < _visible_sprites.size(); s++) {

			Sprite spr = _visible_sprites[s];

			// original: -8
			u8 xSpr = spr._xpos + (_lcd->getBGScrollX() % 8);

			// Is the sprite present in the next 8 pixels
			if (xSpr + 8 > _fetcher_x && xSpr < _fetcher_x + 8) {

				SpriteFetch fetch = SpriteFetch();
				fetch.id = spr._tile_id;
				fetch.spr = &_visible_sprites[s];

				_spr_fetch.push_back(fetch);
			}
		}

		// Sort fetches by sprite x
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

	// todo check if it works
	u32 PixelTransferHandler::spriteColorFetch(u8 pos, u32 color, u8 colorId) {

		// find the sprite with the lowest xpos that touches the pixel.
		u8 pixelPos = _fetcher_x + pos;

		for (u8 i = 0; i < _spr_fetch.size(); i++) {

			SpriteFetch fetch = _spr_fetch[i];

			// X pos in Sprite space
			u8 sprLinePos = pixelPos - (fetch.spr->_xpos);

			// check if the sprite touches the pixel
			if (sprLinePos >= 8) {
				continue;
			}

			if (!fetch.spr->isFlippedX()) {
				sprLinePos = 7 - sprLinePos;
			}

			// calculate the color id
			u8 sprColorId = (bit(fetch.hi, sprLinePos) << 1) | (bit(fetch.lo, sprLinePos));

			// Transparent color or doesn't have priority
			if (sprColorId == 0 || (fetch.spr->noPriority() && colorId > 0)) {
				continue;
			}

			return _lcd->getOBJcolor(fetch.spr->pallet(), sprColorId);
		}

		return color;
	}

	// Pushing

	bool PixelTransferHandler::pushToFifoProcedure() {

		// fifo is full
		if (_pixel_fifo.size() > 8) return false;

		// adjust the x for scrolling 
		int x = _fetcher_x - (8 - (_lcd->getBGScrollX() & 0b111));

		if (x < 0) return false;

		for (u8 i = 0; i < 8; i++) {

			u8 colorIndex = (bit(_bg_fetch.hi, 7 - i) << 1) | (bit(_bg_fetch.lo, 7 - i));

			u32 pixel = _lcd->getBGWindowColor(colorIndex);

			// Adapt the color for sprites, window, etc.

			if (!_lcd->lcdEnabled())
				pixel = 0;

			if (_lcd->objEnabled() && _spr_fetch.size() > 0)
				pixel = spriteColorFetch(i, pixel, colorIndex);

			_pixel_fifo.push(pixel);
			_fifo_x++;
		}

		_spr_fetch.clear();

		return true;
	}

	void PixelTransferHandler::pushToVBufferProcedure(std::shared_ptr<u32> vbuffer) {

		// switch to a general pixel fifo

		if (_pixel_fifo.size() > 8) {

			// push a pixel to the raster
			u32 color = _pixel_fifo.front();
			_pixel_fifo.pop();

			// Only render if pixel is visible with the scroll
			if (_lx >= (_lcd->getBGScrollX() & 0b111)) {

				vbuffer.get()[_lcd->getScanline() * X_RESOLUTION + _pushed_x] = color;

				_pushed_x++;
			}

			_lx++;
		}
	}
}
