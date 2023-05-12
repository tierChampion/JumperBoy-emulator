#pragma once

#include <common.h>
#include <ppu/lcd.h>
#include <ppu/vram.h>

namespace jmpr {

	class PPU {

		LCD _lcd;
		VRAM _vram;

	public:

		PPU();

		LCD* getLCD() { return &_lcd; }
		VRAM* getVRAM() { return &_vram; }
	};
}