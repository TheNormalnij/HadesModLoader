add_library(HadesModCore SHARED)

add_definitions(-DHADES_MOD_LOADER -DNOMINMAX)

include_directories (
   src/.
   ../libs/lua-5.2.2/src
   ../libs/libdetours/Detours/src
   ../libs/json
)

file(GLOB_RECURSE HADES_MOD_CORE_FILES src/*.cpp src/*.h)

target_sources(
    HadesModCore
    PUBLIC
        ${HADES_MOD_CORE_FILES} )

target_link_libraries(HadesModCore PRIVATE libdetours liblua)
target_precompile_headers(HadesModCore PRIVATE src/pch.h)
