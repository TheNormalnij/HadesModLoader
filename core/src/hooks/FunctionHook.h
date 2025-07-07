//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <functional>
#include <vector>
#include <algorithm>

#include "Mem.h"

template <size_t N> struct HookNameStr {
    constexpr HookNameStr(const char (&str)[N]) { std::copy_n(str, N, value); }
    char value[N];
};

template <HookNameStr functionName, typename ReturnT, typename... Args> class FunctionHook {
  public:
    FunctionHook() { instance = this; };
    ~FunctionHook() { Uninstall(); }

    void Install(void *dest, size_t size) {
        hookPos = dest;
        hookSize = size;

        Install();
    }

    void Install() {
        if (installed)
            return;

        if (!hookPos)
            return;

        BackupOriginalCode();
        InitializeJumpCode();
        CreateJump();
        installed = true;
    };

    void Uninstall() {
        if (!installed)
            return;

        RestoreOriginalCode();
        installed = false;
    }

  private:
    void BackupOriginalCode() {
        overridedData.resize(hookSize);
        std::memcpy(overridedData.data(), hookPos, hookSize);
    };

    void InitializeJumpCode() {
        patch.resize(hookSize, 0x90);

        // Mov RAX, <address>
        patch[0] = 0x48;
        patch[1] = 0xB8;

        *reinterpret_cast<uintptr_t *>(&patch.at(2)) = reinterpret_cast<uintptr_t>(&JumpHandler);

        // JMP RAX
        patch[10] = 0xFF;
        patch[11] = 0xE0;
    }

    void CreateJump() { Mem::MemCpyUnsafe(hookPos, (void *)patch.data(), patch.size()); };

    void RestoreOriginalCode() { Mem::MemCpyUnsafe(hookPos, (void *)overridedData.data(), overridedData.size()); }

    auto Handler(Args... args) {
        RestoreOriginalCode();
        if (onPreFunction) {
            onPreFunction(args...);
        }

        if constexpr (std::is_same_v<ReturnT, void>) {
            reinterpret_cast<void(__fastcall *)(Args...)>(hookPos)(args...);
            CreateJump();
            if (onPostFunction)
                onPostFunction();
            return;
        } else {
            ReturnT ret = reinterpret_cast<ReturnT(__fastcall *)(Args...)>(hookPos)(args...);
            CreateJump();
            return onPostFunction ? onPostFunction(ret) : ret;
        }

    }

    static auto JumpHandler(Args... args) { return instance->Handler(args...); }

    static inline FunctionHook *instance{};

  private:
    void *hookPos{};
    size_t hookSize{};
    bool installed{};

    std::vector<uint8_t> overridedData{};
    std::vector<uint8_t> patch{};

  public:
    std::function<void(Args&...)> onPreFunction;
    std::function<ReturnT(ReturnT)> onPostFunction;
};
