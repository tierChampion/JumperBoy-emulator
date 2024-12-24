#include <ui/user_settings.h>

#include <fstream>
#include <string>

// TODO switch to specific .config or APPDATA locations
const char *LOCATION_FILE = "./location.txt";
const char *SETTINGS_FILE = "./settings.json";

namespace jmpr
{
    UserSettings::UserSettings()
    {
        setToDefault();
        loadSettings();
    }

    void UserSettings::setToDefault()
    {
        settings_location = SETTINGS_FILE;

        recents = std::vector<std::string>();
        pallets = std::vector<Pallet>();
        input_maps = std::vector<InputMap>();

        pallets.push_back(getDefaultPallet());
        input_maps.push_back(getDefaultInputMap());

        rom_folder = "";
        boot_folder = "";
        save_folder = "";
        input_selection = 0;
        pallet_selection = 0;
        resolution_scale = 1;
        volume = 1;
    }

    Pallet UserSettings::getDefaultPallet() const
    {
        Pallet p;
        p.name = "Classic GB";
        p.colors = {0xFFFF,
                    0x3DEF,
                    0x1CE7,
                    0x0C63};
        return p;
    }

    InputMap UserSettings::getDefaultInputMap() const
    {
        InputMap i;
        i.name = "Default";
        i.inputs = {
            {"W", JumperInput::UP},
            {"A", JumperInput::LEFT},
            {"S", JumperInput::DOWN},
            {"D", JumperInput::RIGHT},
            {"H", JumperInput::A},
            {"J", JumperInput::B},
            {"N", JumperInput::SELECT},
            {"M", JumperInput::START},
            {"O", JumperInput::MAX_SPEED}};

        return i;
    }

    Pallet UserSettings::currentPallet() const
    {
        return pallets[pallet_selection];
    }

    InputMap UserSettings::currentInputMap() const
    {
        return input_maps[input_selection];
    }

    // TODO how to have the user change the settings file (load a new one during execution) ?
    void UserSettings::loadSettings()
    {
        loadSettingLocation();
        loadSettingsFile();
        save();
    }

    void UserSettings::loadSettingLocation()
    {
        std::ifstream f(LOCATION_FILE);

        if (f.is_open())
        {
            std::getline(f, settings_location);
            f.close();
        }
        else
        {
            std::ofstream o(LOCATION_FILE);
            if (o.is_open())
            {
                o << SETTINGS_FILE;
                o.close();
            }
        }
    }

    void UserSettings::loadSettingsFile()
    {
        std::ifstream f(settings_location);

        if (!f.is_open())
            return;

        nlohmann::json jsonData;
        bool success = true;
        try
        {
            f >> jsonData;
            fromJson(jsonData);
        }
        catch (const nlohmann::json::parse_error &e)
        {
            success = false;
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
        catch (const nlohmann::json::type_error &e)
        {
            success = false;
            std::cerr << "Type error: " << e.what() << std::endl;
        }
        catch (const nlohmann::json::out_of_range &e)
        {
            success = false;
            std::cerr << "Missing field: " << e.what() << std::endl;
        }
        if (!success)
        {
            setToDefault();
        }
    }

    void UserSettings::save()
    {
        nlohmann::json jsonData;
        toJson(jsonData);

        std::ofstream f(settings_location);
        if (f.is_open())
        {
            f << jsonData;
        }
        f.close();
    }

    void UserSettings::fromJson(const nlohmann::json &j)
    {
        j.at("recents").get_to(recents);
        j.at("pallets").get_to(pallets);
        j.at("input_maps").get_to(input_maps);

        j.at("rom_folder").get_to(rom_folder);
        j.at("boot_folder").get_to(boot_folder);
        j.at("save_folder").get_to(save_folder);

        j.at("input_selection").get_to(input_selection);
        j.at("pallet_selection").get_to(pallet_selection);
        j.at("resolution_scale").get_to(resolution_scale);
        j.at("volume").get_to(volume);
    }

    void UserSettings::toJson(nlohmann::json &j)
    {
        j = nlohmann::json{
            {"recents", recents},
            {"pallets", pallets},
            {"input_maps", input_maps},

            {"rom_folder", rom_folder},
            {"boot_folder", boot_folder},
            {"save_folder", save_folder},

            {"input_selection", input_selection},
            {"pallet_selection", pallet_selection},
            {"resolution_scale", resolution_scale},
            {"volume", volume},
        };
    }
}

namespace nlohmann
{
    void from_json(const nlohmann::json &j, jmpr::Pallet &p)
    {
        j.at("name").get_to(p.name);
        j.at("colors").get_to(p.colors);
    }

    void to_json(nlohmann::json &j, const jmpr::Pallet &p)
    {
        j = nlohmann::json{
            {"name", p.name},
            {"colors", p.colors}};
    }

    void from_json(const nlohmann::json &j, jmpr::InputMap &i)
    {
        j.at("name").get_to(i.name);
        j.at("inputs").get_to(i.inputs);
    }

    void to_json(nlohmann::json &j, const jmpr::InputMap &i)
    {
        j = nlohmann::json{
            {"name", i.name},
            {"inputs", i.inputs}};
    }
}
