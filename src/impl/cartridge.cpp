#include <headers/cartridge.h>
#include <unordered_map>
#include <fstream>

namespace jmpr {

	Cartridge::Cartridge(const char* file) {

		std::ifstream stream;
		stream.open(file, std::fstream::in | std::fstream::binary);

		auto begin = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto end = stream.tellg();
		stream.seekg(0, std::ios::beg);

		_rom_size = (end - begin);

		_rom_data = new u8[_rom_size];

		stream.read((char*)_rom_data, _rom_size);

		_header.formatHeader(&_rom_data[0x100]);

		_filename = file;

		stream.close();
	}

	void CartridgeHeader::formatHeader(u8* header) {

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

		_header_checksum = 0; // TODO ...

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
		{0x00, "ROM ONLY"},
		{0x01, "MBC1"},
		{0x02, "MBC1+RAM"},
		{0x03, "MBC1+RAM+BATTERY"},
		{0x05, "MBC2"},
		{0x06, "MBC2+BATTERY"},
		{0x08, "ROM+RAM"},
		{0x09, "ROM+RAM+BATTERY"},
		{0x0B, "MMM01"},
		{0x0C, "MMM01+RAM"},
		{0x0D, "MMM01+RAM+BATTERY"},
		{0x0F, "MBC3+TIMER+BATTERY"},
		{0x10, "MBC3+TIMER+RAM+BATTERY"},
		{0x11, "MBC3"},
		{0x12, "MBC3+RAM"},
		{0x13, "MBC3+RAM+BATTERY"},
		{0x19, "MBC5"},
		{0x1A, "MBC5+RAM"},
		{0x1B, "MBC5+RAM+BATTERY"},
		{0x1C, "MBC5+RUMBLE"},
		{0x1D, "MBC5+RUMBLE+RAM"},
		{0x1E, "MBC5+RUMBLE+RAM+BATTERY"},
		{0x20, "MBC6"},
		{0x22, "MBC7+SENSOR+RUMBLE+RAM+BATTERY"},
		{0xFC, "POCKET CAMERA"},
		{0xFD, "BANDAI TAMA5"},
		{0xFE, "HuC3"},
		{0xFF, "HuC1+RAM+BATTERY"},
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

	std::ostream& operator<<(std::ostream& os, const Cartridge& cartridge) {

		os << "---Cartridge---" << std::endl;
		os << "Title: " << cartridge._header._title << std::endl;
		os << "Manufacturer Code: " << cartridge._header._manufacturer_code << std::endl;
		os << "Version: " << (int)cartridge._header._version << std::endl;

		os << "Rom size: " << romSize(cartridge._header._rom_size) << std::endl;
		os << "Ram size: " << RAM_SIZES[cartridge._header._ram_size] << std::endl;

		// Check if applicable
		//os << "CGB Flag: " << CGB_FLAGS.at(cartridge._header._cgb_flag) << std::endl;
		// Add also the licensees

		os << "Cartridge Hardware: " << CARTRIDGE_TYPES.at(cartridge._header._cartridge_type) << std::endl;

		os << "Super GameBoy compatibility: " <<
			(cartridge._header._sgb_flag == SUPPORTS_SGB ? "Yes" : "No") << std::endl;

		os << "Destination: " << DESTINATION_CODES[cartridge._header._destination_code] << std::endl;

		os << "Global Checksum: " << (int)cartridge._header._global_checksum << std::endl;

		return os;
	}

}