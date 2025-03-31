//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"
#include "HooksSystem.h"
#include "detours.h"
#include "Mem.h"
#include "LuaManager.h"
#include "ModApi.h"

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

    if (gLuaLoadCb && std::wstring_view(lpFileName).ends_with(L"Content\\Scripts\\Main.lua")) {
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
    struct LibToGameVariant {
        const char *name;
        eGameVariant variant;
    };

    const LibToGameVariant librarires[2] = {{"EngineWin64sv.dll", eGameVariant::VULKAN},
                                            {"EngineWin64s.dll", eGameVariant::DX12}};

    HANDLE libraryHandle = 0;
    for (size_t i = 0; i < 2; i++) {
        libraryHandle = LoadLibraryA(librarires[i].name);
        if (libraryHandle) {
            gModApi.gameVariant = librarires[i].variant;
            break;
        }
    }

    if (!libraryHandle)
        return;

    m_GameDllOffset = reinterpret_cast<uint64_t>(libraryHandle);

    m_HookTable.Init();
    m_HookTable.ApplyOffset(m_GameDllOffset);
    PathBufferNames();
}

static uint64_t __fastcall loadBufferHook(void* rdx, const char* buffer, size_t size, const char* name,
    const char* mode) {
    std::string newName{"@Content/Scripts/"};
    newName = newName + name + ".lua";

    return LuaManager::luaL_loadbufferx(LuaManager::GetLuaState(), buffer, size, newName.c_str(), mode);
}

void HooksSystem::PathBufferNames() {
    // mov rcx, address
    // call rcx
    uint8_t path[] = {0x48, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xCC, 0xBB, 0xAA, 0xFF, 0xD1};

    auto *address = reinterpret_cast<uintptr_t *>(&path[2]);
    *address = reinterpret_cast<uintptr_t>(&loadBufferHook);

    Mem::MemCpyUnsafe((void *)m_HookTable.ScriptManager_Load_path, (void *)path, sizeof(path));
}

void HooksSystem::SetLuaLoadCallback(std::function<void()> callback) {
    gLuaLoadCb = callback;
    CreateFileHook();
}

HooksSystem *HooksSystem::Instance() { 
    if (!gHooksInstance)
        gHooksInstance = new HooksSystem();

    return gHooksInstance;
}
