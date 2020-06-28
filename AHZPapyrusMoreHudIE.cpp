#include "AHZPapyrusMoreHudIE.h"
#include "skse64/PluginManager.h"

#include "skse64_common/skse_version.h"
#include "common/ICriticalSection.h"
#include <mutex>

namespace papyrusMoreHudIE {

	class FormVisitor: public BGSListForm::Visitor
	{
	public:

		FormVisitor::FormVisitor(TESForm* form) :m_form(form) {}

		virtual bool Accept(TESForm* form)
		{
			if (!m_form || !form)
				return true;

			return !(m_form->formID == form->formID);
		}

	private:
		TESForm* m_form;

	};

	typedef std::map<UInt32, BSFixedString> AhzIconItemCache;
	typedef std::map<std::string, BGSListForm*> AhzIconFormListCache;
	static ICriticalSection	s_iconItemCacheLock;
	static AhzIconItemCache s_ahzRegisteredIcons;
	static AhzIconFormListCache s_ahzRegisteredIconFormLists;
	static std::recursive_mutex mtx;

	UInt32 GetVersion(StaticFunctionTag* base)
	{
		return PLUGIN_VERSION;
	}

	void RegisterIconFormList(StaticFunctionTag* base, BSFixedString iconName, BGSListForm* list)
	{
		//_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		std::lock_guard <recursive_mutex> lock(mtx);
		if (!IsIconFormListRegistered(base, iconName))
		{
			s_ahzRegisteredIconFormLists.insert(AhzIconFormListCache::value_type(iconName.c_str(), list));
		}
	}
	void UnRegisterIconFormList(StaticFunctionTag* base, BSFixedString iconName)
	{
		//_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		std::lock_guard <recursive_mutex> lock(mtx);
		if (IsIconFormListRegistered(base, iconName))
		{
			s_ahzRegisteredIconFormLists.erase(iconName.c_str());
		}
	}

	bool IsIconFormListRegistered(std::string iconName)
	{
		//_MESSAGE("IsIconItemRegistered %d", itemID);
		std::lock_guard <recursive_mutex> lock(mtx);
		// Create an iterator of map
		AhzIconFormListCache::iterator it;

		// Find the element with key itemID
		it = s_ahzRegisteredIconFormLists.find(iconName);

		// Check if element exists in map or not
		return (it != s_ahzRegisteredIconFormLists.end());
	}

	bool IsIconFormListRegistered(StaticFunctionTag* base, BSFixedString iconName)
	{
		return IsIconFormListRegistered(iconName.c_str());
	}

	bool HasForm(std::string iconName, UInt32 formId)
	{
		std::lock_guard <recursive_mutex> lock(mtx);
		if (IsIconFormListRegistered(iconName))
		{
			auto formList = s_ahzRegisteredIconFormLists[iconName];

			if (!formId)
				return false;

			auto formFromId = LookupFormByID(formId);

			if (!formFromId)
				return false;

			FormVisitor visitor(formFromId);
			return formList->Visit(visitor);
		}	
		return false;
	}

	bool IsIconItemRegistered(StaticFunctionTag* base, UInt32 itemID)
	{
		//_MESSAGE("IsIconItemRegistered %d", itemID);
		std::lock_guard <recursive_mutex> lock(mtx);
		// Create an iterator of map
		AhzIconItemCache::iterator it;

		// Find the element with key itemID
		it = s_ahzRegisteredIcons.find(itemID);

		// Check if element exists in map or not
		return (it != s_ahzRegisteredIcons.end());
	}



	void AddIconItem(StaticFunctionTag* base, UInt32 itemID, BSFixedString iconName)
	{
		//_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		std::lock_guard <recursive_mutex> lock(mtx);
		if (!IsIconItemRegistered(base, itemID))
		{
			s_ahzRegisteredIcons.insert(AhzIconItemCache::value_type(itemID, iconName));
		}
	}

	void RemoveIconItem(StaticFunctionTag* base, UInt32 itemID)
	{
		//_MESSAGE("RemoveIconItem %d", itemID);
		std::lock_guard <recursive_mutex> lock(mtx);
		if (IsIconItemRegistered(base, itemID))
		{
			s_ahzRegisteredIcons.erase(itemID);
		}
	}

	void AddIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs, VMArray<BSFixedString> iconNames)
	{
		std::lock_guard <recursive_mutex> lock(mtx);
		if (itemIDs.Length() != iconNames.Length())
		{
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
	}

	void RemoveIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs)
	{
		std::lock_guard <recursive_mutex> lock(mtx);
		for (UInt32 i = 0; i < itemIDs.Length(); i++)
		{
			UInt32 itemID;
			itemIDs.Get(&itemID, i);
			if (itemID)
			{
				RemoveIconItem(base, itemID);
			}
		}
	}

	string GetIconName(UInt32 itemID)
	{
		string iconName("");
		std::lock_guard <recursive_mutex> lock(mtx);

		if (IsIconItemRegistered(NULL, itemID))
		{
			iconName.append(s_ahzRegisteredIcons[itemID].c_str());
		}

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