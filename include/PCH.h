#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>

namespace WinAPI = SKSE::WinAPI;

#ifndef NDEBUG
#include <iostream>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

#include "IForEachScriptObjectFunctor.h"


using namespace std::literals;
namespace logger = SKSE::log;

template <typename T1, typename T2>
inline T2* dyna_cast(T1* base)
{
    auto asForm = static_cast<T1*>(base);
    auto ret = (asForm)->As<T2>();
    return ret;
}
#define DYNAMIC_CAST(base, srcType, targetType) (dyna_cast<srcType, targetType>(base))


// namespace logger
// {
// 	using namespace SKSE::log;
// }



#define DLLEXPORT __declspec(dllexport)

#include "Version.h"

