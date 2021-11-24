#include "PCH.h"
#include "Scaleform.h"
#include "AHZScaleform.h"
#include "SKSE/API.h"
#include "AHZPapyrusMoreHud.h"
#include "HashUtil.h"

namespace Scaleform
{
    CAHZScaleform m_ahzScaleForm;
    
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
            a_params.retVal->SetString(g_currentMenu.c_str());
        }
    };

    class SKSEScaleform_ShowBookRead : public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            a_params.retVal->SetBool(m_ahzScaleForm.m_showBookRead);
        } 
    };


    class SKSEScaleform_EnableItemCardResize : public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            a_params.retVal->SetBool(m_ahzScaleForm.m_enableItemCardResize);
        }
    };

    class SKSEScaleform_GetWasBookRead : public RE::GFxFunctionHandler
    {
        public:
            void Call(Params& a_params) override
        {
            if (a_params.args && a_params.argCount && a_params.args[0].IsNumber())
            {
                std::uint32_t formID = std::static_cast<std::uint32_t>(a_params.args[0].GetNumber());

                RE::TESForm* bookForm = RE::LookupFormByID(formID);
                a_params.retVal->SetBool(m_ahzScaleForm.GetWasBookRead(bookForm));
            }
        }
    };


class SKSEScaleform_GetIconForItemId : public RE::GFxFunctionHandler
{
        public:
            void Call(Params& a_params) override
	{
		if (args && args->args && args->numArgs > 1 && args->args[0].GetType() == GFxValue::kType_Number && args->args[1].GetType() == GFxValue::kType_String)
		{
			UInt32 formID = (UInt32)args->args[0].GetNumber();
			
			const char * name = args->args[1].GetString();
			
			if (!name)
			{
				return;
			}

			SInt32 itemId = (SInt32)HashUtil::CRC32(name, formID & 0x00FFFFFF);
			s_lastIconName.clear();
			s_lastIconName.append(papyrusMoreHudIE::GetIconName(itemId));
			GFxValue obj;
			args->movie->CreateObject(&obj);
			GFxValue	fxValue;
			fxValue.SetString(s_lastIconName.c_str());
			obj.SetMember("iconName", &fxValue);

			// Add the object to the scaleform function
			args->args[2].SetMember("returnObject", &obj);
		}
	}
};

class SKSEScaleform_HasFormId : public RE::GFxFunctionHandler
{
        public:
            void Call(Params& a_params) override
	{
		if (a_params.args && a_params.argCount > 1 && a_params.args[0].IsString() && a_params.args[1].IsNumber())
		{
			auto iconName = a_params.args[0].GetString();
			auto formId = std::static_cast<std::uint32_t>(a_params.args[1].GetNumber());

			if (!iconName)
			{
				return;
			}

			args->result->SetBool(papyrusMoreHudIE::HasForm(string(iconName), formId));
		}
	}
};

class SKSEScaleform_AHZLog : public RE::GFxFunctionHandler
{
        public:
            void Call(Params& a_params) override
	{
#if _DEBUG
		_MESSAGE("%s", a_params.args[0].GetString());
#else  // Only allow release verbosity for a release build
		if (a_params.args && a_params.argCount > 1 && a_params.args[1].IsBool() && a_params.args[1].GetBool())
		{
			_MESSAGE("%s", a_params.args[0].GetString());
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
        return true;
    }

    void RegisterCallbacks()
    {
        auto scaleform = SKSE::GetScaleformInterface();
        scaleform->Register(RegisterScaleformFunctions, "AHZmoreHUDInventory");
        logger::info("Registered all scaleform callbacks");
    }

}