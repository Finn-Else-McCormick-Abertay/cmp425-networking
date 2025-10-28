#include <SFML/Graphics.hpp>
#include <util/console.h>
#include <terrain/world.h>
#include <window.h>
#include <util/prelude.h>

#include <map>
#include <iostream>

using namespace std;

int main() {
    console::info("Client init");
    
    auto world = World();

    auto chunk = make_optional<Chunk>();
    chunk->set_tile_at({0, 1}, Tile(Tile::Stone));
    chunk->set_tile_at({1, 1}, Tile(Tile::Stone));
    chunk->set_tile_at({2, 1}, Tile(Tile::Stone));
    chunk->set_tile_at({1, 2}, Tile(Tile::Stone));

    world.set_chunk({0,0}, chunk);

    Window window;

    sf::Texture placeholder_texture = sf::Texture("resources/assets/textures/placeholder.png");
    placeholder_texture.setRepeated(true);
    
    sf::Texture tileset = sf::Texture("resources/assets/textures/tileset/template.png");
    tileset.setSmooth(false);

    window.set_close_request_callback([](Window& window) -> bool {
        console::debug("Window: Close Requested");
        return true;
    });
    window.set_on_resized_callback([](Window& window, uvec2 new_size) {
        console::debug("Window: Resized to {}", new_size);
    });
    window.set_draw_callback([&](sf::RenderTarget& target) {
        float viewHeightLogical = 300;
        float windowAspect = (float)target.getSize().y / target.getSize().x;

        target.setView(sf::View({0.f, 0.f}, {viewHeightLogical / windowAspect, viewHeightLogical}));

        auto chunk_true_size = Chunk::SIZE_TILES * Tile::SIZE;

        auto chunk_debug_rect = sf::RectangleShape(to_sfvec_of<float>(chunk_true_size));
        chunk_debug_rect.setOutlineColor(sf::Color(255, 255, 255, 100));
        chunk_debug_rect.setOutlineThickness(0.5);
        chunk_debug_rect.setFillColor(sf::Color::Transparent);

        auto make_tile_rect = [](sf::Color fill, sf::Color outline = sf::Color::Transparent) -> sf::RectangleShape {
            sf::RectangleShape rect(sf_fvec2(Tile::SIZE, Tile::SIZE));
            rect.setFillColor(fill); rect.setOutlineColor(outline); rect.setOutlineThickness(1);
            return rect;
        };
        map<Tile::Type, sf::RectangleShape> tile_rects = {
            {Tile::Air, make_tile_rect(sf::Color::Transparent)},
            {Tile::Stone, make_tile_rect(sf::Color(255, 255, 255))},
        };
        tile_rects.at(Tile::Stone).setTexture(&tileset);

        for (auto& chunk : world) {
            ivec2 chunk_true_coords(chunk.get_coords().x * chunk_true_size.x, chunk.get_coords().y * chunk_true_size.y);

            for (auto [local_pos, tile] : chunk) {
                if (tile->type() == Tile::Air) continue;
                auto& rect = tile_rects.at(tile->type());
                rect.setPosition(to_sfvec_of<float>(chunk_true_coords + local_pos * Tile::SIZE));

                uint8 shape = (uint8)tile->shape();

                uvec2 texture_tile_index = uvec2(shape % 6, shape / 6);

                rect.setTextureRect(sf::IntRect(to_sfvec_of<int>(texture_tile_index * Tile::SIZE), to_sfvec(ivec2(Tile::SIZE, Tile::SIZE))));
                target.draw(rect);
            }

            // Draw debug rect
            chunk_debug_rect.setPosition(to_sfvec_of<float>(chunk_true_coords));
            target.draw(chunk_debug_rect);
        }
    });

    window.enter_main_loop();
}