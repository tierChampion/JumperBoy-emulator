#pragma once

#include <common.h>
#include <vector>

namespace jmpr
{

	class MBC
	{

	protected:
		std::vector<std::vector<u8>> _rom_banks;
		std::vector<std::vector<u8>> _ram_banks;

		bool _has_ram;
		bool _has_battery;

		bool _waiting_for_save;
		u16 _rom_bank_mask;

	public:
		MBC(bool hasRam, bool hasBattery);

		virtual u8 read(u16 address) const = 0;
		virtual void write(u16 address, u8 data) = 0;

		bool hasSavePending() const;
		void save(const std::string& path);
		void loadSave(const std::string& path);
	};

	class NoMBC : public MBC
	{

	public:
		NoMBC(const std::vector<u8> &romData, bool hasRam, bool hasBattery);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	class MBC1 : public MBC
	{

		u8 _ram_enabled;
		u8 _rom_bank_num;
		u8 _ram_bank_num;
		u8 _banking_mode;

	public:
		MBC1(const std::vector<u8> &romData, u32 ramSize, bool hasBattery);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	enum class RamRtcMode
	{
		RAM,
		RTC,
		NONE
	};

	class MBC3 : public MBC
	{

		u8 _ram_timer_enabled;
		u8 _rom_bank_num;
		u8 _ram_rtc_select;
		u8 _latch_clk_data;

		RamRtcMode _mode;

		u8 _clk_registers[5];

		bool _has_timer;

	public:
		MBC3(const std::vector<u8> &romData, u32 ramSize, bool hasBattery, bool hasTimer);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	class MBC5 : public MBC
	{
		u8 _ram_enabled;
		u16 _rom_bank_num;
		u8 _ram_bank_num;

	public:
		MBC5(const std::vector<u8> &romData, u32 ramSize, bool hasBattery);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	std::unique_ptr<MBC> giveAppropriateMBC(u8 cartridgeHardware, const std::vector<u8> &romData, u32 ramSize);
}