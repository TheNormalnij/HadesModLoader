//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "SymbolLoader.h"

bool SymbolLoader::Initialize() {
    HINSTANCE dbgHelper = LoadLibraryA("dbghelp.dll");

    if (!dbgHelper)
        return false;

    SymSetOptions = (decltype(SymSetOptions))GetProcAddress(dbgHelper, "SymSetOptions");
    SymInitialize = (decltype(SymInitialize))GetProcAddress(dbgHelper, "SymInitialize");
    SymCleanup = (decltype(SymCleanup))GetProcAddress(dbgHelper, "SymCleanup");
    SymFromName = (decltype(SymFromName))GetProcAddress(dbgHelper, "SymFromName");
    SymLoadModuleEx = (decltype(SymLoadModuleEx))GetProcAddress(dbgHelper, "SymLoadModuleEx");

    SymSetOptions(SYMOPT_DEFERRED_LOADS);
    return SymInitialize(GetCurrentProcess(), nullptr, FALSE) != 0;
}

uintptr_t SymbolLoader::GetSymbolAddress(const char *name) const noexcept {
    SYMBOL_INFO_PACKAGE symbolInfo = {0};
    symbolInfo.si.SizeOfStruct = sizeof(SYMBOL_INFO);
    symbolInfo.si.MaxNameLen = MAX_SYM_NAME;

    if (!SymFromName(GetCurrentProcess(), name, &symbolInfo.si))
        return 0;

    return symbolInfo.si.Address;
}

bool SymbolLoader::LoadModuleSymbols(HANDLE handle, const char *moduleName) {
    HANDLE proc = GetCurrentProcess();
    return SymLoadModuleEx(proc, NULL, moduleName, NULL, (DWORD64)handle, 0, NULL, 0) != 0;
}

void SymbolLoader::Cleanup() { SymCleanup(GetCurrentProcess()); }
