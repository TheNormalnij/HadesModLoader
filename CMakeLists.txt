cmake_minimum_required(VERSION 3.20)

project(HadesModLoader VERSION 1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MODLOADER_DEDICATED_LAUNCHER "Build dedicated launcher" OFF)
option(MODLOADER_DLL_HOOK_LAUNCHER "Build dll hook launcher" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(libs/lua-5.2.2)
add_subdirectory(libs/libdetours)

if(${MODLOADER_DEDICATED_LAUNCHER})
add_subdirectory(launcher)
endif()

if(${MODLOADER_DLL_HOOK_LAUNCHER})
add_subdirectory(loader-dllhook)
endif()

add_subdirectory(core)
