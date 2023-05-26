#pragma once

#include <common.h>
#include <vector>

namespace jmpr {

	class MBC {

	protected:

		std::vector<std::unique_ptr<u8>> _rom_banks;
		std::vector<std::unique_ptr<u8>> _ram_banks;

		bool _has_ram;

	public:

		MBC(bool hasRam);

		virtual u8 read(u16 address) const;
		virtual void write(u16 address, u8 data);
	};

	class NoMBC : public MBC {

	public:

		NoMBC(u8* romData, bool hasRam);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	class MBC1 : public MBC {

		u8 _ram_enabled;
		u8 _rom_bank_num;
		u8 _ram_bank_num;
		u8 _banking_mode;

	public:

		MBC1(u8* romData, u32 romSize, bool hasRam);

		u8 read(u16 address) const override;
		void write(u16 address, u8 data) override;
	};

	MBC* giveAppropriateMBC(u8 cartridgeHardware, u32 romSize, u8* romData);
}