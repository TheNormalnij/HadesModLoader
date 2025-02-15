//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "HooksSystem.h"
#include "detours.h"

static HooksSystem* gHooksInstance;

static std::function<void()> gLuaLoadCb{};

typedef HANDLE(WINAPI *CreateFileW_t)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

CreateFileW_t OriginalCreateFileW = NULL;

static void RemoveCreateFileHook();

HANDLE WINAPI HookedCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {

    if (gLuaLoadCb && std::wstring(lpFileName).ends_with(L"Content\\Scripts\\Main.lua")) {
        // The game resets lua state before loading a save
        // We need register our functions again when the game do that
        gLuaLoadCb();
    }

    return OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                               dwFlagsAndAttributes, hTemplateFile);
}

static void CreateFileHook() {
    OriginalCreateFileW = (CreateFileW_t)DetourFindFunction("kernel32.dll", "CreateFileW");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID &)OriginalCreateFileW, HookedCreateFileW);
    DetourTransactionCommit();
}

static void RemoveCreateFileHook() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID &)OriginalCreateFileW, HookedCreateFileW);
    DetourTransactionCommit();
}

HooksSystem::HooksSystem() { 
    HANDLE libraryHandle = LoadLibraryA("EngineWin64sv.dll");
    m_HookTable.Init();
    m_HookTable.ApplyOffset(reinterpret_cast<uintptr_t>(libraryHandle));
}

void HooksSystem::SetLuaLoadCallback(std::function<void()> callback) {
    gLuaLoadCb = callback;
    CreateFileHook();
}

HooksSystem *HooksSystem::Instance() { 
    if (gHooksInstance)
        return gHooksInstance;
    else {
        gHooksInstance = new HooksSystem();
    }
}
