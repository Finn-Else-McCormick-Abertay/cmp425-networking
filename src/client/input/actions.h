#if !defined(INPUT__ACTIONS_H) || defined(__INPUT_ACTION_SYMBOL_DEFINITIONS__)
#define INPUT__ACTIONS_H

// All this palaver is to allow this header to be used for both the extern declarations and the actual definition
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
    
    INPUT_ACTION(jump, bool);
    INPUT_ACTION(run, bool);
    
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

    namespace hotbar {
        INPUT_ACTION(slot_1, bool);
        INPUT_ACTION(slot_2, bool);
        INPUT_ACTION(slot_3, bool);
        INPUT_ACTION(slot_4, bool);
        INPUT_ACTION(slot_5, bool);
        INPUT_ACTION(slot_6, bool);
        INPUT_ACTION(slot_7, bool);
        INPUT_ACTION(slot_8, bool);
        INPUT_ACTION(slot_9, bool);
        INPUT_ACTION(slot_10, bool);

        INPUT_ACTION(next_slot, bool);
        INPUT_ACTION(previous_slot, bool);
    }

    namespace debug {
        INPUT_ACTION(tile, bool);
        INPUT_ACTION(tick, bool);
        INPUT_ACTION(actor, bool);
        INPUT_ACTION(network, bool);
        
        INPUT_ACTION(modifier_invert, bool);
        
        INPUT_ACTION(cycle_interpolation, bool);
        INPUT_ACTION(disable_interpolation, bool);
        INPUT_ACTION(default_interpolation, bool);
    }
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