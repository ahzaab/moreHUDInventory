
#include "AHZScaleformHook.h"

string g_currentMenu;

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
   string menuName(evn->menuName.data);

   if ((menuName == "InventoryMenu") ||
      (menuName == "Crafting Menu") ||
      (menuName == "ContainerMenu") ||
      (menuName == "BarterMenu") ||
      (menuName == "Main Menu")  // Load it in the main menu to sneak in and flip the extendData flag
      && (evn->opening))
   {
      g_currentMenu.clear();
      g_currentMenu.append(menuName.c_str());
      GFxMovieView *view = MenuManager::GetSingleton()->GetMovieView(&evn->menuName);
      HUDMenu *hudMenu = dynamic_cast<HUDMenu*>(MenuManager::GetSingleton()->GetMenu(&evn->menuName));
      if (view)
      {
         GFxValue hudComponent;
         GFxValue result;
         GFxValue args[2];

         if (!view)
         {
            _ERROR("The IMenu returned NULL. The moreHUDInventory movie will not be loaded.");
         }

         args[0].SetString("AHZItemCardContainer");
         view->Invoke("_root.getNextHighestDepth", &args[1], NULL, 0);
         view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

         if (!hudComponent.objectInterface)
         {
            _ERROR("moreHUD Inventory Edition could not create an empty movie clip for the HUDMenu. The moreHUDInventory movie will not be loaded.");
            return EventResult::kEvent_Continue;
         }

         args[0].SetString("AHZmoreHUDInventory.swf");
         hudComponent.Invoke("loadMovie", &result, &args[0], 1);
         return EventResult::kEvent_Continue;
      }
   }

   return EventResult::kEvent_Continue;
}
