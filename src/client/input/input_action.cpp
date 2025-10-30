#include "input_action.h"

bool input_impl::I_InputAction::down() const { return _down; }
bool input_impl::I_InputAction::just_pressed() const { return _down && !_was_down_last_frame; }
bool input_impl::I_InputAction::just_released() const { return !_down && _was_down_last_frame; }

// Modifiers

fvec2 actions::mod::juggle::pos_x(fvec2 v) { return fvec2(v.x, 0.f); }
fvec2 actions::mod::juggle::neg_x(fvec2 v) { return fvec2(-v.x, 0.f); }
fvec2 actions::mod::juggle::pos_y(fvec2 v) { return fvec2(0.f, v.x); }
fvec2 actions::mod::juggle::neg_y(fvec2 v) { return fvec2(0.f, -v.x); }

fvec2 actions::mod::constrain_length_to_norm(fvec2 v) {
    if (vmath_hpp::length2(v) > 1) return vmath_hpp::normalize(v);
    return v;
}