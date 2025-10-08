#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>

using namespace std;
using namespace sf;

void render(RenderWindow& window) {
    window.clear(sf::Color::Black);

    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color(100, 250, 50));

    window.draw(shape);

    window.display();
}

int main() {
    cout << "Client init" << endl;

    RenderWindow window(VideoMode({800, 600}), "CMP425 Coursework");
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const optional event = window.pollEvent()) {
            // "close requested" event: we close the window
            if (event->is<Event::Closed>()) window.close();

            if (event->is<Event::Resized>()) render(window);
        }

        render(window);
    }
}