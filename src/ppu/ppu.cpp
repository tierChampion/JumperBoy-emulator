#include <ppu/ppu.h>

#include <cpu/interrupt.h>
#include <ppu/dma.h>

namespace jmpr
{
	PPU::PPU(InterruptHandler *intHandler)
	{
		_vram = std::make_unique<VRAM>();
		_oam = std::make_unique<OAM>();
		_cram[0] = std::make_shared<CRAM>(false);
		_cram[1] = std::make_shared<CRAM>(true);

		_vbuffer = std::array<u16, PIXEL_COUNT>();

		_lcd = std::make_unique<LCD>(intHandler);

		_pt_handler = PixelTransferHandler(_lcd.get(), _vram.get(), _cram);

		_it_handler = intHandler;

		_line_dots = 0;
		_curr_frame = 0;
		_last_frame_time = 0;
		_frame_ready = false;

		_lcd->setMode(LCDMode::OAM_SCAN);
	}

	void PPU::reboot()
	{
		_lcd->reboot();

		_line_dots = 0;
		_curr_frame = 0;
		_last_frame_time = 0;
		_frame_ready = false;

		_pt_handler.reboot();

		_lcd->setMode(LCDMode::OAM_SCAN);
	}

	void PPU::update()
	{
		switch (_lcd->mode())
		{
		case LCDMode::HBLANK:
			HBlankProcess();
			break;
		case LCDMode::VBLANK:
			VBlankProcess();
			break;
		case LCDMode::OAM_SCAN:
			OAMScanProcess();
			break;
		case LCDMode::TRANSFER:
			TransferProcess();
			break;
		}

		_line_dots++;
	}

	bool PPU::pendingRender() const
	{
		return _frame_ready;
	}

	void PPU::dispatchRender()
	{
		_frame_ready = false;
	}

	u16 PPU::readBuffer(u32 index)
	{
		return _vbuffer[index];
	}
}