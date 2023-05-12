#include <ppu/ppu.h>

namespace jmpr {

	PPU::PPU() {

		_graphics_state = GraphicsState();
		_vram = VRAM();
	}
}