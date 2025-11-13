#pragma once
#include <typeinfo>
#include <typeindex>

#include <util/helper/singleton.h>

#include <prelude.h>
#include <prelude/vec.h>
#include <prelude/containers.h>

#include <input/input_action.h>
#include <input/actions.h>
#include <input/input_aliases.h>
#include <SFML/Window/Event.hpp>

class InputManager { DECL_SINGLETON(InputManager);
    bool _initialised = false;
    using Action = input_impl::IInputAction;

public:
    DECL_REGISTRY_WITH_ARGS(Action, str name, const std::type_info& value_type, input_impl::ActionDefinition&&);
    static void init(); // Initialise the manager. Must be called exactly once, at the start of main.

    static void bind(Mouse::Axis, Action& action);        static void unbind(Mouse::Axis, Action& action);
    static void bind(Mouse::Button, Action& action);      static void unbind(Mouse::Button, Action& action);
    static void bind(Mouse::Wheel, Action& action);       static void unbind(Mouse::Wheel, Action& action);

    static void bind(Key, Action& action);                static void unbind(Key, Action& action);
    static void bind(ScanCode, Action& action);           static void unbind(ScanCode, Action& action);

    static void bind(Controller::Button, Action& action); static void unbind(Controller::Button, Action& action);
    static void bind(Controller::Axis, Action& action);   static void unbind(Controller::Axis, Action& action);
    
    static void setup_default_binds();


private:
    struct ActionMeta {
        str name; std::type_index value_type; input_impl::ActionDefinition definition;
        set<Action*> simple_dependents; set<Action*> complex_dependents;
    };
    set<Action*> _actions; bstmap<Action*, ActionMeta> _action_meta;
    bstmap<Action*, bstmap<Action*, input_impl::ActionModifier>> _complex_dependencies;

    static Action* get_action_by_name(const str& name);
    static const str& get_name(Action*);
    static const input_impl::ActionDefinition& get_definition(Action*);
    static bool is_dependent(Action*);

    template<typename T> void set_action_value(Action* action, T val, bool dependent_override = false) {
        if (!is_dependent(action) || dependent_override) {
            fvec2 true_val = input_impl::value_helper<T, fvec2>::cast(val);
            if (_action_meta.at(action).definition.modifier) true_val = _action_meta.at(action).definition.modifier(true_val);
            action->set_value(true_val);
            for(auto& dep_action : _action_meta.at(action).simple_dependents) set_action_value(dep_action, true_val, true);
            for(auto& complex_action : _action_meta.at(action).complex_dependents) recalculate_complex_action(complex_action);
        }
    }
    void recalculate_complex_action(Action*);

    // Binds
    set<Action*> _bound_mouse;
    bstmap<Mouse::Button, set<Action*>> _bound_mouse_buttons;
    bstmap<Mouse::Wheel, set<Action*>> _bound_mouse_wheels;
    bstmap<Key, set<Action*>> _bound_key_codes;
    bstmap<ScanCode, set<Action*>> _bound_scan_codes;
    bstmap<Controller::Button, set<Action*>> _bound_controller_buttons;
    bstmap<Controller::Axis, set<Action*>> _bound_controller_axes;
    
    friend class Window;
    void receive_event(const sf::Event&); // Called for every input event
    void tick(); // Called at start of frame
    
    sf::ivec2 _prev_frame_mouse_position;

    static constexpr float JOYSTICK_DEADZONE = 0.1f;
};