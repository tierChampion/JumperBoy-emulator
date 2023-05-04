#include <io.h>

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

	There is much stuff, like specific register for the CGB and IF, etc...
	*/

	// Initial states: https://gbdev.io/pandocs/Power_Up_Sequence.html
	IO::IO() :
		_serial_trans{ 0x00, 0x7E },
		_tim_div{ 0x18, 0x00, 0x00, 0xF8 },
		_audio{ 0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F,
		0xFF, 0x9F, 0xFF, 0xBF, 0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1 },
		_wave_patts{ 0x00 },
		_lcd_stuff{ 0x91, 0x81, 0x00, 0x00, 0x91, 0x00, 0xFF, 0xFC,
		0x00, 0x00, // weird
		0x00, 0x00 },
		_vram_dma{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
		_bg_obj_pallets{ 0xFF, 0xFF }
	{

		_pad_input = 0xCF;
		_vram_select = 0xFF;
		_disable_bootrom = 0xFF;
		_wram_select = 0xFF;
	}

	u8 IO::read(u16 address) const {

		u8 range = loByte(address);
		u8 out = 0;

		if (range == 0x0) {
			out = _pad_input;
		}
		else if (range >= 0x1 && range <= 0x2) {
			out = _serial_trans[range - 0x1];
		}
		else if (range >= 0x4 && range <= 0x7) {
			out = _tim_div[range - 0x4];
		}
		else if (range >= 0x10 && range <= 0x26) {
			out = _audio[range - 0x10];
		}
		else if (range >= 0x30 && range <= 0x3F) {
			out = _wave_patts[range - 0x30];
		}
		else if (range >= 0x40 && range <= 0x4B) {
			out = _lcd_stuff[range - 0x40];
		}
		else if (range == 0x4F) {
			out = _vram_select;
		}
		else if (range == 0x50) {
			out = _disable_bootrom;
		}
		else if (range >= 0x51 && range <= 0x55) {
			out = _vram_dma[range - 0x51];
		}
		else if (range >= 0x68 && range <= 0x69) {
			out = _bg_obj_pallets[range - 0x68];

		}
		else if (range == 0x70) {
			out = _wram_select;
		}
		else {
			std::cerr << "Error: trying to write to an unknown IO register." << std::endl;
		}

		return out;
	}

	void IO::write(u16 address, u8 data) {

		u8 range = loByte(address);

		if (range == 0x0) {
			_pad_input = data;
		}
		else if (range >= 0x1 && range <= 0x2) {
			_serial_trans[range - 0x1] = data;
		}
		else if (range >= 0x4 && range <= 0x7) {
			_tim_div[range - 0x4] = data;
		}
		else if (range >= 0x10 && range <= 0x26) {
			_audio[range - 0x10] = data;
		}
		else if (range >= 0x30 && range <= 0x3F) {
			_wave_patts[range - 0x30] = data;
		}
		else if (range >= 0x40 && range <= 0x4B) {
			_lcd_stuff[range - 0x40] = data;
		}
		else if (range == 0x4F) {
			_vram_select = data;
		}
		else if (range == 0x50) {
			_disable_bootrom = data;
		}
		else if (range >= 0x51 && range <= 0x55) {
			_vram_dma[range - 0x51] = data;
		}
		else if (range >= 0x68 && range <= 0x69) {
			_bg_obj_pallets[range - 0x68] = data;

		}
		else if (range == 0x70) {
			_wram_select = data;
		}
		else {
			std::cerr << "Error: trying to write to an unknown IO register." << std::endl;
		}
	}
}