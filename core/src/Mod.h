//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <filesystem>
#include "include/HadesModApi.h"

class lua_State;

class Mod {
  public:
    Mod(const std::filesystem::directory_entry &path) : m_modPath{path} {};

    bool Load();
    void OnLuaCreated(lua_State *luaState);

  private:
    using ModuleInit_t = void(__cdecl *)(IModApi*);
    using LuaCreatedApi_t = void(__cdecl *)(lua_State *);
      
    std::filesystem::path m_modPath;
    bool m_luaInit{};

    LuaCreatedApi_t m_createdCallback{};
};