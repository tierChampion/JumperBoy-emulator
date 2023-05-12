#include <ppu/ppu.h>

namespace jmpr {

	PPU::PPU() {

		_lcd = LCD();
		_vram = VRAM();
	}
}