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

        inline static bool key_active{false};
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
            else {
                return false;
            }
                
        }

        

    public:
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr, RE::BSTEventSource<RE::InputEvent*>*)
        {
            if (!eventPtr)
                return RE::BSEventNotifyControl::kContinue;

            if (RE::PlayerCharacter* player = Cache::GetPlayerSingleton(); !player || !player->Is3DLoaded()) {
                return RE::BSEventNotifyControl::kContinue;
            }
            RE::IUIMessageData*  a_data       = nullptr;
            const auto data         = a_data ? static_cast<RE::HUDData*>(a_data) : nullptr;
            const auto crossHairRef = data ? data->crossHairRef.get() : RE::TESObjectREFRPtr();
            RE::PlayerCharacter* player       = Cache::GetPlayerSingleton();

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
                    else if (a_event->GetDevice() == RE::INPUT_DEVICE::kVRTotal) {
                        const auto controlMap = RE::ControlMap::GetSingleton();
                        const auto idCode     = controlMap ? controlMap->GetMappedKey("Sprint", a_event->GetDevice()) : RE::ControlMap::kInvalid;
                        key_code              = idCode;
                    }

                    else
                        key_code = mask;

                    if (key_code >= SKSE::InputMap::kMaxMacros)
                        continue;
                    if (settings->activate_key_locking) {
                        if (data && crossHairRef) {

                        }
                        if (auto objRef = crossHairRef.get(); crossHairRef && (crossHairRef->GetBaseObject()->GetFormType() == RE::FormType::Door || objRef->GetBaseObject()->GetFormType() == RE::FormType::Container))
                        {
                            auto the_key = crossHairRef->GetLock()->key;
                            if (the_key != nullptr) {
                                logger::debug("[ACTIVATE EVENT] key is = {}", the_key->GetName());
                                if (player->GetItemCount(the_key) >= 1) {
                                    logger::debug("[ACTIVATE EVENT] player has {} in their inventory", player->GetItemCount(the_key), the_key->GetName());
                                    if (IsCorrectKey(key_code)) {
                                        logger::debug("[ACTIVATE EVENT] correct key is pressed");
                                        if (settings->DAKLock->value != 1) {
                                            objRef->SetActivationBlocked(true);
                                            settings->DAKLock->value = 1;
                                            logger::debug("[ACTIVATE EVENT] Changed global <{} to {}>", settings->DAKLock->GetFormEditorID(), settings->DAKLock->value);
                                            SKSE::GetTaskInterface()->AddTask([]() { Cache::GetPlayerSingleton()->UpdateCrosshairs(); });
                                        }
                                        else if (settings->DAKLock->value != 0) {
                                            settings->DAKLock->value = 0;
                                        }
                                    }
                                }
                            }
                        }
                    }
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
                static bool has_key(const RE::TESObjectREFRPtr& a_object) {
                    const auto lock = a_object ? a_object->GetLock() : nullptr;
                    return lock && lock->key;
                }

            };

            struct SendHUDMessage
            {
                static void thunk(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
                {
                    const auto data         = a_data ? static_cast<RE::HUDData*>(a_data) : nullptr;
                    const auto crossHairRef = data ? data->crossHairRef.get() : RE::TESObjectREFRPtr();
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
                                        }
                                        else
                                            settings->DAKLock->value = 0;
                                    }
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
    }

} // namespace Event
