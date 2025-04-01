//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

struct HookTable {
    uintptr_t luaState;
    uintptr_t lua_pcallk;
    uintptr_t luaL_loadbufferx;
};
