//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "HooksSystem.h"

#include "ModApi.h"
#include "hooks/LoadBufferHook.h"
#include "hooks/CreateFileHook.h"

const struct {
    const char *name;
    eGameVariant variant;
} librarires[2] = {{"EngineWin64sv.dll", eGameVariant::VULKAN}, {"EngineWin64s.dll", eGameVariant::DX12}};

static HooksSystem* gHooksInstance;

std::function<void()> HooksSystem::LuaLoadCb{};

HooksSystem::HooksSystem() {
    size_t libraryVariant = 0;

    HANDLE libraryHandle = 0;
    for (size_t i = 0; i < 2; i++) {
        libraryHandle = LoadLibraryA(librarires[i].name);
        if (libraryHandle) {
            libraryVariant = i;
            gModApi.gameVariant = librarires[i].variant;
            break;
        }
    }

    if (!libraryHandle)
        return;

    m_GameDllHandle = libraryHandle;

    m_symLoader.Initialize();
    m_symLoader.LoadModuleSymbols(libraryHandle, librarires[libraryVariant].name);

    InitHookTable();

    Hooks::LoadBufferHook::Install(m_symLoader, m_HookTable.luaL_loadbufferx);

    // The game resets lua state before loading a save
    // We need register our functions again when the game do that
    Hooks::CreateFileHook::Install([](const wchar_t *fileName) {
        if (LuaLoadCb && std::wstring_view(fileName).ends_with(L"Content\\Scripts\\Main.lua")) {
            LuaLoadCb();
        }
    });
}

void HooksSystem::InitHookTable() {
    m_HookTable.luaState = m_symLoader.GetSymbolAddress("sgg::ScriptManager::LUA_INTERFACE");
    m_HookTable.lua_pcallk = m_symLoader.GetSymbolAddress("lua_pcallk");
    m_HookTable.luaL_loadbufferx = m_symLoader.GetSymbolAddress("luaL_loadbufferx");
}

HooksSystem *HooksSystem::Instance() { 
    if (!gHooksInstance)
        gHooksInstance = new HooksSystem();

    return gHooksInstance;
}
