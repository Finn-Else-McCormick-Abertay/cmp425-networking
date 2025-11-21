#include "debug_system.h"

#include <input/actions.h>
#include <assets/asset_manager.h>
#include <render/render_manager.h>

DebugSystem::DebugSystem(World* world) : _world(world) {}

void DebugSystem::tick(float dt) {
    if (actions::toggle_debug.just_pressed()) _show_debug = !_show_debug;
}

dyn_arr<draw_layer> DebugSystem::draw_layers() const { return { layers::debug::ui_overlay }; }
void DebugSystem::draw(sf::RenderTarget& target, draw_layer layer) {
    if (layer == layers::debug::ui_overlay && _show_debug) {
        auto font_opt = assets::Manager::get_font("monogram"_id);
        if (!font_opt) return;
        const Font& font = font_opt.value();

        auto world_tile_pos = RenderManager::pixel_to_world(actions::cursor.value()) / TILE_SIZE;
        auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
        auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));

        str tile_msg;
        if (auto chunk = _world->chunk_at(chunk_pos)) {
            if (chunk->has(layers::tile::foreground)) {
                auto& layer = chunk->at(layers::tile::foreground);
                auto connections = layer.find_connections_at(local_tile_pos);
                tile_msg += fmt::format("{} {} {}\n{}   {}\n{} {} {}\n",
                    (int)connections[2], (int)connections[5], (int)connections[3],
                    (int)connections[7], (int)connections[6],
                    (int)connections[1], (int)connections[4], (int)connections[0]
                );
                tile_msg += fmt::format("({})", connections);
            }
        }

        str msg = fmt::format("Chunk {}\nTile{}\n{}", chunk_pos, local_tile_pos, tile_msg);
        
        sf::Text debug_text(font, "", 16);
        debug_text.setOutlineThickness(2);
        debug_text.setOutlineColor(sf::Color::Black);

        debug_text.setPosition(to_sfvec(RenderManager::pixel_to_ui(actions::cursor.value())));
        debug_text.setString(msg);
        target.draw(debug_text);
    }
}