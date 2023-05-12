#pragma once

#include <common.h>

namespace jmpr {

	enum class LCDMode {
		HBLANK,
		VBLANK,
		OAM_SCAN,
		DRAW
	};

	class LCD {

		u8 _lcdc;
		u8 _ly;
		u8 _lyc;
		u8 _status;

		u8 _scy, _scx;
		u8 _wy, _wx;

		u8 _bgp;
		u8 _obp[2];

		// gbc only registers... 
		u8 _bcps_bgpi;
		u8 _bcpd_bgpd;
		u8 _ocps_obpi;
		u8 _ocpd_obpd;

	public:

		LCD();

		u8 read(u8 address);
		void write(u8 address, u8 data);

		// lcd control
		bool lcdEnabled() const;
		u16 windowTileMapAreaBegin() const;
		bool windowEnabled() const;
		u16 tileDataAreaBegin() const;
		u16 bgTileMapAreaBegin() const;
		u8 objSize() const;
		bool objEnabled() const;
		u8 bgWindowPriority() const;

		// stat
		bool statInterruptTypeEnabled(LCDMode source) const;
		bool yCompare() const;
		LCDMode mode() const;

		// palettes
		u32 getBGWindowColor(u8 index) const;
		u32 getOBJcolor(u8 objIndex, u8 colIndex) const;
	};
}