#include "PCH.h"

#include "AHZScaleform.h"
#include "HashUtil.h"

bool m_showBookRead;
bool m_showBookSkill;
bool m_showKnownEnchantment;
bool m_showPosNegEffects;

double CAHZScaleform::mRound(double r)
{
   return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

CAHZScaleform::CAHZScaleform()
{
}

CAHZScaleform::~CAHZScaleform()
{
}



void CAHZScaleform::ExtendItemCard(RE::GFxMovieView * view, RE::GFxValue * object, RE::InventoryEntryData * item)
{
	if (!item || !object || !view || !item->type)
	{
		return;
	}


	RE::GFxValue obj;
	view->CreateObject(&obj);

	if (item->type->GetFormType() == kFormType_Armor || item->type->GetFormType() == kFormType_Weapon && m_showKnownEnchantment)
	{
		RegisterBoolean(&obj, "enchantmentKnown", GetIsKnownEnchantment(item));
		// Add the object to the scaleform function
		object->SetMember("AHZItemCardObj", &obj);
	}
	else if (item->type->GetFormType() == kFormType_Book)
	{
		if (m_showBookSkill)
		{
			string bookSkill = GetBookSkill(item->type);
			if (bookSkill.length())
			{
				RegisterString(&obj, "bookSkill", bookSkill.c_str());
			}
		}

		// Add the object to the scaleform function
		object->SetMember("AHZItemCardObj", &obj);
	}
	else if (item->type->GetFormType() == kFormType_Potion)
	{
		if (m_showPosNegEffects)
		{
			auto alchItem = DYNAMIC_CAST(item->type, RE::TESForm, RE::AlchemyItem);
			// Check the extra data for enchantments learned by the player
			if (item->extendDataList && alchItem)
			{
				for (auto it = item->extendDataList->Begin(); !it.End(); ++it)
				{
					auto pExtraDataList = it.Get();

					// Search extra data for player created poisons
					if (pExtraDataList)
					{
						if (pExtraDataList->HasType(kExtraData_Poison))
						{
							if (ExtraPoison* extraPoison = static_cast<RE::ExtraPoison*>(pExtraDataList->GetByType(kExtraData_Poison)))
							{
								alchItem = extraPoison->poison;
							}
						}
					}
				}
			}

			if (alchItem && alchItem->effectItemList.count)
			{
				uint32_t effectCount = alchItem->effectItemList.count;
				uint32_t negEffects = 0;
				uint32_t posEffects = 0;
				bool survivalMode = isSurvivalMode();

				for (auto i = 0; i < effectCount; i++)
				{
					if (alchItem->effectItemList[i]->mgef)
					{
						string effectName = string(alchItem->effectItemList[i]->mgef->description.c_str());
						size_t found = effectName.find("[SURV=");
						bool surVivalDescFound = (found != string::npos);

						if (((alchItem->effectItemList[i]->mgef->properties.flags & EffectSetting::Properties::kEffectType_Detrimental) == EffectSetting::Properties::kEffectType_Detrimental) ||
							((alchItem->effectItemList[i]->mgef->properties.flags & EffectSetting::Properties::kEffectType_Hostile) == EffectSetting::Properties::kEffectType_Hostile))
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
		object->SetMember("AHZItemCardObj", &obj);
	}

	// Static icons
	auto name = item->GetDisplayName();
	auto itemId = std::static_cast<std::sint32_t>(HashUtil::CRC32(name, item->type->formID & 0x00FFFFFF));
	auto iconName = papyrusMoreHudIE::GetIconName(itemId);

	if (iconName.length())
	{
		RegisterString(object, "AHZItemIcon", iconName.c_str());
	}


	RegisterBoolean(object, "AHZdbmNew", papyrusMoreHudIE::HasForm("dbmNew", item->type->formID));
	RegisterBoolean(object, "AHZdbmDisp", papyrusMoreHudIE::HasForm("dbmDisp", item->type->formID));
	RegisterBoolean(object, "AHZdbmFound", papyrusMoreHudIE::HasForm("dbmFound", item->type->formID));

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

bool CAHZScaleform::GetWasBookRead(TESForm *theObject)
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

string CAHZScaleform::GetBookSkill(TESForm * form)
{
    string desc;
    if (theObject->->GetFormType() == RE::FormType::Book) {
        auto item = DYNAMIC_CAST(theObject, RE::TESForm, RE::TESObjectBOOK);

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
					if (fname && fname->name.data)
					{
						desc.append(fname->name.data);
					}
                }
            }
        }
    }
    return desc;
}

bool CAHZScaleform::GetIsKnownEnchantment(RE::InventoryEntryData * item)
{
	return GetIsKnownEnchantment_Impl(item) > 0;
}

uint32_t CAHZScaleform::GetIsKnownEnchantment_Impl(RE::InventoryEntryData * item)
{
   bool enchantmentKnown = false;

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














   if (item->type->GetFormType() == kFormType_Armor || item->type->GetFormType() == kFormType_Weapon)
   {
      EnchantmentItem * enchantment = NULL;
      TESEnchantableForm * enchantable = DYNAMIC_CAST(item->type, TESForm, TESEnchantableForm);

      if (enchantable) { // Check the item for a base enchantment
         enchantment = enchantable->enchantment;
      }

      // Check the extra data for enchantments learned by the player
      if (item->extendDataList && enchantable)
      {
         for (ExtendDataList::Iterator it = item->extendDataList->Begin(); !it.End(); ++it)
         {
            BaseExtraList * pExtraDataList = it.Get();

            if (pExtraDataList)
            {
               if (pExtraDataList->HasType(kExtraData_Enchantment))
               {
                  if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(pExtraDataList->GetByType(kExtraData_Enchantment)))
                  {
                     enchantment = extraEnchant->enchant;

                     // For now, assume true since we have extra enchantment
                     enchantmentKnown = true;
                  }
               }
            }
         }
      }

      if (enchantment)
      {
         if ((enchantment->flags & TESForm::kFlagPlayerKnows) == TESForm::kFlagPlayerKnows) {
            enchantmentKnown = true;
         }

         if (enchantment->data.baseEnchantment)
         {
            if ((enchantment->data.baseEnchantment->flags & TESForm::kFlagPlayerKnows) == TESForm::kFlagPlayerKnows) {
               enchantmentKnown = true;
            }
         }
      }
   }

   return enchantmentKnown;
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

void CAHZScaleform::RegisterString(GFxValue * dst, const char * name, const char * str)
{
   GFxValue	fxValue;
   fxValue.SetString(str);
   dst->SetMember(name, &fxValue);
};

void CAHZScaleform::RegisterNumber(GFxValue * dst, const char * name, double value)
{
   GFxValue	fxValue;
   fxValue.SetNumber(value);
   dst->SetMember(name, &fxValue);
};

void CAHZScaleform::RegisterBoolean(GFxValue * dst, const char * name, bool value)
{
   GFxValue	fxValue;
   fxValue.SetBool(value);
   dst->SetMember(name, &fxValue);
};