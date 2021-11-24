#pragma once

namespace SKSE
{
    class ActorValueList
    {
    public:
        inline static constexpr uint32_t kNumActorValues = static_cast<uint32_t>(RE::ActorValue::kTotal);

        static ActorValueList* GetSingleton(void);
        RE::ActorValueInfo*        GetActorValue(RE::ActorValue id);

    private:
        ActorValueList();
        ~ActorValueList() = default;

        enum class ActorValueInfoFormID: RE::FormID
        {
            kAVOneHanded = 0x0000044C,
            kAVTwoHanded = 0x0000044D,
            kAVMarksman = 0x0000044E,
            kAVBlock = 0x0000044F,
            kAVSmithing = 0x00000450,
            kAVHeavyArmor = 0x00000451,
            kAVLightArmor = 0x00000452,
            kAVPickpocket = 0x00000453,
            kAVLockpicking = 0x00000454,
            kAVSneak = 0x00000455,
            kAVAlchemy = 0x00000456,
            kAVSpeechCraft = 0x00000457,
            kAVAlteration = 0x00000458,
            kAVConjuration = 0x00000459,
            kAVDestruction = 0x0000045A,
            kAVMysticism = 0x0000045B,
            kAVRestoration = 0x0000045C,
            kAVEnchanting = 0x0000045D,
            kAVMagickaRateMod = 0x00000646,
            kAVVampire = kAVMagickaRateMod,
            kAVHealRatePowerMod = 0x00000645,
            kAVWerewolf = kAVHealRatePowerMod,
            kAVFavorActive = 0x000005F6,
            kAVFavorsPerDay = 0x000005F7,
            kAVFavorsPerDayTimer = 0x000005F8,
            kAVWaitingForPlayer = 0x00000606,
            kAVLastBribedIntimidated = 0x00000601,
            kAVLastFlattered = 0x00000602,
            kAVFame = 0x000005E3,
            kAVInfamy = 0x000005E4,
            kAVAlchemyMod = 0x00000611,
            kAVEnchantingMod = 0x00000618,
            kAVLightArmorMod = 0x0000060D,
            kAVSneakMod = 0x00000610,
            kAVHeavyArmorMod = 0x0000060C,
            kAVSmithingMod = 0x0000060B
        };

        std::map<RE::ActorValue, ActorValueInfoFormID> m_actorValueInfo;
    };

}