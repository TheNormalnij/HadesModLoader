//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include "HookTable.h"
#include <functional>
#include "SymbolLoader.h"

class HooksSystem {
  public:
    void SetLuaLoadCallback(std::function<void()> callback) { LuaLoadCb = callback; };
    const HookTable &GetHoohTable() const noexcept { return m_HookTable; };
    void *GetGameDllHandle() const noexcept { return m_GameDllHandle; };
    SymbolLoader &GetSymbolLoader() { return m_symLoader; };
    static HooksSystem *Instance();

  private:
    HooksSystem();
    void InitHookTable();

  private:
    static std::function<void()> LuaLoadCb;
    HookTable m_HookTable{};
    HANDLE m_GameDllHandle{};
    SymbolLoader m_symLoader{};
};
