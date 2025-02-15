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

	return luaL_dofile(luaState, strPath.c_str());
}

lua_State *LuaManager::GetLuaState() {
    return *reinterpret_cast<lua_State **>(HooksSystem::Instance()->GetHoohTable().luaState);
}
