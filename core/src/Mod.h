//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include "include/HadesModApi.h"
#include <filesystem>

class lua_State;

struct IModInterface {
    using ModuleInit_t = bool(__cdecl *)(IModApi *);
    using LuaCreatedApi_t = void(__cdecl *)(lua_State *);
    using Start_t = bool(__cdecl *)();
    using Stop_t = bool(__cdecl *)();

    ModuleInit_t Init{};
    LuaCreatedApi_t LuaCreated{};
    Start_t Start{};
    Stop_t Stop{};
};

class Mod {
  public:
    Mod(const std::filesystem::directory_entry &path) : m_modPath{path} {};

    bool Load();
    void OnLuaCreated(lua_State *luaState);

    bool Start();
    bool Stop();

    int64_t GetPriority() const noexcept { return m_priority; };
    auto &GetLocalization() const noexcept { return m_localizations; };

  private:
    bool LoadLib();

  private:
    bool m_hasLib{};
    bool m_libInited{};
    bool m_enabled{};
    std::filesystem::path m_modPath;
    std::string m_modName{};
    std::string m_libName{};
    bool m_luaInit{};
    int64_t m_priority{};
    std::vector<std::string> m_localizations;

    IModInterface m_interface{};
};
