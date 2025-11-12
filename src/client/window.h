#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <functional>
#include <filesystem>

#include <util/vec.h>
#include <util/std_aliases.h>

class Window {
public:
    Window(
        str title = "CMP425 Coursework",
        uvec2 size = { 800, 600 }
    );

    bool is_open() const;

    const str& title() const;
    void set_title(const str&);

    void set_icon(const std::filesystem::path&);

    void enter_loop();

private:
    sf::RenderWindow _render_window;
    str _title;

    void process_thread();
    void render_thread();
};