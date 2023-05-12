#include <bus.h>

#include <cpu/cpu.h>
#include <cartridge.h>
#include <ram.h>
#include <ppu/vram.h>
#include <io/io.h>

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
		_vram = nullptr;
		_io = nullptr;
	}

	/**
	* Read a byte of data at the desired address. Gets mapped to the desired address.
	* @param address Address to read.
	*/
	u8 Bus::read(u16 address) {

		if (between(address, 0x0000, 0x7FFF)) {
			return _cart->read(address);
		}
		else if (between(address, 0x8000, 0x9FFF)) {
			// Video Ram
			return _vram->readVRAM(address);
		}
		else if (between(address, 0xA000, 0xBFFF)) {
			// Cartridge Ram
			return _cart->read(address);
		}
		else if (between(address, 0xC000, 0xDFFF)) {
			// Work Ram
			return _ram->readWorkRam(address);
		}
		else if (between(address, 0xE000, 0xFDFF)) {
			// Echo Ram, unused
			return 0;
		}
		else if (between(address, 0xFE00, 0xFE9F)) {
			// Sprite tables
			return _vram->readOAM(address);
		}
		else if (between(address, 0xFEA0, 0xFEFF)) {
			// Prohibited area
			return 0;
		}
		else if (address == 0xFF0F) {
			// IF register
			return _cpu->readInterruptFlagRegister();
		}
		else if (between(address, 0xFF00, 0xFF7F)) {
			// IO registers
			return _io->read(address);
		}
		else if (between(address, 0xFF80, 0xFFFE)) {
			// High Ram
			return _ram->readHighRam(address);
		}
		else if (address == 0xFFFF) {
			// Interrupt enabled register
			return _cpu->readInterruptEnabledRegister();
		}

		// should never happen
		return 0;
	}

	/**
	* Write data at the desired address. Gets mapped to the desired address.
	* @param address Address to write.
	*/
	void Bus::write(u16 address, u8 data) {

		if (between(address, 0x0000, 0x7FFF)) {
			_cart->write(address, data);
		}
		else if (between(address, 0x8000, 0x9FFF)) {
			// Video Ram
			_vram->writeVRAM(address, data);
		}
		else if (between(address, 0xA000, 0xBFFF)) {
			// Cartridge Ram
			_cart->write(address, data);
		}
		else if (between(address, 0xC000, 0xDFFF)) {
			// Work Ram
			_ram->writeWorkRam(address, data);
		}
		else if (between(address, 0xE000, 0xFDFF)) {
			// Echo Ram, unused
		}
		else if (between(address, 0xFE00, 0xFE9F)) {
			// Sprite tables
			_vram->writeOAM(address, data);
		}
		else if (between(address, 0xFEA0, 0xFEFF)) {
			// Prohibited area
		}
		else if (address == 0xFF0F) {
			_cpu->writeInterruptFlagRegister(data);
		}
		else if (between(address, 0xFF00, 0xFF7F)) {
			// IO registers
			_io->write(address, data);
		}
		else if (between(address, 0xFF80, 0xFFFE)) {
			// High Ram
			_ram->writeHighRam(address, data);
		}
		else {
			// Interrupt enabled register
			_cpu->writeInterruptEnabledRegister(data);
		}
	}
}
