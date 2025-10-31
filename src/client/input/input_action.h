#pragma once
#include <typeinfo>
#include <string>
#include <util/vec.h>
#include <concepts>
#include <functional>

class InputManager;

namespace actions { namespace mod {
    fvec2 constrain_length_to_norm(fvec2);
    namespace juggle {
        fvec2 pos_x(fvec2); fvec2 pos_y(fvec2);
        fvec2 neg_x(fvec2); fvec2 neg_y(fvec2);
    }
}}

namespace input_impl {
    using ActionModifier = std::function<fvec2(fvec2)>;
    struct ComplexDefinitionComponent {
        std::string name;
        ActionModifier modifier = ActionModifier();
    };

    struct ActionDefinition {
        bool is_position = false;
        std::string value_mirrors = ""; // Simple dependency
        std::vector<ComplexDefinitionComponent> value_sums = {}; // Complex dependency
        ActionModifier modifier = ActionModifier(); // Final post-processing step
    };
}

namespace input_impl {
    template<typename From, typename To> class value_helper                      { public: value_helper() = delete; static To cast(From v); };
    template<> class value_helper<float, bool>                                   { public: static bool cast(float v) { return v > 0.f; } };
    template<typename T1, std::convertible_to<T1> T2> class value_helper<T1, T2> { public: static T2 cast(T1 v)   { return v; } };
    template<std::convertible_to<float> T> class value_helper<T, fvec2>          { public: static fvec2 cast(T v) { return fvec2(value_helper<T, float>::cast(v), 0); } };
    template<std::convertible_to<float> T> class value_helper<fvec2, T>          { public: static T cast(fvec2 v) { return v.x; } };

    class IInputAction {
    public:
        bool down() const;
        bool just_pressed() const;
        bool just_released() const;

        operator bool() const;
    
    protected:  IInputAction() = default;
    private:
        friend class InputManager;
        bool _down, _was_down_last_frame;
        virtual bool get_value_as_bool() const = 0; virtual float get_value_as_float() const = 0; virtual fvec2 get_value_as_fvec2() const = 0;
        virtual void set_value(bool) = 0; virtual void set_value(float) = 0; virtual void set_value(fvec2) = 0;
    };
    
    template<typename TValue>
    class InputAction : public IInputAction {
    public:
        InputAction(std::string name, ActionDefinition definition = {}) {
            InputManager::Registry::register_action(*this, name, typeid(TValue), std::move(definition));
        }
        TValue value() const { return _value; }

        operator TValue() const { return _value; }
    
    private:
        friend class InputManager;
        TValue _value;
        
        bool  get_value_as_bool()  const override { return value_helper<TValue, bool >::cast(_value); }
        float get_value_as_float() const override { return value_helper<TValue, float>::cast(_value); }
        fvec2 get_value_as_fvec2() const override { return value_helper<TValue, fvec2>::cast(_value); }

        void set_value(bool v)  override { _value = value_helper<bool,  TValue>::cast(v); }
        void set_value(float v) override { _value = value_helper<float, TValue>::cast(v); }
        void set_value(fvec2 v) override { _value = value_helper<fvec2, TValue>::cast(v); }
    };
}