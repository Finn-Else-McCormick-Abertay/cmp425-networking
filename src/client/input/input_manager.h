#pragma once
#include <SFML/Window/Event.hpp>
#include <input/input_action.h>
#include <typeinfo>
#include <typeindex>
#include <map>
#include <set>
#include <string>
#include <util/console.h>
#include <util/vec.h>

#include <input/actions.h>

class InputManager {
private:
    InputManager() = default;
    static InputManager& inst();
    bool _initialised = false;

public:
    using Action = input_impl::I_InputAction;

    static void init();
    static void setup_default_binds();

    static void bind_mouse(Action& action);                 static void unbind_mouse(Action& action);
    static void bind(sf::Mouse::Button, Action& action);    static void unbind(sf::Mouse::Button, Action& action);
    static void bind(sf::Mouse::Wheel, Action& action);     static void unbind(sf::Mouse::Wheel, Action& action);
    static void bind(sf::Keyboard::Key, Action& action);    static void unbind(sf::Keyboard::Key, Action& action);
    static void bind(sf::Keyboard::Scan, Action& action);   static void unbind(sf::Keyboard::Scan, Action& action);

public:
    template<typename ValueType, input_impl::ActionTypeVariant Variant>
    static void register_action(Action& action, std::string name) {
        if (inst()._initialised) { console::error("register_action({}<{}>:{}) called when already InputManager initialised", name, typeid(ValueType).name(), (int)Variant); return; }
        inst()._actions.emplace(&action, ActionMeta(name, std::type_index(typeid(ValueType)), Variant));
    }

private:
    struct ActionMeta {
        ActionMeta(std::string name, std::type_index value_type, input_impl::ActionTypeVariant variant);
        std::string name; std::type_index value_type; input_impl::ActionTypeVariant variant;
    };
    std::map<Action*, ActionMeta> _actions;
    std::map<Action*, std::set<Action*>> _dependent_actions;

    template<typename T>
    void set_action_value(Action* action, T val, bool is_dependent = false) {
        if (!is_dependent && _actions.at(action).variant == input_impl::ActionTypeVariant::AtCursor) return;
        action->set_value(val);
        if (_dependent_actions.contains(action)) { for(auto dep_action : _dependent_actions.at(action)) { set_action_value(dep_action, val, true); } }
    }

    // Binds
    std::set<Action*> _bound_mouse;
    std::map<sf::Mouse::Button, std::set<Action*>> _bound_mouse_buttons;
    std::map<sf::Mouse::Wheel, std::set<Action*>> _bound_mouse_wheels;
    std::map<sf::Keyboard::Key, std::set<Action*>> _bound_key_codes;
    std::map<sf::Keyboard::Scan, std::set<Action*>> _bound_scan_codes;
    
    friend class Window;
    void receive_event(const sf::Event&); // Called for every input event
    void tick(); // Called at start of frame
    
    sf::Vector2i _prev_frame_mouse_position;
};