//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include "SymbolLoader.h"

namespace Hooks::LocalizationHook {
void Install(SymbolLoader &symLoader);
void AddLocalizationPath(std::string path);
}
