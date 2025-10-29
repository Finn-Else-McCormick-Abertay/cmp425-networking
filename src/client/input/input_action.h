#pragma once
#include <typeinfo>
#include <string>
#include <util/vec.h>
#include <concepts>

class InputManager;
namespace input_impl {
    class I_InputAction {
    public:
        virtual ~I_InputAction() = default;
        
        bool down() const;
        bool just_pressed() const;
        bool just_released() const;

    protected:
        I_InputAction() = default;
        bool _down, _was_down_last_frame;
        friend class InputManager;

        virtual void set_value(bool) = 0;
        virtual void set_value(float) = 0;
        virtual void set_value(fvec2) = 0;
    };

    template<typename From, typename To> class ActionValueCastHelper{ public: static To cast(From v); };

    template<std::convertible_to<float> T> class ActionValueCastHelper<T, fvec2>{ public: static fvec2 cast(T v) { return fvec2(v, 0); } };
    template<std::convertible_to<float> T> class ActionValueCastHelper<fvec2, T>{ public: static T cast(fvec2 v) { return vmath_hpp::length(v); } };
    template<typename T1, std::convertible_to<T1> T2> class ActionValueCastHelper<T1, T2>{ public: static T2 cast(T1 v) { return v; } };

    enum class ActionTypeVariant { None, Position, AtCursor };
    
    template<typename ValueType, ActionTypeVariant Variant = ActionTypeVariant::None>
    class InputAction : public I_InputAction {
    public:
        InputAction(std::string name) { InputManager::register_action<ValueType, Variant>(*this, name); }
        
        ValueType value() const { return _value; }
        
    protected:
        ValueType _value;
        friend class InputManager;

        virtual void set_value(bool v) override { _value = ActionValueCastHelper<bool, ValueType>::cast(v); }
        virtual void set_value(float v) override { _value = ActionValueCastHelper<float, ValueType>::cast(v); }
        virtual void set_value(fvec2 v) override { _value = ActionValueCastHelper<fvec2, ValueType>::cast(v); }
    };
}