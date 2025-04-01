//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#ifndef HADES_MOD_LOADER
#define HADES_MOD_API __declspec(dllexport)
#endif

#define MOD_API_VERSION 2

enum class eGameVariant: uint8_t {
    UNKNOWN,
    VULKAN,
    DX12,
    x86,
};

struct IModApi {
    using GetGameDll_t = void*(__cdecl*)();
    using GetSymbolAddress_t = uint64_t(__cdecl*)(const char* symbolName);
    using LoadDllSymbolds_t = bool(__cdecl*)(void* handle, const char* dllName);

    uint64_t version;
    eGameVariant gameVariant;
    GetGameDll_t GetGameDll;
    GetSymbolAddress_t GetSymbolAddress;
    LoadDllSymbolds_t LoadDllSymbols;
};

#ifdef HADES_MOD_API

extern "C" {
class lua_State;
// We call this function before the game loads "Main.lua"
HADES_MOD_API void _cdecl HadesModLuaCreated(lua_State *luaState);

// We call this function when load your library
HADES_MOD_API bool _cdecl HadesModInit(const IModApi *modApi);

// We call this function when a user enables the mods
// Return false if your mod requests fresh start
HADES_MOD_API bool _cdecl HadesModStart();

// We call this function when a user disables the mods
// Return false if your mod requests restart
HADES_MOD_API bool _cdecl HadesModStop();
}

#endif //  HADESDLLEXPORT_API
