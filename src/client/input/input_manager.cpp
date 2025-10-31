#include "input_manager.h"
#include <util/vec.h>
#include <util/vec_convert.h>

using namespace std;

SINGLETON_INST_DEF(InputManager)

const string& InputManager::get_name(Action* action) { return inst()._action_meta.at(action).name; }
const input_impl::ActionDefinition& InputManager::get_definition(Action* action) { return inst()._action_meta.at(action).definition; }
bool InputManager::is_dependent(Action* action) { return !inst()._action_meta.at(action).definition.value_mirrors.empty() || !inst()._action_meta.at(action).definition.value_sums.empty(); }

InputManager::Action* InputManager::get_action_by_name(const string& name) {
    for (auto& [action, meta] : inst()._action_meta) { if (name == meta.name) { return action; } }
    return nullptr;
}

void InputManager::init() {
    if (inst()._initialised) { console::error("Attempted to initialise InputManager a second time"); return; }
    inst()._initialised = true;

    for (auto& [action, meta] : inst()._action_meta) {
        if (!meta.definition.value_mirrors.empty()) {
            auto dependency = get_action_by_name(meta.definition.value_mirrors); 
            if (dependency) inst()._action_meta.at(dependency).simple_dependents.insert(action);
            else console::error("{} attempted to depend on non-existent action '{}'.", meta.name, meta.definition.value_mirrors);
        }
        if (!meta.definition.value_sums.empty()) {
            std::map<Action*, input_impl::ActionModifier> map;
            for (auto& comp : meta.definition.value_sums) {
                auto dependency = get_action_by_name(comp.name); 
                if (dependency) {
                    inst()._action_meta.at(dependency).complex_dependents.insert(action);
                    map[dependency] = comp.modifier;
                }
                else console::error("{} attempted to depend on non-existent action '{}'.", meta.name, comp.name);
            }
            inst()._complex_dependencies.emplace(action, std::move(map));
        }
    }

    console::info("InputManager initialised.");
}

void InputManager::recalculate_complex_action(Action* action) {
    if (!_complex_dependencies.contains(action)) { console::error("Attempted to recalculate non-complex action '{}'", get_name(action)); return; }

    fvec2 accumulated;
    for (auto& [dependency_action, modifier] : _complex_dependencies.at(action)) {
        auto val = dependency_action->get_value_as_fvec2();
        if (modifier) val = modifier(val);
        accumulated += val;
    }
    set_action_value(action, accumulated, true);
}

void InputManager::setup_default_binds() {
    bind(Mouse::Motion, actions::cursor);
    bind(Mouse::Button::Left, actions::click);
    bind(Mouse::Button::Left, actions::place);
    bind(Mouse::Button::Right, actions::destroy);

    bind(Key::Enter, actions::select);
    bind(Controller::Button::XBOX_A, actions::select);

    bind(Key::Escape, actions::back);
    bind(Controller::Button::XBOX_B, actions::select);
    
    bind(ScanCode::A, internal_actions::_left_move);
    bind(ScanCode::D, internal_actions::_right_move);
    bind(ScanCode::W, internal_actions::_up_move);
    bind(ScanCode::S, internal_actions::_down_move);

    bind(Controller::Axis::X, internal_actions::_horizontal_move);
    bind(Controller::Axis::Y, internal_actions::_vertical_move);
}

