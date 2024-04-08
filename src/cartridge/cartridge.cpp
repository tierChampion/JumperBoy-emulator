#include <cartridge/cartridge.h>

#include <cartridge/mbc.h>

#include <unordered_map>
#include <fstream>

namespace jmpr
{

	// 0x0149: how much ram is present on the cartridge in bytes.
	static const u32 RAM_SIZES[5] = {
		0,
		0x800, // Unofficial value. Somtimes used in homebrew
		0x8000,
		0x20000,
		0x10000};

	/**
	 * Load a cartridge from a binary file.
	 * @param file Path of the file containing the game
	 */
	Cartridge::Cartridge(const std::string &file) : _error(false)
	{

		std::ifstream stream;
		stream.open(file, std::fstream::in | std::fstream::binary);

		if (!stream)
		{
			std::cerr << "Error: the ROM path (" << file << ") isn't valid." << std::endl;
			_error = true;
			return;
		}

		auto begin = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto end = stream.tellg();
		stream.seekg(0, std::ios::beg);

		_rom_size = (u32)(end - begin);

		_rom_data = std::unique_ptr<u8>(new u8[_rom_size]);

		stream.read((char *)_rom_data.get(), _rom_size);

		_header.formatHeader(&_rom_data.get()[0x100]);

		_filename = file;

		_savename = (file.substr(0, file.find(".gb")) + ".sav");

		stream.close();

		std::cout << *this << std::endl;

		_mbc = giveAppropriateMBC(_header._cartridge_type, _rom_size, _rom_data.get(), RAM_SIZES[_header._ram_size]);

		if (_mbc == nullptr)
		{
			_error = true;
		}

		if (!isValid())
		{
			std::cerr << "Error: The provided ROM (" << _header._title << ") isn't valid." << std::endl;
			_error = true;
			return;
		}

		_mbc->loadSave(_savename.c_str());
	}

	/**
	 * Read from the rom data on the cartridge.
	 * @param address Address to read from.
	 */
	u8 Cartridge::read(u16 address) const
	{

		return _mbc->read(address);
	}

	/**
	 * Write to the rom data on the cartridge.
	 * @param address Address to write to.
	 */
	void Cartridge::write(u16 address, u8 data)
	{

		_mbc->write(address, data);
	}

	void Cartridge::handleSaves() const
	{

		if (_mbc->hasSavePending())
		{

			_mbc->save(_savename.c_str());
		}
	}

	/**
	 * Fetch and decode the information of the header of the
	 * rom.
	 * @param header Bytes to decode starting at location 0x0100
	 */
	void CartridgeHeader::formatHeader(u8 *header)
	{

		for (int i = 0; i < 4; i++)
			_entry[i] = header[i];

		for (int i = 0; i < 0x30; i++)
			_nintendo_logo[i] = header[i + 0x04];

		for (int i = 0; i < 16; i++)
			_title[i] = (char)header[i + 0x34];

		for (int i = 0; i < 4; i++)
			_manufacturer_code[i] = (char)header[i + 0x3F];

		_cgb_flag = header[0x43];

		_new_licensee_code = merge16(header[0x44], header[0x45]);

		_sgb_flag = header[0x46];

		_cartridge_type = header[0x47];

		_rom_size = header[0x48];
		_ram_size = header[0x49];

		_destination_code = header[0x4A];

		_old_licensee_code = header[0x4B];
		_version = header[0x4C];

		u8 valid_details = 0;
		for (int i = 0x34; i < 0x4D; i++)
		{
			valid_details = valid_details - header[i] - 1;
		}

		_header_checksum = header[0x4D];

		_valid_sum = (valid_details == _header_checksum);

		_global_checksum = merge16(header[0x4E], header[0x4F]);
	}

	/*
	 * HEADER CODES https://raw.githubusercontent.com/gb-archive/salvage/master/txt-files/gbrom.txt
	 */

	// 0x0104 - 0x0133: Valid bytes for the ROM to be allowed.
	static constexpr u8 VALID_NINTENDO_LOGO[0x30] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

