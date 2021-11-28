#include "PCH.h"
#include "Papyrus.h"
#include "AHZPapyrusMoreHudIE.h"

namespace moreHUD
{
    auto Papyrus::Register() -> bool
    {
        auto papyrus = SKSE::GetPapyrusInterface();
        if (!papyrus->Register(PapyrusMoreHudIE::RegisterFunctions)) {
            return false;
        } else {
            logger::info("registered papyrus PapyrusMoreHud funcs");
        }

        return true;
    }
}
