#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    constexpr auto path = L"Data/SKSE/Plugins/DynamicActivationKey.ini";
    logger::info("Loading settings");

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(path);
 
    DAKModifierKey = std::stoi(ini.GetValue("Keys", "iDAKModifierKey", "42"));
    DAKControllerKey = std::stoi(ini.GetValue("Keys", "iDAKControllerKey", "274"));
    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    };

    (void)ini.SaveFile(path);
    // Load settings
    logger::info("Loaded settings");

    
};

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID         result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}

void Settings::SaveKeySettings(std::string a_setting, int a_keyCode)
{
    constexpr auto path     = L"Data/SKSE/Plugins/DynamicActivationKey.ini";
    logger::debug("Load settings for saving");
    auto        keySet = std::to_string(a_keyCode);
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(path);
    ini.SetValue("Keys", a_setting.c_str(), keySet.c_str());
    (void)ini.SaveFile(path);
    logger::debug("saved new settings");
}

void Settings::SetKey(int a_keyCode) {
    DAKModifierKey = a_keyCode;
}

void Settings::SetControllerKey(int a_keyCode) {
    DAKControllerKey = a_keyCode;
}


void Settings::LoadForms() noexcept
{
    auto dataHandler = RE::TESDataHandler::GetSingleton();
    logger::info("Loading forms");
    DAKGlobal = dataHandler->LookupForm(RE::FormID(0x801), DAKPluginName)->As<RE::TESGlobal>();
    logger::debug("Parsed Global Form id successfully, ID is {}", DAKGlobal->GetFormID());

    logger::info("All Forms loaded");

} // Credits: Colinswrath https://github.com/colinswrath/BladeAndBlunt
