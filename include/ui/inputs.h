#pragma once

#include <common.h>

namespace jmpr
{
    enum class JumperInput
    {
        NONE,
        RIGHT,
        LEFT,
        UP,
        DOWN,
        A,
        B,
        SELECT,
        START,
        MAX_SPEED
    };

    enum class NewJoypadInput
    {
        RIGHT,
        LEFT,
        UP,
        DOWN,
        A,
        B,
        SELECT,
        START,
    };

    bool isJoypadInput(const JumperInput &input);

    JumperInput nameToInput(const std::string &inputName);

    std::string inputToName(const JumperInput &input);
}