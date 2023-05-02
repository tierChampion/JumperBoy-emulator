#include <bus.h>

namespace jmpr {
	/*
	0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
	4000	7FFF	16 KiB ROM Bank 01~NN	From cartridge, switchable bank via mapper (if any)
	8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
	A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
	C000	CFFF	4 KiB Work RAM (WRAM)
	D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1~7
	E000	FDFF	Mirror of C000~DDFF (ECHO RAM)	Nintendo says use of this area is prohibited.
	FE00	FE9F	Sprite attribute table (OAM)
	FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited
	FF00	FF7F	I/O Registers
	FF80	FFFE	High RAM (HRAM)
	FFFF	FFFF	Interrupt Enable register (IE)
	*/

	/**
	* Create a Bus that is connected to no components.
	*/
	Bus::Bus() {
		_cpu = nullptr;
		_cart = nullptr;
		_ram = nullptr;
	}

	/**
	* Read a byte of data at the desired address. Gets mapped to the desired address.
	* @param address Address to read.
	*/
	u8 Bus::read(u16 address) {

		if (address < 0x8000) {
			return _cart->read(address);
		}
		else if (address < 0xA000) {
			// Video Ram
			noImpl();
		}
		else if (address < 0xC000) {
			// Cartridge Ram
			return _cart->read(address);
		}
		else if (address < 0xE000) {
			// Work Ram
			return _ram->readWorkRam(address);
		}
		else if (address < 0xFE00) {
			// Echo Ram, unused
			return 0;
		}
		else if (address < 0xFEA0) {
			// Sprite tables
			noImpl();
		}
		else if (address < 0xFF00) {
			// Prohibited area
			return 0;
		}
		else if (address < 0xFF80) {
			// IO registers
			noImpl();
		}
		else if (address < 0xFFFF) {
			// High Ram
			return _ram->readHighRam(address);
		}
		else if (address == 0xFFFF) {
			// Interrupt enabled register
			noImpl();
		}

		// should never happen
		return 0;
	}

	/**
	* Write data at the desired address. Gets mapped to the desired address.
	* @param address Address to write.
	*/
	void Bus::write(u16 address, u8 data) {

		if (address < 0x8000) {
			_cart->write(address, data);
		}
		else if (address < 0xA000) {
			// Video Ram
			noImpl();
		}
		else if (address < 0xC000) {
			// Cartridge Ram
			_cart->write(address, data);
		}
		else if (address < 0xE000) {
			// Work Ram
			_ram->writeWorkRam(address, data);
		}
		else if (address < 0xFE00) {
			// Echo Ram, unused
		}
		else if (address < 0xFEA0) {
			// Sprite tables
			noImpl();
		}
		else if (address < 0xFF00) {
			// Prohibited area
		}
		else if (address < 0xFF80) {
			// IO registers
			noImpl();
		}
		else if (address < 0xFFFF) {
			// High Ram
			_ram->writeHighRam(address, data);
		}
		else {
			// Interrupt enabled register
			noImpl();
		}
	}

	/**
	* Write 2 bytes of data at the desired address. Gets mapped to the desired address.
	* @param address Address to write.
	*/
	void Bus::write16(u16 address, u16 data) {

		write(address + 1, (data >> 8) & 0xFF);
		write(address, data & 0xFF);
	}
}
