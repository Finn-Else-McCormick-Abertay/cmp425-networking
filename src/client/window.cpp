#include "window.h"

#include <prelude/opt.h>
#include <alias/thread.h>
#include <util/vec_convert.h>

#include <input/input_manager.h>
#include <system/system_manager.h>
#include <render/render_manager.h>

#include <game_loop.h>

using sf::Event;

Window::Window(str title, uvec2 size) :
    _title(title),
    _render_window(
        sf::VideoMode(to_sfvec(size)),
        title,
        sf::Style::Default, // Window border
        sf::State::Windowed, // Windowed vs fullscreen
        {} // OpenGL context settings
    ) {
    _open = true;
    RenderManager::set_target(&_render_window);
    set_icon("resources/assets/textures/placeholder.png");
}

void Window::enter_loop() {    
    if (!_render_window.setActive(false)) print<error, Window>("Failed to deactivate OpenGL context.");
    thread render_thread = thread(&Window::render_thread, this);
    process_thread();
    render_thread.join();
}

bool Window::is_open() const { return _open; }

const str& Window::title() const { return _title; }
void Window::set_title(const str& title) { _title = title; _render_window.setTitle(_title); }

void Window::set_icon(const filepath& path) {
    if (!filesystem::exists(path)) return print<error, Window>("Could not set icon to non-existent image '{}'.", path);
    sf::Image image;
    if (!image.loadFromFile(path)) return print<error, Window>("Could not set icon to '{}' - file failed to load.", path);
    _render_window.setIcon(image);
}

void Window::process_thread() {
    GameLoop game_loop;
    while (is_open()) {
        // Tick InputManager (for handling stuff like 'just pressed')
        InputManager::inst().tick();

        // Poll input
        while (const opt event = _render_window.pollEvent()) {
            // Send events to the input manager
            if (event) InputManager::inst().receive_event(event.value());

            // Handle close requests (currently just closes, should trigger save logic / are you sure dialog)
            if (event->is<Event::Closed>()) _open = false;

            // Update camera aspects on resize
            else if (event->is<Event::Resized>()) RenderManager::set_target(&_render_window);
        }

        game_loop.tick();
    }
}

void Window::render_thread() {
    if (!_render_window.setActive(true)) print<error, Window>("Could not activate OpenGL context on render thread.");
    
    RenderManager::set_target(&_render_window);

    //sf::Clock frame_clock;
    while (is_open()) {
        //sf::Time delta_time = frame_clock.restart();
        
        RenderManager::render();
        _render_window.display();
    }
    _render_window.close();
}