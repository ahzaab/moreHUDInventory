#pragma once

#include "SKSE\API.h"

namespace Events
{
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
