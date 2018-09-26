#include "AHZScaleform.h"


bool m_showBookRead;
bool m_showBookSkill;
bool m_showKnownEnchantment;
bool m_showPosNegEffects;
static TESGlobal *g_survivalModeEnabled = NULL;

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



void CAHZScaleform::ExtendItemCard(GFxMovieView * view, GFxValue * object, InventoryEntryData * item)
{
	if (!item || !object || !view || !item->type)
	{
		return;
	}


	GFxValue obj;
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
			AlchemyItem *alchItem = DYNAMIC_CAST(item->type, TESForm, AlchemyItem);
			// Check the extra data for enchantments learned by the player
			if (item->extendDataList && alchItem)
			{
				for (ExtendDataList::Iterator it = item->extendDataList->Begin(); !it.End(); ++it)
				{
					BaseExtraList * pExtraDataList = it.Get();

					// Search extra data for player created poisons
					if (pExtraDataList)
					{
						if (pExtraDataList->HasType(kExtraData_Poison))
						{
							if (ExtraPoison* extraPoison = static_cast<ExtraPoison*>(pExtraDataList->GetByType(kExtraData_Poison)))
							{
								alchItem = extraPoison->poison;
							}
						}
					}
				}
			}

			if (alchItem && alchItem->effectItemList.count)
			{
				UInt32 effectCount = alchItem->effectItemList.count;
				UInt32 negEffects = 0;
				UInt32 posEffects = 0;
				bool survivalMode = isSurvivalMode();

				for (int i = 0; i < effectCount; i++)
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
	GFxValue * itemIdMember = NULL;
	object->GetMember("itemId", itemIdMember);
	if (itemIdMember)
	{
		UInt32 itemID = itemIdMember->GetNumber();
		string iconName = papyrusMoreHudIE::GetIconName(itemID);

		if (iconName.length())
		{
			RegisterString(object, "AHZItemIcon", iconName.c_str());
		}
	}
}

bool CAHZScaleform::isSurvivalMode()
{
	if (!g_survivalModeEnabled)
	{
		tArray<TESForm*> temp = DataHandler::GetSingleton()->arrGLOB;
		for (int i = 0; i < temp.count; i++)
		{
			TESGlobal *glob = DYNAMIC_CAST(temp[i], TESForm, TESGlobal);
			if (glob) {
				string globName(glob->unk20.Get());
				if (globName == "Survival_ModeToggle")
				{
					g_survivalModeEnabled = glob;
					break;
				}
			}
		}
	}

	return (g_survivalModeEnabled && g_survivalModeEnabled->unk34);
}

void CAHZScaleform::Initialize()
{
   m_showBookRead = g_ahzConfiguration.GetBooleanValue("General", "bShowBookRead", true);
   m_showBookSkill = g_ahzConfiguration.GetBooleanValue("General", "bShowBookSkill", true);
   m_showKnownEnchantment = g_ahzConfiguration.GetBooleanValue("General", "bShowKnownEnchantment", true);
   m_enableItemCardResize = g_ahzConfiguration.GetBooleanValue("General", "bEnableItemCardResize", true); 
   m_showPosNegEffects = g_ahzConfiguration.GetBooleanValue("General", "bShowPosNegEffects", true); 
}

bool CAHZScaleform::GetWasBookRead(TESForm *form)
{
   if (!form)
      return false;

   if (form->GetFormType() != kFormType_Book)
      return false;

   TESObjectBOOK *item = DYNAMIC_CAST(form, TESForm, TESObjectBOOK);
   if (item && ((item->data.flags & TESObjectBOOK::Data::kType_Read) == TESObjectBOOK::Data::kType_Read))
   {
      return true;
   }
   else
   {
      return false;
   }
}

string CAHZScaleform::GetBookSkill(TESForm * form)
{
   string desc;
   if (form->GetFormType() == kFormType_Book)
   {
      TESObjectBOOK *item = DYNAMIC_CAST(form, TESForm, TESObjectBOOK);

      if (!item)
         return desc;

      // If this is a spell book, then it is not a skill book
      if ((item->data.flags & TESObjectBOOK::Data::kType_Spell) == TESObjectBOOK::Data::kType_Spell)
         return desc;

      if (((item->data.flags & TESObjectBOOK::Data::kType_Skill) == TESObjectBOOK::Data::kType_Skill) &&
         item->data.teaches.skill)
      {
         ActorValueList * avList = ActorValueList::GetSingleton();
         if (avList)
         {
            ActorValueInfo * info = avList->GetActorValue(item->data.teaches.skill);
            if (info)
            {
               TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);
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

bool CAHZScaleform::GetIsKnownEnchantment(InventoryEntryData * item)
{
   bool enchantmentKnown = false;

   if (!item || !item->type)
   {
      return false;
   }

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