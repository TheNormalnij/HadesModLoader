//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <dbghelp.h>

class SymbolLoader {
  public:
    SymbolLoader() {};

    uintptr_t GetSymbolAddress(const char *sym) const noexcept;

    bool Initialize();
    bool LoadModuleSymbols(HANDLE handle, const char *dllPath);
    void Cleanup();

  private:
    decltype(&SymSetOptions) SymSetOptions{};
    decltype(&SymInitialize) SymInitialize{};
    decltype(&SymCleanup) SymCleanup{};
    decltype(&SymFromName) SymFromName{};
    decltype(&SymLoadModuleEx) SymLoadModuleEx{};
};
