#if !defined(INPUT__ACTIONS_H) || defined(__INPUT_ACTION_SYMBOL_DEFINITIONS__)
#define INPUT__ACTIONS_H

// All this palava is to allow this header to be used for both the extern declarations and the actual definition
#define INPUT_ACTION(Name, Type, ...) extern input_impl::InputAction<Type> Name
#ifdef __INPUT_ACTION_SYMBOL_DEFINITIONS__
#undef INPUT_ACTION
#define INPUT_ACTION(Name, Type, ...) input_impl::InputAction<Type> Name(#Name __VA_OPT__(, input_impl::ActionDefinition{) __VA_ARGS__ __VA_OPT__(}))
#endif
#undef __INPUT_ACTION_SYMBOL_DEFINITIONS__

#include <input/input_action.h>

namespace actions {
    INPUT_ACTION(cursor, fvec2, .is_position = true);
    INPUT_ACTION(click, fvec2, .value_mirrors = "cursor");

    INPUT_ACTION(place, fvec2, .value_mirrors = "cursor");
    INPUT_ACTION(destroy, fvec2, .value_mirrors = "cursor");
    INPUT_ACTION(interact, fvec2, .value_mirrors = "cursor");
    
    INPUT_ACTION(select, bool);
    INPUT_ACTION(back, bool);
    
    INPUT_ACTION(move, fvec2,
        .value_sums = {
            { "_horizontal_move", mod::juggle::pos_x },
            { "_vertical_move", mod::juggle::neg_y },
            { "_left_move", mod::juggle::neg_x },
            { "_right_move", mod::juggle::pos_x },
            { "_up_move", mod::juggle::pos_y },
            { "_down_move", mod::juggle::neg_y },
        },
        .modifier = mod::constrain_length_to_norm
    );
}

namespace internal_actions {
    INPUT_ACTION(_left_move, float);
    INPUT_ACTION(_right_move, float);
    INPUT_ACTION(_up_move, float);
    INPUT_ACTION(_down_move, float);

    INPUT_ACTION(_horizontal_move, float);
    INPUT_ACTION(_vertical_move, float);
}

#endif