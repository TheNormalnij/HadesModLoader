//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "ModLuaDefs.h"

#include <filesystem>

#include "HooksSystem.h"
#include "lua.hpp"

namespace fs = std::filesystem;

int ModRequire(lua_State *L) {
    auto scriptRelativePath = luaL_checkstring(L, 1);

    lua_Debug debugInfo{};

    if (!lua_getstack(L, 1, &debugInfo) || !lua_getinfo(L, "S", &debugInfo))
        return luaL_error(L, "Cannot get debug info");

    const char *source = debugInfo.source;

    if (source == nullptr || *source != '@')
        return luaL_error(L, "Cannot get source path");

    source++; // skip @

    fs::path scriptPath{source};
    scriptPath = scriptPath.parent_path() / scriptRelativePath;

    auto gamePath = fs::current_path().parent_path();
    auto relativePath = scriptPath.lexically_relative(gamePath).lexically_normal();
    auto relativePathAsString = relativePath.string();

    if (relativePathAsString.starts_with(".."))
        return luaL_error(L, "Cannot go outside of the game folder");

    auto scriptToLoad = (gamePath / relativePath).string();

    lua_settop(L, 1); 
    luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED_SUBMODULES");
    lua_getfield(L, 2, relativePathAsString.c_str());

    if (lua_toboolean(L, -1))
        return 1; // Package was loaded

    lua_pop(L, 1); /* remove 'getfield' result */

    int fileLoadStatus = luaL_loadfilex(L, scriptToLoad.c_str(), 0);

    if (fileLoadStatus == LUA_ERRGCMM)
        return luaL_error(L, "GC error");

    if (fileLoadStatus == LUA_ERRMEM)
        return luaL_error(L, "Out of memory");

    if (fileLoadStatus == LUA_ERRSYNTAX)
        return luaL_error(L, "Syntax error");

    if (fileLoadStatus != LUA_OK)
        return luaL_error(L, "Unknown error");

    lua_call(L, 0, 1);

    if (!lua_isnil(L, -1))        /* non-nil return? */
        lua_setfield(L, 2, relativePathAsString.c_str()); /* _LOADED[name] = returned value */
    lua_getfield(L, 2, relativePathAsString.c_str());
    if (lua_isnil(L, -1)) {       /* module did not set a value? */
        lua_pushboolean(L, 1);    /* use true as result */
        lua_pushvalue(L, -1);     /* extra copy to be returned */
        lua_setfield(L, 2, relativePathAsString.c_str()); /* _LOADED[name] = true */
    }

    return 1;
}

void ModLuaDefs::Load(lua_State *L) {
    lua_register(L, "ModRequire", ModRequire);
}
