#include "debug_system.h"

#include <input/actions.h>
#include <assets/asset_manager.h>
#include <render/render_manager.h>
#include <network/network_manager.h>
#include <world/world_manager.h>

void DebugSystem::tick(float dt) {
    if (actions::debug::modifier.down()) {
        if (actions::debug::tile.just_pressed()) _show_tile_debug = !_show_tile_debug;
        if (actions::debug::network.just_pressed()) _show_network_debug = !_show_network_debug;
    }
}

dyn_arr<draw_layer> DebugSystem::draw_layers() const { return { layers::tile::foreground + 1, layers::debug::ui_overlay }; }
void DebugSystem::draw(sf::RenderTarget& target, draw_layer layer) {
    auto level_opt = WorldManager::level("world"_id);

    if (layer == layers::tile::foreground + 1 && _show_tile_debug && level_opt) {
        sf::RectangleShape chunk_rect(sf::fvec2(TILE_SIZE * Chunk::SIZE_TILES, TILE_SIZE * Chunk::SIZE_TILES));
        chunk_rect.setFillColor(sf::Color::Transparent);
        chunk_rect.setOutlineColor(sf::Color::Cyan);
        chunk_rect.setOutlineThickness(1.f);
        for (auto& chunk : level_opt->get().chunks()) {
            auto& chunk_coords = chunk.pos();
            auto true_chunk_coords = chunk_coords * (int)Chunk::SIZE_TILES * (int)TILE_SIZE;
            chunk_rect.setPosition(to_sfvec_of<float>(true_chunk_coords));
            target.draw(chunk_rect);
        }
    }

    if (layer == layers::debug::ui_overlay && (_show_tile_debug || _show_network_debug)) {
        auto& font = AssetManager::get_font("monogram"_id);
        
        if (_show_tile_debug) {
            auto world_tile_pos = RenderManager::pixel_to_world(actions::cursor.value()) / TILE_SIZE;
            auto chunk_pos = ivec2(floorf(world_tile_pos.x / Chunk::SIZE_TILES), floorf(world_tile_pos.y / Chunk::SIZE_TILES));
            auto local_tile_pos = to_uvec(world_tile_pos - (chunk_pos * (float)Chunk::SIZE_TILES));

            str tile_msg;
            /*if (auto chunk_opt = _world->chunk_at(chunk_pos)) {
                Chunk& chunk = chunk_opt.value();
                if (chunk.has(layers::tile::foreground)) {
                    auto& layer = chunk.at(layers::tile::foreground);
                    auto connections = layer.find_connections_at(local_tile_pos);
                    tile_msg += fmt::format("{} {} {}\n{}   {}\n{} {} {}\n",
                        (int)connections[2], (int)connections[5], (int)connections[3],
                        (int)connections[7], (int)connections[6],
                        (int)connections[1], (int)connections[4], (int)connections[0]
                    );
                    tile_msg += fmt::format("({})", connections);
                }
            }*/

            str msg = fmt::format("Chunk {}\nTile{}\n{}", chunk_pos, local_tile_pos, tile_msg);
            
            sf::Text debug_text(font, msg, 16);
            debug_text.setOutlineThickness(2);
            debug_text.setOutlineColor(sf::Color::Black);
            debug_text.setPosition(to_sfvec(RenderManager::pixel_to_ui(actions::cursor.value())));
            target.draw(debug_text);
        }

        auto cam_opt = RenderManager::ui_camera();
        if (_show_network_debug && cam_opt) {
            Camera& cam = *cam_opt;
            auto cam_size = cam.as_view().getSize();
            auto top_left = -cam_size / 2.f + sf::fvec2(6.f, -2.f);

            str debug_msg = NetworkManager::debug_message();

            sf::Text debug_text(font, debug_msg, 16);
            debug_text.setOutlineThickness(2);
            debug_text.setOutlineColor(sf::Color::Black);
            debug_text.setPosition(top_left);
            target.draw(debug_text);
        }
    }
}