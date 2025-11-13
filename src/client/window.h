#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <prelude.h>
#include <prelude/filesystem.h>

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