void InputManager::receive_event(const sf::Event& event) {
    // -- Keyboard --
    
    if (bool is_pressed = event.is<sf::Event::KeyPressed>(); is_pressed || event.is<sf::Event::KeyReleased>()) {
        auto key_event = is_pressed ? event.getIf<sf::Event::KeyPressed>() : (sf::Event::KeyPressed*)event.getIf<sf::Event::KeyReleased>();

        if (_bound_key_codes.contains(key_event->code)) {
            for (auto action : _bound_key_codes.at(key_event->code)) { action->_down = is_pressed; set_action_value(action, is_pressed); }
        }
        if (_bound_scan_codes.contains(key_event->scancode)) {
            for (auto action : _bound_scan_codes.at(key_event->scancode)) { action->_down = is_pressed; set_action_value(action, is_pressed); }
        }
    }

    // -- Mouse --

    // Mouse button
    else if (bool is_pressed = event.is<sf::Event::MouseButtonPressed>(); is_pressed || event.is<sf::Event::MouseButtonReleased>()) {
        auto button_event = is_pressed ? event.getIf<sf::Event::MouseButtonPressed>() : (sf::Event::MouseButtonPressed*)event.getIf<sf::Event::MouseButtonReleased>();

        for (auto action : _bound_mouse) { if (get_definition(action).is_position) { action->set_value(to_fvec(button_event->position)); } }

        if (_bound_mouse_buttons.contains(button_event->button)) {
            for (auto action : _bound_mouse_buttons.at(button_event->button)) { action->_down = is_pressed; set_action_value(action, is_pressed); }
        }
    }

    // Mouse motion
    else if (auto mouse_event = event.getIf<sf::Event::MouseMoved>()) {
        for (auto action : _bound_mouse) {
            if (get_definition(action).is_position) { set_action_value(action, to_fvec(mouse_event->position)); }
            else { set_action_value(action, to_fvec(mouse_event->position - _prev_frame_mouse_position)); }
        }
    }

    // Mouse scroll
    else if (auto wheel_event = event.getIf<sf::Event::MouseWheelScrolled>()) {
        if (_bound_mouse_wheels.contains(wheel_event->wheel)) {
            for (auto action : _bound_mouse_wheels.at(wheel_event->wheel)) {
                set_action_value(action, wheel_event->delta);
            }
        }
    }

    // Mouse entered or left window
    //else if (bool entered = event.is<sf::Event::MouseEntered>(); entered || event.is<sf::Event::MouseLeft>()) {}

    // -- Controller --

    // Controller button
    else if (bool is_pressed = event.is<sf::Event::JoystickButtonPressed>(); is_pressed || event.is<sf::Event::JoystickButtonReleased>()) {
        auto button_event = is_pressed ? event.getIf<sf::Event::JoystickButtonPressed>() : (sf::Event::JoystickButtonPressed*)event.getIf<sf::Event::JoystickButtonReleased>();
        if (_bound_controller_buttons.contains((Controller::Button)button_event->button)) {
            for (auto action : _bound_controller_buttons.at((Controller::Button)button_event->button)) {
                action->_down = is_pressed; set_action_value(action, is_pressed);
            }
        }
    }

    // Controller axis
    else if (auto joystick_event = event.getIf<sf::Event::JoystickMoved>()) {
        if (_bound_controller_axes.contains(joystick_event->axis)) {
            for (auto action : _bound_controller_axes.at(joystick_event->axis)) {
                bool exceeds_deadzone = abs(joystick_event->position) > JOYSTICK_DEADZONE;
                action->_down = exceeds_deadzone;
                set_action_value(action, exceeds_deadzone ? joystick_event->position / 100.f : 0.f);
            }
        }
    }

    // Active controllers changed
    else if (bool connected = event.is<sf::Event::JoystickConnected>(); connected || event.is<sf::Event::JoystickDisconnected>()) {
        auto connection_event = connected ? event.getIf<sf::Event::JoystickConnected>() : (sf::Event::JoystickConnected*)event.getIf<sf::Event::JoystickDisconnected>();

        if (connected) console::debug("Controller '{}' connected as Controller #{}",
            sf::Joystick::getIdentification(connection_event->joystickId).name.toAnsiString(),
            connection_event->joystickId
        );
        else console::debug("Controller #{} disconnected", connection_event->joystickId);
    }
}

void InputManager::tick() {
    for (auto action : _actions) { action->_was_down_last_frame = action->_down; }
    _prev_frame_mouse_position = sf::Mouse::getPosition();
}

void InputManager::bind(Mouse::Axis, Action& action)            { inst()._bound_mouse.insert(&action); }
void InputManager::bind(Mouse::Button x, Action& action)        { inst()._bound_mouse_buttons[x].insert(&action); }
void InputManager::bind(Mouse::Wheel x, Action& action)         { inst()._bound_mouse_wheels[x].insert(&action); }
void InputManager::bind(Key x, Action& action)                  { inst()._bound_key_codes[x].insert(&action); }
void InputManager::bind(ScanCode x, Action& action)             { inst()._bound_scan_codes[x].insert(&action); }
void InputManager::bind(Controller::Button x, Action& action)   { inst()._bound_controller_buttons[x].insert(&action); }
void InputManager::bind(Controller::Axis x, Action& action)     { inst()._bound_controller_axes[x].insert(&action); }

void InputManager::unbind(Mouse::Axis, Action& action)          { inst()._bound_mouse.erase(&action); }
void InputManager::unbind(Mouse::Button x, Action& action)      { inst()._bound_mouse_buttons[x].erase(&action); }
void InputManager::unbind(Mouse::Wheel x, Action& action)       { inst()._bound_mouse_wheels[x].erase(&action); }
void InputManager::unbind(Key x, Action& action)                { inst()._bound_key_codes[x].erase(&action); }
void InputManager::unbind(ScanCode x, Action& action)           { inst()._bound_scan_codes[x].erase(&action); }
void InputManager::unbind(Controller::Button x, Action& action) { inst()._bound_controller_buttons[x].erase(&action); }
void InputManager::unbind(Controller::Axis x, Action& action)   { inst()._bound_controller_axes[x].erase(&action); }

void InputManager::Registry::__register(Action& action, string name, const std::type_info& value_type, input_impl::ActionDefinition&& definition) {
    if (!inst()._initialised) {
        inst()._actions.insert(&action);
        inst()._action_meta.emplace(&action, InputManager::ActionMeta{ name, std::type_index(value_type), move(definition) } );
    }
    else console::error("register_action({}<{}>) called after input manager initialisation.", name, value_type.name());
}

void InputManager::Registry::__unregister(Action& action) {
    // Not handling linkages properly cause it should never actually happen, it's just here for consistency with the other singletons
    inst()._actions.erase(&action);
    inst()._action_meta.erase(&action);
}