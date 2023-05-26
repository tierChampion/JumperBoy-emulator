#include <cartridge/mbc.h>

namespace jmpr {

	MBC* giveAppropriateMBC(u8 cartridgeHardware, u32 romSize, u8* romData) {

		// No MBC
		if (cartridgeHardware == 0x00 || cartridgeHardware == 0x08 || cartridgeHardware == 0x09) {

			return new NoMBC(romData, cartridgeHardware != 0x00);
		}
		// MBC1
		else if (between(cartridgeHardware, 0x01, 0x03)) {

			return new MBC1(romData, romSize, cartridgeHardware != 0x01);
		}
	}

	// MBC template

	MBC::MBC(bool hasRam) {

		_rom_banks = std::vector<std::unique_ptr<u8>>();
		_ram_banks = std::vector<std::unique_ptr<u8>>();

		_has_ram = hasRam;
	}

	u8 MBC::read(u16 address) const {

		return 0xFF;
	}

	void MBC::write(u16 address, u8 data) {}

	// No MBC

	NoMBC::NoMBC(u8* romData, bool hasRam) : MBC(hasRam) {

		// Only a single ROM bank.
		_rom_banks.push_back(std::unique_ptr<u8>(romData));

		// Add a possible 8 KiB of ram
		if (_has_ram) {
			_ram_banks.push_back(std::unique_ptr<u8>(new u8[0x2000]));
		}
	}

	u8 NoMBC::read(u16 address) const {

		if (between(address, 0x0000, 0x7FFF)) {
			return _rom_banks[0].get()[address];
		}
		else if (between(address, 0xA000, 0xBFFF) && _has_ram) {
			return _ram_banks[0].get()[address - 0xA000];
		}
	}

	void NoMBC::write(u16 address, u8 data) {

		// Only RAM is writeable
		if (between(address, 0xA000, 0xBFFF) && _has_ram) {
			_ram_banks[0].get()[address - 0xA000] = data;
		}
	}

	// MBC 1

	MBC1::MBC1(u8* romData, u32 romSize, bool hasRam) : MBC(hasRam) {

		_rom_bank_num = 1;
		_ram_enabled = 0;
		_ram_bank_num = 0;
		_banking_mode = 0;

		u8 romBankCount = romSize / 0x4000;

		for (u8 i = 0; i < romBankCount; i++) {
			_rom_banks.push_back(std::unique_ptr<u8>(&romData[0x4000 * i]));
		}

		// Only allocate RAM if needed
		if (!_has_ram) return;

		u8 ramBankCount = 4;

		// Alternate wiring used for more ROM at the cost of RAM.
		if (romSize > 0x2000)
			ramBankCount = 1;

		for (u8 i = 0; i < ramBankCount; i++) {
			_ram_banks.push_back(std::unique_ptr<u8>(new u8[0x2000]));
		}
	}

	u8 MBC1::read(u16 address) const {

		if (between(address, 0x0000, 0x3FFF)) {

			u8 bankId = _banking_mode ? _ram_bank_num : 0;

			return _rom_banks[bankId].get()[address];
		}

		else if (between(address, 0x4000, 0x7FFF)) {

			return _rom_banks[_rom_bank_num].get()[address - 0x4000];
		}

		else if (between(address, 0xA000, 0xBFFF) && _ram_enabled == 0x0A && _has_ram) {

			u8 bankId = _banking_mode ? _ram_bank_num : 0;

			return _ram_banks[bankId].get()[address - 0xA000];
		}

		return 0xFF;
	}

	void MBC1::write(u16 address, u8 data) {

		if (between(address, 0x0000, 0x1FFF)) {

			_ram_enabled = data & 0xF;
		}
		else if (between(address, 0x2000, 0x3FFF)) {

			_rom_bank_num = (data & 0b11111);

			if (_rom_bank_num == 0)
				_rom_bank_num = 1;
		}
		else if (between(address, 0x4000, 0x5FFF)) {

			_ram_bank_num = (data & 0b11);
		}
		else if (between(address, 0x6000, 0x7FFF)) {

			_banking_mode = data & 1;
		}
		else if (between(address, 0xA000, 0xBFFF) && _ram_enabled && _has_ram) {

			_ram_banks[_ram_bank_num].get()[address - 0xA000] = data;
		}
	}
}