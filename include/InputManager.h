#pragma once
#include "CLib/Key.h"
#include "Cache.h"
#include "HotkeyManager.h"
#include "Settings.h"

namespace Event
{
    class InputEventSink final : public Singleton<InputEventSink>, public RE::BSTEventSink<RE::InputEvent*>
    {
        friend class Singleton<InputEventSink>;

    public:
        using Event       = RE::InputEvent*;
        using EventSource = RE::BSTEventSource<Event>;

        inline static bool key_active{ false };

        static void Register()
        {
            if (auto manager = RE::BSInputDeviceManager::GetSingleton()) {
                manager->AddEventSink(GetSingleton());
                SKSE::log::info("Successfully registered input event.");
            }
            else {
                SKSE::log::error("Failed to register input event.");
            }
        };

        static void ChangeGlobal(RE::TESGlobal* global_toChange, int new_value)
        {
            global_toChange->value = new_value;
            logger::debug("changed Global {} to {}", global_toChange->GetFormEditorID(), new_value);
        };

        inline std::uint32_t RemapKey(std::uint32_t a_key, RE::INPUT_DEVICE a_device)
        {
            switch (a_device) {
            case RE::INPUT_DEVICE::kKeyboard:
                return a_key;
            case RE::INPUT_DEVICE::kMouse:
                return a_key + SKSE::InputMap::kMacro_MouseButtonOffset;
            case RE::INPUT_DEVICE::kGamepad:
                return SKSE::InputMap::GamepadMaskToKeycode(a_key);
            default:
                return a_key;
            }
        }

        static bool IsCorrectKey(uint32_t compare_key)
        {
            Settings* settings = Settings::GetSingleton();

            if (compare_key == settings->DAKControllerKey) {
                return true;
            }
            else if (compare_key == settings->DAKModifierKey) {
                return true;
            }
            else {
                return false;
            }
        }

        static void UpdateHUD()
        {
            SKSE::GetTaskInterface()->AddTask([]() { Cache::GetPlayerSingleton()->UpdateCrosshairs(); });
        }

    public:
        RE::BSEventNotifyControl ProcessEvent(const Event* a_event, [[maybe_unused]] EventSource*)
        {
            if (!a_event || !RE::Main::GetSingleton()->gameActive) {
                return RE::BSEventNotifyControl::kContinue;
            }

            HotkeyManager::Process(a_event);

            return RE::BSEventNotifyControl::kContinue;
        }

    private:
        InputEventSink() = default;
    };

    namespace UI
    {

        namespace CrossHair
        {
            enum : std::uint32_t
            {
                kPrompt,
                kName,
                kTag
            };

            struct detail
            {
                static bool has_gold_value(const RE::TESObjectREFRPtr& a_object)
                {
                    const auto base = a_object ? a_object->GetBaseObject() : nullptr;
                    return base && base->GetGoldValue() > 0;
                }

                static bool has_key(const RE::TESObjectREFRPtr& a_object)
                {
                    const auto lock = a_object ? a_object->GetLock() : nullptr;
                    return lock && lock->key;
                }
            };

            struct SendHUDMessage
            {
                static void thunk(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
                {
                    const auto           data         = a_data ? static_cast<RE::HUDData*>(a_data) : nullptr;
                    const auto           crossHairRef = data ? data->crossHairRef.get() : RE::TESObjectREFRPtr();
                    RE::PlayerCharacter* player       = Cache::GetPlayerSingleton();

                    if (data && crossHairRef) {
                        const auto settings = Settings::GetSingleton();
                        if (settings->activate_key_locking) {
                            if (crossHairRef.get()->GetBaseObject()->formType == RE::FormType::Door || crossHairRef.get()->GetBaseObject()->formType == RE::FormType::Container) {
                                logger::debug("[CROSSHAIR] crosshair Ref active");
                                if (crossHairRef.get()->GetLock() != nullptr) {
                                    logger::debug("[CROSSHAIR] has a lock");
                                    if (auto the_key = crossHairRef.get()->GetLock()->key; the_key != nullptr && player->GetItemCount(the_key) > 0) {
                                        logger::debug("[CROSSHAIR] found key");
                                        if (settings->DAKGlobal->value == 1) {
                                            logger::debug("[CROSSHAIR] found DAK status ");
                                            settings->DAKLock->value = 1;
                                            logger::debug("[CROSSHAIR] changed Global {} to {}", settings->DAKLock->GetFormEditorID(), settings->DAKLock->value);
                                            SKSE::GetTaskInterface()->AddTask([]() { Cache::GetPlayerSingleton()->UpdateCrosshairs(); });
                                        }
                                        else
                                            settings->DAKLock->value = 0;
                                    }
                                    else
                                        settings->DAKLock->value = 0;
                                }
                            }
                        }
                    }
                    func(a_this, a_menuName, a_type, a_data);
                }

                inline static REL::Relocation<decltype(thunk)> func;
            };

            static void Install()
            {
                REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(39535, 40621), REL::Relocate(0x289, 0x280) };
                stl::write_thunk_call<SendHUDMessage>(target.address());
            }
        } // namespace CrossHair
    }     // namespace UI

} // namespace Event
