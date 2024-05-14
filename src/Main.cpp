#include "InputManager.h"
#include "Logging.h"
#include "Papyrus.h"
#include "SKSE/Interfaces.h"
#include "Settings.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type <=> SKSE::MessagingInterface::kDataLoaded == 0) {
        auto settings = Settings::GetSingleton();
        settings->LoadSettings();
        settings->LoadForms();
        Event::InputEventSink::Register();
        Event::UI::CrossHair::Install();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitializeLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", plugin->GetName(), version);
    Init(skse);

    Cache::CacheAddLibAddresses();
    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener))
        return false;

    auto papyrus = SKSE::GetPapyrusInterface();
    papyrus->Register(Papyrus::Bind);

    logger::info("{} has finished loading.", plugin->GetName());

    return true;
}
