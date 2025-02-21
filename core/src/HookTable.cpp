//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "HookTable.h"

void HookTable::ApplyOffset(uintptr_t offset) {
    uintptr_t *self = reinterpret_cast<uintptr_t*>(this);
    for (size_t i = 0; i < sizeof(HookTable) / sizeof(uintptr_t); i++) {
        *self += offset;
        self++;
    }
}

void HookTable::Init() {
    luaState = 0x6D82C0;
    lua_pcallk = 0x3A6140;
}
