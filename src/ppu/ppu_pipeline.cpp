#include <ppu/ppu.h>

#include <gb.h>
#include <cpu/interrupt.h>

namespace jmpr {

	void PPU::HBlankProcess() {

		// Scanline is done
		if (_line_dots >= 456) {

			_lcd.jumpScanline();
			_pt_handler.jumpWindowScanline();

			// Next scanlines are no longer visible
			if (_lcd.getScanline() >= Y_RESOLUTION) {

				_lcd.setMode(LCDMode::VBLANK);

				_it_handler->requestInterrupt(InterruptType::VBLANK);

				if (_lcd.statInterruptTypeEnabled(LCDMode::VBLANK)) {
					_it_handler->requestInterrupt(InterruptType::STAT);
				}

				_curr_frame++;
				_frame_ready = true;
				GameBoy::requestSaveHandling();
				manageFrameTiming();
			}
			else {

				_lcd.setMode(LCDMode::OAM_SCAN);
			}

			_line_dots = 0xFFFF;
		}
	}

	void PPU::manageFrameTiming() {

		u32 frameEnd = GameBoy::getCurrentTime();
		u32 frameLength = frameEnd - _last_frame_time;

		if (frameLength < (1000.0f / FPS)) {
			GameBoy::delay((1000.0f / FPS) - frameLength);
		}

		_last_frame_time = GameBoy::getCurrentTime();
	}

	void PPU::VBlankProcess() {

		if (_line_dots >= 456) {

			_lcd.jumpScanline();
			_pt_handler.jumpWindowScanline();

			// Last VBlank scanline was reached
			if (_lcd.getScanline() >= 154) {

				_lcd.reset();
				_pt_handler.resetWindow();
			}

			_line_dots = 0xFFFF;
		}
	}

	void PPU::OAMScanProcess() {

		if (_line_dots >= 80) {
			_lcd.setMode(LCDMode::TRANSFER);

			_pt_handler.prepareForTransfer();
		}

		// Check if the sprite is visible
		else if ((_line_dots & 1) == 0) {

			// Maximum number of sprites not reached
			if (_pt_handler._visible_sprites.size() < 10) {

				// One scan takes two dots.
				u16 id = _line_dots / 2;

				Sprite spr = _oam.ppuRead(id);

				if (spr._xpos != 0 &&
					_lcd.getScanline() + 16 >= spr._ypos &&
					_lcd.getScanline() + 16 < spr._ypos + _lcd.objSize()) {

					std::cout << static_cast<int>(spr._tile_id) << std::endl;

					// Add sprite to the list of visible ones
					_pt_handler._visible_sprites.push_back(spr);
				}
			}
		}
	}

	void PPU::TransferProcess() {

		_pt_handler.pixelTransferProcedure(_vbuffer, _line_dots);

		// The entire visible line was transfered
		if (_pt_handler.transferComplete()) {

			// reset
			_pt_handler.resetFifo();
			_lcd.setMode(LCDMode::HBLANK);

			if (_lcd.statInterruptTypeEnabled(LCDMode::HBLANK)) {
				_it_handler->requestInterrupt(InterruptType::STAT);
			}
		}
	}
}