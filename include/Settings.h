#pragma once

class Settings : public Singleton<Settings>
{
public:
    void LoadSettings() noexcept;

    void LoadForms() noexcept;
    std::string            FileName;
    inline static uint32_t DAKModifierKey;
    inline static uint32_t DAKControllerKey;
    RE::TESGlobal*         DAKGlobal;
    RE::TESGlobal*         DAKKeyBind;
    const std::string_view DAKPluginName = "Dynamic Activation Key.esp";

    static RE::FormID ParseFormID(const std::string& str);
    static void       SaveKeySettings(std::string a_setting, int a_keyCode);
    void              SetKey(int a_keyCode);
    void              SetControllerKey(int a_keyCode);

    inline static bool debug_logging{};
};
