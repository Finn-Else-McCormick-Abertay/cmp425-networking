#include "window.h"

#include <optional>
#include <util/vec_convert.h>
#include <input/input_manager.h>
#include <system/system_manager.h>
#include <camera/camera_manager.h>

using namespace std;
using sf::Event;

Window::Window(string title, uvec2 size) : _render_window(
    sf::VideoMode(to_sfvec(size)),
    title,
    sf::Style::Default, // Window border
    sf::State::Windowed, // Windowed vs fullscreen
    {} // OpenGL context settings
) {
    CameraManager::update_aspect(to_vec(_render_window.getSize()));
    CameraManager::inst()._target = &_render_window;
}

bool Window::is_open() const { return _render_window.isOpen(); }


void Window::set_draw_callback(function<void(sf::RenderTarget&)>&& cb) { _draw_cb = move(cb); }
void Window::set_close_request_callback(function<bool(Window&)>&& cb) { _close_request_cb = move(cb); }


void Window::enter_main_loop() {
    sf::Clock frame_clock;
    while (is_open()) {
        // Tick InputManager (for handling stuff like 'just pressed')
        InputManager::inst().tick();

        // Poll input
        while (const optional event = _render_window.pollEvent()) {
            // Send events to the input manager
            if (event) InputManager::inst().receive_event(event.value());

            // Special handling for window management events
            if (event->is<Event::Closed>() && (!_close_request_cb || _close_request_cb(*this))) _render_window.close();

            else if (event->is<Event::Resized>()) {
                // Update cameras           
                CameraManager::update_aspect(to_vec(_render_window.getSize()));
                // Redraw window to account for new size
                render();
            }
        }

        // Tick systems
        sf::Time delta_time = frame_clock.restart();
        SystemManager::tick(delta_time.asSeconds());

        // Render frame
        render();
    }
}

void Window::render() {
    _render_window.clear(sf::Color::Magenta);

    if (Camera* active_camera = CameraManager::get_active_camera()) _render_window.setView(active_camera->as_view());
    else _render_window.setView(_render_window.getDefaultView());

    if (_draw_cb) _draw_cb(_render_window);

    _render_window.display();
}