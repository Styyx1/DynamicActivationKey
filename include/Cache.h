#pragma once

namespace Cache
{
    inline uintptr_t PlayerSingletonAddress;

    inline void CacheAddLibAddresses()
    {
        // 1.6 = 403521
        // 1.5 = 517014
        PlayerSingletonAddress = REL::RelocationID(517014, 403521).address();
    }

    inline RE::PlayerCharacter* GetPlayerSingleton()
    {
        REL::Relocation<RE::NiPointer<RE::PlayerCharacter>*> singleton{ PlayerSingletonAddress };
        return singleton->get();
    }

} // namespace Cache
