//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "Mem.h"

void Mem::MemSetUnsafe(void *dest, int val, size_t size) {
    DWORD oldProtect;
    VirtualProtect(dest, 1024, PAGE_EXECUTE_READWRITE, &oldProtect);

    // Disable control hotswap
    std::memset(dest, val, size);

    DWORD restoredFrom;
    VirtualProtect(dest, 1024, oldProtect, &restoredFrom);
}

void Mem::MemCpyUnsafe(void *dest, void *src, size_t size) {
    DWORD oldProtect;
    VirtualProtect(dest, 1024, PAGE_EXECUTE_READWRITE, &oldProtect);

    // Disable control hotswap
    std::memcpy(dest, src, size);

    DWORD restoredFrom;
    VirtualProtect(dest, 1024, oldProtect, &restoredFrom);
}
