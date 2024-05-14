#include "Papyrus.h"
#include "Settings.h"

namespace Papyrus
{

    inline int GetModKeySetting(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, bool forController)
    {
        auto           settings = Settings::GetSingleton();
        int            a_hotkey;
        constexpr auto path = L"Data/SKSE/Plugins/DynamicActivationKey.ini";
        logger::info("temp load settings");
        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(path);
        if (forController) {
            settings->DAKControllerKey = std::stoi(ini.GetValue("Keys", "iDAKControllerKey", "274"));
            a_hotkey                   = settings->DAKControllerKey;
        }
        else {
            settings->DAKModifierKey = std::stoi(ini.GetValue("Keys", "iDAKModifierKey", "42"));
            a_hotkey                 = settings->DAKModifierKey;
        }
        logger::debug("key setting is {}", a_hotkey);
        return a_hotkey;
    }

    inline void SetModKeySetting(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, int a_keyCode)
    {
        Settings*   settings       = Settings::GetSingleton();
        std::string keySettingName = "iDAKModifierKey";
        settings->SetKey(a_keyCode);
        settings->SaveKeySettings(keySettingName, a_keyCode);
        logger::debug("Saved new key setting as {}", settings->DAKModifierKey);
    }

    inline void SetModControllerKeySetting(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, int a_keyCode)
    {
        Settings*   settings       = Settings::GetSingleton();
        std::string keySettingName = "iDAKControllerKey";
        settings->SetControllerKey(a_keyCode);
        settings->SaveKeySettings(keySettingName, a_keyCode);
        logger::debug("Saved new key setting as {}", settings->DAKControllerKey);
    }

    inline bool GetLockSetting(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*)
    {
        auto           settings = Settings::GetSingleton();
        bool           a_option;
        constexpr auto path = L"Data/SKSE/Plugins/DynamicActivationKey.ini";
        logger::info("temp load settings");
        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(path);
        settings->activate_key_locking = ini.GetBoolValue("Settings", "bActivateLocking");
        a_option                       = settings->activate_key_locking;
        return a_option;
    }

    inline void SetLockSettingBool(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, bool lock_option)
    {
        Settings* settings = Settings::GetSingleton();
        settings->ChangeLockOption(lock_option);
        settings->SaveLockOption(lock_option);
        logger::debug("changed lock enable setting");
    }

    bool Bind(VM* a_vm)
    {
        if (!a_vm) {
            logger::critical("couldn't get VM State"sv);
            return false;
        }

        logger::info("Binding functions..."sv);
        BIND(GetModKeySetting, true);
        logger::info("Registered GetModKeySetting"sv);
        BIND(SetModKeySetting);
        logger::info("Registered SetModKeySetting"sv);
        BIND(SetModControllerKeySetting);
        logger::info("Registered SetModControllerKeySetting"sv);
        BIND(GetLockSetting);
        logger::info("Registered GetLockSetting"sv);
        BIND(SetLockSettingBool);
        logger::info("Registered SetLockSettingBool"sv);

        return true;
    }
} // namespace Papyrus
