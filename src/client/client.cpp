#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>

#include <terrain/world.h>

using namespace std;

void render(sf::RenderWindow& window, const World& world) {
    window.clear(sf::Color::Black);
    
    sf::View view(sf::FloatRect({0.f, 0.f}, {300.f, 200.f}));
    window.setView(view);

    auto chunk_debug_rect = sf::RectangleShape(sf::Vector2f{ Chunk::SIZE_TILES.x, Chunk::SIZE_TILES.y });
    for (auto& [coord, chunk] : world.chunks()) {
        chunk_debug_rect.setPosition(sf::Vector2f( chunk.get_coords().x * Chunk::SIZE_TILES.x, chunk.get_coords().y * Chunk::SIZE_TILES.y ));
        window.draw(chunk_debug_rect);
    }

    window.display();
}

int main() {
    cout << "Client init" << endl;
    
    auto world = World();
    world.set_chunk(ivec2{10, 2}, make_optional<Chunk>());
    world.set_chunk(ivec2{2, 1}, make_optional<Chunk>());

    sf::RenderWindow window(sf::VideoMode({800, 600}), "CMP425 Coursework");
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const optional event = window.pollEvent()) {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>()) window.close();

            if (event->is<sf::Event::Resized>()) render(window, world);
        }

        render(window, world);
    }
}