#include "PCH.h"
#include "Scaleform.h"
#include "AHZScaleform.h"
#include "SKSE/API.h"
#include "AHZPapyrusMoreHudIE.h"
#include "HashUtil.h"
#include "Events.h"

namespace Scaleform
{
    std::string s_lastIconName;

    /**** scaleform functions ****/

    class SKSEScaleform_InstallHooks : public RE::GFxFunctionHandler
    {
        public:
            void Call([[maybe_unused]] Params& a_params) override
        {
        }
    };

    class SKSEScaleform_GetCurrentMenu: public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            logger::trace("Current Menu: {}", Events::g_currentMenu.c_str());
            a_params.retVal->SetString(Events::g_currentMenu.c_str());
        }
    };

    class SKSEScaleform_ShowBookRead : public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            logger::trace("ShowBookRead: {}", CAHZScaleform::Singleton().m_showBookRead);
            a_params.retVal->SetBoolean(CAHZScaleform::Singleton().m_showBookRead);
        } 
    };


    class SKSEScaleform_EnableItemCardResize : public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            a_params.retVal->SetBoolean(CAHZScaleform::Singleton().m_enableItemCardResize);
        }
    };

    class SKSEScaleform_GetWasBookRead : public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            if (a_params.args && a_params.argCount && a_params.args[0].IsNumber())
            {
                auto formID = static_cast<std::uint32_t>(a_params.args[0].GetNumber());
                auto bookForm = RE::TESForm::LookupByID(formID);
                auto isReadBook = CAHZScaleform::Singleton().GetWasBookRead(bookForm);
                logger::trace("GetWasBookRead: {}", isReadBook);
                a_params.retVal->SetBoolean(isReadBook);
            }
        }
    };


class SKSEScaleform_GetIconForItemId : public RE::GFxFunctionHandler
{
        public:
            void Call(Params& a_params) override
	{
		if (a_params.args && a_params.argCount > 1 && a_params.args[0].IsNumber() && a_params.args[1].IsString())
		{
			auto formID = static_cast<std::uint32_t>(a_params.args[0].GetNumber());
			
			const char * name = a_params.args[1].GetString();
			
			if (!name)
			{
                s_lastIconName.clear();
                a_params.retVal->SetString(s_lastIconName.c_str());
				return;
			}

			int32_t itemId = static_cast<int32_t>(SKSE::HashUtil::CRC32(name, formID & 0x00FFFFFF));
			s_lastIconName.clear();
			s_lastIconName.append(PapyrusMoreHudIE::GetIconName(itemId));
			a_params.retVal->SetString(s_lastIconName.c_str());

		}
	}
};

class SKSEScaleform_GetFormIcons : public RE::GFxFunctionHandler
{
    public:
    void Call(Params& a_params) override
	{
		if (a_params.args && a_params.argCount && a_params.args[0].IsNumber())
		{
			auto formId = static_cast<RE::FormID>(a_params.args[0].GetNumber());

            auto customIcons = PapyrusMoreHudIE::GetFormIcons(formId);
            RE::GFxValue          customIconArray;
            a_params.movie->CreateArray(a_params.retVal);

            if (!customIcons.empty()){
                RE::GFxValue          entry;
                a_params.retVal->SetArraySize(static_cast<uint32_t>(customIcons.size()));
                auto idx = 0;
                for (auto& ci: customIcons)
                {
                    entry.SetString(ci);
                    a_params.retVal->SetElement(idx++, entry);
                }  
            }
            else{
                a_params.retVal->SetArraySize(0);
            }
		}
	}
};

class SKSEScaleform_AHZLog : public RE::GFxFunctionHandler
{
        public:
            void Call(Params& a_params) override
	{
#if _DEBUG
		logger::trace("{}", a_params.args[0].GetString());
#else  // Only allow release verbosity for a release build
		if (a_params.args && a_params.argCount > 1 && a_params.args[1].IsBool() && a_params.args[1].GetBool())
		{
			logger::info("{}", a_params.args[0].GetString());
		}
#endif
	}
};



    typedef std::map<const std::type_info*, RE::GFxFunctionHandler*> FunctionHandlerCache;
    static FunctionHandlerCache                                      g_functionHandlerCache;

    template <typename T>
    void RegisterFunction(RE::GFxValue* dst, RE::GFxMovieView* movie, const char* name)
    {
        // either allocate the object or retrieve an existing instance from the cache
        RE::GFxFunctionHandler* fn = nullptr;

        // check the cache
        FunctionHandlerCache::iterator iter = g_functionHandlerCache.find(&typeid(T));
        if (iter != g_functionHandlerCache.end())
            fn = iter->second;

        if (!fn) {
            // not found, allocate a new one
            fn = new T;

            // add it to the cache
            // cache now owns the object as far as refcounting goes
            g_functionHandlerCache[&typeid(T)] = fn;
        }

        // create the function object
        RE::GFxValue fnValue;
        movie->CreateFunction(&fnValue, fn);

        // register it
        dst->SetMember(name, fnValue);
    }

    auto RegisterScaleformFunctions(RE::GFxMovieView* a_view, RE::GFxValue* a_root) -> bool
    {
        RegisterFunction<SKSEScaleform_InstallHooks>(a_root, a_view, "InstallHooks");
        RegisterFunction<SKSEScaleform_GetCurrentMenu>(a_root, a_view, "GetCurrentMenu");
        RegisterFunction<SKSEScaleform_ShowBookRead>(a_root, a_view, "ShowBookRead");
        RegisterFunction<SKSEScaleform_EnableItemCardResize>(a_root, a_view, "EnableItemCardResize");
        RegisterFunction<SKSEScaleform_GetWasBookRead>(a_root, a_view, "GetWasBookRead");
        RegisterFunction<SKSEScaleform_GetIconForItemId>(a_root, a_view, "GetIconForItemId");
        RegisterFunction<SKSEScaleform_GetFormIcons>(a_root, a_view, "GetFormIcons");
        RegisterFunction<SKSEScaleform_AHZLog>(a_root, a_view, "AHZLog");
        return true;
    }

    void RegisterCallbacks()
    {
        auto scaleform = SKSE::GetScaleformInterface();
        scaleform->Register(RegisterScaleformFunctions, "AHZmoreHUDInventory");
        logger::info("Registered all scaleform callbacks");
    }

    void RegisterInventory(RE::GFxMovieView * view, RE::GFxValue * object, RE::InventoryEntryData * item){
        CAHZScaleform::Singleton().ExtendItemCard(view, object, item);
    }

}