#include "PCH.h"

#include "ActorValueList.h"
#include "AHZScaleform.h"
#include "HashUtil.h"

double CAHZScaleform::mRound(double r)
{
   return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

CAHZScaleform::CAHZScaleform(): 
m_showBookRead(false), 
m_showBookSkill(false),
m_showKnownEnchantment{false}, 
m_showPosNegEffects{false}, 
m_enableItemCardResize(false)
{
}

CAHZScaleform::~CAHZScaleform()
{
}

void CAHZScaleform::ExtendItemCard(RE::GFxMovieView * view, RE::GFxValue * object, RE::InventoryEntryData * item)
{
	if (!item || !object || !view || !item->object)
	{
		return;
	}


	RE::GFxValue obj;
	view->CreateObject(&obj);
    
	if ((item->object->GetFormType() == RE::FormType::Armor || item->object->GetFormType() == RE::FormType::Weapon) && m_showKnownEnchantment)
	{
		RegisterNumber(&obj, "enchantmentKnown", GetIsKnownEnchantment(item));
		// Add the object to the scaleform function
		object->SetMember("AHZItemCardObj", obj);
	}
	else if (item->object->GetFormType() == RE::FormType::Book)
	{
		if (m_showBookSkill)
		{
			std::string bookSkill = GetBookSkill(item->object);
			if (bookSkill.length())
			{
				RegisterString(&obj, "bookSkill", bookSkill.c_str());
			}
		}

		// Add the object to the scaleform function
		object->SetMember("AHZItemCardObj", obj);
	}
	else if (item->object->GetFormType() == RE::FormType::AlchemyItem)
	{
		if (m_showPosNegEffects)
		{
			auto alchItem = DYNAMIC_CAST(item->object, RE::TESForm, RE::AlchemyItem);
			// Check the extra data for enchantments learned by the player
			if (item->extraLists && alchItem)
			{
				for (auto& list : *item->extraLists)
				{
					auto pExtraDataList = list;

					// Search extra data for player created poisons
					if (pExtraDataList)
					{
						if (pExtraDataList->HasType(RE::ExtraDataType::kPoison))
						{
							if (RE::ExtraPoison* extraPoison = static_cast<RE::ExtraPoison*>(pExtraDataList->GetByType(RE::ExtraDataType::kPoison)))
							{
								alchItem = extraPoison->poison;
							}
						}
					}
				}
			}

			if (alchItem && alchItem->effects.size())
			{
				uint32_t negEffects = 0;
				uint32_t posEffects = 0;
				bool survivalMode = isSurvivalMode();

				for (auto& mgef: alchItem->effects)
				{
					if (mgef)
					{
						std::string effectName = std::string(mgef->baseEffect->magicItemDescription.c_str());
						size_t found = effectName.find("[SURV=");
						bool surVivalDescFound = (found != std::string::npos);
                        //RE::EffectSetting::EffectSettingData::Flag::kDetrimental
						if (mgef->baseEffect->data.flags.any(RE::EffectSetting::EffectSettingData::Flag::kDetrimental, RE::EffectSetting::EffectSettingData::Flag::kHostile))
						{
							// Do not include the survival mode effects when not in survival mode
							if (!survivalMode && surVivalDescFound)
							{
								continue;
							}
							negEffects++;
						}
						else
						{
							// Do not include the survival mode effects when not in survival mode
							if (!survivalMode && surVivalDescFound)
							{
								continue;
							}
							posEffects++;
						}
					}
				}

				RegisterNumber(&obj, "PosEffects", posEffects);
				RegisterNumber(&obj, "NegEffects", negEffects);
			}
		}
		// Add the object to the scaleform function
		object->SetMember("AHZItemCardObj", obj);
	}

	// Static icons
	auto name = item->GetDisplayName();
	auto itemId = static_cast<std::int32_t>(SKSE::HashUtil::CRC32(name, item->object->formID & 0x00FFFFFF));
	auto iconName = PapyrusMoreHudIE::GetIconName(itemId);

	if (iconName.length())
	{
		RegisterString(object, "AHZItemIcon", iconName.c_str());
	}

	RegisterBoolean(object, "AHZdbmNew", PapyrusMoreHudIE::HasForm("dbmNew", item->object->formID));
	RegisterBoolean(object, "AHZdbmDisp", PapyrusMoreHudIE::HasForm("dbmDisp", item->object->formID));
	RegisterBoolean(object, "AHZdbmFound", PapyrusMoreHudIE::HasForm("dbmFound", item->object->formID));

    auto customIcons = PapyrusMoreHudIE::GetFormIcons(item->object->formID);

    if (!customIcons.empty()){
        RE::GFxValue          entry;
        RE::GFxValue          customIconArray;
        view->CreateArray(std::addressof(customIconArray));
        customIconArray.SetArraySize(static_cast<uint32_t>(customIcons.size()));
        auto idx = 0;
        for (auto& ci: customIcons)
        {
            entry.SetString(ci);
            customIconArray.SetElement(idx++, entry);
        }  
        object->SetMember("AHZCustomIcons", customIconArray);
    }

}

bool CAHZScaleform::isSurvivalMode()
{
    using TESGlobal = RE::TESGlobal;
    const auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
    const auto survival = dobj ? dobj->GetObject<TESGlobal>(RE::DEFAULT_OBJECT::kSurvivalModeEnabled) : nullptr;
    return survival ? survival->value == 1.0F : false;
}

void CAHZScaleform::Initialize()
{
   m_showBookRead = g_ahzConfiguration.GetBooleanValue("General", "bShowBookRead", true);
   m_showBookSkill = g_ahzConfiguration.GetBooleanValue("General", "bShowBookSkill", true);
   m_showKnownEnchantment = g_ahzConfiguration.GetBooleanValue("General", "bShowKnownEnchantment", true);
   m_enableItemCardResize = g_ahzConfiguration.GetBooleanValue("General", "bEnableItemCardResize", true); 
   m_showPosNegEffects = g_ahzConfiguration.GetBooleanValue("General", "bShowPosNegEffects", true); 
}

bool CAHZScaleform::GetWasBookRead(RE::TESForm *theObject)
{
    if (!theObject)
        return false;

    if (theObject->GetFormType() != RE::FormType::Book)
        return false;

    auto item = DYNAMIC_CAST(theObject, RE::TESForm, RE::TESObjectBOOK);
    if (item && (item->IsRead())) {
        return true;
    } else {
        return false;
    }
}

std::string CAHZScaleform::GetBookSkill(RE::TESForm * form)
{
    std::string desc;
    if (form->GetFormType() == RE::FormType::Book) {
        auto item = DYNAMIC_CAST(form, RE::TESForm, RE::TESObjectBOOK);

        if (!item)
            return desc;

        // If this is a spell book, then it is not a skill book
        if ((item->data.flags & RE::OBJ_BOOK::Flag::kTeachesSpell) == RE::OBJ_BOOK::Flag::kTeachesSpell)
            return desc;

        if (((item->data.flags & RE::OBJ_BOOK::Flag::kAdvancesActorValue) == RE::OBJ_BOOK::Flag::kAdvancesActorValue) &&
            item->data.teaches.actorValueToAdvance != RE::ActorValue::kNone) {
            auto avList = SKSE::ActorValueList::GetSingleton();
            if (avList) {
                auto info = avList->GetActorValue(item->data.teaches.actorValueToAdvance);
                if (info) {
					auto fname = DYNAMIC_CAST(info, RE::ActorValueInfo, RE::TESFullName);
					if (fname && fname->GetFullName())
					{
						desc.append(fname->GetFullName());
					}
                }
            }
        }
    }
    return desc;
}

auto CAHZScaleform::MagicDisallowEnchanting(RE::BGSKeywordForm* pKeywords) -> bool
{
    if (pKeywords) {
        for (uint32_t k = 0; k < pKeywords->numKeywords; k++) {
            if (pKeywords->keywords[k]) {
                auto keyword = pKeywords->GetKeywordAt(k).value_or(nullptr);
                if (keyword) {
                    // Had to add this check because https://www.nexusmods.com/skyrimspecialedition/mods/34175?
                    // sets the editor ID for 'MagicDisallowEnchanting' to null (╯°□°）╯︵ ┻━┻
                    auto   asCstr = keyword->GetFormEditorID();
                    std::string keyWordName = asCstr ? asCstr : "";
                    if (keyWordName == "MagicDisallowEnchanting") {
                        return true;  // Is enchanted, but cannot be enchanted by player
                    }
                }
            }
        }
    }
    return false;
}

uint32_t CAHZScaleform::GetIsKnownEnchantment(RE::InventoryEntryData * item)
{
   if (!item || !item->object)
   {
      return 0;
   }

    //auto pPC = RE::PlayerCharacter::GetSingleton();
    auto baseForm = item->object;

    if ((baseForm) &&
        (baseForm->GetFormType() == RE::FormType::Weapon ||
            baseForm->GetFormType() == RE::FormType::Armor ||
            baseForm->GetFormType() == RE::FormType::Ammo ||
            baseForm->GetFormType() == RE::FormType::Projectile)) {
        RE::EnchantmentItem* enchantment = nullptr;
        auto                 keyWordForm = baseForm->As<RE::BGSKeywordForm>();
        auto                 enchantable = baseForm->As<RE::TESEnchantableForm>();
        if (baseForm->GetFormType() == RE::FormType::Projectile) {
            enchantable = baseForm->As<RE::TESEnchantableForm>();
            keyWordForm = baseForm->As<RE::BGSKeywordForm>();
        }

        bool wasExtra = false;
        if (enchantable) {  // Check the item for a base enchantment
            enchantment = enchantable->formEnchanting;
        }

		if (item->extraLists){
			for (auto& list: *item->extraLists){
				auto extraEnchant = static_cast<RE::ExtraEnchantment*>(list->GetByType(RE::ExtraDataType::kEnchantment));
				if (extraEnchant) {
					wasExtra = true;
					enchantment = extraEnchant->enchantment;
				}
			}
		}

        if (enchantment) {
            if ((enchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                return MagicDisallowEnchanting(enchantment) ? 2 : 1;
            } else if (MagicDisallowEnchanting(enchantment)) {
                return 2;
            }

            auto baseEnchantment = static_cast<RE::EnchantmentItem*>(enchantment->data.baseEnchantment);
            if (baseEnchantment) {
                if ((baseEnchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                    return MagicDisallowEnchanting(baseEnchantment) ? 2 : 1;
                } else if (MagicDisallowEnchanting(baseEnchantment)) {
                    return 2;
                }
            }
        }

        // Its safe to assume that if it not a base enchanted item, that it was enchanted by the player and therefore, they
        // know the enchantment
        if (wasExtra) {
            return 1;
        } else if (enchantable) {
            return MagicDisallowEnchanting(keyWordForm) ? 2 : 0;
        }
    }
    return 0;
}

void CAHZScaleform::ReplaceStringInPlace(std::string& subject, const std::string& search,
   const std::string& replace)
{
   size_t pos = 0;
   while ((pos = subject.find(search, pos)) != std::string::npos)
   {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
   }
};

void CAHZScaleform::RegisterString(RE::GFxValue * dst, const char * name, const char * str)
{
   RE::GFxValue	fxValue;
   fxValue.SetString(str);
   dst->SetMember(name, fxValue);
};

void CAHZScaleform::RegisterNumber(RE::GFxValue * dst, const char * name, double value)
{
   RE::GFxValue	fxValue;
   fxValue.SetNumber(value);
   dst->SetMember(name, fxValue);
};

void CAHZScaleform::RegisterBoolean(RE::GFxValue * dst, const char * name, bool value)
{
   RE::GFxValue	fxValue;
   fxValue.SetBoolean(value);
   dst->SetMember(name, fxValue);
};