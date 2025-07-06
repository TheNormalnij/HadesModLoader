//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ModMetaParser.h"
#include <json.h>

bool ModMetaParser::Parse(const char *data, size_t size) {
    constexpr size_t MAX_SJSON_TOKENS = 200;
    json_token_t tokens[MAX_SJSON_TOKENS]{};

    const size_t tokensCount = sjson_parse(data, size, tokens, MAX_SJSON_TOKENS);

    if (tokensCount > MAX_SJSON_TOKENS) {
        // Whole file was parsed
        return false;
    }

    if (tokensCount < 1 || tokens[0].type != JSON_OBJECT) {
        // Should start from object
        return false;
    }

    size_t tokenId = 1;
    while (tokenId) {
        const auto &token = tokens[tokenId];

        std::string_view key{data + token.id, token.id_length};

        switch (token.type) {
        case JSON_STRING: {
            if (key == "Name") {
                name = std::string{data + token.value, token.value_length};
            } else if (key == "Library") {
                libName = std::string{data + token.value, token.value_length};
            }
            break;
        }
        case JSON_PRIMITIVE: {
            if (key == "Priority") {
                priority = std::stoi(std::string{data + token.value, token.value_length});
            }
            break;
        }
        case JSON_ARRAY: {
            if (key == "Localization") {
                const size_t childsCount = token.value_length;
                localizations.reserve(childsCount);

                if (childsCount > 0) {
                    auto* childToken = &tokens[token.child];
                    for (size_t i = 0; i < childsCount; i++) {
                        if (childToken->type == JSON_STRING && childToken->value_length) {
                            std::string path{};
                            path.resize(childToken->value_length);

                            size_t newSize = json_unescape(&path.at(0), childToken->value_length,
                                                           data + childToken->value, childToken->value_length);

                            path.resize(newSize);
                            localizations.push_back(std::move(path));
                        }
                        childToken = &tokens[childToken->sibling];
                    }
                }
            }
            break;
        }
        }

        tokenId = token.sibling;
    }
    return true;
}
