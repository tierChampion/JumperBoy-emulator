#include <ppu/ppu.h>

#include <cpu/interrupt.h>

namespace jmpr {

	PPU::PPU(InterruptHandler* intHandler) {

		_vbuffer = std::unique_ptr<u32>(new u32[X_RESOLUTION * Y_RESOLUTION]);

		_lcd = LCD(intHandler);
		_vram = VRAM(&_lcd);
		_oam = OAM(&_lcd);

		_inter_handler = intHandler;

		_line_dots = 0;
		_curr_frame = 0;
		_last_frame_time = 0;
		_frame_ready = false;

		_pt_handler = PixelTransferHandler(&_lcd, &_vram);

		_lcd.setMode(LCDMode::OAM_SCAN);
	}

	void PPU::update() {

		_line_dots++;

		switch (_lcd.mode()) {
		case LCDMode::HBLANK: HBlankProcess(); break;
		case LCDMode::VBLANK: VBlankProcess(); break;
		case LCDMode::OAM_SCAN: OAMScanProcess(); break;
		case LCDMode::TRANSFER: DrawingProcess(); break;
		}
	}

	bool PPU::pendingRender() const {
		return _frame_ready;
	}

	void PPU::dispatchRender() {
		_frame_ready = false;
	}

	u32 PPU::readBuffer(u32 index) {

		return _vbuffer.get()[index];
	}
}