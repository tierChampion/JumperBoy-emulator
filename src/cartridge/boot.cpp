#include <cartridge/boot.h>

#include <fstream>

namespace jmpr
{
    BootRom::BootRom() : _disable_boot(0), _rom_data(0)
    {
    }

    void BootRom::load(const std::string &bootFile)
    {
        std::ifstream stream;
        stream.open(bootFile, std::fstream::in | std::fstream::binary);

        if (!stream)
        {
            std::cerr << "Error: the BootROM path (" << bootFile << ") isn't valid." << std::endl;
            return;
        }

        auto begin = stream.tellg();
        stream.seekg(0, std::ios::end);
        auto end = stream.tellg();
        stream.seekg(0, std::ios::beg);

        u32 rom_size = (u32)(end - begin);

        _rom_data.resize(rom_size);

        stream.read((char *)_rom_data.data(), _rom_data.size());
    }

    u8 BootRom::read(u16 address) const
    {
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
        _disable_boot = 0;
    }

    bool BootRom::isValid() const
    {
        return _disable_boot == 0 && _rom_data.size() > 0;
    }

}