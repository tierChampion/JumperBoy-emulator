#pragma once

#include <common.h>
#include <ppu/lcd.h>
#include <ppu/vmem.h>
#include <ppu/pixel_transfer.h>

#include <queue>

namespace jmpr
{

	class InterruptHandler;
	class VideoDMA;

	class PPU
	{

		// Main components
		std::array<u16, PIXEL_COUNT> _vbuffer;
		std::unique_ptr<LCD> _lcd;
		std::unique_ptr<VRAM> _vram;
		std::unique_ptr<OAM> _oam;
		std::array<std::shared_ptr<CRAM>, 2> _cram;

		// Timing
		u16 _line_dots;
		u8 _curr_frame;
		u32 _last_frame_time;
		bool _frame_ready;

		PixelTransferHandler _pt_handler;

		// Interrupts
		InterruptHandler *_it_handler;

		VideoDMA *_vdma;

	public:
		PPU() {}
		PPU(InterruptHandler *intHandler);

		void connectVideoDMA(VideoDMA *vdma) { _vdma = vdma; }

		void reboot();

		LCD* getLCD() { return _lcd.get(); }
		VRAM *getVRAM() { return _vram.get(); }
		OAM *getOAM() { return _oam.get(); }
		CRAM *getCRAM(u8 id) { return _cram[id].get(); }

		u16 readBuffer(u32 index);

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