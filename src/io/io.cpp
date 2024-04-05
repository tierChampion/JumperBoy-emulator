#include <io/io.h>

#include <io/joypad.h>
#include <io/timer.h>
#include <apu/apu.h>
#include <ppu/lcd.h>
#include <ppu/dma.h>

namespace jmpr {
	/*
	FF00			Joypad input
	FF01	FF02	Serial transfer
	FF04	FF07	Timer and divider
	FF10	FF26	Audio
	FF30	FF3F	Wave pattern
	FF40	FF4B	LCD control, status, position, scrolling and palettes
	FF4F			VRAM bank select
	FF50			Disable boot roms
	FF51	FF55	VRAM DMA
	FF68	FF69	BG / OBJ palettes
	FF70			WRAM Bank Select

	There is more stuff, like specific register for the CGB, etc...
	*/

	// Initial states: https://gbdev.io/pandocs/Power_Up_Sequence.html
	IO::IO(Joypad* pad, Timer* tim, APU* apu, LCD* lcd, ObjectDMA* odma, VideoDMA* vdma) :
		_serial_trans{ 0x00, 0x7E },
		_vram_dma{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
		_bg_obj_pallets{ 0xFF, 0xFF }
	{
		_joypad = pad;
		_timer = tim;
		_apu = apu;
		_lcd = lcd;
		_odma = odma;
		_vdma = vdma;

		_vram_select = 0xFF;
		_disable_bootrom = 0xFF;
		_wram_select = 0xFF;
	}

	u8 IO::read(u16 address) const {

		u8 range = loByte(address);
		u8 out = 0;

		if (range == 0x0) {
			out = _joypad->readP1Register();
		}
		else if (between(range, 0x1, 0x2)) {
			out = _serial_trans[range - 0x1];
		}
		else if (between(range, 0x4, 0x7)) {
			out = _timer->read(address);
		}
		else if (between(range, 0x10, 0x3F)) {
			out = _apu->read(address);
		}
		else if (range == 0x46) {
			out = _odma->readDMA();
		}
		else if (between(range, 0x40, 0x4B)) {
			out = _lcd->read(range);
		}
		else if (range == 0x4F) {
			out = _vram_select;
		}
		else if (range == 0x50) {
			out = _disable_bootrom;
		}
		else if (range == 0x55) {
			out = _vdma->readDMA();
		}
		else if (between(range, 0x68, 0x6B)) {
			out = _bg_obj_pallets[range - 0x68]; // cgb only
		}
		else if (range == 0x6C) {
			out = _lcd->getPriorityMode();
		}
		else if (range == 0x70) {
			out = _wram_select;
		}
		else {
			printf("Error: trying to read from an unknown IO register: %04X\n", address);
		}

		return out;
	}

	void IO::write(u16 address, u8 data) {

		u8 range = loByte(address);

		if (range == 0x0) {
			_joypad->writeP1Register(data);
		}
		else if (between(range, 0x1, 0x2)) {
			_serial_trans[range - 0x1] = data;
		}
		else if (between(range, 0x4, 0x7)) {
			_timer->write(range, data);
		}
		else if (between(range, 0x10, 0x3F)) {
			_apu->write(range, data);
		}
		else if (range == 0x46) {
			_odma->requestDMA(data);
		}
		else if (between(range, 0x40, 0x4B)) {
			_lcd->write(range, data);
		}
		else if (range == 0x4F) {
			_vram_select = data;
		}
		else if (range == 0x50) {
			_disable_bootrom = data;
		}
		else if (between(range, 0x51, 0x55)) {
			_vdma->write(range, data);
		}
		else if (between(range, 0x68, 0x6B)) {
			_bg_obj_pallets[range - 0x68] = data;
		}
		else if (range == 0x6C) {
			_lcd->write(range, data);
		}
		else if (range == 0x70) {
			_wram_select = data;
		}
		else {
			printf("Error: trying to write to an unknown IO register: %04X\n", address);
		}
	}
}
