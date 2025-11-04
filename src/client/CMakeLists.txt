
target_sources(${CLIENT_NAME} PRIVATE
    client.cpp
    window.cpp

    input/input_manager.cpp
    input/input_action.cpp

    player/interaction_system.cpp

    camera/camera.cpp
    camera/camera_manager.cpp

    assets/asset_manager.cpp
)