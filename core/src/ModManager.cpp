//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include <filesystem>
#include "ModManager.h"
#include "HooksSystem.h"
#include "lua/ModLuaDefs.h"
#include "LuaManager.h"

static ModManager gModCore{}; 

void ModManager::Init() {
    ScanMods();
    HooksSystem::Instance()->SetLuaLoadCallback([this]() { this->OnLuaCreated(); });
}

void ModManager::Deinit() {

}

ModManager *ModManager::Instance() { return &gModCore; }

void ModManager::OnLuaCreated() {
    LuaManager::Reinit();
    auto luaState = LuaManager::GetLuaState();

    for (auto &mod : m_Mods) {
        mod->OnLuaCreated(luaState);
    }
}

void ModManager::ScanMods() {
    namespace fs = std::filesystem;
    using path = fs::path;

    if (!fs::is_directory("../Content/HeavenMods"))
        return;

    for (auto &modDir : fs::directory_iterator("../Content/HeavenMods")) {
        if (!fs::is_directory(modDir))
            continue;
         
        auto mod = std::make_unique<Mod>(modDir);

        if (mod->Load())
            m_Mods.emplace_back(std::move(mod));
    }
}
