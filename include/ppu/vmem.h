#pragma once

#include <common.h>

#include <array>

namespace jmpr
{

	struct Sprite
	{
		u8 _ypos;
		u8 _xpos;
		u8 _tile_id;
		u8 _attributes; // prio, yflip, xflip, palet, ..., ...

		void fromVRAM(u8 data);

		u8 operator[](int i) const;
		u8 &operator[](int i);

		bool noPriority() const;
		bool isFlippedY() const;
		bool isFlippedX() const;
		u8 pallet() const;
		u8 bank() const;
		u8 cgbPallet() const;
	};

	class LCD;

	class VRAM
	{
		std::array<std::array<u8, 0x2000>, 2> _vram;
		u8 _vram_bank;

	public:
		VRAM();

		bool isAccessible() const;

		u8 read(u16 address) const;
		void write(u16 address, u8 data);

		u8 getBank();
		void setBank(u8 bank);

		u8 ppuRead(u8 bank, u16 address) const;
	};

	class OAM
	{
		std::array<Sprite, 40> _oam;

	public:
		OAM();

		u8 read(u16 address, bool dmaRead) const;
		void write(u16 address, u8 data, bool dmaWrite);

		Sprite ppuRead(u16 spriteId) const;

	private:
		bool isAccessible() const;
	};

	class CRAM
	{
		std::array<u8, 0x40> _cram;
		u8 _bcps;
		bool _obj;

	public:
		CRAM() {}
		CRAM(bool isObjPallets);

		u8 read(u16 address) const;
		void write(u16 address, u8 data);
		u8 ppuRead(u8 pallet, u8 color, u8 byte) const;
	};
}