#pragma once

#include <common.h>
#include <vector>

namespace jmpr {

	class MBC {

	protected:

		std::vector<std::unique_ptr<u8>> _rom_banks;
		std::vector<std::unique_ptr<u8>> _ram_banks;

		bool _has_ram;
		bool _has_battery;

		bool _waiting_for_save;

	public:

		MBC(bool hasRam, bool hasBattery);

		virtual u8 read(u16 address) const;
		virtual void write(u16 address, u8 data);

		bool hasSavePending() const;
		void save(const char* path);
		void loadSave(const char* path);
	};

	class NoMBC : public MBC {

	public:

		NoMBC(u8* romData, bool hasRam, bool hasBattery);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	class MBC1 : public MBC {

		u8 _ram_enabled;
		u8 _rom_bank_num;
		u8 _ram_bank_num;
		u8 _banking_mode;

	public:

		MBC1(u8* romData, u32 romSize, u32 ramSize, bool hasBattery);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	class MBC3 : public MBC {

		u8 _ram_timer_enabled;
		u8 _rom_bank_num;
		u8 _ram_bank_num;
		u8 _latch_clk_data;

		u8 _clk_registers[5];

		bool _has_timer;

	public:

		MBC3(u8* romData, u32 romSize, u32 ramSize, bool hasBattery, bool hasTimer);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	MBC* giveAppropriateMBC(u8 cartridgeHardware, u32 romSize, u8* romData, u32 ramSize);
}