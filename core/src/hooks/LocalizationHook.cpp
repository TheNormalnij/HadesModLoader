//
// Copyright (c) Uladzislau Nikalayevich <thenormalnij@gmail.com>. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "pch.h"

#include "FunctionHook.h"
#include "LocalizationHook.h"

static std::vector<std::string> localizations{};

static FunctionHook<"sgg::GameDataManager::ReadTextData", void> hook;

using ReadTextData_params_t = void(__fastcall *)(const char *filePath, void *textStorage, void *localization);
static ReadTextData_params_t ReadTextData{};

using fsSetPathForResourceDir_t = void(__fastcall *)(void *pIO, int mount, int id, const char *path);
static fsSetPathForResourceDir_t fsSetPathForResourceDir{};

static void *TEXT_DATA{};
static void *UiLang{};

struct ResourceDirectoryInfo {
    void *pIO;
    char path[512];
    bool bundled;
};
static ResourceDirectoryInfo *pTextResourcePath{};

constexpr int MOUNT_NONE = 0x0;
constexpr int TEXT_RESOPURCES = 0x1C;

void setTextDataPath(const char *path) {
    // Invalidate string
    pTextResourcePath->path[0] = 0;
    fsSetPathForResourceDir(pTextResourcePath->pIO, MOUNT_NONE, TEXT_RESOPURCES, path);
}

void Hooks::LocalizationHook::Install(SymbolLoader &symLoader) {
    hook.Install(reinterpret_cast<void *>(symLoader.GetSymbolAddress("sgg::GameDataManager::ReadTextData")), 12);

    ReadTextData = reinterpret_cast<ReadTextData_params_t>(symLoader.GetSymbolAddress(
        "?ReadTextData@GameDataManager@sgg@@SAXPEBDAEAV?$string_hash_map@PEAVTextData@sgg@@U?$hash@PEBD@eastl@@U?$str_"
        "equal_to@PEBD@4@Vallocator_forge@4@@eastl@@AEBV?$basic_string@DVallocator_forge@eastl@@@4@@Z"));

    TEXT_DATA = reinterpret_cast<void *>(symLoader.GetSymbolAddress("sgg::GameDataManager::TEXT_DATA"));
    UiLang = reinterpret_cast<void *>(symLoader.GetSymbolAddress("sgg::Localization::UiLang"));
    pTextResourcePath = reinterpret_cast<ResourceDirectoryInfo *>(symLoader.GetSymbolAddress("gResourceDirectories") +
                                                                  TEXT_RESOPURCES * sizeof(ResourceDirectoryInfo));

    fsSetPathForResourceDir =
        reinterpret_cast<fsSetPathForResourceDir_t>(symLoader.GetSymbolAddress("fsSetPathForResourceDir"));

    hook.onPostFunction = []() {
        setTextDataPath("../Content/Mods");
        for (const auto &path : localizations) {
            ReadTextData(path.c_str(), TEXT_DATA, UiLang);
        }

        setTextDataPath("../Content/Game/Text");
    };
}

void Hooks::LocalizationHook::AddLocalizationPath(std::string path) { localizations.push_back(path); }
