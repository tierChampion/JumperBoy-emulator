#pragma once

#include <common.h>
#include <ppu/lcd.h>
#include <ppu/vmem.h>
#include <ppu/pixel_transfer.h>

#include <queue>

namespace jmpr {

#define X_RESOLUTION 160
#define Y_RESOLUTION 144
#define FPS 59.7f


	class PPU {

		// Main components
		std::unique_ptr<u32> _vbuffer;
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
		InterruptHandler* _inter_handler;

	public:

		PPU(InterruptHandler* intHandler);

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
		void DrawingProcess();

		void manageFrameTiming();

		void resetFifo();
		void renderingProcess();
		void fetchProcess();
		bool addProcess();
		void pushProcess();
	};
}