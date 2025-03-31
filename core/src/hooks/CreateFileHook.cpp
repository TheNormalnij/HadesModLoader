//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "CreateFileHook.h"
#include "detours.h"

static std::function<void(const wchar_t *)> preLoadFile;

using CreateFileW_t = HANDLE(WINAPI *)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

CreateFileW_t OriginalCreateFileW = nullptr;

static void RemoveCreateFileHook();

HANDLE WINAPI HookedCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    preLoadFile(lpFileName);
    return OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                               dwFlagsAndAttributes, hTemplateFile);
}

static void AddCreateFileHook() {
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


void Hooks::CreateFileHook::Install(std::function<void(const wchar_t *)> handler) {
    preLoadFile = handler;
    AddCreateFileHook();
}
