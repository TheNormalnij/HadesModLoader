//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "Mod.h"
#include "LuaManager.h"
#include "ModApi.h"
#include <sjson/parser.h>
#include <fstream>

namespace fs = std::filesystem;

bool Mod::Load() {
    using namespace sjson;
    const auto metaPath = m_modPath / "meta.sjson";

    std::ifstream file(metaPath);

    if (!file.is_open())
        return false;

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    Parser parser{buffer.data(), buffer.size()};

    if (!parser.is_valid())
        return false;

    if (!parser.object_begins())
        return false;

    StringView modName;
    if (!parser.read("Name", modName))
        return false;

    m_modName = {modName.c_str(), modName.size()};

    StringView modLib;
    if (parser.read("Library", modLib))
        m_libName = {modLib.c_str(), modLib.size()};

    if (!parser.read("Priority", m_priority))
        m_priority = 0;

    parser.object_ends();

    return true;
}

void Mod::OnLuaCreated(lua_State *luaState) {
    if (m_interface.LuaCreated)
        m_interface.LuaCreated(luaState);

    const auto luaPath = m_modPath / "init.lua";
    if (fs::is_regular_file(luaPath))
        LuaManager::DoScriptFile(luaPath);
}

bool Mod::Start() {
    if (m_enabled)
        return false;

    if (m_libName.empty())
        return false;

    bool libStatus = m_libInited || LoadLib();

    if (!libStatus)
        return false;

    if (m_interface.Start)
        return m_interface.Start();

    return true;
}

bool Mod::Stop() {
    if (!m_enabled)
        return true;

    if (m_interface.Stop)
        return m_interface.Stop();

    return true;
}

bool Mod::LoadLib() {
    auto libPath = m_modPath / m_libName;
    HMODULE handle = LoadLibraryW(libPath.c_str());

    if (!handle)
        return true;

    m_interface.Init = (IModInterface::ModuleInit_t)(GetProcAddress(handle, "HadesModInit"));

    bool status = true;

    if (m_interface.Init)
        status = m_interface.Init(&gModApi);

    if (!status)
        return false;

    m_interface.LuaCreated = (IModInterface::LuaCreatedApi_t)(GetProcAddress(handle, "HadesModLuaCreated"));
    m_interface.Start = (IModInterface::Start_t)(GetProcAddress(handle, "HadesModStart"));
    m_interface.Stop = (IModInterface::Stop_t)(GetProcAddress(handle, "HadesModStop"));

    return true;
}
