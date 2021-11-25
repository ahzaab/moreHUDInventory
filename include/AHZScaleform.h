#pragma once
#include "AHZConfiguration.h"
#include "AHZPapyrusMoreHudIE.h"

//using namespace std;

class CAHZScaleform
{
public:
   CAHZScaleform();
   ~CAHZScaleform();
   static void ExtendItemCard(RE::GFxMovieView * view, RE::GFxValue * object, RE::InventoryEntryData * item);
   static void Initialize();
   static bool m_showBookRead;
   static bool m_enableItemCardResize;
   static bool GetWasBookRead(RE::TESForm *form);
   static bool isSurvivalMode();

private:
   static void ReplaceStringInPlace(std::string& subject, const std::string& search,
      const std::string& replace);
   static auto MagicDisallowEnchanting(RE::BGSKeywordForm* pKeywords) -> bool;
   static void RegisterString(RE::GFxValue * dst, const char * name, const char * str);
   static void RegisterNumber(RE::GFxValue * dst, const char * name, double value);
   static void RegisterBoolean(RE::GFxValue * dst, const char * name, bool value);
   static double mRound(double d);
   static std::string GetBookSkill(RE::TESForm *form);
   static bool GetIsKnownEnchantment(RE::InventoryEntryData * item);
   static uint32_t GetIsKnownEnchantment_Impl(RE::InventoryEntryData * item);

   static bool m_showBookSkill;
   static bool m_showKnownEnchantment;
   static bool m_showPosNegEffects;
};
