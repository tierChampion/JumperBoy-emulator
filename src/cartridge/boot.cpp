#include <cartridge/boot.h>

#include <fstream>

namespace jmpr
{
    BootRom::BootRom() : _disable_boot(0xFF), _rom_data(0), _filename("")
    {
    }

    bool BootRom::load(const std::string &bootFile)
    {
        _filename = bootFile;
        if (bootFile == "")
        {
            clearBoot();
            return true;
        }
        std::ifstream stream;
        stream.open(bootFile, std::fstream::in | std::fstream::binary);

        if (!stream)
        {
            std::cerr << "Error: the BootROM path (" << bootFile << ") isn't valid." << std::endl;
            return false;
        }

        auto begin = stream.tellg();
        stream.seekg(0, std::ios::end);
        auto end = stream.tellg();
        stream.seekg(0, std::ios::beg);

        u32 rom_size = (u32)(end - begin);

        // TODO for cgb, the size is 0x100 + 0x700 = 0x800
        if (rom_size != 0x100)
            return false;

        _rom_data.resize(rom_size);

        stream.read((char *)_rom_data.data(), _rom_data.size());
        return true;
    }

    void BootRom::clearBoot()
    {
        if (!isValid())
        {
            _rom_data.clear();
            _rom_data.resize(0);
        }
    }

    u8 BootRom::read(u16 address) const
    {
        // TODO support cgb as well
        return _rom_data[address];
    }

    void BootRom::setDisableBoot(u8 data)
    {
        _disable_boot = data;
    }

    u8 BootRom::getDisableBoot() const
    {
        return _disable_boot;
    }

    void BootRom::reboot()
    {
        _disable_boot = _rom_data.size() > 0 ? 0 : 0xFF;
    }

    bool BootRom::isValid() const
    {
        return _disable_boot == 0 && _rom_data.size() > 0;
    }

}