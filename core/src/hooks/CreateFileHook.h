//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <functional>

namespace Hooks::CreateFileHook {
void Install(std::function<void(const wchar_t*)> handler);
}
