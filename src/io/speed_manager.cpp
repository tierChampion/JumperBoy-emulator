#include <io/speed_manager.h>

#include <gb.h>

namespace jmpr
{
    SpeedManager::SpeedManager() : _key_0(0), _key_1(0)
    {
    }

    void SpeedManager::reboot()
    {
        _key_0 = GameBoy::getInstance()->isCGB() ? 0 : 0b100;
        _key_1 = 0;
    }

    bool SpeedManager::isDoubleSpeed()
    {
        return bit(_key_1, 7);
    }

    u8 SpeedManager::getKey1()
    {
        return _key_1;
    }

    bool SpeedManager::isSwitchRequested()
    {
        return bit(_key_1, 0);
    }

    void SpeedManager::requestSpeedSwitch()
    {
        _key_1 = set(_key_1, 0);
    }

    void SpeedManager::toggleSpeed()
    {
        if (isSwitchRequested())
        {
            _key_1 = (~(_key_1 & 0x80)) & 0x80;
            std::cout << int(_key_1) << std::endl;
        }
    }
}