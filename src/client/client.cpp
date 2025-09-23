#include <iostream>

#include <client.h>
#include <util.h>
#include <shared_test_class.h>

#include <SFML/Window.hpp>
#include <optional>

using namespace std;

int main() {
    SharedTestClass shared_test;
    cout << "Hello, client!\n" << TestClass::test() << '\n' << util::test_function() << '\n' << shared_test.test();

    sf::Window window(sf::VideoMode({800, 600}), "My window");
     // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }
    }
}