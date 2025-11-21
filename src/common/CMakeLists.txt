
set(COMMON_SOURCES
    terrain/chunk.cpp
    terrain/world.cpp

    data/data_manager.cpp
    data/handle/handles.cpp

    save/save_manager.cpp

    render/drawable.cpp

    system/system_manager.cpp
    system/system.cpp

    console/print.cpp
)

target_sources(${CLIENT_NAME} PRIVATE ${COMMON_SOURCES})
target_sources(${SERVER_NAME} PRIVATE ${COMMON_SOURCES})