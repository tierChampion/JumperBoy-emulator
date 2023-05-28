#include <cartridge/mbc.h>

namespace jmpr {

	// MBC 1

	MBC1::MBC1(u8* romData, u32 romSize, u32 ramSize, bool hasBattery) : MBC(ramSize > 0, hasBattery) {

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

		u8 ramBankCount = ramSize / 0x2000;

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

			u8 bankId = _rom_banks.size() > 0b11111 ? (_ram_bank_num << 5) | _rom_bank_num : _rom_bank_num;

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

			if (_has_battery) _waiting_for_save = true;
		}
	}

	// MBC 3

	MBC3::MBC3(u8* romData, u32 romSize, u32 ramSize, bool hasBattery, bool hasTimer) : MBC(ramSize > 0, hasBattery) {

		_has_timer = hasTimer;

		_rom_bank_num = 1;
		_ram_timer_enabled = 0;
		_ram_bank_num = 0;
		_latch_clk_data = 0;

		_clk_registers[0] = 0;
		_clk_registers[1] = 0;
		_clk_registers[2] = 0;
		_clk_registers[3] = 0;
		_clk_registers[4] = 0;

		u8 romBankCount = romSize / 0x4000;

		for (u8 i = 0; i < romBankCount; i++) {
			_rom_banks.push_back(std::unique_ptr<u8>(&romData[0x4000 * i]));
		}

		// Only allocate RAM if needed
		if (!_has_ram) return;

		u8 ramBankCount = ramSize / 0x2000;

		for (u8 i = 0; i < ramBankCount; i++) {
			_ram_banks.push_back(std::unique_ptr<u8>(new u8[0x2000]));
		}
	}

	u8 MBC3::read(u16 address) const {

		if (between(address, 0x0000, 0x3FFF)) {

			return _rom_banks[0].get()[address];
		}
		else if (between(address, 0x4000, 0x7FFF)) {

			return _rom_banks[_rom_bank_num].get()[address - 0x4000];
		}

		else if (between(address, 0xA000, 0xBFFF) && _ram_timer_enabled == 0x0A && _has_ram) {

			// todo
			return _ram_banks[_ram_bank_num].get()[address - 0xA000];
		}

		return 0xFF;
	}

	void MBC3::write(u16 address, u8 data) {

		if (between(address, 0x0000, 0x1FFF)) {

			_ram_timer_enabled = (data & 0xF);
		}
		else if (between(address, 0x2000, 0x3FFF)) {

			_rom_bank_num = (data & 0b1111111);
		}
		else if (between(address, 0x4000, 0x5FFF)) {

			// todo
		}
		else if (between(address, 0x6000, 0x7FFF)) {

			_latch_clk_data = data;
		}
		else if (between(address, 0xA000, 0xBFFF) && _ram_timer_enabled == 0x0A) {

			// todo
			_ram_banks[_ram_bank_num].get()[address - 0xA000] = data;

			if (_has_battery && _has_ram) _waiting_for_save = true;
		}
	}
}