//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "LuaManager.h"
#include "HooksSystem.h"
#include "lua/ModLuaDefs.h"
#include "lua/TempValuesLuaDefs.h"
#include <fstream>
#include <filesystem>

#include <lua.hpp>

namespace fs = std::filesystem;

const static fs::path gamePath = fs::current_path().parent_path();

void LuaManager::Reinit() {
    lua_State *luaState = GetLuaState();
    ModLuaDefs::Load(luaState);
    TempValuesLuaDefs::Load(luaState);
}

bool LuaManager::DoScriptFile(const std::filesystem::path &path) {
    if (!LoadScriptFile(path))
        return false;

    return lua_pcallk(GetLuaState(), 0, 0, 0, 0, 0) == LUA_OK;
}

bool LuaManager::LoadScriptFile(const std::filesystem::path &path) {
    std::ifstream file{path, std::ifstream::ate};
    if (!file.is_open())
        return false;

    std::vector<char> buffer{};
    buffer.resize(file.tellg());
    file.seekg(0);
    file.read(buffer.data(), buffer.size());
    file.close();

    auto luaName = "@" + path.lexically_relative(gamePath).lexically_normal().string();

    int fileLoadStatus = luaL_loadbufferx(GetLuaState(), buffer.data(), buffer.size(), luaName.c_str(), nullptr);

    return fileLoadStatus == LUA_OK;
}

int LuaManager::lua_pcallk(lua_State *L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k) {
    return ((int64_t(__fastcall *)(lua_State *, int, int, int, int, lua_CFunction))HooksSystem::Instance()->GetHoohTable().lua_pcallk)(L, nargs, nresults, errfunc, ctx, k);
}

int LuaManager::luaL_loadbufferx(lua_State *L, const char *buffer, size_t size, const char *name, const char *mode) {
    return (
        (int64_t(__fastcall *)(lua_State *, const char *, size_t, const char *, const char *))HooksSystem::Instance()
            ->GetHoohTable()
            .luaL_loadbufferx)(L, buffer, size, name, mode);
}

lua_State *LuaManager::GetLuaState() {
    return *reinterpret_cast<lua_State **>(HooksSystem::Instance()->GetHoohTable().luaState);
}
