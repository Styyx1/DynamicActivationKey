#include "HotkeyManager.h"
#include "CLib/Key.h"
#include "InputManager.h"
#include "Settings.h"
namespace
{
    class HotkeyContext
    {
    public:

       inline static std::string logBool(bool &bool_to_log)
        {
            if (bool_to_log)
                return "true";
            if (!bool_to_log)
                return "false";
        }


        explicit HotkeyContext(const Settings* config) : hotkey(config->DAKModifierKey), hotkeyController(config->DAKControllerKey) {}

        void Update(const RE::ButtonEvent* a_button)
        {
            if (!a_button->HasIDCode()) {
                return;
            }

            if (a_button->IsPressed()) {
                auto key = CLib::ParseKey(a_button->GetIDCode(), a_button->GetDevice());

                hotkey.UpdateDown(key);
                hotkeyController.Update(key);

                if (a_button->IsDown()) {
                    hotkey.UpdateDown(key);
                    hotkeyController.Update(key);
                }                
            }
        }

        void Finalize(Event::InputEventSink* app)
        {
            bool            wasChanged       = false;
            const Settings* settings         = Settings::GetSingleton();
            auto            dak_global_value = settings->DAKGlobal->value;
            for (std::uint32_t count = 2; count > 0; --count) {
                bool done = false;

                if (hotkey.IsActive()) {
                    if (dak_global_value != 1 && !wasChanged) {
                        app->ChangeGlobal(settings->DAKGlobal, 1.0);
                        wasChanged = true;
                        logger::debug("bool was changed is {}", logBool(wasChanged));
                        if (wasChanged) {
                            if (Cache::game_version == SKSE::RUNTIME_SSE_LATEST_AE) {
                                app->UpdateHUD();
                                logger::debug("HUD changed");
                            }
                            else
                                logger::debug("no HUD updated");                           
                        }                        
                        done = true;
                    }
                }

                if (hotkeyController.IsActive()) {
                    if (dak_global_value != 1 && !wasChanged) {
                        app->ChangeGlobal(settings->DAKGlobal, 1.0);
                        wasChanged = true;
                        logger::debug("bool was changed is {}", logBool(wasChanged));
                        if (wasChanged) {
                            if (Cache::game_version == SKSE::RUNTIME_SSE_LATEST_AE) {
                                app->UpdateHUD();
                                logger::debug("HUD changed");
                            }
                            else
                                logger::debug("no HUD updated");
                        }
                        done = true;
                    }
                }
                else {
                    if (!hotkey.IsActive() && !hotkeyController.IsActive()) {
                        if (dak_global_value != 0) {
                            app->ChangeGlobal(settings->DAKGlobal, 0.0);
                            wasChanged = true;
                            logger::debug("bool was changed is {}", logBool(wasChanged));
                            if (wasChanged) {
                                if (Cache::game_version == SKSE::RUNTIME_SSE_LATEST_AE) {
                                    app->UpdateHUD();
                                    logger::debug("HUD changed");
                                }
                                else
                                    logger::debug("no HUD updated");
                            }                            
                            done = true;
                        }
                    }
                }
                if (done) {
                    break;
                }
            }
        }

    private:
        CLib::KeyCombo hotkey;
        CLib::Key      hotkeyController;
    };
} // namespace

void HotkeyManager::Process(const RE::InputEvent* const* a_event)
{
    auto config = Settings::GetSingleton();

    HotkeyContext ctx{ config };

    for (auto event = *a_event; event; event = event->next) {
        if (auto button = event->AsButtonEvent()) {
            ctx.Update(button);
        }
    }

    auto app = Event::InputEventSink::GetSingleton();

    ctx.Finalize(app);
}
