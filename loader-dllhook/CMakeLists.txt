add_library(HadesModLoaderDllHook SHARED)

target_sources(
    HadesModLoaderDllHook
    PUBLIC
        src/main.cpp
    PRIVATE
        xinput_exports.def
)

if(MSVC)
    target_link_options(HadesModLoaderDllHook PRIVATE "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/xinput_exports.def")
endif()

target_link_libraries(HadesModLoaderDllHook PUBLIC "Xinput9_1_0")
set_target_properties(HadesModLoaderDllHook PROPERTIES OUTPUT_NAME "Xinput9_1_0")
