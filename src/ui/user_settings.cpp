#include <ui/user_settings.h>

#include <fstream>
#include <string>

const char *LOCATION_FILE = "./location.txt";
const char *SETTINGS_FILE = "./settings.json";

namespace jmpr
{
    UserSettings::UserSettings()
    {
        // TODO set default values
        setToDefault();
        loadSettings();
    }

    void UserSettings::setToDefault()
    {
        settings_location = SETTINGS_FILE;
        // TODO ...
    }

    // TODO how to have the user change the settings file (load a new one during execution)
    void UserSettings::loadSettings()
    {
        loadSettingLocation();
        loadSettingsFile();
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
            // TODO create the location file if it doesn't exist
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
            // TODO set to default values
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

        j.at("input_selection").get_to(rom_folder);
        j.at("pallet_selection").get_to(boot_folder);
        j.at("resolution_scale").get_to(save_folder);
        j.at("volume").get_to(save_folder);
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