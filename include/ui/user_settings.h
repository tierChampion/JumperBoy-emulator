#include <common.h>
#include <ui/inputs.h>

#include <json.hpp>

namespace jmpr
{
    class UserSettings
    {
    public:
        std::vector<std::string> recents;
        std::vector<std::array<u16, 4>> pallets;
        std::vector<std::map<std::string, JumperInput>> input_maps;
        std::string rom_folder;
        std::string boot_folder;
        std::string save_folder;
        u8 input_selection;
        u8 pallet_selection;
        u8 resolution_scale;
        u8 volume;

        std::string settings_location;

    public:
        UserSettings();

        void save();
        void fromJson(const nlohmann::json& j);
        void toJson(nlohmann::json& j);
        void loadSettings();

    private:
        void setToDefault();
        void loadSettingLocation();
        void loadSettingsFile();
    };
}