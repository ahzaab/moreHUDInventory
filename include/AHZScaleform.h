#pragma once
#include "AHZConfiguration.h"
#include "AHZPapyrusMoreHudIE.h"

struct CompletionistRequest
{
   RE::FormID m_formId;
};
struct CompletionistResponse {
   RE::FormID m_formID;
   bool m_icontype; // false = New, true = Found
   bool m_display;
};  

class CAHZScaleform
{
public:
   static CAHZScaleform& Singleton()
   {
      static CAHZScaleform theInstance;
      return theInstance;
   };

   CAHZScaleform(CAHZScaleform& other) = delete;
   void operator=(const CAHZScaleform&) = delete;

   void ExtendItemCard(RE::GFxMovieView * view, RE::GFxValue * object, RE::InventoryEntryData * item);
   void Initialize();
   bool m_showBookRead;
   bool m_enableItemCardResize;
   bool GetWasBookRead(RE::TESForm *form);
   bool isSurvivalMode();
   std::optional<CompletionistResponse> m_completionistResponse{std::nullopt};
   bool m_completionistInstalled{};
   
private:
   CAHZScaleform();
   static void ReplaceStringInPlace(std::string& subject, const std::string& search,
      const std::string& replace);
   auto MagicDisallowEnchanting(RE::BGSKeywordForm* pKeywords) -> bool;
   void RegisterString(RE::GFxValue * dst, const char * name, const char * str);
   void RegisterNumber(RE::GFxValue * dst, const char * name, double value);
   void RegisterBoolean(RE::GFxValue * dst, const char * name, bool value);
   double mRound(double d);
   std::string GetBookSkill(RE::TESForm *form);
   uint32_t GetIsKnownEnchantment(RE::InventoryEntryData * item);
   uint32_t GetIsKnownEnchantment_Impl(RE::InventoryEntryData * item);

   bool m_showBookSkill;
   bool m_showKnownEnchantment;
   bool m_showPosNegEffects;
};

namespace Scaleform
{
   void RegisterListener();
}

//extern CAHZScaleform g_ahzScaleform;
