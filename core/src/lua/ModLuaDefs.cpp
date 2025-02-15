//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "ModLuaDefs.h"

#include <iostream>

#include "HooksSystem.h"
#include "lua.hpp"

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


void ModLuaDefs::Load(lua_State *L) {
    luaL_loadstring(L, baseScript);
}
