#include <ppu/vmem.h>

#include <ppu/lcd.h>

namespace jmpr
{

	// VRAM

	VRAM::VRAM() : _vram_bank(0),
				   _vram(std::array<std::array<u8, 0x2000>, 2>())
	{
	}

	bool VRAM::isAccessible() const
	{
		return true;
		/*
		return (_lcd->mode() == LCDMode::HBLANK ||
			_lcd->mode() == LCDMode::VBLANK ||
			_lcd->mode() == LCDMode::OAM_SCAN);
		//	*/
	}

	u8 VRAM::read(u16 address) const
	{
		if (!isAccessible())
			return 0xFF;

		return _vram[_vram_bank][address - 0x8000];
	}

	void VRAM::write(u16 address, u8 data)
	{
		if (!isAccessible())
			return;

		_vram[_vram_bank][address - 0x8000] = data;
	}

	u8 VRAM::ppuRead(u8 bank, u16 address) const
	{
		return _vram[bank][address - 0x8000];
	}

	u8 VRAM::getBank()
	{
		return 0xFE | _vram_bank;
	}

	void VRAM::setBank(u8 bank)
	{
		_vram_bank = bank & 1;
	}

	// Sprites

	void Sprite::fromVRAM(u8 data)
	{
		_attributes = data;
	}

	bool Sprite::noPriority() const
	{
		return bit(_attributes, 7);
	}

	bool Sprite::isFlippedY() const
	{
		return bit(_attributes, 6);
	}

	bool Sprite::isFlippedX() const
	{
		return bit(_attributes, 5);
	}

	u8 Sprite::pallet() const
	{
		return bit(_attributes, 4);
	}

	u8 Sprite::bank() const
	{
		return bit(_attributes, 3);
	}

	u8 Sprite::cgbPallet() const
	{
		return _attributes & 0b111;
	}

	u8 Sprite::operator[](int i) const
	{
		if (i == 0)
		{
			return _ypos;
		}
		else if (i == 1)
		{
			return _xpos;
		}
		else if (i == 2)
		{
			return _tile_id;
		}
		else
		{
			return _attributes;
		}
	}

	u8 &Sprite::operator[](int i)
	{
		if (i == 0)
		{
			return _ypos;
		}
		else if (i == 1)
		{
			return _xpos;
		}
		else if (i == 2)
		{
			return _tile_id;
		}
		else
		{
			return _attributes;
		}
	}

	// OAM

	OAM::OAM() : _oam(std::array<Sprite, 40>())
	{
	}

	bool OAM::isAccessible() const
	{
		return true;
		/*
		return (_lcd->mode() == LCDMode::HBLANK ||
			_lcd->mode() == LCDMode::VBLANK);
		//	*/
	}

	u8 OAM::read(u16 address, bool dmaRead) const
	{
		if (!dmaRead && !isAccessible())
			return 0xFF;

		u8 mappedAddr = (address - 0xFE00) / 4;

		return (_oam[mappedAddr])[address % 4];
	}

	void OAM::write(u16 address, u8 data, bool dmaWrite)
	{
		if (!dmaWrite && !isAccessible())
			return;

		u8 mappedAddr = (address - 0xFE00) / 4;

		(_oam[mappedAddr])[address % 4] = data;
	}

	Sprite OAM::ppuRead(u16 spriteId) const
	{
		return _oam[spriteId];
	}

	CRAM::CRAM(bool isObjPallets) : _cram{0}, _bcps(0), _obj(isObjPallets) {}

	u8 CRAM::read(u16 address) const
	{
		if (address == 0xFF68 || address == 0xFF6A)
		{
			return _bcps;
		}
		else if (address == 0xFF69 || address == 0xFF6B)
		{
			return _cram[_bcps & 0x3F];
		}
		else {
			return 0x00;
		}
	}

	void CRAM::write(u16 address, u8 data)
	{
		if (address == 0xFF68 || address == 0xFF6A)
		{
			_bcps = data;
		}
		else if (address == 0xFF69 || address == 0xFF6B)
		{
			_cram[_bcps & 0x3F] = data;
			if (bit(_bcps, 7) == 1)
			{
				_bcps = (_bcps & 0x80) | ((_bcps + 1) & 0x3F);
			}
		}
	}

	u8 CRAM::ppuRead(u8 pallet, u8 color, u8 byte) const {
		if (color == 0 && _obj) {
			return 0x00;
		}
		return _cram[byte + color * 2 + pallet * 8];
	}
}