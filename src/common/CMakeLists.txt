
set(COMMON_SOURCES
    terrain/tile.cpp
    terrain/tile_type.cpp
    terrain/chunk.cpp
    terrain/world.cpp

    data/data_manager.cpp
    data/namespaced_id.cpp

    system/system_manager.cpp
    system/system.cpp

    util/console.cpp
)

target_sources(${CLIENT_NAME} PRIVATE ${COMMON_SOURCES})
target_sources(${SERVER_NAME} PRIVATE ${COMMON_SOURCES})