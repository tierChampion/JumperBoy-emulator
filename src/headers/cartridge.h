#pragma once

#include <headers/common.h>

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

		void formatHeader(u8* header);
	};

	class Cartridge {

		const char* _filename;
		u32 _rom_size;
		u8* _rom_data;
		CartridgeHeader _header;

	public:

		Cartridge(const char* file);

		friend std::ostream& operator<<(std::ostream& os, const Cartridge& cartridge);
	};

}