//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "HookTable.h"
#include <functional>

class lua_State;

class HooksSystem {
  public:
    void SetLuaLoadCallback(std::function<void()> callback) { LuaLoadCb = callback; };
    const HookTable &GetHoohTable() const noexcept { return m_HookTable; };
    uint64_t GetGameDllOffset() const noexcept { return m_GameDllOffset; };
    static HooksSystem *Instance();

  private:
    HooksSystem();

  private:
    static std::function<void()> LuaLoadCb;
    HookTable m_HookTable{};
    uint64_t m_GameDllOffset{};
};
