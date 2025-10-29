#include "input_action.h"

bool input_impl::I_InputAction::down() const { return _down; }
bool input_impl::I_InputAction::just_pressed() const { return _down && !_was_down_last_frame; }
bool input_impl::I_InputAction::just_released() const { return !_down && _was_down_last_frame; }