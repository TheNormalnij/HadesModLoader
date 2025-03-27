//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <filesystem>

class lua_State;
typedef int (*lua_CFunction)(lua_State *L);

class LuaManager {
  public:
    static void Reinit();
    static bool DoScriptFile(const std::filesystem::path &path);
    static bool LoadScriptFile(const std::filesystem::path &path);
    static int lua_pcallk(lua_State *L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k);
    static int luaL_loadbufferx(lua_State *L, const char* buffer, size_t size, const char* name, const char* mode);
    static lua_State *GetLuaState();
};
