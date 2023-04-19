#include <headers/cartridge.h>
#include <unordered_map>

namespace jmpr {

	Cartridge::Cartridge(const char* file) {

	}

	void CartridgeHeader::formatHeader(u8 header[0x50]) {

		for (int i = 0; i < 4; i++)
			_entry[i] = header[i];

		for (int i = 0; i < 0x30; i++)
			_nintendo_logo[i] = header[i + 0x04];

		for (int i = 0; i < 16; i++)
			_title[i] = (char)header[i + 0x34];

		for (int i = 0; i < 4; i++)
			_manufacturer_code[i] = (char)header[i + 0x3F];

		_cgb_flag = header[0x43];

		_new_licensee_code = header[0x44] << 8 | header[0x45];

		_sgb_flag = header[0x46];

		_cartridge_type = header[0x47];

		_rom_size = header[0x48];

		_ram_size = header[0x49];

		_destination_code = header[0x4A];

		_old_licensee_code = header[0x4B];

		_version = header[0x4C];

		// TODO ...
		_header_checksum = 0;

		_global_checksum = header[0x4E] << 8 | header[0x4F];
	}


	/*
	* HEADER CODES
	*/


	// 0x0104 - 0x0133: Valid bytes for the ROM to be allowed.
	static constexpr u8 VALID_NINTENDO_LOGO[0x30] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};

	// 0x143: support GameBoy Color features.
	static const std::unordered_map<u8, const char*> CGB_FLAGS = {
		{0x80, "ALL_GBS"},
		{0xC0, "CGB_ONLY"},
		// Unknown mode.
		{0x42, "PGB_MODE"},
		{0x43, "PGB_MODE"}
	};

	// 0x0146: support Super GameBoy functions.
	static const u8 SUPPORTS_SGB = 0x03;

	// 0x0147: kind of hardware present on the cartridge
	static const std::unordered_map<u8, const char*> CARTRIDGE_TYPES = {
		{0x00, "ROM ONLY"}
		// TODO ...
	};

	/**
	* Computes the size of the rom with the flag (0x0148) on the cartridge.
	* @param flag Value on the cartridge
	*/
	inline u32 romSize(u8 flag) {
		return 0x8000 * (1 << flag);
	}

	// 0x0149: how much ram is present on the cartridge.
	static const u32 RAM_SIZES[5] = {
		0,
		0x800, // Unofficial value. Somtimes used in homebrew
		0x8000,
		0x20000,
		0x10000
	};

	// 0x014A: where the game is intended to be sold.
	static const char* DESTINATION_CODES[2] = {
		"Japan",
		"Overseas"
	};

	// 0x0144 - 0x0145: game publisher. Only considered if the old licensee
	// code is exactly 33.
	static const std::unordered_map<u16, const char*> NEW_LICENSEE_CODE = {
		{0x0000, "None"},
		// TODO ... 
	};

	// 0x014B: game publisher. 
	static const std::unordered_map<u8, const char*> OLD_LICENSEE_CODES = {
		{0x00, "None"},
		// TODO ... 
	};
}