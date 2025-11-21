
target_sources(${CLIENT_NAME} PRIVATE
    client.cpp
    window.cpp

    input/input_manager.cpp
    input/input_action.cpp

    player/interaction_system.cpp

    render/camera.cpp
    render/render_manager.cpp

    assets/asset_manager.cpp

    debug/debug_system.cpp
)