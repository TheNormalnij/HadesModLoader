//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "Mod.h"
#include "LuaManager.h"
#include "ModApi.h"

namespace fs = std::filesystem;

bool Mod::Load() { 
    if (fs::is_regular_file(m_modPath / "init.lua"))
        m_luaInit = true;

    auto modulePath = m_modPath / "module.dll";
    if (!fs::is_regular_file(modulePath))
        return m_luaInit;

    HMODULE handle = LoadLibraryW(modulePath.c_str());

    if (!handle)
        return true;

    auto initFunction = (ModuleInit_t)(GetProcAddress(handle, "HadesModInit"));

    bool status = true;

    if (initFunction)
        status = initFunction(&gModApi);

    if (!status)
        return false;

    m_createdCallback = (LuaCreatedApi_t)(GetProcAddress(handle, "HadesModLuaCreated"));

    return true;
}

void Mod::OnLuaCreated(lua_State *luaState) {
    if (m_createdCallback)
        m_createdCallback(luaState);

    if (m_luaInit)
        LuaManager::LoadScriptFile(m_modPath / "init.lua");
}
