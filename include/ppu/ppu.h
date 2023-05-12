#pragma once

#include <common.h>
#include <ppu/graphics_state.h>
#include <ppu/vram.h>

namespace jmpr {

	class PPU {

		GraphicsState _graphics_state;
		VRAM _vram;

	public:

		PPU();

		GraphicsState* getState() { return &_graphics_state; }
		VRAM* getVRAM() { return &_vram; }
	};
}