	// 0x143: support GameBoy Color features.
	static const std::unordered_map<u8, const char *> CGB_FLAGS = {
		{0x00, "No CGB"},
		{0x80, "All GBS"},
		{0xC0, "CGB Only"},
		{0x42, "PGB Mode (Unknown)"},
		{0x44, "PGB Mode (Unknown)"}};

	// 0x0146: support Super GameBoy functions.
	static const u8 SUPPORTS_SGB = 0x03;

	// 0x0147: kind of hardware present on the cartridge
	static const std::unordered_map<u8, const char *> CARTRIDGE_TYPES = {
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
	 * Computes the size of the rom with the flag (0x0148) on the cartridge in bytes.
	 * @param flag Value on the cartridge
	 */
	u32 CartridgeHeader::getRomSize() const
	{
		return 0x8000 * (1 << _rom_size);
	}

	// 0x014A: where the game is intended to be sold.
	static const char *DESTINATION_CODES[2] = {
		"Japan",
		"Overseas"};

	// 0x0144 - 0x0145: game publisher code for games released after
	// the SGB. Only considered if the old licensee
	// code is exactly 33.
	static const std::unordered_map<u16, const char *> NEW_LICENSEE_CODES = {
		{0x3030, "None"},
		{0x3031, "Nintendo"},
		{0x0038, "Capcom"},
		{0x3133, "Electronic Arts"},
		{0x3138, "HudsonSoft"},
		{0x3139, "B-Ai"},
		{0x3230, "KSS"},
		{0x3232, "Pow"},
		{0x3234, "PCM Complete"},
		{0x3235, "San-X"},
		{0x3238, "Kemco Japan"},
		{0x3239, "Seta"},
		{0x3330, "Viacom"},
		{0x3331, "Nintendo"},
		{0x3332, "Bandia"},
		{0x3333, "Ocean/Acclaim"},
		{0x3334, "Konami"},
		{0x3335, "Hector"},
		{0x3337, "Taito"},
		{0x3338, "Hudson"},
		{0x3339, "Banpresto"},
		{0x3431, "UbiSoft"},
		{0x3432, "Atlus"},
		{0x3434, "Malibu"},
		{0x3436, "Angel"},
		{0x3437, "Pullet-proof"},
		{0x3439, "Irem"},
		{0x3530, "Absolute"},
		{0x3531, "Acclaim"},
		{0x3532, "Activision"},
		{0x3533, "American Sammy"},
		{0x3534, "Konami"},
		{0x3535, "Hi Tech Entertainment"},
		{0x3536, "LJN"},
		{0x3537, "Matchbox"},
		{0x3538, "Mattel"},
		{0x3539, "Milton Bradley"},
		{0x3630, "Titus"},
		{0x3631, "Virgin"},
		{0x3634, "LucasArts"},
		{0x3637, "Ocean"},
		{0x3639, "Electronic Arts"},
		{0x3730, "InfoGrames"},
		{0x3731, "Interplay"},
		{0x3732, "Broderbund"},
		{0x3733, "Sculptured"},
		{0x3735, "Sci"},
		{0x3738, "T*HQ"},
		{0x3739, "Accolade"},
		{0x3830, "Misawa"},
		{0x3833, "Lozc"},
		{0x3836, "Tokuma Shoten I*"},
		{0x3837, "Tsukuda Ori*"},
		{0x3931, "Chun Soft"},
		{0x3932, "Video System"},
		{0x3933, "Ocean/Acclaim"},
		{0x3935, "Varie"},
		{0x3936, "Yonezawa/s'Pal"},
		{0x3937, "Kaneko"},
		{0x3939, "Pack in Soft"},
	};

	// 0x014B: game publisher code for games released before the SGB.
	static const std::unordered_map<u8, const char *> OLD_LICENSEE_CODES = {
		{0x00, "None"},
		{0x01, "Nintendo"},
		{0x08, "Capcom"},
		{0x09, "Hot-B"},
		{0x0A, "Jaleco"},
		{0x0B, "Coconuts"},
		{0x0C, "Elite Systems"},
		{0x13, "Electronic Arts"},
		{0x18, "HudsonSoft"},
		{0x19, "ITC Entertainment"},
		{0x1A, "Yanoman"},
		{0x1D, "Clary"},
		{0x1F, "Virgin"},
		{0x24, "PCM Complete"},
		{0x25, "San-X"},
		{0x28, "Kotobuki Systems"},
		{0x29, "Seta"},
		{0x30, "Infogrames"},
		{0x31, "Nintendo"},
		{0x32, "Bandai"},
		{0x33, "USE NEW LICENSEE CODE"},
		{0x34, "Konami"},
		{0x35, "Hector"},
		{0x38, "Capcom"},
		{0x39, "Banpresto"},
		{0x3C, "Entertainment I"},
		{0x3E, "Gremlin"},
		{0x41, "UbiSoft"},
		{0x42, "Atlus"},
		{0x44, "Malibu"},
		{0x46, "Angel"},
		{0x47, "Spectrum Holoby"},
		{0x49, "Irem"},
		{0x4A, "Virgin"},
		{0x4D, "Malibu"},
		{0x4F, "U.S. Gold"},
		{0x50, "Absolute"},
		{0x51, "Acclaim"},
		{0x52, "Activision"},
		{0x53, "American Sammy"},
		{0x54, "Gametek"},
		{0x55, "Park Place"},
		{0x56, "LJN"},
		{0x57, "Matchbox"},
		{0x59, "Milton Bradley"},
		{0x5A, "Mindscape"},
		{0x58, "Romstar"},
		{0x5C, "Naxat Soft"},
		{0x5D, "TradeWest"},
		{0x60, "Titus"},
		{0x61, "Virgin"},
		{0x67, "Ocean"},
		{0x69, "Electronics Arts"},
		{0x6E, "Elite Systems"},
		{0x6F, "Electro Brain"},
		{0x70, "Infogrames"},
		{0x71, "Interplay"},
		{0x72, "Broderbund"},
		{0x73, "Sculptered Soft"},
		{0x75, "The Sales Curve"},
		{0x78, "T*HQ"},
		{0x79, "Accolade"},
		{0x7A, "Triffix Entertainment"},
		{0x7C, "Micropose"},
		{0x7F, "Kemco"},
		{0x80, "Misawa Entertainment"},
		{0x83, "LOZC"},
		{0x86, "Tokuma Shoten I"},
		{0x8B, "Bullet-proof Entertainment"},
		{0x8C, "Vic Tokai"},
		{0x8E, "APE"},
		{0x8F, "I'Max"},
		{0x91, "Chun Soft"},
		{0x92, "Video System"},
		{0x93, "Truburava"},
		{0x95, "Varie"},
		{0x96, "Yonezawa/s'pal"},
		{0x97, "Kaneko"},
		{0x99, "Arc"},
		{0x9A, "Nihon Bussan"},
		{0x9B, "Tecmo"},
		{0x9C, "Imagineer"},
		{0x9D, "Banpresto"},
		{0x9F, "Nova"},
		{0xA1, "Hori Electric"},
		{0xA2, "Bandai"},
		{0xA4, "Konami"},
		{0xA6, "Kawada"},
		{0xA7, "Takara"},
		{0xA9, "Technos Japan"},
		{0xAA, "Broderbund"},
		{0xAC, "Toei Animation"},
		{0xAD, "Toho"},
		{0xAF, "Namco"},
		{0xB0, "Acclaim"},
		{0xB1, "ASCII or Nexoft"},
		{0xB2, "Bandai"},
		{0xB4, "Enix"},
		{0xB6, "HAL"},
		{0xB7, "SNK"},
		{0xB9, "Pony Canyon"},
		{0xBA, "Culture Brain O"},
		{0xBB, "SunSoft"},
		{0xBD, "Sony ImageSoft"},
		{0xBF, "Sammy"},
		{0xC0, "Taito"},
		{0xC2, "Kemco"},
		{0xC3, "SquareSoft"},
		{0xC4, "Tokuma Shoten I"},
		{0xC5, "Data East"},
		{0xC6, "Tonkin House"},
		{0xC8, "Koei"},
		{0xC9, "UFL"},
		{0xCA, "Ultra"},
		{0xCB, "Vap"},
		{0xCC, "USE"},
		{0xCD, "Meldac"},
		{0xCE, "Pony Canyon Or"},
		{0xCF, "Angel"},
		{0xD0, "Taito"},
		{0xD1, "Sofel"},
		{0xD2, "Quest"},
		{0xD3, "Sigma Enterprises"},
		{0xD4, "Ask Kodansha"},
		{0xD6, "Naxat Soft"},
		{0xD7, "Copya Systems"},
		{0xD9, "Banpresto"},
		{0xDA, "Tomy"},
		{0xDB, "LJN"},
		{0xDD, "NCS"},
		{0xDE, "Human"},
		{0xDF, "Altron"},
		{0xE0, "Jaleco"},
		{0xE1, "Towachiki"},
		{0xE2, "Uutaka"},
		{0xE3, "Varie"},
		{0xE5, "Epoch"},
		{0xE7, "Athena"},
		{0xE8, "Asmik"},
		{0xE9, "Natsume"},
		{0xEA, "King Records"},
		{0xEB, "Atlus"},
		{0xEC, "Epic/Sony Records"},
		{0xEE, "IGS"},
		{0xF0, "A Wave"},
		{0xF3, "Extreme Entertainment"},
		{0xFF, "LJN"},
	};

	/**
	 * Get the name of the licensed company.
	 */
	const char *CartridgeHeader::getLicenseeName() const
	{
		if (_old_licensee_code == 0x33)
		{
			// Use the New Licensee Code
			if (auto search = NEW_LICENSEE_CODES.find(_new_licensee_code);
				search != NEW_LICENSEE_CODES.end())
				return search->second;
		}
		else
		{
			// Use the Old Licensee Code
			if (auto search = OLD_LICENSEE_CODES.find(_old_licensee_code);
				search != OLD_LICENSEE_CODES.end())
				return search->second;
		}

		return "Unknown";
	}

	/**
	 * Checks if the cartridge is valid. Done by the Boot ROM in normal execution.
	 */
	bool Cartridge::isValid() const
	{
		if (_error || _rom_size == 0)
			return false;

		return std::equal(std::begin(_header._nintendo_logo),
						  std::end(_header._nintendo_logo),
						  std::begin(VALID_NINTENDO_LOGO)) &&
			   _header._valid_sum;
	}

	/**
	 * Checks if the catridge is a CGB cartridge.
	 */
	bool Cartridge::isColor() const
	{
		return _header._cgb_flag == 0x80 || _header._cgb_flag == 0xC0;
	}

	std::ostream &operator<<(std::ostream &os, const Cartridge &cartridge)
	{
		os << "---Cartridge---" << std::endl;
		// General Game information
		os << "Title: " << cartridge._header._title << std::endl;
		os << "Manufacturer Code: " << cartridge._header._manufacturer_code << std::endl;
		os << "Version: " << (int)cartridge._header._version << std::endl;
		os << "Licensee code: " << cartridge._header.getLicenseeName() << std::endl;
		os << "Destination: " << DESTINATION_CODES[cartridge._header._destination_code] << std::endl;
		// Hardware Specification
		os << "Rom size: " << (cartridge._header.getRomSize() >> 10) << " KiB" << std::endl;
		os << "Ram size: " << (RAM_SIZES[cartridge._header._ram_size] >> 10) << " KiB" << std::endl;
		os << "Cartridge Hardware: " << CARTRIDGE_TYPES.at(cartridge._header._cartridge_type) << std::endl;
		// Console Compatibility
		os << "GameBoy Color compatibility: " << CGB_FLAGS.at(cartridge._header._cgb_flag) << std::endl;
		os << "Super GameBoy compatibility: " << (cartridge._header._sgb_flag == SUPPORTS_SGB ? "Yes" : "No") << std::endl;
		// Checksums
		os << "Header Checksum: " << (int)cartridge._header._header_checksum << std::endl;
		os << "Global Checksum: " << (int)cartridge._header._global_checksum << std::endl;
		os << "Valid Checksum: " << (cartridge._header._valid_sum ? "True" : "False") << std::endl;

		return os;
	}
}