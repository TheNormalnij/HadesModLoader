//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "TempValuesLuaDefs.h"
#include "lua.hpp"

#include <unordered_map>

static std::unordered_map<std::string, std::string> runtimeData{};

// SetTempRuntimeData(string key, string/number/table value)
static int SetTempRuntimeData(lua_State *L) {
    size_t keyLen;
    const char *keyStr = lua_tolstring(L, 1, &keyLen);

    if (!keyStr)
        return luaL_error(L, "Missing key argument");

    lua_getglobal(L, "luabins");
    lua_getfield(L, -1, "save");
    lua_pushvalue(L, 2);
    lua_pcall(L, 1, 1, 0);

    size_t dataLen;
    const char *valueStr = lua_tolstring(L, -1, &dataLen);
    lua_pop(L, -3);

    runtimeData[{keyStr, keyLen}] = {valueStr, dataLen};

    return 0;
}

// string/number/table GetTempRuntimeData(string key)
static int GetTempRuntimeData(lua_State *L) {
    size_t keyLen;
    const char *keyStr = lua_tolstring(L, 1, &keyLen);

    if (!keyStr)
        return luaL_error(L, "Missing key argument");

    const auto data = runtimeData.find({keyStr, keyLen});

    if (data == runtimeData.end()) {
        lua_pushboolean(L, false);
        return 1;
    }

    lua_getglobal(L, "luabins");
    lua_getfield(L, -1, "load");
    lua_remove(L, -2);
    lua_pushlstring(L, data->second.data(), data->second.length());
    lua_pcall(L, 1, 2, 0);
    
    return 2;
}

void TempValuesLuaDefs::Load(lua_State *L) {
    lua_register(L, "SetTempRuntimeData", SetTempRuntimeData);
    lua_register(L, "GetTempRuntimeData", GetTempRuntimeData);
}
