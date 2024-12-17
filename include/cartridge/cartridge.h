#pragma once

#include <common.h>

namespace jmpr {

	struct CartridgeHeader {

		u8 _entry[4];
		u8 _nintendo_logo[0x30];
		char _title[16];
		char _manufacturer_code[4];
		u8 _cgb_flag;
		u16 _new_licensee_code;
		u8 _sgb_flag;
		u8 _cartridge_type;
		u8 _rom_size;
		u8 _ram_size;
		u8 _destination_code;
		u8 _old_licensee_code;
		u8 _version;
		u8 _header_checksum;
		u16 _global_checksum;
		bool _valid_sum;

		void formatHeader(const std::vector<u8>::iterator& header);

		u32 getRomSize() const;
		const char* getLicenseeName() const;
	};

	class MBC;

	class Cartridge {

		bool _error;
		std::string _filename;
		std::string _savename;
		// u32 _rom_size = 0; // TODO
		std::vector<u8> _rom_data;
		CartridgeHeader _header;
		MBC* _mbc;

	public:

		Cartridge() { _error = false; _filename = ""; }
		Cartridge(const std::string& file);

		bool isValid() const;
		bool isColor() const;

		u8 read(u16 address) const;
		void write(u16 address, u8 data);

		void handleSaves() const;

		friend std::ostream& operator<<(std::ostream& os, const Cartridge& cartridge);
	};
}