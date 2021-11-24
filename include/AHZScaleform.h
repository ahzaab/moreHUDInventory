#pragma once
#include "AHZConfiguration.h"
#include "AHZPapyrusMoreHudIE.h"

//using namespace std;

class CAHZScaleform
{
public:
   CAHZScaleform();
   ~CAHZScaleform();
   void ExtendItemCard(RE::GFxMovieView * view, RE::GFxValue * object, RE::InventoryEntryData * item);
   void Initialize();
   bool m_showBookRead;
   bool m_enableItemCardResize;
   bool GetWasBookRead(RE::TESForm *form);
   bool isSurvivalMode();

private:
   static void ReplaceStringInPlace(std::string& subject, const std::string& search,
      const std::string& replace);

   static void RegisterString(RE::GFxValue * dst, const char * name, const char * str);
   static void RegisterNumber(RE::GFxValue * dst, const char * name, double value);
   static void RegisterBoolean(RE::GFxValue * dst, const char * name, bool value);
   static double mRound(double d);
   string GetBookSkill(RE::TESForm *form);
   bool GetIsKnownEnchantment(RE::InventoryEntryData * item);
   uint32_t GetIsKnownEnchantment_Impl(RE::InventoryEntryData * item);

   bool m_showBookSkill;
   bool m_showKnownEnchantment;
   bool m_showPosNegEffects;
};
