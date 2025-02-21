//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "LuaManager.h"
#include "HooksSystem.h"
#include "lua/ModLuaDefs.h"

#include <lua.hpp>

void LuaManager::Reinit() {

    lua_State *luaState = GetLuaState();
    ModLuaDefs::Load(luaState);
}

bool LuaManager::LoadScriptFile(const std::filesystem::path &path) {
    auto *luaState = GetLuaState();
	auto strPath = path.string();

    int top = lua_gettop(luaState);
	bool state = luaL_dofile(luaState, strPath.c_str());

    lua_settop(luaState, top);

    return state;
}

int LuaManager::lua_pcallk(lua_State *L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k) {
    return ((int64_t(__fastcall *)(lua_State *, int, int, int, int, lua_CFunction))HooksSystem::Instance()->GetHoohTable().lua_pcallk)(L, nargs, nresults, errfunc, ctx, k);
}

lua_State *LuaManager::GetLuaState() {
    return *reinterpret_cast<lua_State **>(HooksSystem::Instance()->GetHoohTable().luaState);
}
