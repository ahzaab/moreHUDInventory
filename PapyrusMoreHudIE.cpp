#include "PapyrusMoreHudIE.h"
#include "skse64/PluginManager.h"

#include "skse64_common/skse_version.h"


namespace papyrusMoreHudIE {
	static map<UInt32, string> g_ahzRegisteredIcons;

	UInt32 GetVersion(StaticFunctionTag* base)
	{
		return PLUGIN_VERSION;
	}

	bool IsIconItemRegistered(StaticFunctionTag* base, UInt32 itemID)
	{
		// Create an iterator of map
		std::map<UInt32, string>::iterator it;

		// Find the element with key itemID
		it = g_ahzRegisteredIcons.find(itemID);

		// Check if element exists in map or not
		return (it != g_ahzRegisteredIcons.end());
	}

	void AddIconItem(StaticFunctionTag* base, UInt32 itemID, BSFixedString iconName)
	{
		if (!IsIconItemRegistered(base, itemID))
		{
			g_ahzRegisteredIcons.insert(pair<UInt32, string>(itemID, string(iconName.c_str())));
		}
	}

	void RemoveIconItem(StaticFunctionTag* base, UInt32 itemID)
	{
		if (!IsIconItemRegistered(base, itemID))
		{
			g_ahzRegisteredIcons.erase(itemID);
		}
	}

	void AddIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs, VMArray<BSFixedString> iconNames)
	{
		if (itemIDs.Length() != iconNames.Length())
			return;

		for (UInt32 i = 0; i < itemIDs.Length(); i++)
		{
			UInt32 * itemID = NULL;
			BSFixedString * iconName = NULL;
			itemIDs.Get(itemID, i);
			iconNames.Get(iconName, i);
			if (itemID && iconName)
			{
				AddIconItem(base, *itemID, *iconName);
			}
		}
	}

	void RemoveIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs)
	{
		for (UInt32 i = 0; i < itemIDs.Length(); i++)
		{
			UInt32 * itemID = NULL;
			BSFixedString * iconName = NULL;
			itemIDs.Get(itemID, i);
			if (itemID)
			{
				RemoveIconItem(base, *itemID);
			}
		}
	}
}

#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"

bool papyrusMoreHudIE::RegisterFuncs(VMClassRegistry* registry)
{
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, UInt32>("GetVersion", "AhzMoreHudIE", papyrusMoreHudIE::GetVersion, registry));

	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, UInt32>("IsIconItemRegistered", "AhzMoreHudIE", papyrusMoreHudIE::IsIconItemRegistered, registry));

	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, void, UInt32, BSFixedString>("AddIconItem", "AhzMoreHudIE", papyrusMoreHudIE::AddIconItem, registry));

	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, void, UInt32>("RemoveIconItem", "AhzMoreHudIE", papyrusMoreHudIE::RemoveIconItem, registry));

	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, void, VMArray<UInt32>, VMArray<BSFixedString>>("AddIconItems", "AhzMoreHudIE", papyrusMoreHudIE::AddIconItems, registry));

	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, void, VMArray<UInt32>>("RemoveIconItems", "AhzMoreHudIE", papyrusMoreHudIE::RemoveIconItems, registry));


	registry->SetFunctionFlags("AhzMoreHudIE", "GetVersion", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("AhzMoreHudIE", "IsIconItemRegistered", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("AhzMoreHudIE", "AddIconItem", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("AhzMoreHudIE", "RemoveIconItem", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("AhzMoreHudIE", "AddIconItems", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("AhzMoreHudIE", "RemoveIconItems", VMClassRegistry::kFunctionFlag_NoWait);

	return true;
}