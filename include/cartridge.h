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

		void formatHeader(u8* header);

		u32 getRomSize() const;
		const char* getLicenseeName() const;
	};


	class Cartridge {

		const char* _filename;
		u32 _rom_size = 0;
		std::unique_ptr<u8> _rom_data;
		CartridgeHeader _header;
		// type

	public:

		Cartridge() { _filename = ""; }
		Cartridge(const char* file);

		bool isValid() const;

		u8 read(u16 address) const;
		void write(u16 address, u8 data);

		friend std::ostream& operator<<(std::ostream& os, const Cartridge& cartridge);
	};
}