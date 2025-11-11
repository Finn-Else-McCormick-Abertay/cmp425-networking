#include <util/console.h>
#include <util/prelude.h>

#include <window.h>
#include <input/input_manager.h>

#include <SFML/Graphics.hpp>
#include <terrain/world.h>
#include <player/interaction_system.h>
#include <camera/camera.h>

#define __INPUT_ACTION_SYMBOL_DEFINITIONS__
#include <input/actions.h>

#include <glaze/json.hpp>

#include <util/helper/enum_serialization.h>

#include <data/data_manager.h>
#include <data/namespaced_id.h>

#include <assets/asset_manager.h>
#include <type_traits>

#include <save/save_manager.h>

#include <util/std_aliases.h>

using namespace std;

int main() {

    InputManager::init();
    InputManager::setup_default_binds();

    data::Manager::reload();
    
    World world = SaveManager::load().or_else([](){ return std::make_optional<World>(); }).value();

    auto player_camera = Camera("player");
    auto interaction_system = player::InteractionSystem(&world);

    Window window;

    window.set_close_request_callback([](Window& window) -> bool {
        print<debug>("Window: Close Requested");
        return true;
    });
    window.set_draw_callback([&](sf::RenderTarget& target) {
        auto chunk_true_size = Chunk::SIZE_TILES * Tile::SIZE;

        auto chunk_debug_rect = sf::RectangleShape(sf_fvec2(chunk_true_size - 2, chunk_true_size - 2));
        chunk_debug_rect.setOutlineColor(sf::Color(255, 255, 255, 100));
        chunk_debug_rect.setOutlineThickness(1);
        chunk_debug_rect.setFillColor(sf::Color::Transparent);
        
        sf::RectangleShape tile_rect(sf_fvec2(Tile::SIZE, Tile::SIZE));

        for (auto& chunk : world) {
            ivec2 chunk_true_coords(chunk.get_coords() * chunk_true_size);
            
            // Draw debug rect
            chunk_debug_rect.setPosition(to_sfvec_of<float>(chunk_true_coords + ivec2(1, 1)));
            target.draw(chunk_debug_rect);

            // Draw tiles
            for (auto [local_pos, tile] : chunk) {
                if (tile->type().model_type() == data::TileHandle::ModelType::Block) {
                    tile_rect.setTexture(&assets::Manager::get_tile_texture(tile->type().id()));
                    tile_rect.setPosition(to_sfvec_of<float>(chunk_true_coords + to_fvec(local_pos) * Tile::SIZE));

                    uint8 shape = (uint8)tile->shape();
                    uvec2 texture_tile_index = uvec2(shape % 6, shape / 6);
                    tile_rect.setTextureRect(sf::IntRect(to_sfvec_of<int>(texture_tile_index * Tile::SIZE), to_sfvec(ivec2(Tile::SIZE, Tile::SIZE))));
                    target.draw(tile_rect);
                }
            }
        }
    });

    window.enter_main_loop();

    SaveManager::save(world);
}