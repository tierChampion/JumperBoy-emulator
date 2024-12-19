#pragma once

#include <common.h>

namespace jmpr
{
  class BootRom
  {
    std::vector<u8> _rom_data;
    u8 _disable_boot;

  public:
    BootRom();

    void load(const std::string &bootFile);
    void reboot();

    bool isValid() const;

    u8 read(u16 address) const;
    void setDisableBoot(u8 data);
    u8 getDisableBoot() const;
  };
}
