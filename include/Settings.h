#pragma once

class Settings : public Singleton<Settings>
{
public:
    void LoadSettings() noexcept;

    void LoadForms() noexcept;

    std::string            FileName;
    inline static uint32_t high_key;
    RE::FormID             HighStanceSpellFormID;
    RE::SpellItem*         HighStanceSpell;

    static RE::FormID ParseFormID(const std::string& str);

    inline static bool debug_logging{};
};
