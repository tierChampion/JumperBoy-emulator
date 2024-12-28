#pragma once

#include <common.h>
#include <ui/inputs.h>

#include <json.hpp>

namespace jmpr
{
    struct Pallet
    {
        std::string name;
        std::array<u16, 4> colors;
    };

    struct InputMap
    {
        std::string name;
        std::map<std::string, JumperInput> inputs;
    };

    class UserSettings
    {
    public:
        std::vector<std::string> recents;
        std::vector<Pallet> pallets;
        std::vector<InputMap> input_maps;
        std::string rom_folder;
        std::string boot_folder;
        std::string save_folder;
        u8 input_selection;
        u8 pallet_selection;
        int resolution_scale;
        float volume;

        std::string settings_location;

    public:
        UserSettings();

        void save();
        void fromJson(const nlohmann::json &j);
        void toJson(nlohmann::json &j);
        void loadSettings();

        Pallet currentPallet() const;
        InputMap currentInputMap() const;

    private:
        void setToDefault();
        Pallet getDefaultPallet() const;
        InputMap getDefaultInputMap() const;
        void loadSettingLocation();
        void loadSettingsFile();
    };
}

namespace nlohmann
{
    void from_json(const nlohmann::json &j, jmpr::Pallet &p);
    void to_json(nlohmann::json &j, const jmpr::Pallet &p);

    void from_json(const nlohmann::json &j, jmpr::InputMap &i);
    void to_json(nlohmann::json &j, const jmpr::InputMap &i);
}