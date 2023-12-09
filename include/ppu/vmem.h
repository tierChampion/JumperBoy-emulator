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

		bool noPriority() const;
		bool isFlippedY() const;
		bool isFlippedX() const;
		u8 pallet() const;
	};

	class LCD;

	// todo split into two VRAM and OAM
	class VRAM {

		u8 _vram[0x2000];

	public:

		VRAM();

		bool isAccessible() const;

		u8 read(u16 address) const;
		void write(u16 address, u8 data);

		u8 ppuRead(u16 address) const;
	};

	class OAM {

		Sprite _oam[40];

	public:

		OAM();

		u8 read(u16 address, bool dmaRead) const;
		void write(u16 address, u8 data, bool dmaWrite);

		Sprite ppuRead(u16 spriteId) const;

	private:

		bool isAccessible() const;
	};
}