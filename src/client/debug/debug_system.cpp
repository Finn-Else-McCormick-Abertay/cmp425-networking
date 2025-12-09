#include "debug_system.h"

#include <input/actions.h>
#include <assets/asset_manager.h>
#include <render/render_manager.h>
#include <network/network_manager.h>

DebugSystem::DebugSystem(World* world) : _world(world) {}

void DebugSystem::tick(float dt) {
    if (actions::debug::modifier.down()) {
        if (actions::debug::tile.just_pressed()) _show_tile_debug = !_show_tile_debug;
        if (actions::debug::network.just_pressed()) _show_network_debug = !_show_network_debug;
    }
}

dyn_arr<draw_layer> DebugSystem::draw_layers() const { return { layers::tile::foreground + 1, layers::debug::ui_overlay }; }
void DebugSystem::draw(sf::RenderTarget& target, draw_layer layer) {
    if (layer == layers::tile::foreground + 1 && _show_tile_debug && _world) {
        sf::RectangleShape chunk_rect(sf::fvec2(TILE_SIZE * Chunk::SIZE_TILES, TILE_SIZE * Chunk::SIZE_TILES));
        chunk_rect.setFillColor(sf::Color::Transparent);
        chunk_rect.setOutlineColor(sf::Color::Cyan);
        chunk_rect.setOutlineThickness(1.f);
        for (auto& chunk : *_world) {
            auto& chunk_coords = chunk.get_coords();
            auto true_chunk_coords = chunk_coords * (int)Chunk::SIZE_TILES * (int)TILE_SIZE;
            chunk_rect.setPosition(to_sfvec_of<float>(true_chunk_coords));
            target.draw(chunk_rect);
        }
    }

    if (layer == layers::debug::ui_overlay && (_show_tile_debug || _show_network_debug)) {
        auto font_opt = assets::Manager::get_font("monogram"_id);
        if (!font_opt) return;
        const Font& font = font_opt.value();
        
        auto cam_opt = RenderManager::ui_camera();
        if (!cam_opt) return;
        Camera& cam = cam_opt.value();

        if (_show_tile_debug && _world) {
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
            
            sf::Text debug_text(font, msg, 16);
            debug_text.setOutlineThickness(2);
            debug_text.setOutlineColor(sf::Color::Black);
            debug_text.setPosition(to_sfvec(RenderManager::pixel_to_ui(actions::cursor.value())));
            target.draw(debug_text);
        }

        if (_show_network_debug) {
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