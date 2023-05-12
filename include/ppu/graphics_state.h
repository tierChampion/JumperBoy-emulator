#pragma once

#include <common.h>

namespace jmpr {

	class GraphicsState {

		u8 _lcdc;
		u8 _ly;
		u8 _lyc;
		u8 _status;

		u8 _scy, _scx;
		u8 _wy, _wx;

		u8 _bgp;
		u8 _obp0, _obp1;

		// gbc only registers... 
		u8 _bcps_bgpi;
		u8 _bcpd_bgpd;
		u8 _ocps_obpi;
		u8 _ocpd_obpd;

	public:

		GraphicsState();

		u8 read(u8 address);
		void write(u8 address, u8 data);
	};
}