#include "PCH.h"

#include "Events.h"

namespace Events
{
    std::string g_currentMenu;
    auto MenuHandler::GetSingleton() -> MenuHandler*
    {
        static MenuHandler singleton;
        return std::addressof(singleton);
    }

    void MenuHandler::Sink()
    {
        auto ui = RE::UI::GetSingleton();
        ui->AddEventSink(static_cast<RE::BSTEventSink<RE::MenuOpenCloseEvent>*>(MenuHandler::GetSingleton()));
    }

    auto MenuHandler::ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) -> RE::BSEventNotifyControl
    {
        if (a_event == nullptr) {
            return RE::BSEventNotifyControl::kContinue;
        }

        if ((a_event->menuName == RE::GiftMenu::MENU_NAME ||
            a_event->menuName == RE::InventoryMenu::MENU_NAME ||
            a_event->menuName == RE::CraftingMenu::MENU_NAME ||
            a_event->menuName == RE::ContainerMenu::MENU_NAME ||
            a_event->menuName == RE::BarterMenu::MENU_NAME ||
            a_event->menuName == RE::MagicMenu::MENU_NAME ||
            a_event->menuName == RE::MainMenu::MENU_NAME) // Load it in the main menu to sneak in and flip the extendData flag
            && a_event->opening) {
            auto view = RE::UI::GetSingleton()->GetMovieView(a_event->menuName);

            g_currentMenu.clear();
            g_currentMenu.append(a_event->menuName.c_str());

            if (view) {
                RE::GFxValue hudComponent;
                RE::GFxValue result;
                RE::GFxValue args[2];

                if (!view) {
                    logger::error("The IMenu returned NULL. The moreHUDInventory movie will not be loaded."sv);
                }

                args[0].SetString("AHZItemCardContainer");
                // -16384 is used to load it behind the existing item card.  Using SwapDepth causes issues when zooming in the 3D models from the menu.
                args[1].SetNumber(-16384);

                view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

                if (!hudComponent.IsObject()) {
                    logger::error("moreHUD Inventory Edition could not create an empty movie clip. The moreHUD moreHUDInventory movie will not be loaded."sv);
                    return RE::BSEventNotifyControl::kStop;
                }

                args[0].SetString("AHZmoreHUDInventory.swf");
                hudComponent.Invoke("loadMovie", &result, &args[0], 1);
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    void Install()
    {
        MenuHandler::Sink();
        logger::info("registered menu event"sv);

        logger::info("Installed all event sinks"sv);
    }

}
