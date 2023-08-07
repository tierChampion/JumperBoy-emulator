#pragma once

#include <common.h>
#include <ppu/lcd.h>
#include <ppu/vmem.h>
#include <ppu/pixel_transfer.h>

#include <queue>

namespace jmpr {

	class PPU {

		// Main components
		std::shared_ptr<u32> _vbuffer;
		LCD _lcd;
		VRAM _vram;
		OAM _oam;

		// Timing
		u16 _line_dots;
		u8 _curr_frame;
		u32 _last_frame_time;
		bool _frame_ready;

		PixelTransferHandler _pt_handler;

		// Interrupts
		InterruptHandler* _it_handler;

	public:

		PPU() {}
		PPU(InterruptHandler* intHandler);

		void reboot();

		LCD* getLCD() { return &_lcd; }
		VRAM* getVRAM() { return &_vram; }
		OAM* getOAM() { return &_oam; }

		u32 readBuffer(u32 index);

		void update();
		bool pendingRender() const;
		void dispatchRender();

	private:

		void HBlankProcess();
		void VBlankProcess();
		void OAMScanProcess();
		void TransferProcess();

		void manageFrameTiming();
	};
}