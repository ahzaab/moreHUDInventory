cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

project(
    AHZmoreHUDInventory
    VERSION 2.2.1.0
    LANGUAGES CXX
    DESCRIPTION "SKSE64 moreHUD Inventory Edition plugin"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# Keep release optimization enabled while retaining symbols for crash loggers.
if(MSVC)
    add_compile_options("$<$<CONFIG:Release>:/Zi>")
    add_link_options("$<$<CONFIG:Release>:/DEBUG:FULL;/OPT:REF;/OPT:ICF>")
endif()

# This repository currently supports the unified SE/AE binary. The separate
# Inventory VR implementation will be migrated later.
set(ENABLE_SKYRIM_SE ON CACHE BOOL "Enable Skyrim SE support" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "Enable Skyrim AE support" FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL "Enable Skyrim VR support" FORCE)
set(SKSE_SUPPORT_XBYAK ON CACHE BOOL "Enable Xbyak trampoline support" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Build dependency tests" FORCE)
add_subdirectory(external/CommonLibSSE-NG)
include(external/CommonLibSSE-NG/cmake/CommonLibSSE.cmake)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${headers} ${sources}
)

source_group(
    TREE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
)

find_package(spdlog CONFIG REQUIRED)
find_package(xbyak CONFIG REQUIRED)

add_commonlibsse_plugin(
    ${PROJECT_NAME}
    NAME "Ahzaab's moreHUD Inventory Plugin"
    AUTHOR "Ahzaab"
    VERSION ${PROJECT_VERSION}
    USE_ADDRESS_LIBRARY
    SOURCES
        ${headers}
        ${sources}
        ${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
        xbyak::xbyak
)

target_precompile_headers(${PROJECT_NAME} PRIVATE include/PCH.h)

if(MSVC)
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
            /MP
            /permissive-
            /W4
            /we4715
            /wd4061
            /wd4200
            /wd4201
            /wd4265
            /wd4266
            /wd4324
            /wd4371
            /wd4514
            /wd4582
            /wd4583
            /wd4623
            /wd4625
            /wd4626
            /wd4710
            /wd4711
            /wd4820
            /wd5026
            /wd5027
            /wd5045
            /wd5053
            /wd5204
            /wd5220
    )
endif()

# Deploy the DLL and its matching PDB to each process-scoped target supplied by
# build.ps1. No machine-wide Skyrim path is embedded in this project.
foreach(DEPLOY_TARGET $ENV{SkyrimPluginTargets})
    set(PLUGIN_DEPLOY_DIRECTORY "${DEPLOY_TARGET}/SKSE/Plugins")
    message("Adding deployment target ${PLUGIN_DEPLOY_DIRECTORY}.")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGIN_DEPLOY_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${PROJECT_NAME}>
            "${PLUGIN_DEPLOY_DIRECTORY}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_PDB_FILE:${PROJECT_NAME}>
            "${PLUGIN_DEPLOY_DIRECTORY}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
    )
endforeach()
