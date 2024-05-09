#pragma once
#include "Cache.h"
#include "Settings.h"

namespace Event
{
    class InputEventSink final : public Singleton<InputEventSink>, public RE::BSTEventSink<RE::InputEvent*>
    {
        friend class Singleton<InputEventSink>;

    public:
        using Event       = RE::InputEvent*;
        using EventSource = RE::BSTEventSource<Event>;

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

        static bool IsCorrectKey(uint32_t compare_key)
        {
            Settings* settings = Settings::GetSingleton();
            if (compare_key == settings->DAKModifierKey || compare_key == settings->DAKControllerKey) {
                return true;
            }
            else
                return false;
        }

    public:
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr, RE::BSTEventSource<RE::InputEvent*>*)
        {
            if (!eventPtr)
                return RE::BSEventNotifyControl::kContinue;

            if (RE::PlayerCharacter* player = Cache::GetPlayerSingleton(); !player || !player->Is3DLoaded()) {
                return RE::BSEventNotifyControl::kContinue;
            }

            for (RE::InputEvent* evnt = *eventPtr; evnt; evnt = evnt->next) {
                switch (evnt->eventType.get()) {
                case RE::INPUT_EVENT_TYPE::kButton:
                    Settings*        settings = Settings::GetSingleton();
                    RE::ButtonEvent* a_event  = evnt->AsButtonEvent();
                    bool             held     = a_event->IsHeld();
                    uint32_t         mask     = a_event->idCode;
                    uint32_t         key_code;

                    if (a_event->GetDevice() == RE::INPUT_DEVICE::kMouse) {
                        key_code = SKSE::InputMap::kMacro_NumKeyboardKeys + mask;
                    }

                    else if (a_event->GetDevice() == RE::INPUT_DEVICE::kGamepad) {
                        key_code = SKSE::InputMap::GamepadMaskToKeycode(mask);
                    }

                    else
                        key_code = mask;

                    if (key_code >= SKSE::InputMap::kMaxMacros)
                        continue;

                    if (IsCorrectKey(key_code) && held) {
                        if (settings->DAKGlobal->value != 1) {
                            settings->DAKGlobal->value = 1;
                            logger::debug("changed Global {} to {}", settings->DAKGlobal->GetFormEditorID(), settings->DAKGlobal->value);
                            SKSE::GetTaskInterface()->AddTask([]() { Cache::GetPlayerSingleton()->UpdateCrosshairs(); });
                        }
                    }
                    else {
                        if (IsCorrectKey(key_code) && !held) {
                            if (settings->DAKGlobal->value != 0) {
                                settings->DAKGlobal->value = 0;
                                logger::debug("changed Global {} back to {}", settings->DAKGlobal->GetFormEditorID(), settings->DAKGlobal->value);
                                SKSE::GetTaskInterface()->AddTask([]() { Cache::GetPlayerSingleton()->UpdateCrosshairs(); });
                            }
                        }
                    }
                }
            }
            return RE::BSEventNotifyControl::kContinue;
        };

    private:
        InputEventSink() = default;
    };

} // namespace Event
