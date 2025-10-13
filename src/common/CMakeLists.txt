
set(COMMON_SOURCES
    terrain/tile.cpp
    terrain/chunk.cpp
    terrain/world.cpp
)

target_sources(${CLIENT_NAME} PRIVATE ${COMMON_SOURCES})
target_sources(${SERVER_NAME} PRIVATE ${COMMON_SOURCES})