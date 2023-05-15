#pragma once

#include <common.h>

namespace jmpr {

	struct Sprite {
		u8 _ypos;
		u8 _xpos;
		u8 _tile_id;
		u8 _attributes; // prio, yflip, xflip, palet, ..., ...

		u8 operator[](int i) const;
		u8& operator[](int i);
	};

	class LCD;

	// todo split into two VRAM and OAM
	class VRAM {

		LCD* _lcd;
		u8 _vram[0x2000];

	public:

		VRAM() {}
		VRAM(LCD* lcd);

		bool isAccessible() const;

		u8 read(u16 address) const;
		void write(u16 address, u8 data);

		u8 ppuRead(u16 address) const;
	};

	class OAM {

		LCD* _lcd;
		Sprite _oam[40];

	public:

		OAM() {}
		OAM(LCD* lcd);

		u8 read(u16 address, bool dmaRead) const;
		void write(u16 address, u8 data, bool dmaWrite);

		Sprite ppuRead(u16 spriteId) const;

	private:

		bool isAccessible() const;
	};
}