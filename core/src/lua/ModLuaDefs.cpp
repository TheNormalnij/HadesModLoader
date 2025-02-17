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

const char *baseScript = R""""(
local oldImport = Import

local preloadHandles = {}
local postLoadHandles = {}

function Import(fileName)
    local handlers = preloadHandles[fileName]
    if handlers then
        for k, v in ipairs(handlers) do
            v(fileName)
        end
    end
    oldImport(fileName)

    handlers = postLoadHandles[fileName]

    if handlers then
        for k, v in ipairs(handlers) do
            v(fileName)
        end
    end
end

local function addHandler(t, key, value)
    local arr = t[key]
    if arr then
        table.insert(arr, value)
    else
        t[key] = {value}
    end
end

function addPreImport(fileName, handler)
    addHandler(preloadHandles, handler)
end

function addPostImport(fileName, handler)
    addHandler(postLoadHandles, handler)
end

)"""";

static int ModImport(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return luaL_error(L, "Argument 1 must be a string");
    }

    auto relativePath = lua_tostring(L, 1);

    lua_Debug debugInfo{};

    if (!lua_getstack(L, 1, &debugInfo) || !lua_getinfo(L, "S", &debugInfo))
        return 0;

    const char *source = debugInfo.source;

    if (source == nullptr || *source != '@')
        return 0;

    source++; // skip @

    fs::path scriptPath{source};

    auto scriptToLoad = (scriptPath.parent_path() / relativePath).string();

    luaL_dofile(L, scriptToLoad.c_str());

    return 0;
}

static int CreateMessageWindow(lua_State *L) {
    if (!lua_isstring(L, 1)) {
        return luaL_error(L, "Argument 1 must be a number");
    }

    if (!lua_isstring(L, 2)) {
        return luaL_error(L, "Argument 2 must be a number");
    }

    auto strOne = lua_tostring(L, 1);
    auto strTwo = lua_tostring(L, 2);

    MessageBoxA(NULL, strTwo, strOne, MB_OK);
    return 0;
}

void ModLuaDefs::Load(lua_State *L) {
    lua_register(L, "ModImport", ModImport);
    lua_register(L, "CreateMessageWindow", CreateMessageWindow);
    luaL_loadstring(L, baseScript);
}
