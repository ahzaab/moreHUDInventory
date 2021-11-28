#pragma once

#include "SKSE\API.h"

namespace Events
{
    extern std::string g_currentMenu;
    using EventResult = RE::BSEventNotifyControl;

    class MenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        static MenuHandler*              GetSingleton();
        static void                      Sink();
        virtual RE::BSEventNotifyControl ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

    };

    void Install();
}
