#include "AHZScaleform.h"

bool m_showBookRead;
bool m_showBookSkill;
bool m_showKnownEnchantment;


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
}

void CAHZScaleform::Initialize()
{
   m_showBookRead = g_ahzConfiguration.GetBooleanValue("General", "bShowBookRead", true);
   m_showBookSkill = g_ahzConfiguration.GetBooleanValue("General", "bShowBookSkill", true);
   m_showKnownEnchantment = g_ahzConfiguration.GetBooleanValue("General", "bShowKnownEnchantment", true);
   m_enableItemCardResize = g_ahzConfiguration.GetBooleanValue("General", "bEnableItemCardResize", true); 
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