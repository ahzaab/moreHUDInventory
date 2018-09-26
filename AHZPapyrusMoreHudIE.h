class VMClassRegistry;
struct StaticFunctionTag;

#include "skse64/GameTypes.h"
#include "skse64/PapyrusArgs.h"
#include <string.h>
#include <map>

using namespace std;

namespace papyrusMoreHudIE
{
	bool RegisterFuncs(VMClassRegistry* registry);

	UInt32 GetVersion(StaticFunctionTag* base);
	bool IsIconItemRegistered(StaticFunctionTag* base, UInt32 
	
	
	
	);
	void AddIconItem(StaticFunctionTag* base, UInt32 itemID, BSFixedString iconName);
	void RemoveIconItem(StaticFunctionTag* base, UInt32 itemID);
	void AddIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs, VMArray<BSFixedString> iconNames);
	void RemoveIconItems(StaticFunctionTag* base, VMArray<UInt32> itemIDs);
	string GetIconName(UInt32 itemID);
}
