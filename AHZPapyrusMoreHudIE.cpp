#include "AHZPapyrusMoreHudIE.h"
#include "skse64/PluginManager.h"

#include "skse64_common/skse_version.h"
#include "common/ICriticalSection.h"

namespace papyrusMoreHudIE {
	typedef std::map<UInt32, BSFixedString> AhzIconItemCache;
	static ICriticalSection	s_iconItemCacheLock;
	static AhzIconItemCache s_ahzRegisteredIcons;

	UInt32 GetVersion(StaticFunctionTag* base)
	{
		return PLUGIN_VERSION;
	}

	bool IsIconItemRegistered(StaticFunctionTag* base, UInt32 itemID)
	{
		bool found = false;
		_MESSAGE("IsIconItemRegistered %d", itemID);
		s_iconItemCacheLock.Enter();
		// Create an iterator of map
		AhzIconItemCache::iterator it;

		// Find the element with key itemID
		it = s_ahzRegisteredIcons.find(itemID);

		// Check if element exists in map or not
		found = (it != s_ahzRegisteredIcons.end());

		s_iconItemCacheLock.Leave();

		return found;
	}

	void AddIconItem(StaticFunctionTag* base, UInt32 itemID, BSFixedString iconName)
	{
		_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		s_iconItemCacheLock.Enter();
		if (!IsIconItemRegistered(base, itemID))
		{
			s_ahzRegisteredIcons.insert(AhzIconItemCache::value_type(itemID, iconName));
		}
		s_iconItemCacheLock.Leave();
	}

	void RemoveIconItem(StaticFunctionTag* base, UInt32 itemID)
	{
		_MESSAGE("RemoveIconItem %d", itemID);
		s_iconItemCacheLock.Enter();
		if (!IsIconItemRegistered(base, itemID))
		{
			s_ahzRegisteredIcons.erase(itemID);
		}
		s_iconItemCacheLock.Leave();
	}

	void AddIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs, VMArray<BSFixedString> iconNames)
	{
		s_iconItemCacheLock.Enter();
		if (itemIDs.Length() != iconNames.Length())
		{
			s_iconItemCacheLock.Leave();
			return;
		}

		for (UInt32 i = 0; i < itemIDs.Length(); i++)
		{
			UInt32 itemID;
			BSFixedString iconName;
			itemIDs.Get(&itemID, i);
			iconNames.Get(&iconName, i);
			AddIconItem(base, itemID, iconName);
		}
		s_iconItemCacheLock.Leave();
	}

	void RemoveIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs)
	{
		s_iconItemCacheLock.Enter();
		for (UInt32 i = 0; i < itemIDs.Length(); i++)
		{
			UInt32 itemID;
			itemIDs.Get(&itemID, i);
			if (itemID)
			{
				RemoveIconItem(base, itemID);
			}
		}
		s_iconItemCacheLock.Leave();
	}

	string GetIconName(UInt32 itemID)
	{
		string iconName("");
		s_iconItemCacheLock.Enter();

		if (IsIconItemRegistered(NULL, itemID))
		{
			iconName.append(s_ahzRegisteredIcons[itemID].c_str());
		}

		s_iconItemCacheLock.Leave();

		return iconName;
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