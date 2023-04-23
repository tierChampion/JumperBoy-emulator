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
	}

	/**
	* Read data at the desired address. Gets mapped to the desired address.
	* @param address Address to read.
	*/
	u8 Bus::read(u16 address) {

		if (address < 0x8000) {
			if (_cart)
				return _cart->read(address);
			else
				std::cerr << "Warning: the Cartridge is not " <<
				"connected to the Bus, so it could not be read." << std::endl;
		}
		else {
			noImpl();
		}

		return 0;
	}

	/**
	* Write data at the desired address. Gets mapped to the desired address.
	* @param address Address to write.
	*/
	void Bus::write(u16 address, u8 data) {

		if (address < 0x8000) {
			if (_cart)
				_cart->write(address, data);
			else
				std::cerr << "Warning: the Cartridge is not " <<
				"connected to the Bus, so it could not be written to." << std::endl;
		}
		else {
			noImpl();
		}
	}
}
