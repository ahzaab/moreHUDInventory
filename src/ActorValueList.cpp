#include "PCH.h"
#include "ActorValueList.h"


namespace SKSE
{
    ActorValueList::ActorValueList()
    {
        using AV = RE::ActorValue;
        using AVForm = ActorValueInfoFormID;

        m_actorValueInfo.insert({ AV::kOneHanded, AVForm::kAVOneHanded });
        m_actorValueInfo.insert({ AV::kTwoHanded, AVForm::kAVTwoHanded });
        m_actorValueInfo.insert({ AV::kArchery, AVForm::kAVMarksman });
        m_actorValueInfo.insert({ AV::kBlock, AVForm::kAVBlock });
        m_actorValueInfo.insert({ AV::kSmithing, AVForm::kAVSmithing });
        m_actorValueInfo.insert({ AV::kHeavyArmor, AVForm::kAVHeavyArmor });
        m_actorValueInfo.insert({ AV::kLightArmor, AVForm::kAVLightArmor });
        m_actorValueInfo.insert({ AV::kPickpocket, AVForm::kAVPickpocket });
        m_actorValueInfo.insert({ AV::kLockpicking, AVForm::kAVLockpicking });
        m_actorValueInfo.insert({ AV::kSneak, AVForm::kAVSneak });
        m_actorValueInfo.insert({ AV::kAlchemy, AVForm::kAVAlchemy });
        m_actorValueInfo.insert({ AV::kSpeech, AVForm::kAVSpeechCraft });
        m_actorValueInfo.insert({ AV::kAlteration, AVForm::kAVAlteration });
        m_actorValueInfo.insert({ AV::kConjuration, AVForm::kAVConjuration });
        m_actorValueInfo.insert({ AV::kDestruction, AVForm::kAVDestruction });
        m_actorValueInfo.insert({ AV::kIllusion, AVForm::kAVMysticism });
        m_actorValueInfo.insert({ AV::kRestoration, AVForm::kAVRestoration });
        m_actorValueInfo.insert({ AV::kEnchanting, AVForm::kAVEnchanting });
        m_actorValueInfo.insert({ AV::kMagickaRateMult, AVForm::kAVMagickaRateMod });
        m_actorValueInfo.insert({ AV::kHealRateMult, AVForm::kAVHealRatePowerMod });
        m_actorValueInfo.insert({ AV::kFavorActive, AVForm::kAVFavorActive });
        m_actorValueInfo.insert({ AV::kFavorsPerDay, AVForm::kAVFavorsPerDay });
        m_actorValueInfo.insert({ AV::kFavorsPerDayTimer, AVForm::kAVFavorsPerDayTimer });
        m_actorValueInfo.insert({ AV::kWaitingForPlayer, AVForm::kAVWaitingForPlayer });
        m_actorValueInfo.insert({ AV::kLastBribedIntimidated, AVForm::kAVLastBribedIntimidated });
        m_actorValueInfo.insert({ AV::kLastFlattered, AVForm::kAVLastFlattered });
        m_actorValueInfo.insert({ AV::kFame, AVForm::kAVFame });
        m_actorValueInfo.insert({ AV::kInfamy, AVForm::kAVInfamy });
        m_actorValueInfo.insert({ AV::kAlchemyModifier, AVForm::kAVAlchemyMod });
        m_actorValueInfo.insert({ AV::kEnchantingModifier, AVForm::kAVEnchantingMod });
        m_actorValueInfo.insert({ AV::kLightArmorModifier, AVForm::kAVLightArmorMod });
        m_actorValueInfo.insert({ AV::kSneakingModifier, AVForm::kAVSneakMod });
        m_actorValueInfo.insert({ AV::kHeavyArmorModifier, AVForm::kAVHeavyArmorMod });
        m_actorValueInfo.insert({ AV::kSmithingModifier, AVForm::kAVSmithingMod });
    }

    auto ActorValueList::GetSingleton() -> ActorValueList*
    {
        //using func_t = decltype(&ActorValueList::GetSingleton);
        //REL::Relocation<func_t> func{ REL::ID(514139) };
        static ActorValueList instance;
        return std::addressof(instance);
    }

    auto ActorValueList::GetActorValue(RE::ActorValue id) -> RE::ActorValueInfo*
    {
        if (id >= RE::ActorValue::kTotal) {
            return nullptr;
        }

        assert(m_actorValueInfo.find(id) != m_actorValueInfo.end());

        auto avInfo = m_actorValueInfo[id];

        auto dataHandler = RE::BGSDefaultObjectManager::GetSingleton();

        //auto form = dataHandler->LookupByID(static_cast<RE::FormID>(avInfo));

        //assert(form);
        //assert(form->GetFormType() == RE::FormType::ActorValueInfo);

        return static_cast<RE::ActorValueInfo*>(dataHandler->LookupByID(static_cast<RE::FormID>(avInfo)));
    }
}
