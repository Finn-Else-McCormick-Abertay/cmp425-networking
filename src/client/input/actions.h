#if !defined(INPUT__ACTIONS_H) || defined(__INPUT_ACTION_SYMBOL_DEFINITIONS__)
#define INPUT__ACTIONS_H

#include <util/vec.h>
#include <input/input_action.h>

// All this palava is to allow this header to be used for both the extern declarations and the actual definition
#if defined(__INPUT_ACTION_SYMBOL_DEFINITIONS__)
#define INPUT_ACTION(name, type, ...) input_impl::InputAction<type __VA_OPT__(, input_impl::ActionTypeVariant::)__VA_ARGS__> name(#name)
#else
#define INPUT_ACTION(name, type, ...) extern input_impl::InputAction<type __VA_OPT__(, input_impl::ActionTypeVariant::)__VA_ARGS__> name;
#endif

namespace actions {
    INPUT_ACTION(cursor, fvec2, Position);
    INPUT_ACTION(click, fvec2, AtCursor);

    INPUT_ACTION(place, fvec2, AtCursor);
    INPUT_ACTION(destroy, fvec2, AtCursor);
    INPUT_ACTION(interact, fvec2, AtCursor);
    
    INPUT_ACTION(select, bool);
    INPUT_ACTION(back, bool);
    
    INPUT_ACTION(direction, fvec2);
    INPUT_ACTION(left, float);
    INPUT_ACTION(right, float);
    INPUT_ACTION(up, float);
    INPUT_ACTION(down, float);
}

#endif
#undef __INPUT_ACTION_SYMBOL_DEFINITIONS__