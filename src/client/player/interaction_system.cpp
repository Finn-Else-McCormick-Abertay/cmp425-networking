#include "interaction_system.h"

#include <console.h>
#include <util/vec_convert.h>
#include <input/actions.h>
#include <render/render_manager.h>
#include <cmath>
#include <glaze/json.hpp>
#include <world/world_manager.h>
#include <world/coord_helpers.h>
#include <network/network_manager.h>
#include <actor/actor_manager.h>

id player::InteractionSystem::slot_to_tile_id(int slot) {
    switch (slot) {
        case 0: return "stone"_id;
        case 1: return "dirt"_id;
        default: return "unknown"_id;
    }
}

void player::InteractionSystem::tick(float dt) {

    // Hotbar
    if (actions::hotbar::slot_1.just_pressed()) _selected_hotbar_slot = 0;
    else if (actions::hotbar::slot_2.just_pressed()) _selected_hotbar_slot = 1;
    else if (actions::hotbar::slot_3.just_pressed()) _selected_hotbar_slot = 2;
    else if (actions::hotbar::slot_4.just_pressed()) _selected_hotbar_slot = 3;
    else if (actions::hotbar::slot_5.just_pressed()) _selected_hotbar_slot = 4;
    else if (actions::hotbar::slot_6.just_pressed()) _selected_hotbar_slot = 5;
    else if (actions::hotbar::slot_7.just_pressed()) _selected_hotbar_slot = 6;
    else if (actions::hotbar::slot_8.just_pressed()) _selected_hotbar_slot = 7;
    else if (actions::hotbar::slot_9.just_pressed()) _selected_hotbar_slot = 8;
    else if (actions::hotbar::slot_10.just_pressed())_selected_hotbar_slot = 9;

    if (actions::hotbar::next_slot.just_pressed()) { _selected_hotbar_slot += 1; if (_selected_hotbar_slot >= 10) _selected_hotbar_slot -= 10; }
    else if (actions::hotbar::previous_slot.just_pressed()) { _selected_hotbar_slot -= 1; if (_selected_hotbar_slot < 0) _selected_hotbar_slot += 10; }


    auto level_opt = WorldManager::level("world"_id);
    if (level_opt) {
        Level& level = *level_opt;

        if (actions::place.down()) {
            auto world_pos = RenderManager::pixel_to_world(actions::place.value());
            auto [chunk_pos, local_tile_pos] = coords::world_to_chunk_local_tile(world_pos);

            Chunk& chunk = level.get_or_make_chunk_at(chunk_pos);
            chunk[tile_layer::Foreground].set_tile_at(local_tile_pos, slot_to_tile_id(_selected_hotbar_slot));
        }
        if (actions::destroy.down()) {
            auto world_pos = RenderManager::pixel_to_world(actions::place.value());
            auto [chunk_pos, local_tile_pos] = coords::world_to_chunk_local_tile(world_pos);
            
            Chunk& chunk = level.get_or_make_chunk_at(chunk_pos);
            chunk[tile_layer::Foreground].set_tile_at(local_tile_pos, "air"_id);
        }
    }

    if (auto player_opt = NetworkManager::local_client()
        .and_then([](auto& client) { return client.has_player() ? client.uid() : nullopt; })
        .and_then(ActorManager::get_player_actor); player_opt
    ) {
        PlayerActor& player = *player_opt;

        float move_speed = 100.f;
        if (actions::run.down()) move_speed = 200.f;

        fvec2 move_vec = actions::move.value();
        move_vec.y = 0;
        move_vec *= move_speed;

        float gravity = 32.f * (float)TILE_SIZE; // Assuming 1 tile = 1 foot
        if (actions::jump.down()) gravity *= 0.6;
        fvec2 gravity_vec = fvec2(0, gravity);

        player.set_acceleration(move_vec + gravity_vec);

        if (actions::jump.just_pressed() && player.grounded()) {
            player.set_velocity(fvec2(player.velocity().x, -140.f));
        }
    }
}
