#include "skse64/PluginAPI.h"
#include "skse64/skse64_common/skse_version.h"
#include "AHZScaleform.h"
#include "AHZScaleformHook.h"
#include <shlobj.h>
#include "AHZConsole.h"
#include "skse64/ScaleformCallbacks.h"          // for GFxFunctionHandler
#include <stddef.h>                             // for NULL
#include "AHZConfiguration.h"

using namespace std;

#define PLUGIN_VERSION  (10005)
#define PLUGIN_NAME  ("AHZmoreHUDInventory")

IDebugLog	gLog;
PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
CAHZConfiguration g_ahzConfiguration;
CAHZScaleform m_ahzScaleForm;
static UInt32 g_skseVersion = 0;
SKSEScaleformInterface		* g_scaleform = NULL;
SKSEMessagingInterface *g_skseMessaging = NULL;
AHZEventHandler menuEvent;


// Just initialize to start routing to the console window
CAHZDebugConsole theDebugConsole;

/**** scaleform functions ****/

class SKSEScaleform_InstallHooks : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
   }
};

class SKSEScaleform_GetCurrentMenu: public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      args->result->SetString(g_currentMenu.c_str());
   }
};

class SKSEScaleform_ShowBookRead : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      args->result->SetBool(m_ahzScaleForm.m_showBookRead);
   }
};


class SKSEScaleform_EnableItemCardResize : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      args->result->SetBool(m_ahzScaleForm.m_enableItemCardResize);
   }
};

class SKSEScaleform_AHZLog : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
#if _DEBUG
      _MESSAGE("%s", args->args[0].GetString());
#else  // Only allow release verbosity for a release build
      if (args && args->args && args->numArgs > 1 && args->args[1].GetType() == GFxValue::kType_Bool && args->args[1].GetBool())
      {
         _MESSAGE("%s", args->args[0].GetString());
      }
#endif
   }
};


bool RegisterScaleform(GFxMovieView * view, GFxValue * root)
{
   RegisterFunction <SKSEScaleform_InstallHooks>(root, view, "InstallHooks");
   RegisterFunction <SKSEScaleform_ShowBookRead>(root, view, "ShowBookRead");
   RegisterFunction <SKSEScaleform_AHZLog>(root, view, "AHZLog");
   RegisterFunction <SKSEScaleform_GetCurrentMenu>(root, view, "GetCurrentMenu");
   RegisterFunction <SKSEScaleform_EnableItemCardResize>(root, view, "EnableItemCardResize");
   
   MenuManager::GetSingleton()->MenuOpenCloseEventDispatcher()->AddEventSink(&menuEvent);
   return true;
}

// Listens to events dispatched by SKSE
void EventListener(SKSEMessagingInterface::Message* msg)
{
   if (!msg)
   {
      return;
   }

   if (string(msg->sender) == "SKSE" && msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
   {
  
   }
}

extern "C"
{
   bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
   {
      string logPath = "\\My Games\\Skyrim Special Edition\\SKSE\\";
      logPath.append(PLUGIN_NAME);
      logPath.append(".log");

      gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
      gLog.SetPrintLevel(IDebugLog::kLevel_VerboseMessage);
      gLog.SetLogLevel(IDebugLog::kLevel_Message);

      // populate info structure
      info->infoVersion = PluginInfo::kInfoVersion;
      info->name = "Ahzaab's moreHUD Inventory Plugin";
      info->version = PLUGIN_VERSION;

      // store plugin handle so we can identify ourselves later
      g_pluginHandle = skse->GetPluginHandle();

      if (skse->isEditor)
      {
         _ERROR("loaded in editor, marking as incompatible");

         return false;
      }
      else if (skse->runtimeVersion != RUNTIME_VERSION_1_5_39)
      {
         _ERROR("unsupported runtime version %08X", skse->runtimeVersion);

         return false;
      }
      else if (SKSE_VERSION_RELEASEIDX < 53)
      {
         _ERROR("unsupported skse release index %08X", SKSE_VERSION_RELEASEIDX);

         return false;
      }

      // get the scaleform interface and query its version
      g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
      if (!g_scaleform)
      {
         _ERROR("couldn't get scaleform interface");
         return false;
      }

      if (g_scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
      {
         _ERROR("scaleform interface too old (%d expected %d)", g_scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);
         return false;
      }

      // ### do not do anything else in this callback
      // ### only fill out PluginInfo and return true/false

      g_skseMessaging = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
      if (!g_skseMessaging)
      {
         _ERROR("couldn't get messaging interface");
         return false;
      }


      // supported runtime version
      return true;
   }

   void RegisterForIventory(GFxMovieView * view, GFxValue * object, InventoryEntryData * item)
   {
      m_ahzScaleForm.ExtendItemCard(view, object, item);
   }

   bool SKSEPlugin_Load(const SKSEInterface * skse)
   {
      //while (!IsDebuggerPresent())
      //{
      //   Sleep(10);
      //}

      //Sleep(1000 * 2);

      g_ahzConfiguration.Initialize(PLUGIN_NAME);
      m_ahzScaleForm.Initialize();

      // register scaleform callbacks
      g_scaleform->Register(PLUGIN_NAME, RegisterScaleform);

      g_scaleform->RegisterForInventory(RegisterForIventory);

      // Register listener for the gme loaded event
      g_skseMessaging->RegisterListener(skse->GetPluginHandle(), "SKSE", EventListener);

      _MESSAGE("%s -v%d Loaded", PLUGIN_NAME, PLUGIN_VERSION);
      return true;
   }
};