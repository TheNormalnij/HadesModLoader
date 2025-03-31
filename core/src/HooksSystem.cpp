//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "HooksSystem.h"
#include "ModApi.h"
#include "hooks/LoadBufferHook.h"
#include "hooks/CreateFileHook.h"

static HooksSystem* gHooksInstance;

std::function<void()> HooksSystem::LuaLoadCb{};

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

    Hooks::LoadBufferHook::Install((void *)m_HookTable.ScriptManager_Load_path);

    // The game resets lua state before loading a save
    // We need register our functions again when the game do that
    Hooks::CreateFileHook::Install([](const wchar_t *fileName) {
        if (LuaLoadCb && std::wstring_view(fileName).ends_with(L"Content\\Scripts\\Main.lua")) {
            LuaLoadCb();
        }
    });
}

HooksSystem *HooksSystem::Instance() { 
    if (!gHooksInstance)
        gHooksInstance = new HooksSystem();

    return gHooksInstance;
}
