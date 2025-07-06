//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <string>
#include <vector>

class ModMetaParser {
  public:
    bool Parse(const char *data, size_t size);

  public:
    std::string name;
    std::string libName;
    uint64_t priority;
    std::vector<std::string> localizations;
};