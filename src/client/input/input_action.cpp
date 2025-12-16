#include "input_action.h"

#include <input/input_manager.h>

input_impl::IInputAction::IInputAction(const str& name, const std::type_info& value_type, ActionDefinition&& definition) {
    InputManager::Registry::__register(*this, name, value_type, move(definition));
}
input_impl::IInputAction::~IInputAction() { InputManager::Registry::__unregister(*this); }

bool input_impl::IInputAction::down() const { return _down; }
bool input_impl::IInputAction::just_pressed() const { return _down && !_was_down_last_frame; }
bool input_impl::IInputAction::just_released() const { return !_down && _was_down_last_frame; }

input_impl::IInputAction::operator bool() const { return down(); }

// Modifiers

fvec2 actions::mod::juggle::pos_x(fvec2 v) { return fvec2(v.x, 0.f); }
fvec2 actions::mod::juggle::neg_x(fvec2 v) { return fvec2(-v.x, 0.f); }
fvec2 actions::mod::juggle::pos_y(fvec2 v) { return fvec2(0.f, v.x); }
fvec2 actions::mod::juggle::neg_y(fvec2 v) { return fvec2(0.f, -v.x); }

fvec2 actions::mod::constrain_length_to_norm(fvec2 v) {
    if (vmath_hpp::length2(v) > 1) return vmath_hpp::normalize(v);
    return v;
}