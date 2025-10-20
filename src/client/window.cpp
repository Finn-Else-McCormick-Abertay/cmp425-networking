#include "window.h"

#include <optional>
#include <util/vec_convert.h>

using namespace std;
using sf::Event;

Window::Window(string title, uvec2 size) : _render_window(
    sf::VideoMode(to_sfvec(size)),
    title,
    sf::Style::Default, // Window border
    sf::State::Windowed, // Windowed vs fullscreen
    {} // OpenGL context settings
) {}

bool Window::is_open() const { return _render_window.isOpen(); }


void Window::set_draw_callback(function<void(sf::RenderTarget&)>&& cb) { _draw_cb = optional(move(cb)); }
void Window::set_on_resized_callback(function<void(Window&, const uvec2&)>&& cb) { _on_resized_cb = optional(move(cb)); }
void Window::set_close_request_callback(function<bool(Window&)>&& cb) { _close_request_cb = optional(move(cb)); }


void Window::enter_main_loop() {
    while (is_open()) {
        // Poll input
        while (const optional event = _render_window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                // If there is no callback or the callback returns true, close window
                if (!_close_request_cb || _close_request_cb.value()(*this)) _render_window.close();
            }

            else if (event->is<Event::Resized>()) {
                // Call resized callback if present
                _on_resized_cb.and_then([this](auto&& func) { func(*this, to_vec(_render_window.getSize())); return _on_resized_cb; });
                // Redraw window to account for new size
                render();
            }

            // -- Keyboard --

            else if (bool is_pressed = event->is<Event::KeyPressed>(); is_pressed || event->is<Event::KeyReleased>()) {
                // Key
            }

            // -- Mouse --

            else if (bool is_pressed = event->is<Event::MouseButtonPressed>(); is_pressed || event->is<Event::MouseButtonReleased>()) {
                // Mouse button
            }

            else if (event->is<Event::MouseMoved>()) {
                // Mouse motion
            }

            else if (event->is<Event::MouseWheelScrolled>()) {
                // Mouse scroll
            }

            else if (bool entered = event->is<Event::MouseEntered>(); entered || event->is<Event::MouseLeft>()) {
                // Mouse entered or left window
            }

            // -- Controller --

            else if (bool is_pressed = event->is<Event::JoystickButtonPressed>(); is_pressed || event->is<Event::JoystickButtonReleased>()) {
                // Controller button
            }

            else if (event->is<Event::JoystickMoved>()) {
                // Controller axis
            }

            else if (bool connected = event->is<Event::JoystickConnected>(); connected || event->is<Event::JoystickDisconnected>()) {
                // Active controllers changed
            }

        }

        // Render frame
        render();
    }
}

void Window::render() {
    _render_window.clear(sf::Color::Black);
    _draw_cb.and_then([this](auto&& func) { func(_render_window); return _draw_cb; });
    _render_window.display();
}