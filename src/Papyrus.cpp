#include "Papyrus.h"
#include "Settings.h"


namespace Papyrus
{

    inline int GetModKeySetting(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, bool forController)
    {
        auto settings = Settings::GetSingleton();
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
            a_hotkey             = settings->DAKModifierKey;
        }        
        logger::debug("key setting is {}", a_hotkey);
        return a_hotkey;
        
    }

    inline void SetModKeySetting(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, int a_keyCode)
    {
        Settings* settings = Settings::GetSingleton();
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

        return true;
    }
}


