#include <ui/inputs.h>

#include <unordered_map>

namespace jmpr
{

    bool isJoypadInput(const JumperInput &input)
    {
        return input >= JumperInput::RIGHT && input <= JumperInput::START;
    }

    JumperInput nameToInput(const std::string &inputName)
    {
        static const std::unordered_map<std::string, JumperInput> STRING_TO_INPUT = {
            {"NONE", JumperInput::NONE},
            {"RIGHT", JumperInput::RIGHT},
            {"LEFT", JumperInput::LEFT},
            {"UP", JumperInput::UP},
            {"DOWN", JumperInput::DOWN},
            {"A", JumperInput::A},
            {"B", JumperInput::B},
            {"SELECT", JumperInput::SELECT},
            {"START", JumperInput::START},
            {"MAX_SPEED", JumperInput::MAX_SPEED},
        };

        return STRING_TO_INPUT.at(inputName);
    }

    std::string inputToName(const JumperInput &input)
    {
        static const std::unordered_map<JumperInput, std::string> INPUT_TO_STRING = {
            {JumperInput::NONE, "NONE"},
            {JumperInput::RIGHT, "RIGHT"},
            {JumperInput::LEFT, "LEFT"},
            {JumperInput::UP, "UP"},
            {JumperInput::DOWN, "DOWN"},
            {JumperInput::A, "A"},
            {JumperInput::B, "B"},
            {JumperInput::SELECT, "SELECT"},
            {JumperInput::START, "START"},
            {JumperInput::MAX_SPEED, "MAX_SPEED"},
        };

        if (auto search = INPUT_TO_STRING.find(input);
            search != INPUT_TO_STRING.end())
        {
            return search->second;
        }
        return "NONE";
    }
}