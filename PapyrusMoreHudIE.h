class VMClassRegistry;
struct StaticFunctionTag;

#include "skse64/GameTypes.h"
#include "skse64/PapyrusArgs.h"
#include <string.h>
#include <map>

using namespace std;

namespace papyrusMoreHudIE
{
	extern map<UInt32, string> g_ahzRegisteredIcons;

	bool RegisterFuncs(VMClassRegistry* registry);

	UInt32 GetVersion(StaticFunctionTag* base);
	bool IsIconItemRegistered(StaticFunctionTag* base, UInt32 itemID);
	void AddIconItem(StaticFunctionTag* base, UInt32 itemID, BSFixedString iconName);
	void RemoveIconItem(StaticFunctionTag* base, UInt32 itemID);
	void AddIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs, VMArray<BSFixedString> iconNames);
	void RemoveIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs);
}
