#pragma once

#include <common.h>

namespace jmpr {

	enum class LCDMode {
		HBLANK,
		VBLANK,
		OAM_SCAN,
		TRANSFER
	};

	class InterruptHandler;

	class LCD {

		InterruptHandler* _it_handler;

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

		u8 _priority_mode;

	public:

		LCD() {}
		LCD(InterruptHandler* intHandler);

		void reboot();
		void reset();

		u8 read(u8 address) const;
		void write(u8 address, u8 data);

		// positions
		void jumpScanline();
		u8 getScanline() const;
		u8 getBGScrollX() const;
		u8 getBGScrollY() const;
		u8 getWindowX() const;
		u8 getWindowY() const;

		// lcd control
		bool lcdEnabled() const;
		u16 windowTileMapAreaBegin() const;
		bool windowEnabled() const;
		u16 tileDataAreaBegin() const;
		u16 bgTileMapAreaBegin() const;
		u8 objSize() const;
		bool objEnabled() const;
		bool bgWindowPriority() const;
		u8 getPriorityMode() const;

		// stat
		bool statInterruptTypeEnabled(LCDMode source) const;
		void yCompare();
		LCDMode mode() const;
		void setMode(const LCDMode mode);

		// palettes
		u16 getBGWindowColor(u8 index) const;
		u16 getOBJcolor(u8 objIndex, u8 colIndex) const;
	};
}