#include <SFML/Graphics.hpp>
#include <iostream>
#include <util/console.h>

#include <terrain/world.h>

#include <window.h>
#include <util/vec.h>

using namespace std;

int main() {
    console::info("Client init");
    
    auto world = World();
    world.set_chunk(ivec2{10, 2}, make_optional<Chunk>());
    world.set_chunk(ivec2{2, 1}, make_optional<Chunk>());

    Window window;

    window.set_close_request_callback([](Window& window) -> bool {
        console::debug("Window: Close Requested");
        return true;
    });
    window.set_on_resized_callback([](Window& window, uvec2 new_size) {
        console::debug("Window: Resized to {}", new_size);
    });
    window.set_draw_callback([&world](sf::RenderTarget& target) {
        float viewScale = 300;
        float windowAspect = (float)target.getSize().y / target.getSize().x;

        target.setView(sf::View({0.f, 0.f}, {viewScale / windowAspect, viewScale}));

        auto chunk_debug_rect = sf::RectangleShape(sf::Vector2f{ Chunk::SIZE_TILES.x, Chunk::SIZE_TILES.y });
        for (auto& [coord, chunk] : world.chunks()) {
            chunk_debug_rect.setPosition(sf::Vector2f( chunk.get_coords().x * Chunk::SIZE_TILES.x, chunk.get_coords().y * Chunk::SIZE_TILES.y ));
            target.draw(chunk_debug_rect);
        }
    });

    window.enter_main_loop();
}