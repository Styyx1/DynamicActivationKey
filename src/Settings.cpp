#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\StancesReborn.ini)");

    std::string fileName(ini.GetValue("General", "sModFileName", ""));
    std::string high_stance_spell_ID(ini.GetValue("FormID", "HighStanceSpellFormID", ""));

    high_key = std::stoi(ini.GetValue("Keys", "iHighStanceKey", "257"));

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (!high_stance_spell_ID.empty()) {
        HighStanceSpellFormID = ParseFormID(high_stance_spell_ID);
    }

    if (debug_logging) {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    };
    // Load settings
    FileName = fileName;
    logger::info("Loaded settings");
};

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID         result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}

void Settings::LoadForms() noexcept
{
    auto dataHandler = RE::TESDataHandler::GetSingleton();

    logger::info("Loading forms");
    if (HighStanceSpellFormID)
        HighStanceSpell = skyrim_cast<RE::SpellItem*>(dataHandler->LookupForm(HighStanceSpellFormID, FileName));

    logger::info("All Forms loaded");

} // Credits: Colinswrath https://github.com/colinswrath/BladeAndBlunt
