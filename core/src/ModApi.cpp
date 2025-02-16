//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "include/HadesModApi.h"
#include "HooksSystem.h"

static uint64_t GetGameDllOffset() { return HooksSystem::Instance()->GetGameDllOffset(); }

IModApi gModApi{.version{MOD_API_VERSION}, .gameVariant{eGameVariant::VULKAN}, .GetGameDllOffset = GetGameDllOffset};
