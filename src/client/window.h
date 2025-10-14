#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <functional>
#include <optional>

#include <util/vec.h>

class Window {
public:
    Window(
        std::string title = "CMP425 Coursework",
        uvec2 size = { 800, 600 }
    );
    bool is_open() const;

    void set_draw_callback(std::function<void(sf::RenderTarget&)>&& cb);
    void set_on_resized_callback(std::function<void(Window&, const uvec2&)>&& cb);
    void set_close_request_callback(std::function<bool(Window&)>&& cb);

    void enter_main_loop();

private:
    void render();

private:
    sf::RenderWindow _render_window;

    std::optional<std::function<void(sf::RenderTarget&)>> _draw_cb;
    std::optional<std::function<void(Window&, const uvec2&)>> _on_resized_cb;
    std::optional<std::function<bool(Window&)>> _close_request_cb;
};