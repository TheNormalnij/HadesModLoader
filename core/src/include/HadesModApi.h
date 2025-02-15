//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#ifndef HADES_MOD_LOADER
#define HADES_MOD_API __declspec(dllexport)
#endif

#define MOD_API_VERSION 1

struct IModApi {
    uint64_t version;
};

#ifdef HADES_MOD_API

extern "C" {
class lua_State;
// We call this function before the game loads "Main.lua"
HADES_MOD_API void _cdecl HadesModLuaCreated(lua_State *luaState);

// We call this function when load the mod
HADES_MOD_API void _cdecl HadesModInit(IModApi* modApi);
}

#endif //  HADESDLLEXPORT_API
