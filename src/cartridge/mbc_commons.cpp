#include <cartridge/mbc.h>

#include <fstream>

namespace jmpr {

	MBC* giveAppropriateMBC(u8 cartridgeHardware, u32 romSize, u8* romData, u32 ramSize) {

		// No MBC
		if (cartridgeHardware == 0x00 || cartridgeHardware == 0x08 || cartridgeHardware == 0x09) {

			return new NoMBC(romData, ramSize, cartridgeHardware == 0x09);
		}
		// MBC1
		else if (between(cartridgeHardware, 0x01, 0x03)) {

			return new MBC1(romData, romSize, ramSize, cartridgeHardware == 0x03);
		}
		else if (between(cartridgeHardware, 0x0F, 0x13)) {

			return new MBC3(romData, romSize, ramSize,
				cartridgeHardware == 0x0F || cartridgeHardware == 0x10 || cartridgeHardware == 0x13,
				cartridgeHardware == 0x0F || cartridgeHardware == 0x10);
		}

		std::cerr << "Error: The game requested has unsupported cartridge hardware. \n" << std::endl;
		exit(-2);
	}

	// MBC template

	MBC::MBC(bool hasRam, bool hasBattery) {

		_rom_banks = std::vector<std::unique_ptr<u8>>();
		_ram_banks = std::vector<std::unique_ptr<u8>>();

		_has_ram = hasRam;
		_has_battery = hasBattery;

		_waiting_for_save = false;
	}

	bool MBC::hasSavePending() const {
		return _waiting_for_save && _has_ram && _has_battery;
	}

	void MBC::save(const char* path) {

		std::ofstream outSave(path, std::ios::binary | std::ios::trunc);

		if (!outSave) {
			std::cerr << "Failed to open the save file" << std::endl;
			exit(-9);
		}
		outSave.close();

		outSave.open(path, std::ios::binary | std::ios::app);

		if (!outSave) {
			std::cerr << "Failed to open the save file" << std::endl;
			exit(-9);
		}

		for (u8 i = 0; i < _ram_banks.size(); i++) {

			outSave.write(reinterpret_cast<const char*>(_ram_banks[i].get()), 0x2000);
		}
		outSave.close();

		_waiting_for_save = false;
	}

	void MBC::loadSave(const char* path) {

		if (!_has_battery) return;

		std::ifstream inSave(path, std::ios::binary);

		if (inSave) {

			for (u8 i = 0; i < _ram_banks.size(); i++) {

				inSave.read(reinterpret_cast<char*>(_ram_banks[i].get()), 0x2000);
			}

			inSave.close();
		}
		else {

			std::cout << "No save file was found. Creating a new save file." << std::endl;

			std::ofstream outSave(path, std::ios::binary);

			outSave.close();
		}
	}

	// No MBC

	NoMBC::NoMBC(u8* romData, bool hasRam, bool hasBattery) : MBC(hasRam, hasBattery) {

		// Only a single ROM bank.
		_rom_banks.push_back(std::unique_ptr<u8>(romData));

		// Add a possible 8 KiB of ram
		if (_has_ram) {
			_ram_banks.push_back(std::unique_ptr<u8>(new u8[0x2000]));
		}
	}

	u8 NoMBC::read(u16 address) const {

		if (between(address, 0x0000, 0x7FFF)) {
			return _rom_banks[0].get()[address];
		}
		else if (between(address, 0xA000, 0xBFFF) && _has_ram) {
			return _ram_banks[0].get()[address - 0xA000];
		}

		return 0xFF;
	}

	void NoMBC::write(u16 address, u8 data) {

		// Only RAM is writeable
		if (between(address, 0xA000, 0xBFFF) && _has_ram) {

			_ram_banks[0].get()[address - 0xA000] = data;

			if (_has_battery) _waiting_for_save = true;
		}
	}
}