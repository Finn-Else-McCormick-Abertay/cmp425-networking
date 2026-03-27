#pragma once

#include <util/helper/glaze_enum_helper.h>
#include <network/network_manager.h>

DECL_NAMESPACED_ENUM_FORMATTED_GLAZED(actor_detail, enum, InterpolationValue, (
    /** Default interpolation value for this network type. See client and server values below. */
    DEFAULT, 
    
    /** Directly accept sent position. */
    NONE,
    
    /** Accept sent position, velocity and acceleration; keep simulating physics locally. */
    MOTION, 

    /** Linearly interpolate position based on sent position and velocity, and the amount of ticks since the packet was sent. */
    LINEAR_POSITION,
    
    /** Linearly interpolate position and velocity; keep simulating physics locally.
     * Falls back to behaving like MOTION if packets are too far out of date, as interpolation from a very old packet is likely to be inaccurate. */
    LINEAR_MOTION,

    // Not a value. Used for the cycling logic.
    __COUNT__
));

namespace actor_detail {
    static constexpr auto CLIENT_DEFAULT_INTERPOLATION = InterpolationValue::LINEAR_MOTION;
    static constexpr auto SERVER_DEFAULT_INTERPOLATION = InterpolationValue::NONE;
}

class Interpolation {
public:
    using Value = actor_detail::InterpolationValue;
    using enum Value;

    constexpr Interpolation() : _value(DEFAULT) {}
    constexpr Interpolation(Value value) : _value(value) {}

    constexpr operator Value() const { return validated_value(); }
    explicit operator bool() const = delete;

    constexpr bool is_default() const { return _value == DEFAULT; }

    constexpr Value raw_value() const { return _value; }
    constexpr Value validated_value() const {
        assert(_value != __COUNT__);
        switch (_value) {
            case DEFAULT: {
                if constexpr (NetworkManager::is_client()) return actor_detail::CLIENT_DEFAULT_INTERPOLATION;
                if constexpr (NetworkManager::is_server()) return actor_detail::SERVER_DEFAULT_INTERPOLATION;
            } break;
            default: return _value;
        }
    }

    Interpolation operator+(int rhs) const {
        return Interpolation(wrapped_increment(validated_value(), rhs));
    }
    Interpolation operator-(int rhs) const {
        return Interpolation(wrapped_increment(validated_value(), -rhs));
    }

    Interpolation& operator+=(int rhs) {
        _value = wrapped_increment(validated_value(), rhs);
        return *this;
    }
    Interpolation& operator-=(int rhs) {
        _value = wrapped_increment(validated_value(), -rhs);
        return *this;
    }

    Interpolation& operator++() {
        _value = wrapped_increment(validated_value(), 1);
        return *this;
    }
    Interpolation operator++(int) {
        Interpolation old = *this;
        operator++();
        return old;
    }

    Interpolation& operator--() {
        _value = wrapped_increment(validated_value(), -1);
        return *this;
    }
    Interpolation operator--(int) {
        Interpolation old = *this;
        operator--();
        return old;
    }

private:
    Value _value;

    static constexpr Value wrapped_increment(Value value, int increment) {
        return (Value)(((((int)value) + increment - 1) % (((int)Value::__COUNT__) - 1)) + 1);
    }
};

FORMAT_COMPLEX_ENUM_AS_VALUE(Interpolation);