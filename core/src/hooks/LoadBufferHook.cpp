//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "LoadBufferHook.h"

#include "LuaManager.h"
#include "Mem.h"

static uint64_t __fastcall loadBufferHook(void *rdx, const char *buffer, size_t size, const char *name,
                                          const char *mode) {
    std::string newName{"@Content/Scripts/"};
    newName = newName + name + ".lua";

    return LuaManager::luaL_loadbufferx(LuaManager::GetLuaState(), buffer, size, newName.c_str(), mode);
}

void Hooks::LoadBufferHook::Install(void *pos) {
    // mov rcx, address
    // call rcx
    uint8_t path[] = {0x48, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xCC, 0xBB, 0xAA, 0xFF, 0xD1};

    auto *address = reinterpret_cast<uintptr_t *>(&path[2]);
    *address = reinterpret_cast<uintptr_t>(&loadBufferHook);

    Mem::MemCpyUnsafe(pos, (void *)path, sizeof(path));
}
