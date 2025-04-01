cmake_minimum_required(VERSION 3.20)

project(HadesModLoader VERSION 1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MODLOADER_DEDICATED_LAUNCHER "Build dedicated launcher" OFF)
option(MODLOADER_DLL_HOOK_LAUNCHER "Build dll hook launcher" ON)

add_subdirectory(libs/lua-5.2.2)
add_subdirectory(libs/libdetours)

set(MOD_LOADER_INSTALL_LIST HadesModCore)

if(${MODLOADER_DEDICATED_LAUNCHER})
add_subdirectory(launcher)
list(APPEND MOD_LOADER_INSTALL_LIST HadesModdedLauncher)
#set(MOD_LOADER_INSTALL_LIST ${MOD_LOADER_INSTALL_LIST} HadesModdedLauncher)
endif()

if(${MODLOADER_DLL_HOOK_LAUNCHER})
add_subdirectory(loader-dllhook)
list(APPEND MOD_LOADER_INSTALL_LIST HadesModLoaderDllHook)
#set(MOD_LOADER_INSTALL_LIST ${MOD_LOADER_INSTALL_LIST} HadesModLoaderDllHook)
endif()

add_subdirectory(core)

install(TARGETS ${MOD_LOADER_INSTALL_LIST}
        RUNTIME
        DESTINATION ${CMAKE_SOURCE_DIR}/bin
)
