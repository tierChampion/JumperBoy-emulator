#include <cartridge/mbc.h>

#include <cmath>

namespace jmpr
{
	// MBC 1

	MBC1::MBC1(const std::vector<u8>& romData, u32 ramSize, bool hasBattery) : MBC(ramSize > 0, hasBattery)
	{
		_rom_bank_num = 1;
		_ram_enabled = 0;
		_ram_bank_num = 0;
		_banking_mode = 0;

		u8 romBankCount = romData.size() / 0x4000;
		u8 romMaskSize = static_cast<u8>(std::log2(romBankCount - 1)) + 1;
		_rom_bank_mask = (1 << romMaskSize) - 1;

		for (u8 i = 0; i < romBankCount; i++)
		{
			_rom_banks.push_back(std::vector<u8>(romData.begin() + i * 0x4000, romData.begin() + (i + 1) * 0x4000));
		}

		// Only allocate RAM if needed
		if (!_has_ram)
			return;

		u8 ramBankCount = ramSize / 0x2000;

		for (u8 i = 0; i < ramBankCount; i++)
		{
			_ram_banks.push_back(std::vector<u8>(0x2000));
		}
	}

	u8 MBC1::read(u16 address) const
	{
		if (between(address, 0x0000, 0x3FFF))
		{
			u8 bankId = _banking_mode ? (_ram_bank_num << 5) : 0;
			return _rom_banks[bankId & _rom_bank_mask][address];
		}

		else if (between(address, 0x4000, 0x7FFF))
		{
			u8 romMaskSize = static_cast<u8>(std::log2(_rom_banks.size() - 1)) + 1;
			u8 romMask = (1 << romMaskSize) - 1;
			u8 bankId = _rom_banks.size() > 0b11111 ? (_ram_bank_num << 5) | _rom_bank_num : _rom_bank_num;
			return _rom_banks[bankId & _rom_bank_mask][address - 0x4000];
		}

		else if (between(address, 0xA000, 0xBFFF) && _ram_enabled == 0x0A && _has_ram)
		{
			u8 bankId = _banking_mode ? _ram_bank_num : 0;
			return _ram_banks[bankId][address - 0xA000];
		}

		return 0xFF;
	}

	void MBC1::write(u16 address, u8 data)
	{

		if (between(address, 0x0000, 0x1FFF))
		{

			_ram_enabled = data & 0xF;
		}
		else if (between(address, 0x2000, 0x3FFF))
		{

			_rom_bank_num = (data & 0b11111);

			if (_rom_bank_num == 0)
				_rom_bank_num = 1;
		}
		else if (between(address, 0x4000, 0x5FFF))
		{

			_ram_bank_num = (data & 0b11);
		}
		else if (between(address, 0x6000, 0x7FFF))
		{

			_banking_mode = data & 1;
		}
		else if (between(address, 0xA000, 0xBFFF) && _ram_enabled && _has_ram)
		{

			_ram_banks[_ram_bank_num][address - 0xA000] = data;

			if (_has_battery)
				_waiting_for_save = true;
		}
	}

	// MBC 3

	MBC3::MBC3(const std::vector<u8>& romData, u32 ramSize, bool hasBattery, bool hasTimer) : MBC(ramSize > 0, hasBattery)
	{
		_has_timer = hasTimer;

		_rom_bank_num = 1;
		_ram_timer_enabled = 0;
		_ram_rtc_select = 0xFF;
		_latch_clk_data = 0;

		_mode = RamRtcMode::NONE;

		_clk_registers[0] = 0;
		_clk_registers[1] = 0;
		_clk_registers[2] = 0;
		_clk_registers[3] = 0;
		_clk_registers[4] = 0;

		u8 romBankCount = romData.size() / 0x4000;
		u8 romMaskSize = static_cast<u8>(std::log2(romBankCount - 1)) + 1;
		_rom_bank_mask = (1 << romMaskSize) - 1;

		for (u8 i = 0; i < romBankCount; i++)
		{
			_rom_banks.push_back(std::vector<u8>(romData.begin() + i * 0x4000, romData.begin() + (i + 1) * 0x4000));
		}

		// Only allocate RAM if needed
		if (!_has_ram)
			return;

		u8 ramBankCount = ramSize / 0x2000;

		for (u8 i = 0; i < ramBankCount; i++)
		{
			_ram_banks.push_back(std::vector<u8>(0x2000));
		}
	}

	u8 MBC3::read(u16 address) const
	{

		if (between(address, 0x0000, 0x3FFF))
		{
			return _rom_banks[0][address];
		}
		else if (between(address, 0x4000, 0x7FFF))
		{
			return _rom_banks[_rom_bank_num & _rom_bank_mask][address - 0x4000];
		}

		else if (between(address, 0xA000, 0xBFFF) && _ram_timer_enabled == 0x0A)
		{
			if (_mode == RamRtcMode::RAM && _has_ram)
			{
				return _ram_banks[_ram_rtc_select][address - 0xA000];
			}
			else if (_mode == RamRtcMode::RTC && _has_timer)
			{
				return _clk_registers[_ram_rtc_select - 0x8];
			}
		}

		return 0xFF;
	}

	void MBC3::write(u16 address, u8 data)
	{
		if (between(address, 0x0000, 0x1FFF))
		{
			_ram_timer_enabled = (data & 0xF);
		}
		else if (between(address, 0x2000, 0x3FFF))
		{
			_rom_bank_num = (data & 0x7F);
			if (_rom_bank_num == 0)
			{
				_rom_bank_num = 1;
			}
		}
		else if (between(address, 0x4000, 0x5FFF))
		{
			_ram_rtc_select = data;
			if (data < 4 && _has_ram)
			{
				_mode = RamRtcMode::RAM;
			}
			else if (between(data, 0x8, 0xC) && _has_timer)
			{
				_mode = RamRtcMode::RTC;
			}
			else
			{
				_mode = RamRtcMode::NONE;
			}
		}
		else if (between(address, 0x6000, 0x7FFF))
		{
			_latch_clk_data = data;
		}
		else if (between(address, 0xA000, 0xBFFF) && _ram_timer_enabled == 0x0A)
		{
			if (_mode == RamRtcMode::RAM && _has_ram)
			{
				_ram_banks[_ram_rtc_select][address - 0xA000] = data;

				if (_has_battery && _has_ram)
					_waiting_for_save = true;
			}
			else if (_mode == RamRtcMode::RTC && _has_timer)
			{
				_clk_registers[_ram_rtc_select - 8] = data;
			}
		}
	}
}
