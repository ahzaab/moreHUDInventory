#pragma once
#include "skse64/GameData.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "AHZConfiguration.h"

using namespace std;

class CAHZScaleform
{
public:
   CAHZScaleform();
   ~CAHZScaleform();
   void ExtendItemCard(GFxMovieView * view, GFxValue * object, InventoryEntryData * item);
   void Initialize();
   bool m_showBookRead;
   bool m_enableItemCardResize;
   bool CAHZScaleform::GetWasBookRead(TESForm *form);

private:
   static void ReplaceStringInPlace(std::string& subject, const std::string& search,
      const std::string& replace);

   static void RegisterString(GFxValue * dst, const char * name, const char * str);
   static void RegisterNumber(GFxValue * dst, const char * name, double value);
   static void RegisterBoolean(GFxValue * dst, const char * name, bool value);
   static double mRound(double d);
   string GetBookSkill(TESForm *form);
   bool GetIsKnownEnchantment(InventoryEntryData * item);

   bool m_showBookSkill;
   bool m_showKnownEnchantment;
};
