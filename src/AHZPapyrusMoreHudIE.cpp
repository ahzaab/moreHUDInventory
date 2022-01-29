#include "PCH.h"
#include "AHZPapyrusMoreHudIE.h"
#include "version.h"
#include <mutex>
#include "HashUtil.h"

using AhzIconItemCache = std::map<uint32_t, RE::BSFixedString>;
using AhzIconFormListCache = std::map<std::string, RE::BGSListForm*>;
static AhzIconItemCache     s_ahzRegisteredIcons;
static AhzIconFormListCache s_ahzRegisteredIconFormLists;
static std::recursive_mutex mtx;

auto PapyrusMoreHudIE::GetVersion([[maybe_unused]] RE::StaticFunctionTag* base) -> uint32_t
{
    auto version = Version::ASINT;
    logger::trace("GetVersion: {}", version);
    return version;
}

auto PapyrusMoreHudIE::GetFormItemId([[maybe_unused]] RE::StaticFunctionTag* base, RE::TESForm* form) -> uint32_t
{
    if (!form)
    {
        return 0;
    }
    auto crc = SKSE::HashUtil::CRC32(form->GetName() , form->formID & 0x00FFFFFF);
    return crc;
}

void PapyrusMoreHudIE::RegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName, RE::BGSListForm* list)
{
    logger::trace("RegisterIconFormList: {}...", iconName.data());
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (!list)
        return;

    if (!IsIconFormListRegistered(base, iconName)) {
        logger::trace("RegisterIconFormList: {}", iconName.data());
        s_ahzRegisteredIconFormLists.emplace(AhzIconFormListCache::value_type(iconName.c_str(), list));
    }
}

void PapyrusMoreHudIE::UnRegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName)
{
    logger::trace("UnRegisterIconFormList");
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (IsIconFormListRegistered(base, iconName)) {
        s_ahzRegisteredIconFormLists.erase(iconName.c_str());
    }
}

auto PapyrusMoreHudIE::IsIconFormListRegistered_Internal(std::string iconName) -> bool
{
    logger::trace("IsIconFormListRegistered_Internal");
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (s_ahzRegisteredIconFormLists.empty())
        return false;

    // Check if element exists in map or not
    return (s_ahzRegisteredIconFormLists.contains(iconName));
}

auto PapyrusMoreHudIE::IsIconFormListRegistered([[maybe_unused]] RE::StaticFunctionTag* base, RE::BSFixedString iconName) -> bool
{
    return IsIconFormListRegistered_Internal(iconName.c_str());
}

std::vector<std::string_view> PapyrusMoreHudIE::GetFormIcons(RE::FormID formId)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::vector<std::string_view> results;
    for (auto& kvp: s_ahzRegisteredIconFormLists)
    {
        auto list = s_ahzRegisteredIconFormLists[kvp.first];

        if (list && list->HasForm(formId))   
        {
            results.emplace_back(kvp.first);
        }
    }
    return results;
}

auto PapyrusMoreHudIE::HasForm(std::string iconName, uint32_t formId) -> bool
{
    logger::trace("HasForm");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (IsIconFormListRegistered_Internal(iconName)) {
        auto formList = s_ahzRegisteredIconFormLists[iconName];

        if (!formId)
            return false;

        auto formFromId = RE::TESForm::LookupByID(formId);

        if (!formFromId)
            return false;

        return formList->HasForm(formFromId);
    }
    return false;
}

auto PapyrusMoreHudIE::IsIconItemRegistered([[maybe_unused]] RE::StaticFunctionTag* base, uint32_t itemID) -> bool
{
    logger::trace("IsIconItemRegistered");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    // Create an iterator of map
    AhzIconItemCache::iterator it;

    // Find the element with key itemID
    it = s_ahzRegisteredIcons.find(itemID);

    // Check if element exists in map or not
    return (it != s_ahzRegisteredIcons.end());
}


void PapyrusMoreHudIE::AddIconItem(RE::StaticFunctionTag* base, uint32_t itemID, RE::BSFixedString iconName)
{
    logger::trace("AddIconItem");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!IsIconItemRegistered(base, itemID)) {
        s_ahzRegisteredIcons.insert(AhzIconItemCache::value_type(itemID, iconName));
    }
}

void PapyrusMoreHudIE::RemoveIconItem(RE::StaticFunctionTag* base, uint32_t itemID)
{
    logger::trace("RemoveIconItem");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (IsIconItemRegistered(base, itemID)) {
        s_ahzRegisteredIcons.erase(itemID);
    }
}

void PapyrusMoreHudIE::AddIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs, std::vector<RE::BSFixedString> iconNames)
{
    logger::trace("AddIconItems");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (itemIDs.size() != iconNames.size()) {
        return;
    }

    for (uint32_t i = 0; i < itemIDs.size(); i++) {
        uint32_t          itemID;
        RE::BSFixedString iconName;
        itemID = itemIDs[i];
        iconName = iconNames[i];
        AddIconItem(base, itemID, iconName);
    }
}

void PapyrusMoreHudIE::RemoveIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs)
{
    logger::trace("RemoveIconItem");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (uint32_t i = 0; i < itemIDs.size(); i++) {
        uint32_t itemID;
        itemID = itemIDs[i];
        if (itemID) {
            RemoveIconItem(base, itemID);
        }
    }
}

auto PapyrusMoreHudIE::GetIconName(uint32_t itemID) -> std::string
{
    logger::trace("GetIconName");
    std::string                           iconName("");
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (IsIconItemRegistered(nullptr, itemID)) {
        iconName.append(s_ahzRegisteredIcons[itemID].c_str());
    }

    return iconName;
}

auto PapyrusMoreHudIE::RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm) -> bool
{
    a_vm->RegisterFunction("GetVersion", "AhzMoreHudIe", GetVersion);
    a_vm->RegisterFunction("IsIconItemRegistered", "AhzMoreHudIe", IsIconItemRegistered);
    a_vm->RegisterFunction("AddIconItem", "AhzMoreHudIe", AddIconItem);
    a_vm->RegisterFunction("RemoveIconItem", "AhzMoreHudIe", RemoveIconItem);
    a_vm->RegisterFunction("AddIconItems", "AhzMoreHudIe", AddIconItems);
    a_vm->RegisterFunction("RemoveIconItems", "AhzMoreHudIe", RemoveIconItems);
    a_vm->RegisterFunction("RegisterIconFormList", "AhzMoreHudIe", RegisterIconFormList);
    a_vm->RegisterFunction("UnRegisterIconFormList", "AhzMoreHudIe", UnRegisterIconFormList);
    a_vm->RegisterFunction("IsIconFormListRegistered", "AhzMoreHudIe", IsIconFormListRegistered);
    a_vm->RegisterFunction("GetFormItemId", "AhzMoreHudIe", GetFormItemId);
    
    return true;
}