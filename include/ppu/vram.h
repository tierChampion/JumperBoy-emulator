#pragma once

#include <common.h>

namespace jmpr {

	struct Sprite {
		u8 _ypos;
		u8 _xpos;
		u8 _tile_id;
		u8 _attributes;

		u8 operator[](int i) const;
		u8& operator[](int i);
	};

	class VRAM {

		u8 _vram[0x2000];
		Sprite _oam[40];

	public:

		VRAM();

		u8 readVRAM(u16 address) const;
		void writeVRAM(u16 address, u8 data);

		u8 readOAM(u16 address) const;
		void writeOAM(u16 address, u8 data);
	};
}