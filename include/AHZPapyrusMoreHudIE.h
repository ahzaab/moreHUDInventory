
#pragma once

#include "PCH.h"
#include <string.h>
#include <map>

//using namespace std;

class PapyrusMoreHudIE
{
	public:
		static bool   RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm);
		static bool   HasForm(std::string iconName, uint32_t formId);
		static std::string GetIconName(uint32_t itemID);
        static std::vector<std::string_view> GetFormIcons(RE::FormID formId);
        
private:
    static uint32_t GetVersion(RE::StaticFunctionTag* base);
    static uint32_t GetFormItemId([[maybe_unused]] RE::StaticFunctionTag* base, RE::TESForm* form);
    static bool     IsIconItemRegistered(RE::StaticFunctionTag* base, uint32_t);
    static void     AddIconItem(RE::StaticFunctionTag* base, uint32_t itemID, RE::BSFixedString iconName);
    static void     RemoveIconItem(RE::StaticFunctionTag* base, uint32_t itemID);
    static void     AddIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs, std::vector<RE::BSFixedString> iconNames);
    static void     RemoveIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs);
    static void     RegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName, RE::BGSListForm* list);
    static void     UnRegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName);
    static bool     IsIconFormListRegistered(RE::StaticFunctionTag* base, RE::BSFixedString iconName);
    static bool     IsIconFormListRegistered_Internal(std::string iconName);
};
