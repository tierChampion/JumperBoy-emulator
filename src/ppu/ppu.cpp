#include <ppu/ppu.h>

#include <cpu/interrupt.h>

namespace jmpr
{
	PPU::PPU(InterruptHandler *intHandler) : _vram(), _oam()
	{
		_vbuffer = std::shared_ptr<u16>(new u16[X_RESOLUTION * Y_RESOLUTION]);

		_lcd = LCD(intHandler);

		_pt_handler = PixelTransferHandler(&_lcd, &_vram);

		_it_handler = intHandler;

		_line_dots = 0;
		_curr_frame = 0;
		_last_frame_time = 0;
		_frame_ready = false;

		_lcd.setMode(LCDMode::OAM_SCAN);
	}

	void PPU::reboot()
	{
		_lcd.reboot();

		_line_dots = 0;
		_curr_frame = 0;
		_last_frame_time = 0;
		_frame_ready = false;

		_pt_handler.reboot();

		_lcd.setMode(LCDMode::OAM_SCAN);
	}

	void PPU::update()
	{

		switch (_lcd.mode())
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

		return _vbuffer.get()[index];
	}
}