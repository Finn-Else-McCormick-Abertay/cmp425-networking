#include "input_manager.h"
#include <memory>
#include <util/vec.h>
#include <util/vec_convert.h>

using namespace std;
using namespace sf;

InputManager& InputManager::inst() {
    static unique_ptr<InputManager> instance(new InputManager);
    return *instance;
}

InputManager::ActionMeta::ActionMeta(string name, type_index value_type, input_impl::ActionTypeVariant variant)
: name(name), value_type(value_type), variant(variant) {}

void InputManager::init() {
    if (inst()._initialised) {
        console::error("Attempted to initialise InputManager a second time");
        return;
    }
    inst()._initialised = true;

    //for (auto& [action, meta] : inst()._actions) { console::debug("{} : {}", meta.name, (size_t)action); }

    inst()._dependent_actions[&actions::cursor] = {};
    for (auto& [action, meta] : inst()._actions) { if (meta.variant == input_impl::ActionTypeVariant::AtCursor) { inst()._dependent_actions[&actions::cursor].insert(action); } }
    console::debug("cursor has {} dependent actions", inst()._dependent_actions.at(&actions::cursor).size());

    console::info("InputManager initialised.");
}

void InputManager::setup_default_binds() {
    bind_mouse(actions::cursor);
    bind(Mouse::Button::Left, actions::click);
    bind(Mouse::Button::Left, actions::place);
    bind(Mouse::Button::Right, actions::destroy);

    bind(Keyboard::Key::Enter, actions::select);
    bind(Keyboard::Key::Escape, actions::back);
    
    bind(Keyboard::Scan::A, actions::left);
    bind(Keyboard::Scan::D, actions::right);
    bind(Keyboard::Scan::W, actions::up);
    bind(Keyboard::Scan::S, actions::down);
}

void InputManager::receive_event(const Event& event) {
    // -- Keyboard --
    
    if (bool is_pressed = event.is<Event::KeyPressed>(); is_pressed || event.is<Event::KeyReleased>()) {
        auto key_event = is_pressed ? event.getIf<Event::KeyPressed>() : (Event::KeyPressed*)event.getIf<Event::KeyReleased>();

        if (_bound_key_codes.contains(key_event->code)) {
            for (auto action : _bound_key_codes.at(key_event->code)) { action->_down = is_pressed; set_action_value(action, is_pressed); }
        }
        if (_bound_scan_codes.contains(key_event->scancode)) {
            for (auto action : _bound_scan_codes.at(key_event->scancode)) { action->_down = is_pressed; set_action_value(action, is_pressed); }
        }
    }

    // -- Mouse --

    // Mouse button
    else if (bool is_pressed = event.is<Event::MouseButtonPressed>(); is_pressed || event.is<Event::MouseButtonReleased>()) {
        auto button_event = is_pressed ? event.getIf<Event::MouseButtonPressed>() : (Event::MouseButtonPressed*)event.getIf<Event::MouseButtonReleased>();

        for (auto action : _bound_mouse) {
            if (_actions.at(action).variant == input_impl::ActionTypeVariant::Position) {
                action->set_value(fvec2(button_event->position.x, button_event->position.y));
            }
        }

        if (_bound_mouse_buttons.contains(button_event->button)) {
            for (auto action : _bound_mouse_buttons.at(button_event->button)) { action->_down = is_pressed; set_action_value(action, is_pressed); }
        }
    }

    // Mouse motion
    else if (event.is<Event::MouseMoved>()) {
        auto mouse_event = event.getIf<Event::MouseMoved>();

        for (auto action : _bound_mouse) {
            if (_actions.at(action).variant == input_impl::ActionTypeVariant::Position) { set_action_value(action, fvec2(mouse_event->position.x, mouse_event->position.y)); }
            else { set_action_value(action, to_vec_of<float>(mouse_event->position - _prev_frame_mouse_position)); }
        }
    }

    // Mouse scroll
    else if (event.is<Event::MouseWheelScrolled>()) {
    }

    // Mouse entered or left window
    else if (bool entered = event.is<Event::MouseEntered>(); entered || event.is<Event::MouseLeft>()) {
    }

    // -- Controller --

    // Controller button
    else if (bool is_pressed = event.is<Event::JoystickButtonPressed>(); is_pressed || event.is<Event::JoystickButtonReleased>()) {
    }

    // Controller axis
    else if (event.is<Event::JoystickMoved>()) {
    }

    // Active controllers changed
    else if (bool connected = event.is<Event::JoystickConnected>(); connected || event.is<Event::JoystickDisconnected>()) {
    }
}

void InputManager::tick() {
    console::debug("actions::{} -> {} | {}", "click", actions::click.down(), actions::click.value());
    //console::debug("actions::{} -> {} | {}", "left", actions::left.down(), actions::left.value());
    //console::debug("actions::{} -> {} | {}", "cursor", actions::cursor.down(), actions::cursor.value());
    for (auto& [action, meta] : _actions) {
        action->_was_down_last_frame = action->_down;
    }
    _prev_frame_mouse_position = Mouse::getPosition();
}

void InputManager::bind_mouse(Action& action)                   { inst()._bound_mouse.insert(&action); }
void InputManager::bind(sf::Mouse::Button x, Action& action)    { inst()._bound_mouse_buttons[x].insert(&action); }
void InputManager::bind(sf::Mouse::Wheel x, Action& action)     { inst()._bound_mouse_wheels[x].insert(&action); }
void InputManager::bind(sf::Keyboard::Key x, Action& action)    { inst()._bound_key_codes[x].insert(&action); }
void InputManager::bind(sf::Keyboard::Scan x, Action& action)   { inst()._bound_scan_codes[x].insert(&action); }

void InputManager::unbind_mouse(Action& action)                 { inst()._bound_mouse.erase(&action); }
void InputManager::unbind(sf::Mouse::Button x, Action& action)  { inst()._bound_mouse_buttons[x].erase(&action); }
void InputManager::unbind(sf::Mouse::Wheel x, Action& action)   { inst()._bound_mouse_wheels[x].erase(&action); }
void InputManager::unbind(sf::Keyboard::Key x, Action& action)  { inst()._bound_key_codes[x].erase(&action); }
void InputManager::unbind(sf::Keyboard::Scan x, Action& action) { inst()._bound_scan_codes[x].erase(&action); }