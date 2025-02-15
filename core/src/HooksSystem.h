//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <functional>
#include "HookTable.h"

class lua_State;

class HooksSystem {
  public:
    void SetLuaLoadCallback(std::function<void()> callback);
    const HookTable &GetHoohTable() const noexcept { return m_HookTable; };
    static HooksSystem *Instance(); 

private:
    HooksSystem();

private:
    HookTable m_HookTable{};
};
