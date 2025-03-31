//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <vector>
#include <memory>
#include "Mod.h"

class ModManager {
  public:
    void Init();
    void Deinit();

    static ModManager *Instance();

  private:
    void OnLuaCreated();
    void ScanMods();
    void SortMods();

  private:
    std::vector<std::unique_ptr<Mod>> m_Mods;
};
