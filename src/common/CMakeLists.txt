
set(COMMON_SOURCES
    game_loop.cpp

    network/network_manager.cpp
    network/networked.cpp
    network/network_id.cpp
    network/packet.cpp

    world/world_manager.cpp
    world/world.cpp
    world/level.cpp
    world/chunk.cpp

    actor/actor_manager.cpp
    actor/actor.cpp
    actor/player_actor.cpp

    data/data_manager.cpp
    data/namespaced_id.cpp
    data/handle/handles.cpp

    save/save_manager.cpp

    render/drawable.cpp

    system/system_manager.cpp
    system/system.cpp

    cli/components.cpp
    console/print.cpp
)

target_sources(${CLIENT_NAME} PRIVATE ${COMMON_SOURCES})
target_sources(${SERVER_NAME} PRIVATE ${COMMON_SOURCES})