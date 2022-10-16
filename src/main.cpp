#include "PCH.h"

#include <memory>
#include <vector>

#include "AHZScaleform.h"
#include "AHZConsole.h"
#include "AHZConfiguration.h"
#include "AHZPapyrusMoreHudIE.h"
#include "Events.h"
#include "Papyrus.h"
#include "Scaleform.h"
#include "Windows.h"

// Just initialize to start routing to the console window
Debug::CAHZDebugConsole theDebugConsole;
CAHZConfiguration g_ahzConfiguration;
namespace
{
    void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type) {
            case SKSE::MessagingInterface::kDataLoaded:
            {
                logger::info("Registering Events"sv);
                Events::Install();
            } break;
            case SKSE::MessagingInterface::kPostPostLoad:
            {
                Scaleform::RegisterListener();
            }
            break;
        }
    }
}


extern "C"
{
    DLLEXPORT auto SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info) -> bool
    {
        try {
#ifndef NDEBUG
            auto                    msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
            auto                    console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            spdlog::sinks_init_list sink_list = { msvc_sink, console_sink };
            auto                    log = std::make_shared<spdlog::logger>("multi_sink", sink_list.begin(), sink_list.end());
            log->set_level(spdlog::level::trace);
            spdlog::flush_every(std::chrono::seconds(3));
            spdlog::register_logger(log);
#else
            auto path = logger::log_directory();
            if (!path) {
                return false;
            }

            *path /= "AHZmoreHUDInventory.log"sv;

            auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
            auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
            log->set_level(spdlog::level::info);
            log->flush_on(spdlog::level::info);
#endif

            spdlog::set_default_logger(std::move(log));
            spdlog::set_pattern("%s(%#): [%^%l%$] %v");

            logger::info("moreHUD Inventory Edition v{}"sv, Version::NAME);

            a_info->infoVersion = SKSE::PluginInfo::kVersion;
            a_info->name = "Ahzaab's moreHUD Inventory Plugin";
            a_info->version = Version::ASINT;

            if (a_skse->IsEditor()) {
                logger::critical("Loaded in editor, marking as incompatible!"sv);
                return false;
            }

            const auto ver = a_skse->RuntimeVersion();
            if (ver <= SKSE::RUNTIME_1_5_39) {
                logger::critical("Unsupported runtime version {}!"sv, ver.string().c_str());
                return false;
            }

        } catch (const std::exception& e) {
            logger::critical(e.what());
            return false;
        } catch (...) {
            logger::critical("caught unknown exception"sv);
            return false;
        }

        return true;
    }

    DLLEXPORT auto SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) -> bool
    {
        // while (!IsDebuggerPresent())
        // {
        //   Sleep(10);
        // }

        // Sleep(1000 * 2);
        try
        {
            logger::info("moreHUD Inventory Edition loading"sv);            
            SKSE::Init(a_skse);

            SKSE::AllocTrampoline(1 << 6);

            g_ahzConfiguration.Initialize("AHZmoreHUDInventory");
            CAHZScaleform::Singleton().Initialize();

            auto messaging = SKSE::GetMessagingInterface();
            if (!messaging->RegisterListener("SKSE", MessageHandler)) {
                logger::critical("Could not register MessageHandler"sv);
                return false;
            }
            logger::info("registered listener"sv);

            logger::info("Registering Inventory Extension");
            auto scaleform = SKSE::GetScaleformInterface();
            scaleform->Register(Scaleform::RegisterInventory);

            if (!moreHUD::Papyrus::Register()) {
                logger::critical("Could not register papyrus functions"sv);
                return false;
            }

            logger::info("Registering Callbacks"sv);
            Scaleform::RegisterCallbacks();

            logger::info("moreHUDIE loaded"sv);

        } catch (const std::exception& e) {
            logger::critical(e.what());
            return false;
        } catch (...) {
            logger::critical("caught unknown exception"sv);
            return false;
        }

        return true;
    }
};