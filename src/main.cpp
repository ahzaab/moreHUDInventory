#include "PCH.h"

#include <memory>
#include <vector>

#include "AHZScaleform.h"
#include "AHZScaleformHook.h"
#include "AHZConsole.h"
#include "AHZConfiguration.h"
#include "AHZPapyrusMoreHudIE.h"

using namespace moreHUD;

// Just initialize to start routing to the console window
Debug::CAHZDebugConsole theDebugConsole;
CAHZConfiguration g_ahzConfiguration;


extern "C"
{
    DLLEXPORT constinit auto SKSEPlugin_Version = []() {
        SKSE::PluginVersionData v{};
        v.pluginVersion = Version::ASINT;
        v.PluginName("Ahzaab's moreHUD Inventory Plugin"sv);
        v.AuthorName("Ahzaab"sv);
        v.CompatibleVersions({ SKSE::RUNTIME_LATEST });
        v.UsesAddressLibrary(true);
        return v;
    }();

    DLLEXPORT auto SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) -> bool
    {
        // while (!IsDebuggerPresent())
        // {
        //   Sleep(10);
        // }

        // Sleep(1000 * 2);

        try {
#ifndef NDEBUG
            auto                    msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
            auto                    console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            spdlog::sinks_init_list sink_list = { msvc_sink, console_sink };
            auto                    log = std::make_shared<spdlog::logger>("multi_sink", sink_list.begin(), sink_list.end());
            log->set_level(spdlog::level::trace);
            spdlog::flush_every(std::chrono::seconds(3));
            spdlog::set_default_logger(std::move(log));
#else
            auto path = logger::log_directory();
            if (!path) {
                //stl::report_and_fail("Failed to find standard logging directory"sv);
                return false;
            }

            *path /= "moreHUDIE.log"sv;

            auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
            auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
            log->set_level(spdlog::level::info);
            log->flush_on(spdlog::level::info);
            spdlog::set_default_logger(std::move(log));
#endif

            logger::info("moreHUD loading"sv);
            logger::info("moreHUD v{}"sv, Version::NAME);

            SKSE::Init(a_skse);

            SKSE::AllocTrampoline(1 << 6);

            auto messaging = SKSE::GetMessagingInterface();
            if (!messaging->RegisterListener("SKSE", MessageHandler)) {
                logger::critical("Could not register MessageHandler"sv);
                return false;
            }
            logger::info("registered listener"sv);


            if (!moreHUD::Papyrus::Register()) {
                logger::critical("Could not register papyrus functions"sv);
                return false;
            }

            logger::info("Installing patched"sv);
            Patches::Install();

            logger::info("Registering Callbacks"sv);
            Scaleform::RegisterCallbacks();

            logger::info("moreHUD loaded"sv);

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